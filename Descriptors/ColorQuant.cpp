///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Intel Corporation
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
// Applicable File Name:  ColorQuant.cpp
//
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "ColorQuant.h"

using namespace XM;

ColorQuantizerDescriptor::ColorQuantizerDescriptor()
{
	memset( m_Component, 0, 3*sizeof(int) );	// added by mb
	memset( m_BinNumber, 0, 3*sizeof(int) );	// added by mb
}

//----------------------------------------------------------------------------
ColorQuantizerDescriptor::~ColorQuantizerDescriptor()
{
}

/*
//----------------------------------------------------------------------------
unsigned long ColorQuantizerDescriptor::
ExportDDL(GenericDSInterfaceABC *aParentDescription)
{
	GenericDS ColQuant, Component, BinNumber;
	int i, component, bin_number;
	char str[256];
	char comp_label[13][5] = {"R","G","B","Y","Cb","Cr","H","S","V",
		"Max","Min","Diff","Sum"};

	if(!aParentDescription) return (unsigned long) -1;

	ColQuant = aParentDescription->CreateChild("ColorQuantization");

	int NumComponents=3;
	int ColorSpace = atoi(getpara("ColorSpace","0"));
	if(ColorSpace==5) NumComponents = 1; // monochrome (0101)

	char compindx[2]; // LC added
	for(i=0; i<NumComponents; i++)
	{
		Component = ColQuant.CreateChild("Component");
		sprintf(str,"Component%d",i);
		sprintf(compindx, "%d", i); // LC added
		component = atoi(getpara(str,compindx)); // LC changed
		Component.SetValue(comp_label[component]);
		BinNumber = ColQuant.CreateChild("NumOfBins");
		sprintf(str,"BinNumber%d",i);
		bin_number = atoi(getpara(str,"4"));
		BinNumber.SetValue(bin_number);
	}

	return 0;
}

//----------------------------------------------------------------------------
unsigned long ColorQuantizerDescriptor::
ImportDDL(GenericDSInterfaceABC *aDescription)
{
	GenericDS ColQuant, Component, BinNumber;
	string text_component;
	int i, component, bin_number;

	if(!aDescription) return (unsigned long) -1;

	ColQuant = aDescription->GetDescription("ColorQuantization");
	if(ColQuant.isNull()) return (unsigned long) -1;

	int NumComponents = 3;
	int ColorSpace = atoi(getpara("ColorSpace","0"));
	if(ColorSpace==5) NumComponents = 1; // monochrome

	for(i=0; i<NumComponents; i++)
	{
		Component = ColQuant.GetDescription("Component");
		Component.GetTextValue(text_component);
		if(text_component=="R") component = 0;
		else if(text_component=="G") component = 1;
		else if(text_component=="B") component = 2;
		else if(text_component=="Y") component = 3;
		else if(text_component=="Cb") component = 4;
		else if(text_component=="Cr") component = 5;
		else if(text_component=="H") component = 6;
		else if(text_component=="S") component = 7;
		else if(text_component=="V") component = 8;
		else if(text_component=="Max") component = 9;
		else if(text_component=="Min") component = 10;
		else if(text_component=="Diff") component = 11;
		else if(text_component=="Sum") component = 12;
		else
		{
			fprintf(stderr, "\nColor Component Error!!\n");
			exit(-1);
		}
		ColQuant.RemoveChild(Component);

		BinNumber = ColQuant.GetDescription("NumOfBins");
		BinNumber.GetIntValue(bin_number);
		ColQuant.RemoveChild(BinNumber);

		SetComponent(i, component);
		SetBinNumber(i, bin_number);
	}

	return 0;
}

*/

//----------------------------------------------------------------------------
int ColorQuantizerDescriptor::GetComponent(int order)
{
	return m_Component[order];
}


//----------------------------------------------------------------------------
int ColorQuantizerDescriptor::SetComponent(int order, int component)
{
	m_Component[order] = component;

	return 0;
}

//----------------------------------------------------------------------------
int ColorQuantizerDescriptor::GetBinNumber(int order)
{
	return m_BinNumber[order];
}

//----------------------------------------------------------------------------
int ColorQuantizerDescriptor::SetBinNumber(int order, int bin_number)
{
	int error=0;

	if(bin_number>0) m_BinNumber[order] = bin_number;
	else error=-1;

	return error;
}

//----------------------------------------------------------------------------
int ColorQuantizerDescriptor::GetBinNumberByComponent(int component)
{
	int i;
	int NumComponents = 3;
	int ColorSpace = 0;
	if( ColorSpace == 5 )
        NumComponents = 1; // monochrome (0101)

	for( i = 0; i < NumComponents; i++)
		if(m_Component[i]==component) return m_BinNumber[i];

	return -1;
}
