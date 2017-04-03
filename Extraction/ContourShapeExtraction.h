///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Mitsubishi Electric
// ITE-VIL, W.Price
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
// purpose, assign or donate the code to a third party and inhibit third parties// from using the code for non MPEG-7 conforming products.
//
// Copyright (c) 1998-2000.
//
// This notice must be included in all copies or derivative works.
//
// Applicable File Name:  ContourShapeExtraction.h
//
// modified by mb - remove unnecessary parts,interface
// date: 2009-11-14

#ifndef __CONTOURSHAPEEXTRACTION_H__
#define __CONTOURSHAPEEXTRACTION_H__

#include "AddressLib/vopio.h"

namespace XM
{

class ContourShapeExtractionTool
{
public:
	ContourShapeExtractionTool();
	~ContourShapeExtractionTool();

	ContourShapeDescriptor* GetDescriptor();
	int SetDescriptor(ContourShapeDescriptor* aContourShapeDescriptor);

	// background : 0, foreground: 255
	// alpha channel (a_chan) or y_chan of MomVop image is the object mask
	unsigned long extract(MomVop *img);
	// TODO: modify this function to take a single channel MomImage as a parameter

private:

	ContourShapeDescriptor* m_Descriptor;
	MomVop* m_ImageMedia;

	// functions used for extracting the contour shape and the
	// peaks from the contour...
	unsigned long ExtractContour(int n, Point2* const &coords);
	unsigned long ExtractPeaks(int n, const Point2* const &coords);

	// function to extract the curvature values (eccentricity and
	// circularity) from the contour...
	void ExtractCurvature(int n, const Point2 * const &coords, unsigned long &cir, unsigned long &ecc);
};

};


#endif
