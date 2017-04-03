//////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// University of California Santa Barbara, Kapil Chhabra, Yining Deng.
// Mitsubishi Electric ITE-VIL, Leszek Cieplinski, LG-Elite.
// (contributing organizations names)
//
// in the course of development of the MPEG-7 Experimentation Model.
//
// This software module is an implementation of a part of one or more MPEG-7
// Experimentation Model tools as specified by the MPEG-7 Requirements.
//
// ISO/IEC gives users of MPEG-7 free license to this software module or
// modifications thereof for use in hardware or software products claiming
// conformance to MPEG-7.
//
// Those intending to use this software module in hardware or software products
// are advised that its use may infringe existing patents. The original
// developer of this software module and his/her company, the subsequent
// editors and their companies, and ISO/IEC have no liability for use of this
// software module or modifications thereof in an implementation.
//
// Copyright is not released for non MPEG-7 conforming products. The
// organizations named above retain full right to use the code for their own
// purpose, assign or donate the code to a third party and inhibit third
// parties
// from using the code for non MPEG-7 conforming products.
//
// Copyright (c) 1998-2000.
//
// This notice must be included in all copies or derivative works.
//
// Applicable File Name:  DominantColorExtraction.cpp
//

#include <cstdio>

#include <cmath>
#include <cstring>
#include <iostream>
#include <cfloat>

#include "imgutil.h"

#include "DominantColorExtraction.h"
#include "ColorSpaceExtraction.h"
#include "imgutil.h"

#define SC_BIT	 5

#define EPSGLOB  0.01   // global stopping criterion for distortion change
#define EPSSPLT  0.02   // splitting criterion for distortion change
#define DCNMAX      8
#define DSTMIN  255.0
#define SPLFCT   0.10
#define VARTHR   50.0

void (*dominantcolor_colorspaceconversion)(TPixel*, TPixel*, TCoor, TCoor);

//=============================================================================

using namespace XM;

unsigned char* interleave (unsigned char* pixel_R,unsigned char* pixel_G,
			   unsigned char* pixel_B,int size);

DominantColorExtractionTool::DominantColorExtractionTool(bool normalize, bool variance, bool spatial, int bin1, int bin2, int bin3):
  m_Descriptor(0)
{
  srcImage = 0;

  m_Descriptor = new DominantColorDescriptor();

  // set bin numbers to default values
  this->setBinNumbers(bin1, bin2, bin3);
  this->spatialCoherFlag = spatial;	    // calculate spatial coherency
  this->normalizeFlag = normalize;		// do not normalize the values
}

//----------------------------------------------------------------------------
DominantColorExtractionTool::~DominantColorExtractionTool()
{
  // will be released by the clients - by mb
  //if(m_Descriptor)
  //  m_Descriptor->release();
}

//----------------------------------------------------------------------------
// set source image whose descriptor is to be computed
int DominantColorExtractionTool::setSourceImage(MomVop* img)

{
	if( img == NULL ) return -1;
	srcImage = img;
	return 0;

}

//----------------------------------------------------------------------------
DominantColorDescriptor* DominantColorExtractionTool::GetDescriptor()
{
  return m_Descriptor;
}

//----------------------------------------------------------------------------
int DominantColorExtractionTool::SetDescriptor(
DominantColorDescriptor *aDominantColorDescriptor)
{
  if(m_Descriptor==aDominantColorDescriptor)
    return 0;

  if(m_Descriptor)
    delete m_Descriptor;

  m_Descriptor = aDominantColorDescriptor;

  return 0;
}

void DominantColorExtractionTool::setBinNumbers( int bin1, int bin2, int bin3 )
{
	if( bin1 > 0 && bin1 < 256 )
		this->binNum1 = bin1;
	else
		this->binNum1 = 32;

	if( bin2 > 0 && bin2 < 256 )
		this->binNum2 = bin2;
	else
		this->binNum2 = 32;

	if( bin3 > 0 && bin3 < 256 )
		this->binNum3 = bin3;
	else
		this->binNum3 = 32;

}

//---------------------------------------------------------------------------
// varFlag: calculate variance ?
// spatFlag: calculate spatial variance ?
unsigned long DominantColorExtractionTool::extract( MomVop* img, bool normalize, bool varFlag, bool spatFlag)
{
	this->setSourceImage(img);
	return this->extract( normalize, varFlag, spatFlag );
}

//---------------------------------------------------------------------------
// varFlag: calculate variance ?
// spatFlag: calculate spatial variance ?
unsigned long DominantColorExtractionTool::extract( bool normalize, bool varFlag, bool spatFlag )
{
  MomVop* ImageMedia;
  unsigned char* RGB;
  float* LUV;
  int imsize, totsize;

  // source image
  if ( !srcImage )
	  return (unsigned long)-1;
  ImageMedia = srcImage;

  // parameters
  m_Descriptor->SetVariancePresent( varFlag );
  this->spatialCoherFlag = spatFlag;
  this->normalizeFlag = normalize;


  /* check for descriptor */
  if(!m_Descriptor)
    return (unsigned long)-1;


  /* perform extraction */
  imsize = ImageMedia->width*ImageMedia->height;
  totsize = 3*imsize;

  //RGB <==> VYU
  RGB = interleave(ImageMedia->v_chan->data->u, ImageMedia->y_chan->data->u, ImageMedia->u_chan->data->u, imsize);
  LUV = new float[totsize];
  rgb2luv2(RGB, LUV, totsize);

  unsigned char *quantImageAlpha = 0;
  if(ImageMedia->a_chan && ImageMedia->a_chan->data)
    quantImageAlpha = ImageMedia->a_chan->data->u;

  Extract( LUV, imsize, quantImageAlpha, ImageMedia );

  delete [] LUV;
  delete [] RGB;

  return 0UL;
}

//---------------------------------------------------------------------------
void DominantColorExtractionTool::Extract(float* imdata, int imsize, unsigned char* quantImageAlpha, MomVop* ImageMedia)
{
  /*----------------------------------------------------------------------
   imdata ;  color value of input image ( LUV color space, linear array), RGB image is converted to LUV above
   imsize ; total image size ( width * height)

   m_MaxSize   ; Maximun DominantColor Number (initial value DCNMAX = 8)
   m_CurrSize  ; Current(final) DominantColor Number (int)
   m_Weights   ; Percentage of DominantColor (float)
   m_Centroids ; ColorValue of DominantColor (LUV color space, float)
   m_Variances ; ColorVariance of DominantColor (float)
   iwgts       ; Percentage of DominantColor (int)
   icnts       ; ColorValue of DominantColor (RGB color space, int)
   m_Coherency ; Spatial Coherency of DominantColor proposed by LG
  ------------------------------------------------------------------------*/

  int     *closest = new int[imsize];
  double  dist, distold = FLT_MAX, distnew, eps=1.0, tmp;
  float   aglfct = DSTMIN;
  float   splfct = (float) SPLFCT;
  int     i, j, k;

  m_MaxSize = DCNMAX;
  m_CurrSize = 1;
  m_Weights = new float[m_MaxSize];
  memset( m_Weights, 0, m_MaxSize*sizeof(float) );		// added by mb

  m_Centroids = new float*[m_MaxSize];
  m_Variances = new float*[m_MaxSize];
  for(i=0; i<m_MaxSize;i++) {
    //m_Weights[i] = 0.0;
    m_Centroids[i] = new float[3];
	memset( m_Centroids[i], 0, 3*sizeof(float) );		// added by mb
    //m_Centroids[i][0] = 0.0;
    //m_Centroids[i][1] = 0.0;
    //m_Centroids[i][2] = 0.0;
    m_Variances[i] = new float[3];
	memset( m_Variances[i], 0, 3*sizeof(float) );		// added by mb
    //m_Variances[i][0] = 0.0;
    //m_Variances[i][1] = 0.0;
    //m_Variances[i][2] = 0.0;
  }

  /* loop:
     - cluster
     - calculate centroids
     - calculate distortion
     - check distortion change:
       * if change small go to next iteration
       * if change large
         - if maximum number of bins reached QUIT
         - otherwise split one bin using perturbation vector */
  i = 0;
  distnew = Cluster( closest, imdata, imsize, quantImageAlpha );
  while( eps > EPSGLOB ) {

    /* find centroids */
    Centroids( closest, imdata, imsize, quantImageAlpha );

    /* classify bins */
    distnew = Cluster( closest, imdata, imsize, quantImageAlpha );

    /* calculate total distortion */
    if( distold > 0.0 )
      eps = (distold - distnew)/distold;
    else
      eps = 0.0;
    distold = distnew;

    /* decide on splitting */
    if( i == 0 || (eps < EPSSPLT && m_CurrSize < m_MaxSize) ) {
      Split( closest, imdata, imsize, splfct, quantImageAlpha );
      distnew = Cluster( closest, imdata, imsize, quantImageAlpha );
      eps = 1.0;
    }

    /* check for identical codevectors */
    for( j=0; j<m_CurrSize; j++ ) {
      for( k=0; k<j; k++ ) {
        dist = 0.0;
        tmp = m_Centroids[j][0] - m_Centroids[k][0];
        dist += tmp*tmp;
        tmp = m_Centroids[j][1] - m_Centroids[k][1];
        dist += tmp*tmp;
        tmp = m_Centroids[j][2] - m_Centroids[k][2];
        dist += tmp*tmp;

        //if( dist == 0.0 )
        //  std::cerr << "WARNING: two identical codevectors ("
        //       << j << ", " << k << ")" << std::endl;
      }
    }

    i++;

  }


  /* merging using agglomerative clustering */
  Agglom( aglfct );

  /* calculate variances and normalize */


  distnew = Cluster( closest, imdata, imsize, quantImageAlpha );
  Centroids( closest, imdata, imsize, quantImageAlpha);
  distnew = Cluster( closest, imdata, imsize, quantImageAlpha );


  // calculate variances (in LUV color space, imdata is in LUV )?
  if( m_Descriptor->GetVariancePresent() )
	  Vars( closest, imdata, imsize, quantImageAlpha );

  // normalize weights by image size
  for( j=0; j<m_CurrSize; j++ )
    m_Weights[j] /= imsize;

  /* quantize and set descriptor members */
  int *iwgts = new int[m_CurrSize];
  memset( iwgts, 0, m_CurrSize*sizeof(int) );	// added by mb

  int **icnts = new int*[3*m_CurrSize];
  for(i=0;i<m_CurrSize;i++){
    icnts[i] = new int[3];
	memset( icnts[i], 0, 3*sizeof(int) );		// added by mb
  }

  int **ivars=new int*[3*m_CurrSize];
  for(i=0;i<m_CurrSize;i++){
    ivars[i] = new int[3];
	memset(ivars[i], 0, 3*sizeof(int) );		// added by mb
  }

  // scale weights
  for ( i = 0; i< m_CurrSize; i++) {
	if( this->normalizeFlag )	// normalize to MPEG-7 range: 0-32
	  iwgts[i] = 31.99999*m_Weights[i];		//  scale to 0-32 (5 bits) required by MPEG-7
	else
	  iwgts[i] = 99.99999*m_Weights[i];		// 0-100 percent
  }

  // convert back to RGB
  for ( i = 0; i< m_CurrSize; i++) {
    luv2rgb( icnts[i], m_Centroids[i], 3 );
  }
  for(i=0; i<m_CurrSize; i++) {
    for(int j=0; j<3; j++)
	  if( this->normalizeFlag )
		ivars[i][j] = ( m_Variances[i][j] > VARTHR) ? 1 : 0;
	  else
		ivars[i][j] = m_Variances[i][j];
  }

  /* calculate spatial coherency */
  if(this->spatialCoherFlag) {
    m_Coherency = GetSpatialCoherency( imdata, 3, m_CurrSize, m_Centroids, quantImageAlpha, ImageMedia );
  }
  else m_Coherency = 0;

  // modified by mb, 24 june 2008
  bool colspcpres = false;
  bool colquantpres = false;

  /* color space conversion & quantization */

  int component0, component1, component2;
  int bin_number0, bin_number1, bin_number2;
  int max_h;
  float *conversionmatrix;

  component0 = 0;
  component1 = 1;
  component2 = 2; // default RGB

  //in ColorSpace.h typedef enum {RGB,YCRCB,HSV,HMMD,LinearMatrix,Monochrome} ColorSpaceType;


  bin_number0 = this->binNum1;
  bin_number1 = this->binNum2;
  bin_number2 = this->binNum3; // default 5 bit


  max_h = 256;

  if(colspcpres) {	// currently false
    TPixel in[25];
    TPixel res;

    for(i=0; i<m_CurrSize; i++) {
      in[12].y.i=icnts[i][1]; // G
      in[12].u.i=icnts[i][2]; // B
      in[12].v.i=icnts[i][0]; // R

	  //in ColorSpace.h typedef enum {RGB,YCRCB,HSV,HMMD,LinearMatrix,Monochrome} ColorSpaceType;
      switch(m_Descriptor->GetColorSpaceDescriptor()->GetDescriptionColorSpace())
	{
	case RGB:
	  component0 = 0; // R
	  component1 = 1; // G
	  component2 = 2; // B
	  std::cout<<" === RGB ===" << std::endl;
	  break;
	case YCRCB:
	  RGB_To_YUV(&res,in,0,0);
	  component0 = 3; // Y
	  component1 = 4; // Cb
	  component2 = 5; // Cr
	  icnts[i][0]=res.y.i;
	  icnts[i][1]=res.u.i;
	  icnts[i][2]=res.v.i;
	  std::cout<<" === YCRCB ===" << std::endl;
	  break;
	case HSV:
	  RGB_To_HSV(&res,in,0,0);
	  component0 = 6; // H
	  component1 = 7; // S
	  component2 = 8; // V
	  icnts[i][0]=res.u.i;
	  icnts[i][1]=res.v.i;
	  icnts[i][2]=res.y.i;
	  max_h=256;
	  std::cout<<" === HSV ===" << std::endl;
	  break;
	case HMMD:
	  RGB_To_HMMD(&res,in,0,0);
	  component0 = 6; // H
	  component1 = 12; // Sum
	  component2 = 11; // Diff
	  icnts[i][0]=res.y.i;
	  icnts[i][1]=res.u.i;
	  icnts[i][2]=res.v.i;
	  max_h=360;
	  std::cout<<" === HMMD ===" << std::endl;
	  break;
	case LinearMatrix:
	  in[12].y.i=icnts[i][0]; // R
	  in[12].u.i=icnts[i][1]; // G
	  in[12].v.i=icnts[i][2]; // B
	  conversionmatrix = m_Descriptor->
	    GetColorSpaceDescriptor()->
	    GetColorSpaceMatrix();
	  LinearTransform(&res,in,0,0,conversionmatrix);
	  icnts[i][0]=res.y.i;
	  icnts[i][1]=res.u.i;
	  icnts[i][2]=res.v.i;
	  std::cout<<" === LinearMatrix ===" << std::endl;
	  break;
	default:
	  break;
	}
    }
  }

  if(colquantpres) {
    bin_number0 = m_Descriptor->GetColorQuantizerDescriptor()->
      GetBinNumberByComponent(component0);
    bin_number1 = m_Descriptor->GetColorQuantizerDescriptor()->
      GetBinNumberByComponent(component1);
    bin_number2 = m_Descriptor->GetColorQuantizerDescriptor()->
      GetBinNumberByComponent(component2);
  }

  // Normalize the values (0-255) to bin counts (e.g., 0-32)
  if( this->normalizeFlag )
	  for(i=0; i<m_CurrSize; i++) {
		icnts[i][0] = icnts[i][0]*bin_number0/max_h;	// max_h = 256 default
		icnts[i][1] = icnts[i][1]*bin_number1/256;
		icnts[i][2] = icnts[i][2]*bin_number2/256;
	  }

  m_Descriptor->SetColorSpacePresent(colspcpres);
  m_Descriptor->SetColorQuantizationPresent(colquantpres);

  //set final number of dominant colors
  m_Descriptor->SetDominantColorsNumber(m_CurrSize);

  // Set the computed dominant colors
  m_Descriptor->SetDominantColors( iwgts, icnts, ivars );
  m_Descriptor->SetSpatialCoherency(m_Coherency);

  /* free memory */
  delete [] closest;
  delete [] iwgts;

  for(i=0;i<m_CurrSize;++i)
    delete [] icnts[i];
  delete [] icnts;

  for(i=0;i<m_CurrSize;++i)
    delete [] ivars[i];
  delete [] ivars;

  delete [] m_Weights;

  for(i=0;i<m_MaxSize;++i)
    delete [] m_Centroids[i];
  delete m_Centroids;

  for(i=0;i<m_MaxSize;++i)
    delete [] m_Variances[i];
  delete m_Variances;

} /* Extract */

// added by LG SC
//----------------------------------------------------------------------------
int DominantColorExtractionTool::
GetSpatialCoherency(float *ColorData, int dim, int N,float **col_float,
		    unsigned char *quantImageAlpha, MomVop *ImageMedia)
{
  unsigned char *pAlpha;
  double CM = .0;
  int NeighborRange = 1;
  float SimColorAllow = sqrt(DSTMIN);
  bool * IVisit  = new bool [ImageMedia->width*ImageMedia->height];

  for (int x=0; x<ImageMedia->width*ImageMedia->height; x++) {
    pAlpha = &quantImageAlpha[x];
    if(!quantImageAlpha || *pAlpha) IVisit[x] = false;
    else IVisit[x] = true;
  }

  int All_Pixels = 0;
  {
    for (int x=0; x<ImageMedia->width*ImageMedia->height; x++)
      if(IVisit[x]==false) All_Pixels++;
  }
  {
    for (int i=0; i < N; i++) {
      unsigned int Corres_Pixels=0;
      double Coherency=GetCoherencyWithColorAllow(ColorData, dim, IVisit,
						  col_float[i][0],
						  col_float[i][1],
						  col_float[i][2],
						  SimColorAllow,
						  NeighborRange,
						  &Corres_Pixels,
						  ImageMedia);
      CM += Coherency * (double)Corres_Pixels/(double)(All_Pixels);
    }
  }

  delete []IVisit;

  return QuantizeSC(CM);
}

//---------------------------------------------------------------------------
double DominantColorExtractionTool::GetCoherencyWithColorAllow(float *ColorData,  int dim, bool * IVisit,
   float l, float u, float v, //not neccessary for r, g, b, would be l, u, v
   float Allow, int NeighborRange, unsigned int * OUTPUT_Corres_Pixel_Count,
   MomVop *ImageMedia)
{
	int count, i, j;
	int Neighbor_Count = 0;
	unsigned int Pixel_Count = 0;
	double Coherency = 0.0;

	int width = ImageMedia->width;
	int height = ImageMedia->height;
	int ISize = width*height*dim;

	for (count=0; count < ISize; count+=dim)
	{
		i = (count/dim) % width; //width
		j = (count/dim) / width; //height

		float l1, u1, v1;
		l1 = ColorData[count];
		u1 = ColorData[count+1];
		v1 = ColorData[count+2];

		//distance
		double distance;
		distance = sqrt(sqr(l-l1)+sqr(u-u1)+sqr(v-v1));

		if ((distance < Allow) && (IVisit[count/dim] == false))//no overlap checking
		{
			IVisit[count/dim] = true;
			Pixel_Count++;
			int nSameNeighbor = 0;
			for (int y = j - NeighborRange; y <= j + NeighborRange; y++){
				for (int x = i - NeighborRange; x <= i + NeighborRange; x++){
					if (!((i==x) && (j==y))){
						int Index = (y*width+x)*dim;
						if ((Index >= 0) && (Index < ISize)){
							float l2 = ColorData[Index];
							float u2 = ColorData[Index+1];
							float v2 = ColorData[Index+2];
							double distance=sqrt(sqr(l-l2)+sqr(u-u2)+sqr(v-v2));
							if (distance < Allow)
							{nSameNeighbor++;}
						}
					}
				}
			}
			Neighbor_Count += nSameNeighbor;
		}
	}
	*OUTPUT_Corres_Pixel_Count = Pixel_Count;
	int neighbor_check_window_size = NeighborRange*2+1;
	neighbor_check_window_size *= neighbor_check_window_size;

	if (Pixel_Count == 0)
	    Coherency = 0.0;
	else
	    Coherency = (double)Neighbor_Count / (double)Pixel_Count / (double)(neighbor_check_window_size-1);

	return Coherency;
}

//---------------------------------------------------------------------------
double DominantColorExtractionTool::Cluster( int *closest,
                                             float *imdata, int imsize,
					     unsigned char *quantImageAlpha)
{
  int     i, j, jmin;
  double  d1, d2, d3, dist, distmin, disttot=0.0;
  float  *im1, *im2, *im3;
  int	imsize_msk;
  unsigned char *pAlpha;

  /* cluster */
  imsize_msk=0;
  for( i=0, im1=imdata, im2=imdata+1, im3=imdata+2;
       i<imsize;
       i++, im1+=3, im2+=3, im3+=3 ) {
    pAlpha = &quantImageAlpha[i];
    if(!quantImageAlpha || *pAlpha) {
      jmin = 0;
      distmin = FLT_MAX;
      for( j=0; j<m_CurrSize; j++ ) {
	d1 = *im1 - m_Centroids[j][0];
	d2 = *im2 - m_Centroids[j][1];
	d3 = *im3 - m_Centroids[j][2];
	dist = d1*d1 + d2*d2 + d3*d3;
	if( dist < distmin ) {
	  jmin = j;
	  distmin = dist;
	}
      }
      closest[i] = jmin;
      disttot += distmin;
      imsize_msk++;
    }
  }

//	return disttot/imsize;
  return disttot/imsize_msk;
} /* Cluster */

//---------------------------------------------------------------------------
void DominantColorExtractionTool::
Centroids( int *closest, float *imdata, int imsize,
	   unsigned char *quantImageAlpha )
{
  int     i, j;
  double  weight;
  float  *im1, *im2, *im3;
  unsigned char *pAlpha;

  /* reset centroids */
  memset( m_Weights, 0, m_CurrSize*sizeof(float) );	  // class var: float*  m_Weights;
  for( j=0; j<m_CurrSize; j++ ) {
    memset( m_Centroids[j], 0, 3*sizeof(float) );			  // float** m_Centroids;
	//m_Weights[j] = 0.0;
    //m_Centroids[j][0] = 0.0;
    //m_Centroids[j][1] = 0.0;
    //m_Centroids[j][2] = 0.0;
  }

  /* calculate new centroids */
  for( i=0, im1=imdata, im2=imdata+1, im3=imdata+2;
       i<imsize;
       i++, im1+=3, im2+=3, im3+=3 ) {
    pAlpha = &quantImageAlpha[i];
    if(!quantImageAlpha || *pAlpha) {
      int ii = closest[i];
      m_Weights[ii]++;
      m_Centroids[ii][0] += *im1;
      m_Centroids[ii][1] += *im2;
      m_Centroids[ii][2] += *im3;
    }
  }

  for( j=0; j<m_CurrSize; j++ ) {
    weight = m_Weights[j];
    if( weight > 0.0000001 ) {
      m_Centroids[j][0] /= weight;
      m_Centroids[j][1] /= weight;
      m_Centroids[j][2] /= weight;
    }
    else {
        ;
      //std::cerr << "WARNING: zero weight for colour " << j << std::endl;
    }
  }

} /* Centroids */

//---------------------------------------------------------------------------
double DominantColorExtractionTool::Dist( int *closest, float *imdata,
					  int imsize, unsigned char *quantImageAlpha )
{
  int     i, j;
  double  d1, d2, d3, dist=0.0;
  float   *im1, *im2, *im3;
  int 	imsize_msk;
  unsigned char *pAlpha;

  imsize_msk = 0;
  for( i=0, im1=imdata, im2=imdata+1, im3=imdata+2;
       i<imsize;
       i++, im1+=3, im2+=3, im3+=3 ) {
    pAlpha = &quantImageAlpha[i];
    if(!quantImageAlpha || *pAlpha) {
      j = closest[i];
      d1 = *im1 - m_Centroids[j][0];
      d2 = *im2 - m_Centroids[j][1];
      d3 = *im3 - m_Centroids[j][2];
      dist += d1*d1 + d2*d2 + d3*d3;
      imsize_msk++;
    }
  }

//  return dist/imsize;
  return dist/imsize_msk;
} /* Dist */

//---------------------------------------------------------------------------
// Variances
void DominantColorExtractionTool::Vars( int *closest, float *imdata, int imsize, unsigned char *quantImageAlpha )
{
  int     i, j;
  double  tmp;
  unsigned char *pAlpha;

  /* reset variances */
  for(i=0; i<m_CurrSize;i++) {
    memset( m_Variances[i], 0, 3*sizeof(float) );	// float** m_Variances;
	//m_Variances[i][0] = 0.0;
    //m_Variances[i][1] = 0.0;
    //m_Variances[i][2] = 0.0;
  }

  /* estimate variances */
  for( i=0; i<imsize; i++ ) {
    pAlpha = &quantImageAlpha[i];
    if(!quantImageAlpha || *pAlpha) {
      j = closest[i];
      tmp = imdata[3*i] - m_Centroids[j][0];
      m_Variances[j][0] += tmp*tmp;
      tmp = imdata[3*i+1] - m_Centroids[j][1];
      m_Variances[j][1] += tmp*tmp;
      tmp = imdata[3*i+2] - m_Centroids[j][2];
      m_Variances[j][2] += tmp*tmp;
    }
  }

  /* normalise */
  for( j=0; j<m_CurrSize; j++ ) {
    m_Variances[j][0] /= m_Weights[j];
    m_Variances[j][1] /= m_Weights[j];
    m_Variances[j][2] /= m_Weights[j];
  }

} /* Vars */

//---------------------------------------------------------------------------
void DominantColorExtractionTool::Split( int *closest,
                                         float *imdata, int imsize,
                                         double factor,
				      	 unsigned char *quantImageAlpha )
{
  int     i, j, jmax=0;
  double  d1, d2, d3, diff1, diff2, diff3;
  double  d1s[8], d2s[8], d3s[8];  /* variance components */
  double  dists[8], distmax=0.0;   /* total distortion */
  unsigned char *pAlpha;

  /* reset distortions */
  for( j=0; j<m_CurrSize; j++ ) {
    d1s[j] = 0.0;
    d2s[j] = 0.0;
    d3s[j] = 0.0;
    dists[j] = 0.0;
  }

  /* calculate local distortions */
  for( i=0; i<imsize; i++ ) {
    pAlpha = &quantImageAlpha[i];
    if(!quantImageAlpha || *pAlpha) {
      j = closest[i];
      d1 = imdata[3*i] - m_Centroids[j][0];
      d2 = imdata[3*i+1] - m_Centroids[j][1];
      d3 = imdata[3*i+2] - m_Centroids[j][2];
      d1s[j] += d1*d1;
      d2s[j] += d2*d2;
      d3s[j] += d3*d3;
    }
  }

  /* dists are total; variances are normalised */
  for( j=0; j<m_CurrSize; j++ ) {
    dists[j] = d1s[j] + d2s[j] + d3s[j];
    d1s[j] /= m_Weights[j];
    d2s[j] /= m_Weights[j];
    d3s[j] /= m_Weights[j];
  }

  /* find cluster with highest distortion */
  for( j=0; j<m_CurrSize; j++ )
    if( dists[j] > distmax ) {
      jmax = j;
      distmax = dists[j];
    }

  /* split cluster with highest distortion;
     perturbance vector as in XM */
  diff1 = factor*sqrt(d1s[jmax]);
  diff2 = factor*sqrt(d2s[jmax]);
  diff3 = factor*sqrt(d3s[jmax]);
  m_Centroids[m_CurrSize][0] = m_Centroids[jmax][0] + diff1;
  m_Centroids[m_CurrSize][1] = m_Centroids[jmax][1] + diff2;
  m_Centroids[m_CurrSize][2] = m_Centroids[jmax][2] + diff3;
  m_Centroids[jmax][0] = m_Centroids[jmax][0] - diff1;
  m_Centroids[jmax][1] = m_Centroids[jmax][1] - diff2;
  m_Centroids[jmax][2] = m_Centroids[jmax][2] - diff3;
  m_CurrSize++;

} /* Split */

//---------------------------------------------------------------------------
void DominantColorExtractionTool::Agglom( double distthr )
{
  double  d1, d2, d3, dists[8][8], distmin=0.0;
  double  w1min, w2min;
  int     ja, jb=0, jamin, jbmin;

  /* while two closest colours are closer than DISTTHR,
     merge the closest pair */
  do {

    /* initialise distance table */
    for( ja=0; ja<m_CurrSize; ja++ )
      for( jb=0; jb<ja; jb++ ) {
	d1 = m_Centroids[ja][0] - m_Centroids[jb][0];
	d2 = m_Centroids[ja][1] - m_Centroids[jb][1];
	d3 = m_Centroids[ja][2] - m_Centroids[jb][2];
	dists[ja][jb] = d1*d1 + d2*d2 + d3*d3;
      }

    /* find two closest colours */
    distmin = FLT_MAX;
    jamin = 0;
    jbmin = 0;
    for( ja=0; ja<m_CurrSize; ja++ )
      for( jb=0; jb<ja; jb++ )
	if( dists[ja][jb] < distmin ) {
	  distmin = dists[ja][jb];
	  jamin = ja;
	  jbmin = jb;
	}

    if( distmin > distthr )
      break;

    /* merge two closest colours */
    w1min = m_Weights[jamin];
    w2min = m_Weights[jbmin];
    m_Centroids[jbmin][0] = (w1min*m_Centroids[jamin][0] +
			     w2min*m_Centroids[jbmin][0])/(w1min+w2min);
    m_Centroids[jbmin][1] = (w1min*m_Centroids[jamin][1] +
			     w2min*m_Centroids[jbmin][1])/(w1min+w2min);
    m_Centroids[jbmin][2] = (w1min*m_Centroids[jamin][2] +
			     w2min*m_Centroids[jbmin][2])/(w1min+w2min);
    m_Weights[jbmin] += w1min;
    m_CurrSize--;

    /* remove jamin */
    for( jb=jamin; jb<m_CurrSize; jb++ ) {
      m_Weights[jb] = m_Weights[jb+1];
      m_Centroids[jb][0] = m_Centroids[jb+1][0];
      m_Centroids[jb][1] = m_Centroids[jb+1][1];
      m_Centroids[jb][2] = m_Centroids[jb+1][2];
      m_Variances[jb][0] = m_Variances[jb+1][0];
      m_Variances[jb][1] = m_Variances[jb+1][1];
      m_Variances[jb][2] = m_Variances[jb+1][2];
    }

  } while( m_CurrSize > 1 && distmin < distthr );

} /* Agglom */


//---------------------------------------------------------------------------
unsigned char* interleave (unsigned char* pixel_R,
			   unsigned char* pixel_G,
			   unsigned char* pixel_B,int size)
{
  int i;
  unsigned char *pixelarray=new unsigned char [3*size];

  for(i=0;i<size;++i) {
    pixelarray[3*i]=pixel_R[i];
    pixelarray[3*i+1]=pixel_G[i];
    pixelarray[3*i+2]=pixel_B[i];
  }

  return pixelarray;

}

//---------------------------------------------------------------------------
int DominantColorExtractionTool::QuantizeSC( double sc )
{
  if(sc < 0.70)
    return 1;
  else
    return (int)( (sc - 0.70)/(1.0 - 0.70)*(pow(2.0,(double)SC_BIT)-3.0)+.5)+2;
}
//---------------------------------------------------------------------------
