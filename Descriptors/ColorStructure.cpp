////////////////////////////////////////////////////////////////////////
//
// ColorStructure.cpp
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
////////////////////////////////////////////////////////////////////////


#include <cstdio>
#include <cstring>
#include <cassert>
#include <cstdlib>

#include "ColorStructure.h"

using namespace XM;

//=============================================================================
ColorStructureDescriptor::ColorStructureDescriptor():
m_Data(0),
m_Size(0)
{
}

//----------------------------------------------------------------------------
ColorStructureDescriptor::~ColorStructureDescriptor()
{
	if (m_Data)
		delete[] m_Data;
}

//----------------------------------------------------------------------------
unsigned long ColorStructureDescriptor::GetSize(void)
{ return m_Size; }

//----------------------------------------------------------------------------
unsigned long ColorStructureDescriptor::SetSize(unsigned long size)
{
	if (size == 32 || size == 64 || size == 128 || size == 256)
	{
		if (m_Size == size)
			return 0;
		if (m_Data)
			delete m_Data;

		m_Data = new unsigned long[size];
		memset( m_Data, 0, size*sizeof(unsigned long) );		// added by mb

		m_Size = size;
		return 0;
	}

	else
	{
		//std::cerr << "Invalid SetSize(" << size << ")";
		//exit(0);
		return (unsigned long) -1;
	}
	return 0;
}

//----------------------------------------------------------------------------
unsigned long ColorStructureDescriptor::GetElement(unsigned long index)
{
	if (index < m_Size && m_Data)
		return m_Data[index];

	else
	{
		//std::cerr << "Out of bounds GetElement(" << index << ")" << std::endl;
		//exit(0);
		return (unsigned long) -1;
	}
	return 0;
}

//----------------------------------------------------------------------------
unsigned long ColorStructureDescriptor::
SetElement(unsigned long index, int value)
{
	if (index < m_Size && m_Data)
	{
		m_Data[index] = value;
		return 0;
	}

	else
	{
		//std::cerr << "Out of bounds SetElement(" << index << ",...)" << std::endl;
		//exit(0);
		return (unsigned long) -1;
	}
	return 0;
}

#define CSD_MAX_DIST 6701.0f
float ColorStructureDescriptor::distance(ColorStructureDescriptor* csd)
{
    float dist = 0.0f;
//    int size = this->GetSize();
//	for( int i = 0; i < size; i ++)
//		dist += abs( this->GetElement(i) - csd->GetElement(i));

    for( int i = 0; i < m_Size; i ++)
        dist += abs( (int)this->m_Data[i] - (int)csd->m_Data[i]);

	return dist/CSD_MAX_DIST;

}

/*
//----------------------------------------------------------------------------
unsigned long ColorStructureDescriptor::
ExportDDL(GenericDSInterfaceABC *aParentDescription)
{
	if(!aParentDescription)
		return (unsigned long) -1;

	// Determine the coded size & color quantization setting
	int		v, size = GetSize();
	switch(size)
	{
		case  32:	v=1; break;
		case  64:	v=2; break;
		case 128:	v=3; break;
		case 256:	v=4; break;
		default:	v=0;
	}
	assert(v);


	GenericDS	l_DDLDescription;
	GenericDS	MultimediaContent, Image, VisualDescriptor, Values;

	// Create repeating descriptor data
	MultimediaContent = aParentDescription->CreateChild("MultimediaContent");
	if (MultimediaContent.isNull())
		return (unsigned long) -1;
	MultimediaContent.SetAttribute("xsi:type","ImageType");

	Image = MultimediaContent.CreateChild("Image");
	if (Image.isNull())
		return (unsigned long) -1;

	VisualDescriptor = Image.CreateChild("VisualDescriptor");
	if (VisualDescriptor.isNull())
		return (unsigned long) -1;
	VisualDescriptor.SetAttribute("xsi:type","ColorStructureType");
	VisualDescriptor.SetAttribute("colorQuant", v);

	Values = VisualDescriptor.CreateChild("Values");
	if (Values.isNull())
		return (unsigned long) -1;

	// Build values vector
	vector<int> buf(size);
	for (int i = 0; i< size; i++)
		buf[i] = GetElement(i);

	// Set list value into element
	Values.SetValue(buf);

	return 0;
}

//----------------------------------------------------------------------------
unsigned long ColorStructureDescriptor::
ImportDDL(GenericDSInterfaceABC *aDescription)
{
        int size;
	GenericDSInterfaceABC *l_DDLDescriptionInterface = NULL;
	GenericDS	Image, VisualDescriptor, Values;

	//sth this looks different in DummyType!!
	// if aDescription is of correct type
	if(aDescription->GetDSName() == "MultimediaContent")
	{
		string xsitype;
		aDescription->GetTextAttribute("xsi:type", xsitype);
		if(xsitype == "ImageType")
			l_DDLDescriptionInterface = aDescription;
	}

	// ImageType not found
	if(l_DDLDescriptionInterface->isNull())
	  return (unsigned long) -1;

	Image = l_DDLDescriptionInterface->GetChild(0);
	if (Image.isNull())
		return (unsigned long) -1;
	VisualDescriptor = Image.GetChild(0);
	if (VisualDescriptor.isNull())
		return (unsigned long) -1;
	Values = VisualDescriptor.GetChild(0);
	if (Values.isNull())
		return (unsigned long) -1;

	// Set size into descriptor
	VisualDescriptor.GetIntAttribute("colorQuant", size);
	size = 1 << (size + 4);
	SetSize((unsigned long)size);

	// Get data
	vector<int> buf;
	Values.GetIntVector(buf);
	for (int i = 0; i < buf.size() && i < size; i++)
		SetElement((unsigned long)i,buf[i]);

	return 0;
}
*/

