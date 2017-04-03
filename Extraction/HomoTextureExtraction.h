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
// Applicable File Name:  HomoTextureExtraction.h
//
// modified by mb - remove unnecessary parts,interface
// date: 2009-11-13

#ifndef _HOMOTEXTURE_EXTRACTION_H_
#define _HOMOTEXTURE_EXTRACTION_H_

#include "AddressLib/vopio.h"
#include "Descriptors/HomoTexture.h"


namespace XM
{

class HomogeneousTextureExtractionTool
{
public:
    // Null constructor
    HomogeneousTextureExtractionTool(bool endevFlag = true);
    ~HomogeneousTextureExtractionTool();

    // Also connects the Descriptor (result memnory) to the extraction
    // If set to "0" it automatically creates the descriptor
    HomogeneousTextureExtractionTool(HomogeneousTextureDescriptor* HomogeneousTexture);


    // This informs the extractor where the source data comes from
    int SetSourceMedia(MomVop* img);

    HomogeneousTextureDescriptor*  GetDescriptor(void);
    int SetDescriptor(HomogeneousTextureDescriptor* aHomogeneousTextureDescriptor);

    //ImageMedia->y_chan->data->u is used as data, a_chan for shape (?)
    // input image size must be > 128x128
    int extract(MomVop* ImageMedia, bool flag = true);


private:

    HomogeneousTextureDescriptor* m_Descriptor;
    MomVop* srcImage;

};

};


#endif
