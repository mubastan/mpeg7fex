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
// Applicable File Name:  HomoTextureDescriptor.h
//
// modified by mb - remove unnecessary parts,interface
// date: 2009-11-13

#ifndef _HOMO_TEXTURE_H_
#define _HOMO_TEXTURE_H_

namespace XM
{

class HomogeneousTextureDescriptor
{
public:
    HomogeneousTextureDescriptor(bool endevFlag = true);
    ~HomogeneousTextureDescriptor();

	void SetHomogeneousTextureFeature(bool endevFlag, int* HomogeneousTextureFeature);
	int* GetHomogeneousTextureFeature() ;
	int GetHomogeneousTextureFeatureFlag();

    // by mb
    void resetDescriptor();

    float distance(HomogeneousTextureDescriptor* htd);
    float distanceBase(HomogeneousTextureDescriptor* htd);

    void Print();

    //virtual unsigned long ExportDDL(GenericDSInterfaceABC *aParentDescription);
    //virtual unsigned long ImportDDL(GenericDSInterfaceABC *aDescription);

public:
    bool energydeviationflag;
    int feature[62];

};

};

#endif
