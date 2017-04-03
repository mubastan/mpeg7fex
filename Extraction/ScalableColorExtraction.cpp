//////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// TU-Munich, S.Herrmann
// Heinrich-Hertz-Institute (HHI), Bela Makai, Karsten Mueller
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
// purpose, assign or donate the code to a third party and inhibit third parties
// from using the code for non MPEG-7 conforming products.
//
// Copyright (c) 1998-1999.
//
// This notice must be included in all copies or derivative works.
//
// Applicable File Name:  ScalableColorExtraction.cpp
//

#include <cmath>
#include <cstdlib>

#include "AddressLib/vopio.h"
#include "AddressLib/intra.h"

#include "Descriptors/ScalableColor.h"
#include "ScalableColorExtraction.h"
#include "ColorSpaceExtraction.h"		// for color space conversion (RGB_To_HSV)


//sth use delimiters between functions
void histo_3d_hirarch_5( int tabelle[5][255], int tablae, int* histogram, int h_size, int s_size, int v_size, int hist_nr)
{
	int i,j,sum,dif,x1,y1,x2,y2;
	int matrix[16][16];
	int iprint=0;

	for(i=0;i<h_size*s_size*v_size;++i)
	   matrix[i%(h_size)][i/(h_size)]=histogram[i];

	if(iprint==1)
	  {
	  for(i=0;i<h_size;++i)
		{
		for(j=0;j<s_size*v_size;++j)
		  {
		//printf("%4d", matrix[i][j]); //sth don´t write to stdout
		  }
		//printf("\n");
		}
	  }
	//printf("---------------------------------------\n");

	for(i=0;i<tablae;++i)
	  {
	  y1=tabelle[0][i];
	  x1=tabelle[1][i];
	  y2=tabelle[2][i];
	  x2=tabelle[3][i];
	  sum=matrix[y1][x1]+matrix[y2][x2];
	  dif=matrix[y2][x2]-matrix[y1][x1];

	//if(iprint==1)
	  //sth printf("%5d %5d %5d %5d %5d %5d %5d\n",
	  //i,y1,x1,y2,x2,sum,dif);

	  matrix[y1][x1]=sum;
	  matrix[y2][x2]=dif;
	  }

	if(iprint==1)
	{
		//sth printf("reduzierte matrix\n");
	  for(i=0;i<h_size;++i)
		{
		for(j=0;j<s_size*v_size;++j)
		  {
		//sth printf("%4d", matrix[i][j]);
		  }
		//sth printf("\n");
		}
	}


	for(i=0;i<h_size*s_size*v_size;++i)
	  histogram[i]= matrix[i%(h_size)][i/(h_size)];

}

//----------------------------------------------------------------------------
void histo_3d_hirarch_16_5(int tabelle[5][255],int tablae,int *histogram,int h_size,int s_size,int v_size,int hist_nr)
{
	int i,j,sum,dif,x1,y1,x2,y2;
	int matrix[16][16];
	int iprint=0;

	for(i=0;i<h_size*s_size*v_size;++i)
	   matrix[i%(h_size)][i/(h_size)]=histogram[i];
	if(iprint==1)
	  {
	  for(i=0;i<h_size;++i)
		{
		for(j=0;j<s_size*v_size;++j)
		  {
		  //sth printf("%4d", matrix[i][j]);
		  }
		 //sth  printf("\n");
		}
	  }

	for(i=0;i<tablae;++i)
	  {
	  if(tabelle[4][i]<=8) continue;
	  y1=tabelle[0][i];
	  x1=tabelle[1][i];
	  y2=tabelle[2][i];
	  x2=tabelle[3][i];
	  sum=matrix[y1][x1]+matrix[y2][x2];
	  dif=matrix[y2][x2]-matrix[y1][x1];

	//if(iprint==1)
	//sth printf("%5d %5d %5d %5d %5d %5d %5d\n",
	  //i,y1,x1,y2,x2,sum,dif);

	  matrix[y1][x1]=sum;
	  matrix[y2][x2]=dif;
	  }

	if(iprint==1)
	  {
	  //sth printf("reduzierte matrix\n");
	  for(i=0;i<h_size;++i)
		{
		for(j=0;j<s_size*v_size;++j)
		  {
		  //sth printf("%4d", matrix[i][j]);
		  }
		  //sth printf("\n");
		}
	}


	for(i=0;i<h_size*s_size*v_size;++i)
	  histogram[i]= matrix[i%(h_size)][i/(h_size)];

}

//----------------------------------------------------------------------------
void red_bits_pro_bin_5(int* histogram, int NumberOfBitplanesDiscarded, int ivert)
{
	int i,j,wert,wert1,bits_pro_bin,bits_pro_bild;
	int max_bits_pro_bin,anzkof;
	if(NumberOfBitplanesDiscarded==0) return;

	bits_pro_bild=0;
	max_bits_pro_bin=0;
	anzkof=0;
	if(NumberOfBitplanesDiscarded>0)
	   {
	   for(i=0;i<256;++i)
		 {
		 bits_pro_bin=XM::scalableColorQuantValues[i][1]-NumberOfBitplanesDiscarded;
		 if(bits_pro_bin<2)
		   {
		   wert=histogram[i];
		   if(wert>=0) histogram[i]=1;
		   if(wert<0 ) histogram[i]=0;
		   bits_pro_bild=bits_pro_bild+1;
		   }
		 if(bits_pro_bin>=2)
		   {
		   wert=histogram[i];
		   wert1=wert;
		   if(wert<0) wert=-wert;
		   bits_pro_bild=bits_pro_bild+bits_pro_bin;
		   if(bits_pro_bin>max_bits_pro_bin) max_bits_pro_bin=bits_pro_bin;
		   anzkof=anzkof+1;

		   for(j=0;j<NumberOfBitplanesDiscarded;++j)
			 wert=wert>>1;

		   //if((wert==0)&&(wert1>=0)) histogram[i]=1;  //JH: separation btw sign and value?
		   //if((wert==0)&&(wert1<0)) histogram[i]=0;   //JH: separation btw sign and value?
		   if(wert1<0) wert=-wert;
		   histogram[i]=wert;

		  }
	   }
	  }

	/*if(ivert==0)
	printf("+++maxbit:%5d anzbit:%5d anzkof:%5d\n",
	max_bits_pro_bin,bits_pro_bild,anzkof);*/
}

//----------------------------------------------------------------------------
void hsv_hir_quant_lin_5( int* histogram )
{
	int i,wert,maxwert;
	for(i=0;i<256;++i)
	  {
	  maxwert=XM::scalableColorQuantValues[i][2];
	  wert=histogram[i]-XM::scalableColorQuantValues[i][0];
	//printf("i:%5d wert:%5d maxwert:%5d\n",i,wert,maxwert);
	  if(wert>maxwert) wert=maxwert;
	  if(wert< -maxwert) wert=-maxwert;
	  histogram[i]=wert;
	  }
}

//----------------------------------------------------------------------------
void get_sort_differences_5( int tabelle[5][255], int tablae, int *histogram, int h_size, int s_size, int v_size, int hist_nr )
{
	int i,x2,y2;
	int matrix[16][16],sorttab[256],index,ind;
	//int iprint=0;

	for(i=0;i<h_size*s_size*v_size;++i)
	   matrix[i%(h_size)][i/(h_size)]=histogram[i];

	index=1;
	histogram[0]=matrix[0][0];
	  //sth printf("===index:%5d,wert:%5d\n",0,histogram[0]);
	for(i=tablae-1;i>=0;--i)
	  {
	  y2=tabelle[2][i];
	  x2=tabelle[3][i];
	  histogram[index]=matrix[y2][x2];
	  //sth printf("===index:%5d,wert:%5d x:%5d y:%5d ind:%5d\n",
	  //  index,histogram[index],x2,y2,16*x2+y2);
	  sorttab[index]=16*x2+y2;
	  index=index+1;
	  }
	index=0;
	sorttab[0]=0;
	  //sth printf("sortierungstabelle\n");
	  for(i=0;i<256;++i)
		{
		ind=sorttab[i];
		//sth printf("%4d,", sorttab[i]);
		index=index+1;
		if(index>15) {index=0; /*printf("\n")*/;}
	  }


}

//----------------------------------------------------------------------------
using namespace XM;

void  QuantScalableUniform1(TPixel *res, TPixel *in,TCoor x, TCoor y)
{
	/* _intra_infopt   pointer to quantizer descriptor*/
	/* quantized values in channel,  bin_no in ax_chan*/
	int LQUANT_Y=4;
	int LQUANT_U=16;
	int LQUANT_V=4;
	//printf("in QuantScalableUniform\n");
  res->a=in[12].a; /*position 12 is central position of 5x5 2D input matrix*/
  res->y.i=(in[12].y.i*LQUANT_Y)/256;
  res->u.i=(in[12].u.i*LQUANT_U)/256;
  res->v.i=(in[12].v.i*LQUANT_V)/256;
  //res->ax.i=((res->y.i*LQUANT_U)+res->u.i)*LQUANT_V+res->v.i;
  res->ax.i=res->y.i*LQUANT_V*LQUANT_U+res->v.i*LQUANT_U+res->u.i; //geaendert/makai
  //printf("farbwert:%5d %5d %5d %5d\n",
  //res->ax.i,in[12].y,in[12].u,in[12].v);
}

//----------------------------------------------------------------------------
static int initextractionwascalled=0;

void (*scalhist_colorspaceconversion)(TPixel*,TPixel*,TCoor,TCoor);
void (*scalhist_colorquantization)(TPixel*,TPixel*,TCoor,TCoor);

//----------------------------------------------------------------------------
void  pixeltohistogram1(TPixel *res, TPixel *in,TCoor x, TCoor y)
{

//	_intra_infopt   pointer to histogram descriptor
//	_intra_regin1   histogramsize
//	_intra_regout1  maximum value = normalization factor
//	-intra_regout2  total number of pixels


  long qhistval;

  res->a=in[12].a; //position 12 is central position of 5x5 2D input matrix
  res->y=in[12].y;
  res->u=in[12].u;
  res->v=in[12].v;
  res->ax=in[12].ax;

  /* skip transparent pixels*/
  if (!in[12].a.i) return;

  /* compute index in histogram*/
  qhistval=((XM::ScalableColorDescriptor*)_intra_infopt)->GetCoefficient(in[12].ax.i);


  /* increment histogram element*/
  qhistval++;

  /* check for maximum*/
  if (qhistval > _intra_regout1) _intra_regout1=qhistval;

  /* increment pixel counter*/
  _intra_regout2++;
	//printf("qwert:%5d\n",qhistval);
  /* write back histogram result*/
  ((XM::ScalableColorDescriptor*)_intra_infopt)->
    SetCoefficient(in[12].ax.i,qhistval);
}

//----------------------------------------------------------------------------
void  pixeltocolorhist1(TPixel *res, TPixel *in,TCoor x, TCoor y)
{
/*	_intra_infopt   pointer to color histogram descriptor
	_intra_regin1   histogramsize
	_intra_regout1  maximum value (= normalization factor)
	_intra_regout2  total value = normalization factor*/

  int index;
  static TPixel intermediate[25];

  //void (*_intra_infopt0);
//printf("in pixeltocolorhist\n");
  res->a=in[12].a; /*position 12 is central position of 5x5 2D input matrix*/
  res->y=in[12].y;
  res->u=in[12].u;
  res->v=in[12].v;
  res->ax=in[12].ax;

  // perform color space conversion
  //printf("vor colorSpace\n");
  //(*scalhist_colorspaceconversion)(res,in,x,y); // commented out by mb - 26.06.2008
	RGB_To_HSV( res, in, x, y );		// added by mb - 26.06.2008

  //forward results to quantization
  intermediate[12].a=res->a;
  intermediate[12].y=res->y;
  intermediate[12].u=res->u;
  intermediate[12].v=res->v;
  intermediate[12].ax=res->ax;

  // perform quantization

  //(*scalhist_colorquantization)(res,intermediate,x,y);	// commented out by mb - 26.06.2008
  QuantScalableUniform1(res,intermediate,x,y);				// added by mb - 26.06.2008

  /* forward results to quantization*/
  intermediate[12].a=res->a;
  intermediate[12].y=res->y;
  intermediate[12].u=res->u;
  intermediate[12].v=res->v;
  intermediate[12].ax=res->ax;


  /* compute index in histogram*/
    pixeltohistogram1(res,intermediate,x,y);
  //printf("_intra_infopt:%5d _intra_infopt0:%5d _intra_regin1:%5d\n",
	  //_intra_infopt,_intra_infopt0,_intra_regin1 );


	index=(int)res->ax.i;
	//printf("++++%5d %5d\n",res->ax,index);

}


//=============================================================================

ScalableColorExtractionTool::ScalableColorExtractionTool( int numOfCoeff, int numBitPlanesDiscarded ):
m_Descriptor(0),
srcImage(0)
{

	ScalableColorDescriptor* descriptor = new ScalableColorDescriptor();
	descriptor->SetNumberOfCoefficients( numOfCoeff );
	descriptor->SetNumberOfBitplanesDiscarded( numBitPlanesDiscarded);

	SetDescriptor( descriptor );

}

//----------------------------------------------------------------------------
ScalableColorExtractionTool::~ScalableColorExtractionTool()
{
	/* release descriptor*/
	// should be released by the clients - ( mb - 17.07.2008)
	//if (m_Descriptor)
	//  m_Descriptor->release();
}


//----------------------------------------------------------------------------
int ScalableColorExtractionTool::SetSourceMedia(MomVop* img)
{
  if (!img) return -1;

  srcImage = img;

  return 0;
}

//----------------------------------------------------------------------------
ScalableColorDescriptor* ScalableColorExtractionTool::GetDescriptor(void)
{
        return m_Descriptor;
}

//----------------------------------------------------------------------------
int ScalableColorExtractionTool::SetDescriptor(ScalableColorDescriptor* aScalableColorDescriptor)

{
    /* check if new value is different from old value*/
    if (m_Descriptor == aScalableColorDescriptor)
			return 0;
	if (m_Descriptor)
	  delete m_Descriptor;
	m_Descriptor = aScalableColorDescriptor;

	return 0;
}

//----------------------------------------------------------------------------
unsigned long ScalableColorExtractionTool::extract(MomVop* img)
{
	if(!img)
		return 0;

	this->SetSourceMedia( img );

	unsigned long  NumberOfCoefficients = m_Descriptor->GetNumberOfCoefficients();


	/* reset all histogram values*/
#ifdef VERBOSE
	fprintf(stderr,"histogram init: %p (%ld)\n",
		m_Descriptor->GetScalableHistogram(),NumberOfCoefficients);
#endif

	for (unsigned long i = 0; i < NumberOfCoefficients; i++ ) {
	  m_Descriptor->SetCoefficient( i, 0 );
	}
	initextractionwascalled=1;

  if (!ComputeNormalizedHistogram()) return (unsigned long)-1;
  //std::cout<<"SCD - after ComputeNormalizedHistogram" << std::endl;
  //m_Descriptor->Print();
  if (!QuantizeHistogram(0)) return (unsigned long)-1;
  //std::cout<<"SCD - after QuantizeHistogram" << std::endl;
  //m_Descriptor->Print();
  if (!HaarTransform(0)) return (unsigned long)-1;

  return 0;
}



//----------------------------------------------------------------------------
unsigned long* ScalableColorExtractionTool::ComputeNormalizedHistogram(void)

{
	unsigned long NumberOfCoefficients;
	unsigned long Total;

    /* image data*/
    MomVop* theVop;

	/* check for descriptor*/
    if (!m_Descriptor) return 0;


	NumberOfCoefficients = m_Descriptor->GetNumberOfCoefficients();

	/* check for media*/
	if (!srcImage) return 0;
	theVop = srcImage;


	//scalhist_colorspaceconversion=&(RGB_To_HSV);
	//scalhist_colorquantization=&(QuantScalableUniform1);


	/* call extraction macro*/
	{

	_intra_infopt = (void *)m_Descriptor;	// defined in addresslib\intra.h(91)  as global variable
	_intra_regout1=0;
	_intra_regout2=0;
	// function in addresslib\intra.h
	intra_proc(0,theVop,0,
	           0,A_CH|Y_CH|U_CH|V_CH,CON_0,SCAN_HO,
	           255,0,0,0,0,
	           -1,-1,0,0,0,0,
	           size1to1,pixeltocolorhist1);
//	  Norm=_intra_regout1;
	Total=_intra_regout2;

	}


/*****************************************************************/
/* Normaization of bins*/

	{
	  int factor=0,ibinwert;
	  unsigned long i;
	  double binwert;

	  factor=0x7ff; //NoBitsProBin=11


	  //quantisierung der bins
	  for (i=0; i<NumberOfCoefficients; i++) {
	    binwert=(double)(factor)*(double)(m_Descriptor->GetCoefficient(i))/(double)Total;


	    ibinwert=(int)(binwert+0.49999);
	    if(ibinwert>factor) ibinwert=factor;//obsolete

	    m_Descriptor->SetCoefficient(i,ibinwert);
	  }
	}
	return m_Descriptor->GetScalableHistogram();
}

//----------------------------------------------------------------------------
unsigned long *ScalableColorExtractionTool::QuantizeHistogram(unsigned long *aHist)
{
   unsigned long NumberOfCoefficients;
   int factor=15,iwert;
   unsigned long i;
   double wert,potenz=0.4;
   double arg,maxwert;

   NumberOfCoefficients=m_Descriptor->GetNumberOfCoefficients();

   //maxwert=(double)40*(double)1048575/(double)100;
   maxwert=(double)40*(double)2047/(double)100;
    //maxwert=(double)40*(double)4095/(double)100;

   if (!aHist) aHist=m_Descriptor->GetScalableHistogram();
   for (i=0; i<NumberOfCoefficients; i++) {
     wert=(double)(*aHist);
     aHist++;

     if(wert>maxwert) iwert=(int)factor;

     if(wert<=maxwert) {
       arg=wert/maxwert;
       wert=(float)factor*pow(arg,potenz);
       iwert=(int)(wert+0.5);
     }

     if(iwert>factor) iwert=factor;

     m_Descriptor->SetCoefficient(i,iwert);
   }
   return m_Descriptor->GetScalableHistogram();
}


//----------------------------------------------------------------------------
unsigned long *ScalableColorExtractionTool::HaarTransform(unsigned long *aHist)
/*****************************************************************/
/* //sth I can read this, who else? Berechnung der Haar-Koeffizienten */
{
  unsigned long NumberOfCoefficients;
  int i,j,index,tablae=255,iwert,NumberOfBitplanesDiscarded,hist_nr;
  int *histogram_in,*histogram_out;
  int RecHistogram=0;
  int h_size,s_size,v_size,max_color=256;


  int tabelle[5][255]={
    {0,2,4,6,8,10,12,14,0,2,4,6,8,10,12,14,0,2,4,6,8,10,
     12,14,0,2,4,6,8,10,12,14,0,2,4,6,8,10,12,14,0,2,4,6,
     8,10,12,14,0,2,4,6,8,10,12,14,0,2,4,6,8,10,12,14,0,2,
     4,6,8,10,12,14,0,2,4,6,8,10,12,14,0,2,4,6,8,10,12,14,
     0,2,4,6,8,10,12,14,0,2,4,6,8,10,12,14,0,2,4,6,8,10,
     12,14,0,2,4,6,8,10,12,14,0,2,4,6,8,10,12,14,0,2,4,6,
     8,10,12,14,0,2,4,6,8,10,12,14,0,2,4,6,8,10,12,14,0,2,
     4,6,8,10,12,14,0,2,4,6,8,10,12,14,0,2,4,6,8,10,12,14,
     0,2,4,6,8,10,12,14,0,2,4,6,8,10,12,14,0,2,4,6,8,10,
     12,14,0,2,4,6,8,10,12,14,0,2,4,6,8,10,12,14,0,2,4,6,
     8,10,12,14,0,4,8,12,0,4,8,12,0,4,8,12,0,4,8,12,0,4,
     8,12,0,4,8,12,0,4,8,12,0,8,0},

    {0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,2,2,2,2,2,2,
     2,2,3,3,3,3,3,3,3,3,4,4,4,4,4,4,4,4,5,5,5,5,
     5,5,5,5,6,6,6,6,6,6,6,6,7,7,7,7,7,7,7,7,8,8,
     8,8,8,8,8,8,9,9,9,9,9,9,9,9,10,10,10,10,10,10,10,10,
     11,11,11,11,11,11,11,11,12,12,12,12,12,12,12,12,13,13,13,13,13,13,
     13,13,14,14,14,14,14,14,14,14,15,15,15,15,15,15,15,15,0,0,0,0,
     0,0,0,0,2,2,2,2,2,2,2,2,4,4,4,4,4,4,4,4,6,6,
     6,6,6,6,6,6,8,8,8,8,8,8,8,8,10,10,10,10,10,10,10,10,
     12,12,12,12,12,12,12,12,14,14,14,14,14,14,14,14,0,0,0,0,0,0,
     0,0,2,2,2,2,2,2,2,2,8,8,8,8,8,8,8,8,10,10,10,10,
     10,10,10,10,0,0,0,0,2,2,2,2,8,8,8,8,10,10,10,10,0,0,
     0,0,8,8,8,8,0,0,0,0,0,0,0},

    {1,3,5,7,9,11,13,15,1,3,5,7,9,11,13,15,1,3,5,7,9,11,
     13,15,1,3,5,7,9,11,13,15,1,3,5,7,9,11,13,15,1,3,5,7,
     9,11,13,15,1,3,5,7,9,11,13,15,1,3,5,7,9,11,13,15,1,3,
     5,7,9,11,13,15,1,3,5,7,9,11,13,15,1,3,5,7,9,11,13,15,
     1,3,5,7,9,11,13,15,1,3,5,7,9,11,13,15,1,3,5,7,9,11,
     13,15,1,3,5,7,9,11,13,15,1,3,5,7,9,11,13,15,0,2,4,6,
     8,10,12,14,0,2,4,6,8,10,12,14,0,2,4,6,8,10,12,14,0,2,
     4,6,8,10,12,14,0,2,4,6,8,10,12,14,0,2,4,6,8,10,12,14,
     0,2,4,6,8,10,12,14,0,2,4,6,8,10,12,14,0,2,4,6,8,10,
     12,14,0,2,4,6,8,10,12,14,0,2,4,6,8,10,12,14,0,2,4,6,
     8,10,12,14,2,6,10,14,2,6,10,14,2,6,10,14,2,6,10,14,0,4,
     8,12,0,4,8,12,0,4,8,12,4,12,8},

    {0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,2,2,2,2,2,2,
     2,2,3,3,3,3,3,3,3,3,4,4,4,4,4,4,4,4,5,5,5,5,
     5,5,5,5,6,6,6,6,6,6,6,6,7,7,7,7,7,7,7,7,8,8,
     8,8,8,8,8,8,9,9,9,9,9,9,9,9,10,10,10,10,10,10,10,10,
     11,11,11,11,11,11,11,11,12,12,12,12,12,12,12,12,13,13,13,13,13,13,
     13,13,14,14,14,14,14,14,14,14,15,15,15,15,15,15,15,15,1,1,1,1,
     1,1,1,1,3,3,3,3,3,3,3,3,5,5,5,5,5,5,5,5,7,7,
     7,7,7,7,7,7,9,9,9,9,9,9,9,9,11,11,11,11,11,11,11,11,
     13,13,13,13,13,13,13,13,15,15,15,15,15,15,15,15,4,4,4,4,4,4,
     4,4,6,6,6,6,6,6,6,6,12,12,12,12,12,12,12,12,14,14,14,14,
     14,14,14,14,0,0,0,0,2,2,2,2,8,8,8,8,10,10,10,10,2,2,
     2,2,10,10,10,10,8,8,8,8,0,0,0},

    {128,128,128,128,128,128,128,128,128,128,128,
     128,128,128,128,128,128,128,128,128,128,128,
     128,128,128,128,128,128,128,128,128,128,128,
     128,128,128,128,128,128,128,128,128,128,128,
     128,128,128,128,128,128,128,128,128,128,128,
     128,128,128,128,128,128,128,128,128,128,128,
     128,128,128,128,128,128,128,128,128,128,128,
     128,128,128,128,128,128,128,128,128,128,128,
     128,128,128,128,128,128,128,128,128,128,128,
     128,128,128,128,128,128,128,128,128,128,128,
     128,128,128,128,128,128,128,128,128,128,128,
     128,128,128,128,128,128,128,64,64,64,64,
     64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,
     64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,
     64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,32,32,32,32,32,32,
     32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,
     32,32,32,32,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,8,8,
     8,8,8,8,8,8,4,4,4,4,2,2,1}
  };

  int sorttab[256]= {
   0, 4, 8, 12, 32, 36, 40, 44, 128, 132, 136, 140, 160, 164, 168, 172,
	2, 6, 10, 14, 34, 38, 42, 46, 130, 134, 138, 142, 162, 166, 170, 174,
	64, 66, 68, 70, 72, 74, 76, 78, 96, 98, 100, 102, 104, 106, 108, 110, 192, 194, 196, 198, 200, 202, 204, 206, 224, 226, 228, 230, 232, 234, 236, 238,
	16, 18, 20, 22, 24, 26, 28, 30, 48, 50, 52, 54, 56, 58, 60, 62, 80, 82, 84, 86, 88, 90, 92, 94, 112, 114, 116, 118, 120, 122, 124, 126, 144, 146, 148, 150, 152, 154, 156, 158, 176, 178, 180, 182, 184, 186, 188, 190, 208, 210, 212, 214, 216, 218, 220, 222, 240, 242, 244, 246, 248, 250, 252, 254,
	1, 3, 5, 7, 9, 11, 13, 15, 17, 19, 21, 23, 25, 27, 29, 31, 33, 35, 37, 39, 41, 43, 45, 47, 49, 51, 53, 55, 57, 59, 61, 63, 65, 67, 69, 71, 73, 75, 77, 79, 81, 83, 85, 87, 89, 91, 93, 95, 97, 99, 101, 103, 105, 107, 109, 111, 113, 115, 117, 119, 121, 123, 125, 127, 129, 131, 133, 135, 137, 139, 141, 143, 145, 147, 149, 151, 153, 155, 157, 159, 161, 163, 165, 167, 169, 171, 173, 175, 177, 179, 181, 183, 185, 187, 189, 191, 193, 195, 197, 199, 201, 203, 205, 207, 209, 211, 213, 215, 217, 219, 221, 223, 225, 227, 229, 231, 233, 235, 237, 239, 241, 243, 245, 247, 249, 251, 253, 255};


  NumberOfCoefficients = m_Descriptor->GetNumberOfCoefficients();

  // HSV 16x4x4
  h_size = 16;
  s_size = 4;
  v_size = 4;
  hist_nr = 256;
  NumberOfBitplanesDiscarded = m_Descriptor->GetNumberOfBitplanesDiscarded();

  RecHistogram = 0;

  histogram_in=(int *)calloc(max_color, sizeof(int));
  histogram_out=(int *)calloc(max_color, sizeof(int));

  if (!aHist) aHist=m_Descriptor->GetScalableHistogram();

  for (i=0; i<NumberOfCoefficients; i++) {
    histogram_in[i]=*aHist;
    aHist++;
  }

  if(RecHistogram==2) {
    histo_3d_hirarch_16_5(tabelle,tablae,histogram_in, h_size,s_size,v_size,hist_nr);
    hsv_hir_quant_lin_5(histogram_in);
  }

  if(RecHistogram!=2) {
    //printf("vor histo_3d\n");
    histo_3d_hirarch_5(tabelle,tablae,histogram_in, h_size,s_size,v_size,hist_nr);
    //printf("nach histo_3d_hirarch0\n");

    for(j=0;j<256;++j) {
      index=sorttab[j];
      histogram_out[j]=histogram_in[index];
      //printf("j:%5d hist:%5d\n",j,histogram_in[j]);
    }

    hsv_hir_quant_lin_5(histogram_out);

    //set sign before coefficients might be reduced to 0!
    for (i=0; i<hist_nr; i++) {
      m_Descriptor->SetCoeffSign(i,histogram_out[i]);
    }

    red_bits_pro_bin_5(histogram_out,NumberOfBitplanesDiscarded,0);

  }



  for (i=0; i<hist_nr; i++) {
    iwert=histogram_out[i];
    m_Descriptor->SetCoeffValue(i,iwert);
  }


  free(histogram_in);
  free(histogram_out);

  return m_Descriptor->GetScalableHistogram();
}

