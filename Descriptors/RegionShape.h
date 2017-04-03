///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Yong-Sung Kim - Hanyang University, Korea
// (contributing organizations names)
//
// Modified by
// Min-Seok Choi - Hanyang University, Korea
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
// Applicable File Name:  RegionShape.h
//
// modified by mb - remove unnecessary parts,interface
// date: 2009-11-13

#ifndef __REGION_SHAPE__
#define __REGION_SHAPE__

namespace XM
{

#define ART_ANGULAR	12
#define ART_RADIAL 	3

class RegionShapeDescriptor
{
public:
	RegionShapeDescriptor();
	virtual ~RegionShapeDescriptor();

	// Accesssor methods
	bool SetElement(int p, int r, char value);
	bool SetElement(int p, int r, double value);
	char GetElement(int p, int r);
	double GetRealValue(int p, int r);

    // to zero
	void resetDescriptor();

	//virtual unsigned long ExportDDL(GenericDSInterfaceABC *aParentDescription);
	//virtual unsigned long ImportDDL(GenericDSInterfaceABC *aDescription);

private:
	// actual values:
	char m_ArtDE[ART_ANGULAR][ART_RADIAL];

	// An array of RegionShape coefficients
	static double QuantTable[17];
	static double IQuantTable[16];
};

};

#endif
