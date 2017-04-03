///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Yong-Sung Kim - Hanyang University, Korea
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
// Applicable File Name:  RegionShapeExtraction.h
//
// modified by mb - remove unnecessary parts,interface
// date: 2009-11-13

#ifndef __REGIONSHAPEEXTRACTION_H__
#define __REGIONSHAPEEXTRACTION_H__

#include "AddressLib/vopio.h"
#include "Descriptors/RegionShape.h"

namespace XM
{

#ifndef M_PI
#define M_PI	3.141592653
#endif

#define HYPOT(x,y)		sqrt((x)*(x)+(y)*(y))

#define	ART_LUT_RADIUS 50		// Zernike basis function radius
#define	ART_LUT_SIZE (ART_LUT_RADIUS*2+1)

class RegionShapeExtractionTool
{
public:
	RegionShapeExtractionTool();
	~RegionShapeExtractionTool();

	RegionShapeDescriptor* GetDescriptor();
	int SetDescriptor(RegionShapeDescriptor* aRegionShapeDescriptor);

	// uses a_chan of input image, foreground pixel values > 0
	// modified by MB (24.12.2008)
	// TODO: input a single band MomImage
	unsigned long extract(MomVop *img);

private:

    RegionShapeDescriptor* m_Descriptor;

	// input image
	MomVop *m_Media;

	void Reset();
	void GenerateBasisLUT();
	void FindCenterOfMass();
	void FindRadius();
	void ExtractCoefficients();
	void Save();

	double GetReal(int p, int r, double dx, double dy);
	double GetImg(int p, int r, double dx, double dy);

	static bool m_bLUTInit;
	static double m_pBasisR[ART_ANGULAR][ART_RADIAL][ART_LUT_SIZE][ART_LUT_SIZE];	// real-value of RegionShape basis function
	static double m_pBasisI[ART_ANGULAR][ART_RADIAL][ART_LUT_SIZE][ART_LUT_SIZE];	// imaginary-value of RegionShape basis function

	double m_pCoeffR[ART_ANGULAR][ART_RADIAL];
	double m_pCoeffI[ART_ANGULAR][ART_RADIAL];

	double m_centerX, m_centerY;
	double m_radius;
	int m_mass;
};

};

#endif
