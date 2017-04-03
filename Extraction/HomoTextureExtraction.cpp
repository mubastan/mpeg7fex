///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// ICU(Information Communication University), Yong-Ju Jung,Ki Won Yoo,Yong Man Ro.
// ETRI, Mun Churl Kim
// Samsung, Yun Ju Yu, Yang Lim Choi.
// in cooperation with UCSB, Hyundai, HHI.
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
// parties from using the code for non MPEG-7 conforming products.
//
// Copyright (c) 1998-2001.
//
// This notice must be included in all copies or derivative works.
//
// Applicable File Name:  HomoTextureExtraction.cpp
//

#include <stdio.h>

#include "Extraction/HomoTextureExtraction.h"
#include "Extraction/homo_texture_extractor.h"


using namespace XM;

HomogeneousTextureExtractionTool::HomogeneousTextureExtractionTool(bool endevFlag):
m_Descriptor(0),
srcImage(0)
{
    m_Descriptor = new HomogeneousTextureDescriptor(endevFlag);
}

//----------------------------------------------------------------------------
HomogeneousTextureExtractionTool::~HomogeneousTextureExtractionTool()
{
  // release descriptor

	// commented out - mb - 15.07.2008
	// descriptor will be released by the clients
  //if (m_Descriptor)
  //  delete m_Descriptor;
}

//----------------------------------------------------------------------------
// This informs the extractor where the source data is coming from. This
// could be an audio file, image stream etc. Here it's just a filename.
int HomogeneousTextureExtractionTool::SetSourceMedia(MomVop* img)
{
  if(img==0) return -1;
  srcImage = img;
  return 0;
}

//----------------------------------------------------------------------------
HomogeneousTextureDescriptor* HomogeneousTextureExtractionTool::GetDescriptor()

{
  return m_Descriptor;
}

//----------------------------------------------------------------------------
int HomogeneousTextureExtractionTool::SetDescriptor(HomogeneousTextureDescriptor
		       *aHomogeneousTextureDescriptor)

{
  if (m_Descriptor == aHomogeneousTextureDescriptor) return 0;

  if (m_Descriptor) delete m_Descriptor;
  m_Descriptor = aHomogeneousTextureDescriptor;
  return 0;
}

//----------------------------------------------------------------------------
// ImageMedia: alpha channel for arbitrary shape, ImageMedia->y_chan->data->u for actual image data
// flag : 0:base-layer 32-components  1:full-layer 62components (both energy and deviation)
int HomogeneousTextureExtractionTool::extract(MomVop* ImageMedia, bool flag)
{
  int HomogeneousTexture_feature[62];

  int i, j;

  if (ImageMedia==0) return -1;


	if(ImageMedia->height < 128 || ImageMedia->width < 128)
	{
		fprintf(stderr,"Image size should be bigger than 128 x 128.\n");
		return -1;
	}

	//HomogeneousTexture feature extraction
	CHomogeneousTexture_Extractor htex;

	// 2000.10.11 - yjyu@samsung.com
	htex.image_height = ImageMedia->height;
	htex.image_width = ImageMedia->width;

	//if ((ImageMedia->a_chan) && (m_Media->GetMaxAlpha() > m_Media->GetMinAlpha()))
	// modifies y_chan according to the FG mask in a_chan
	// feature is extracted next
	if ( ImageMedia->a_chan ) // modified by mb - 27.06.2008
		htex.ArbitraryShape(ImageMedia->a_chan->data->u,
                            ImageMedia->y_chan->data->u,
                            ImageMedia->height, ImageMedia->width );

	htex.FeatureExtraction(ImageMedia->y_chan->data->u);

	//0:base-layer 32-components  1:full-layer 62components (both energy and deviation)

	//saving 62 features & test show
	HomogeneousTexture_feature[0] = htex.m_dc;
	HomogeneousTexture_feature[1] = htex.m_std;

	for(i=0; i< 5; i++)
		for(j=0; j< 6; j++) {
			HomogeneousTexture_feature[i*6+j+2] = htex.mean2[i][j];
		}

	for(i=0; i< 5; i++)
		for(j=0; j< 6; j++) {
			if (flag != 0)
				HomogeneousTexture_feature[i*6+j+30+2] = htex.dev2[i][j];
			else
				HomogeneousTexture_feature[i*6+j+30+2] = 0;
		}


	m_Descriptor->SetHomogeneousTextureFeature(flag, HomogeneousTexture_feature);	//yjyu - 010223


    return 0;
}

