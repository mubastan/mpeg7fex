///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// NEC Corp., Akio Yamada, Eiji Kasutani.
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
// Applicable File Name:  ColorLayout.h
//
// modified by mb - remove unnecessary parts,interface
// date: 2009-11-13

#ifndef __COLOR_LAYOUT_H__
#define __COLOR_LAYOUT_H__


namespace XM
{

class ColorLayoutDescriptor
{
public:
	ColorLayoutDescriptor(int numYCoef = 6, int numCCoef = 3 );
	virtual ~ColorLayoutDescriptor();


	//virtual unsigned long ExportDDL(GenericDSInterfaceABC *aParentDescription);
    //virtual unsigned long ImportDDL(GenericDSInterfaceABC *DescriptorRoot);	//sth use Descriptor node for this

	// accessor methods
	int GetNumberOfYCoeff();
	int GetNumberOfCCoeff();
	int *GetYCoeff();
	int *GetCbCoeff();
	int *GetCrCoeff();
	void SetNumberOfYCoeff(int NumberOfYCoeff);
	void SetNumberOfCCoeff(int NumberOfCCoeff);
	void SetYCoeff(int *YCoeff);
	void SetCbCoeff(int *CbCoeff);
	void SetCrCoeff(int *CrCoeff);

    /// compute the distance between 2 CLDs
    float distance(ColorLayoutDescriptor* cld);
    /// normalized distance (0.0 ... 1.0)
    float distanceNorm(ColorLayoutDescriptor* cld);

	void Print();

public:

	/// descriptor data
	int m_NumberOfYCoeff, m_NumberOfCCoeff;
	int *m_y_coeff, *m_cb_coeff, *m_cr_coeff;
};

};

#endif
