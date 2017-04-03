///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// TU-Munich, S. Herrmann
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
// Applicable File Name:  ColorSpace.h
//
// modified by mb - remove unnecessary parts,interface
// date: 2009-11-13

#ifndef __ColorSpace__
#define __ColorSpace__


typedef enum {RGB,YCRCB,HSV,HMMD,LinearMatrix,Monochrome} ColorSpaceType;

namespace XM
{

class ColorSpaceDescriptor
{

public:
	ColorSpaceDescriptor();
	virtual ~ColorSpaceDescriptor();

	// accessor methods
	virtual bool GetColorReferenceFlag(void);
	virtual unsigned long SetColorReferenceFlag(bool colorReferenceFlag);
	virtual ColorSpaceType GetDescriptionColorSpace(void);
	virtual unsigned long SetDescriptionColorSpace(ColorSpaceType);
	virtual float* GetColorSpaceMatrix();
	virtual float GetMatrixElement(int xindex,int yindex);
	virtual unsigned long SetMatrixElement(int xindex,int yindex, float value);

	//virtual unsigned long ExportDDL(GenericDSInterfaceABC *aParentDescription);
	//virtual unsigned long ImportDDL(GenericDSInterfaceABC *aDescription);

private:

    // descriptor data
	bool m_ColorReferenceFlag;
	ColorSpaceType  m_DescriptionColorSpace;
	float m_ColorTransMatrix[3][3];
	// 3x3 matrix for LinearMatrix color space
};

};

#endif
