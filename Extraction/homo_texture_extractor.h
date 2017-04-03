// This software module was originally developed by
//
// ICU(Information Communication University), Yong-Ju Jung,Ki Won Yoo,Yong Man Ro.
// ETRI, Mun Churl Kim
// Samsung, Yun Ju Yu, Yang Lim Choi.
// in coorpration with Samsung, UCSB, Hyundai, HHI.
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
// Copyright (c) 1998-2000.
//
// This notice must be included in all copies or derivative works.
//
// Applicable File Name:  CHomogeneousTexture_Extractor.h
//
#ifndef _HOMOTEXTURE_EXTRACTOR_H_
#define _HOMOTEXTURE_EXTRACTOR_H_

#ifndef M_PI
#define	M_PI		(3.14159265358979323846)
#endif



class CHomogeneousTexture_Extractor
{
    public:
        CHomogeneousTexture_Extractor();
        virtual ~CHomogeneousTexture_Extractor();

        void FeatureExtraction(unsigned char* image);
        void ArbitraryShape(unsigned char* a_image, unsigned char *y_image, int image_height, int image_width);

    public:
        int mean2[5][6];
        int dev2[5][6];
        int m_dc;
        int m_std;

        // 2000.10.11 - yjyu@samsung.com
        int image_height;
        int image_width;
};


#endif
