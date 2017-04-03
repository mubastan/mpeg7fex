////////////////////////////////////////////////////////////////////////
//
// ColorStructure.h
//
// This software module was originally developed by
//
// Jim Errico, Sharp Laboratories of America, Camas, WA
// Peter van Beek, Sharp Laboratories of America, Camas, WA
//
//
// in the course of development of the MPEG-7 standard (ISO/IEC 15938).
// This software module is an implementation of a part of one or more
// MPEG-7 tools as specified by the MPEG-7 standard (ISO/IEC 15938).
// ISO/IEC gives users of the MPEG-7 standard free license to this
// software module or modifications thereof for use in hardware or
// software products claiming conformance to the MPEG-7 standard.
// Those intending to use this software module in hardware or software
// products are advised that its use may infringe existing patents.
// The original developer of this software module and his/her company,
// the subsequent editors and their companies, and ISO/IEC have no
// liability for use of this software module or modifications thereof
// in an application. No license to this software is granted for non
// MPEG-7 conforming products.
// Sharp Laboratories of America retains full right to use the software
// module for their own purpose, assign or donate the software module
// to a third party and to inhibit third parties from using the software
// module for non MPEG-7 conforming products.
//
// Copyright (c) 2000
//
// This copyright notice must be included in all copies or derivative
// works of this software module.
//
// modified by mb - remove unnecessary parts,interface
// date: 2009-11-12

#ifndef __COLORSTRUCTURE_H__
#define __COLORSTRUCTURE_H__

namespace XM
{

class ColorStructureDescriptor
{
public:
	ColorStructureDescriptor();
    virtual ~ColorStructureDescriptor();

	unsigned long GetSize(void);
	unsigned long SetSize(unsigned long size);
	unsigned long GetElement(unsigned long index);
	unsigned long SetElement(unsigned long index, int value);

	float distance(ColorStructureDescriptor* csd);

	//virtual unsigned long ExportDDL( GenericDSInterfaceABC *aParentDescription);
	//virtual unsigned long ImportDDL( GenericDSInterfaceABC *aDescription);

public:
	unsigned long* m_Data;
	unsigned long m_Size;
};

};

#endif
