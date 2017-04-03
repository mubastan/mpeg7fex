///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// NEC Corp., Akio Yamada, Eiji Kasutani
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
// Applicable File Name:  ColorLayoutExtraction.h
//
// modified by mb - remove unnecessary parts,interface
// date: 2009-11-13

#ifndef __COLOR_LAYOUT_EXTRACTION_H__
#define __COLOR_LAYOUT_EXTRACTION_H__

#include "AddressLib/vopio.h"
#include "Descriptors/ColorLayout.h"

namespace XM
{
class ColorLayoutExtractionTool
{
public:
	// Null constructor
	ColorLayoutExtractionTool(int numYCoef = 6, int numCCoef = 3);
	virtual ~ColorLayoutExtractionTool();


	int SetSourceMedia(MomVop* img);
	ColorLayoutDescriptor* GetDescriptor(void);
	int SetDescriptor(ColorLayoutDescriptor* aColorLayoutDescriptor);

	// uses img->a_chan for arbitrary shape
	unsigned long extract(MomVop *img, int NumberOfYCoeff, int NumberOfCCoeff);
	unsigned long extract(MomVop *img){ return extract(img, 6, 3); }

private:

	ColorLayoutDescriptor* m_Descriptor;
	MomVop* srcImage;

	// helper functions and variables
	double c[8][8];                     // transform coefficients
	unsigned char zigzag_scan[64];      // Zig-Zag scan pattern
	void CreateSmallImage(MomVop *src, short small_img[3][64]);
	void init_fdct();
	inline void fdct(short *block);
 	inline int quant_ydc(int i);
	inline int quant_cdc(int i);
	inline int quant_ac(int i);
};

};

#endif
