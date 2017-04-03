///////////////////////////////////////////////////////////////////////////////
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
// Applicable File Name:  EdgeHistExtraction.h
//
// modified by mb - remove unnecessary parts,interface
// date: 2009-11-13

#ifndef __EDGEHISTEXTRACTION_H__
#define __EDGEHISTEXTRACTION_H__

#include "AddressLib/vopio.h"
#include "Descriptors/EdgeHist.h"

#define		Te_Define	11
#define		Desired_Num_of_Blocks	1100

namespace XM
{

class EdgeHistogramExtractionTool
{
public:
    // Null constructor
    EdgeHistogramExtractionTool();
    ~EdgeHistogramExtractionTool();

    EdgeHistogramDescriptor* GetDescriptor();
    int SetDescriptor(EdgeHistogramDescriptor* aEdgeHistogramDescriptorInterface);

    // color image is converted to grayscale within the function
    // TODO: a new function taking a grayscale image as input
    unsigned long extract(MomVop *img);


private:
    EdgeHistogramDescriptor* m_Descriptor;

    unsigned long GetBlockSize( unsigned long image_width,
                                unsigned long image_height, unsigned long desired_num_of_blocks);
    void EdgeHistogramGeneration( unsigned char* pImage_Y,
                                  unsigned long image_width,
                                  unsigned long image_height,
                                  unsigned long block_size,
                                  EHD* pLocal_Edge, int Te_Value);
    int GetEdgeFeature( unsigned char *pImage_Y,
                        int image_width, int block_size, int Te_Value);

};

};


#endif
