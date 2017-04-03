///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// TU-Munich, S.Herrmann
// Heinrich-Hertz-Institute (HHI), Bela Makai
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
// Applicable File Name:  ScalableColorExtraction.h
//
// modified by mb - remove unnecessary parts
// date: 2009-11-12

#ifndef __SCALCOLOREXTRACTION_H__
#define __SCALCOLOREXTRACTION_H__

#include "AddressLib/vopio.h"

namespace XM
{

//=============================================================================
class ScalableColorExtractionTool
{
public:
	ScalableColorExtractionTool( int numOfCoeff = 256, int numBitPlanesDiscarded = 0 );
    ~ScalableColorExtractionTool();

	int SetSourceMedia(MomVop* img);
	ScalableColorDescriptor* GetDescriptor(void);
	int SetDescriptor(ScalableColorDescriptor* aScalableColorDescriptor);

	// currently works only for 256 coefficients (?)
	unsigned long extract(MomVop* img);

	unsigned long *ComputeNormalizedHistogram();
	unsigned long *QuantizeHistogram(unsigned long *aHist);
	unsigned long *HaarTransform(unsigned long *aHist);

private:


	ScalableColorDescriptor* m_Descriptor;

	// image whose descriptor is to be computed
	MomVop* srcImage;


};

};

extern "C" void pixeltohistogram1(TPixel *res, TPixel *in, TCoor x, TCoor y);
extern "C" void pixeltocolorhist1(TPixel *res, TPixel *in, TCoor x, TCoor y);
//void pixeltocolorhist1(TPixel *res, TPixel *in,TCoor x, TCoor y);

#endif
