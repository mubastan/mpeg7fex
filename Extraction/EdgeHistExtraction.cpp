//////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// ETRI / Dongguk University, Seoul
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
// Applicable File Name:  EdgeHistExtraction.cpp
//

#include <cmath>
#include <cstring>

#include "Extraction/EdgeHistExtraction.h"

#define	NoEdge				0
#define	vertical_edge			1
#define	horizontal_edge			2
#define	non_directional_edge		3
#define	diagonal_45_degree_edge		4
#define	diagonal_135_degree_edge	5


//=============================================================================


using namespace XM;


//=============================================================================
EdgeHistogramExtractionTool::EdgeHistogramExtractionTool():
m_Descriptor(0)
{
    m_Descriptor = new EdgeHistogramDescriptor();
}

//----------------------------------------------------------------------------
EdgeHistogramExtractionTool::~EdgeHistogramExtractionTool()
{
  // release descriptor
  // commented out - mb - 07.07.2008
  //if (m_Descriptor)
  //  delete m_Descriptor;
}



//----------------------------------------------------------------------------
EdgeHistogramDescriptor* EdgeHistogramExtractionTool::GetDescriptor()

{
        return m_Descriptor;
}


//----------------------------------------------------------------------------
int EdgeHistogramExtractionTool::SetDescriptor(EdgeHistogramDescriptor
		       *aEdgeHistogramDescriptor)

{
  if (m_Descriptor == aEdgeHistogramDescriptor) return 0;

  if (m_Descriptor) delete m_Descriptor;

  m_Descriptor = aEdgeHistogramDescriptor;

  return 0;
}

//----------------------------------------------------------------------------
// alpha channel of image should contain the object mask (0: background, nonzero foreground)
// to extract the descriptor for arbitrary shaped regions
unsigned long EdgeHistogramExtractionTool::extract(MomVop *img)
{
  // select media type you require and remove comment
  MomVop *ImageMedia;

  // First check that it's all correctly initialised
  if (m_Descriptor==NULL) return (unsigned long)-1;

  //if(strcmp(m_Descriptor->GetName(),
  //	    "EdgeHistogramDescriptionInterface") != 0) return (unsigned long)-1;

  // Source image
  ImageMedia = img;
  if (ImageMedia == NULL)
	  return (unsigned long)-1;


	unsigned long	desired_num_of_blocks;
	unsigned long	block_size;
	int		Te_Value;
	EHD		*pLocal_Edge;
	pLocal_Edge = NULL;
	pLocal_Edge = new	EHD[1];

	Te_Value = Te_Define;
	desired_num_of_blocks = Desired_Num_of_Blocks;

// Making Gray Image
	int i, j, xsize, ysize;
	unsigned char	*pGrayImage;
// arbitrary shape (Modified by Dongguk)
	unsigned char *pResampleImage=NULL;
	int max_x = 0, max_y = 0, min_x = ImageMedia->width-1, min_y = ImageMedia->height-1;
	double scale, EWweight, NSweight, EWtop, EWbottom;
	unsigned char NW, NE, SW, SE;
	int min_size, re_xsize, re_ysize;
	if(ImageMedia->a_chan){
		for(j=0; j<ImageMedia->height; j++){
			for(i=0; i<ImageMedia->width; i++){
				if(ImageMedia->a_chan->data->u[j*ImageMedia->width+i]){
					if(max_x<i) max_x = i;
					if(max_y<j) max_y = j;
					if(min_x>i) min_x = i;
					if(min_y>j) min_y = j;
				}
			}
		}
		xsize = max_x - min_x + 1;
		ysize = max_y - min_y + 1;
	}
	else{
		xsize = ImageMedia->width;
		ysize = ImageMedia->height;
		min_x = min_y = 0;
	}
	pGrayImage = new	unsigned char[xsize*ysize];
	for( j=0; j < ysize; j++){
		for( i=0; i < xsize; i++) {
			if(ImageMedia->a_chan){
				if(ImageMedia->a_chan->data->u[(j+min_y)*ImageMedia->width+(i+min_x)]){
					pGrayImage[j*xsize+i]=(unsigned char)((float)(ImageMedia->y_chan->data->u[(j+min_y)*ImageMedia->width+(i+min_x)]
						+ImageMedia->u_chan->data->u[(j+min_y)*ImageMedia->width+(i+min_x)]
						+ImageMedia->v_chan->data->u[(j+min_y)*ImageMedia->width+(i+min_x)])/3.0f);
				}
				else{
					pGrayImage[j*xsize+i] = 0;
				}
			}
			else{
				pGrayImage[j*xsize+i]=(unsigned char)((float)(ImageMedia->y_chan->data->u[j*xsize+i]
					+ImageMedia->u_chan->data->u[j*xsize+i]
					+ImageMedia->v_chan->data->u[j*xsize+i])/3.0f);
			}
		}
	}
	min_size = (xsize>ysize)? ysize: xsize;
	if(min_size<70){
		//upsampling
		scale = 70.0/min_size;
		re_xsize = (int)(xsize*scale+0.5);
		re_ysize = (int)(ysize*scale+0.5);
		pResampleImage = new unsigned char[re_xsize*re_ysize];
		for(j=0; j<re_ysize; j++)for(i=0; i<re_xsize; i++){
			EWweight = i/scale-floor(i/scale);
			NSweight = j/scale-floor(j/scale);
			NW = pGrayImage[(int)floor(i/scale)+(int)floor(j/scale)*xsize];
			NE = pGrayImage[(int)floor(i/scale)+1+(int)floor(j/scale)*xsize];
			SW = pGrayImage[(int)floor(i/scale)+(int)(floor(j/scale)+1)*xsize];
			SE = pGrayImage[(int)floor(i/scale)+1+(int)(floor(j/scale)+1)*xsize];
			EWtop = NW + EWweight*(NE-NW);
			EWbottom = SW + EWweight*(SE-SW);
			pResampleImage[i+j*re_xsize] = (unsigned char)(EWtop + NSweight*(EWbottom-EWtop)+0.5);
		}
		block_size = GetBlockSize(re_xsize, re_ysize, desired_num_of_blocks);
		if(block_size<2)
			block_size = 2;
		EdgeHistogramGeneration(pResampleImage, re_xsize, re_ysize, block_size, pLocal_Edge, Te_Value);
		delete  [] pResampleImage;
	}
	else{
		block_size = GetBlockSize(xsize, ysize, desired_num_of_blocks);
		if(block_size<2)
			block_size = 2;
		EdgeHistogramGeneration(pGrayImage, xsize, ysize, block_size, pLocal_Edge, Te_Value);
	}
// end modification
	m_Descriptor->SetEdgeHistogram( pLocal_Edge );

	delete	[] pLocal_Edge;
	delete	[] pGrayImage;
	return 0;
}

//----------------------------------------------------------------------------
unsigned long EdgeHistogramExtractionTool::GetBlockSize(unsigned long image_width, unsigned long image_height, unsigned long desired_num_of_blocks)
{
	unsigned long	block_size;
	double		temp_size;

	temp_size = (double) sqrt(image_width*image_height/desired_num_of_blocks);
	block_size = (unsigned long) (temp_size/2)*2;

	return block_size;
}

//----------------------------------------------------------------------------
void EdgeHistogramExtractionTool::
EdgeHistogramGeneration(unsigned char* pImage_Y, unsigned long image_width,
			unsigned long image_height, unsigned long block_size,
			EHD* pLocal_Edge, int Te_Value)
{
	int Count_Local[16],sub_local_index;
	int Offset, EdgeTypeOfBlock;
	unsigned int i, j;
	long	LongTyp_Local_Edge[80];

	// Clear
	memset(Count_Local, 0, 16*sizeof(int));
	memset(LongTyp_Local_Edge, 0, 80*sizeof(long));

	for(j=0; j<=image_height-block_size; j+=block_size)
	  for(i=0; i<=image_width-block_size; i+=block_size){
	    sub_local_index = (int)(i*4/image_width)+(int)(j*4/image_height)*4;
	    Count_Local[sub_local_index]++;

	    Offset = image_width*j+i;

	    EdgeTypeOfBlock = GetEdgeFeature(pImage_Y+Offset, image_width,
					     block_size, Te_Value);
	    switch(EdgeTypeOfBlock) {
	    case NoEdge:
	      break;
	    case vertical_edge:
	      LongTyp_Local_Edge[sub_local_index*5]++;
	      break;
	    case horizontal_edge:
	      LongTyp_Local_Edge[sub_local_index*5+1]++;
	      break;
	    case diagonal_45_degree_edge:
	      LongTyp_Local_Edge[sub_local_index*5+2]++;
	      break;
	    case diagonal_135_degree_edge:
	      LongTyp_Local_Edge[sub_local_index*5+3]++;
	      break;
	    case non_directional_edge:
	      LongTyp_Local_Edge[sub_local_index*5+4]++;
	      break;
	    } //switch(EdgeTypeOfBlock)
	  } // for( i )

	for( i=0; i<80; i++) {			// Range 0.0 ~ 1.0
	  sub_local_index = (int)(i/5);
	  pLocal_Edge->Local_Edge[i] =
	    (double)LongTyp_Local_Edge[i]/Count_Local[sub_local_index];
	}
}

//----------------------------------------------------------------------------------------------------------------------
int EdgeHistogramExtractionTool::GetEdgeFeature(unsigned char *pImage_Y,
						   int image_width,
						   int block_size,int Te_Value)
{
	int i, j;
	double	d1, d2, d3, d4;
	int e_index;
	double dc_th = Te_Value;
	double e_h, e_v, e_45, e_135, e_m, e_max;

	d1=0.0;
	d2=0.0;
	d3=0.0;
	d4=0.0;

	for(j=0; j<block_size; j++)for(i=0; i<block_size; i++){

		if(j<block_size/2){
			if(i<block_size/2)
				d1+=(pImage_Y[i+image_width*j]);
			else
				d2+=(pImage_Y[i+image_width*j]);
		}
		else{
			if(i<block_size/2)
				d3+=(pImage_Y[i+image_width*j]);
			else
				d4+=(pImage_Y[i+image_width*j]);
		}
	}
	d1 = d1/(block_size*block_size/4.0);
	d2 = d2/(block_size*block_size/4.0);
	d3 = d3/(block_size*block_size/4.0);
	d4 = d4/(block_size*block_size/4.0);

	e_h = fabs(d1+d2-(d3+d4));
	e_v = fabs(d1+d3-(d2+d4));
	e_45 = sqrt(2)*fabs(d1-d4);
	e_135 = sqrt(2)*fabs(d2-d3);

	e_m = 2*fabs(d1-d2-d3+d4);

	e_max = e_v;
	e_index = vertical_edge;
	if(e_h>e_max){
		e_max=e_h;
		e_index = horizontal_edge;
	}
	if(e_45>e_max){
		e_max=e_45;
		e_index = diagonal_45_degree_edge;
	}
	if(e_135>e_max){
		e_max=e_135;
		e_index = diagonal_135_degree_edge;
	}
	if(e_m>e_max){
		e_max=e_m;
		e_index = non_directional_edge;
	}
	if(e_max<dc_th)
		e_index = NoEdge;

	return(e_index);
}


