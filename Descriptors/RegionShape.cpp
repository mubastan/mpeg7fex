//////////////////////////////////////////////////////////////////////////////
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
// Applicable File Name:  RegionShape.cpp
//
///////////////////////////////////////////////////////////////////////////////

#include <cstdio>
#include <string>

#include "RegionShape.h"

using namespace XM;

// Quantization and Inverse quantization tables
double RegionShapeDescriptor::QuantTable[17] = {0.000000000, 0.003585473, 0.007418411, 0.011535520, 0.015982337, 0.020816302, 0.026111312, 0.031964674, 0.038508176, 0.045926586, 0.054490513, 0.064619488, 0.077016351, 0.092998687, 0.115524524, 0.154032694, 1.000000000};
double RegionShapeDescriptor::IQuantTable[16] =     {0.001763817, 0.005468893, 0.009438835, 0.013714449, 0.018346760, 0.023400748, 0.028960940, 0.035140141, 0.042093649, 0.050043696, 0.059324478, 0.070472849, 0.084434761, 0.103127662, 0.131506859, 0.192540857};

// double RegionShapeDescriptor::QuantTable[17] = {0.000000000, 0.001898192, 0.003927394, 0.006107040, 0.008461237, 0.011020396, 0.013823636, 0.016922475, 0.020386682, 0.024314076, 0.028847919, 0.034210318, 0.040773364, 0.049234601, 0.061160045, 0.081546727, 1.0};
// double RegionShapeDescriptor::IQuantTable[16] = {0.000933785, 0.002895296, 0.004997030, 0.007260591, 0.009712991, 0.012388631, 0.015332262, 0.018603605, 0.022284874, 0.026493722, 0.031407077, 0.037309157, 0.044700757, 0.054597000, 0.069621283, 0.101933409};

RegionShapeDescriptor::RegionShapeDescriptor()
{
	this->resetDescriptor();
}

//----------------------------------------------------------------------------
RegionShapeDescriptor::~RegionShapeDescriptor()
{
}

void RegionShapeDescriptor::resetDescriptor()
{
    for(int i = 0; i < ART_ANGULAR; i++)
        for(int j = 0; j < ART_RADIAL; j++)
            m_ArtDE[i][j] = 0;

}

//----------------------------------------------------------------------------
bool RegionShapeDescriptor::SetElement(int p, int r, char value)
{
	if (p<0 || p>=ART_ANGULAR || r<0 || r>=ART_RADIAL)
		return false;

	m_ArtDE[p][r] = value;
	return true;
}

//----------------------------------------------------------------------------
bool RegionShapeDescriptor::SetElement(int p, int r, double value)
{
	if (p<0 || p>=ART_ANGULAR || r<0 || r>=ART_RADIAL ||
	    value>1.0 || value<0.0)
		return false;

	// Quantization
 	int high = sizeof(QuantTable)/sizeof(double);
	int low = 0;
	int middle;

	while(high-low > 1)
	{
		middle = (high+low)/2;

		if(QuantTable[middle] < value)
			low = middle;
		else
			high = middle;
	}

	m_ArtDE[p][r] = low;

	return true;
}

//----------------------------------------------------------------------------
char RegionShapeDescriptor::GetElement(int p, int r)
{
	if (p<0 || p>=ART_ANGULAR || r<0 || r>=ART_RADIAL)
		return -1;

	return m_ArtDE[p][r];		// Always positive value
}

//----------------------------------------------------------------------------
double RegionShapeDescriptor::GetRealValue(int p, int r)
{
	if (p<0 || p>=ART_ANGULAR || r<0 || r>=ART_RADIAL)
		return -1;

	// Inverse Quantization
	return IQuantTable[(int)m_ArtDE[p][r]];	// Always positive value
}

/*
//----------------------------------------------------------------------------
unsigned long RegionShapeDescriptor::ExportDDL(GenericDSInterfaceABC *aParentDescription)
{
	if (!aParentDescription)
		return (unsigned long)-1;

	GenericDS l_DDLDescription;
	l_DDLDescription = aParentDescription->CreateChild("Descriptor");
	l_DDLDescription.SetAttribute("xsi:type", "RegionShapeType");

	GenericDS ArtDE_element = l_DDLDescription.CreateChild("MagnitudeOfART");

	vector<int> V(35);

	int i,j;
	int n = 0;
	for(i=0 ; i<ART_ANGULAR ; i++)
	for(j=0 ; j<ART_RADIAL ; j++)
	if(i!=0 || j!= 0)
	{
		V[n++] = GetElement(i, j);
	}
	ArtDE_element.SetValue(V);

	return 0;
}

//----------------------------------------------------------------------------
unsigned long RegionShapeDescriptor::ImportDDL(GenericDSInterfaceABC *aDescription)
{
	if (!aDescription)
		return (unsigned long)-1;

	GenericDS l_DDLDescription;
	GenericDSInterfaceABC *l_DDLDescriptionInterface = NULL;

	string xsitype;
	if (aDescription->GetDSName() == "Descriptor")
	{
		aDescription->GetTextAttribute("xsi:type", xsitype);
		if (xsitype == "RegionShapeType")
		{
			l_DDLDescriptionInterface = aDescription;
		}
	}

	if (!l_DDLDescriptionInterface)
	{
		l_DDLDescription = aDescription->GetDescription("Descriptor");

		while (!l_DDLDescription.isNull())
		{
			l_DDLDescription.GetTextAttribute("xsi:type", xsitype);
			if (xsitype == "RegionShapeType")
				break;
			l_DDLDescription = l_DDLDescription.GetNextSibling("Descriptor");
		}

		if (l_DDLDescription.isNull())
			return (unsigned long)-1;

		l_DDLDescriptionInterface = l_DDLDescription.GetInterface();
	}

	GenericDS ArtDE_element = l_DDLDescriptionInterface->GetDescription("MagnitudeOfART");

	vector<int> V;
	ArtDE_element.GetIntVector(V);

	int i, j;
	int n = 0;
	for(i=0 ; i<ART_ANGULAR ; i++)
	for(j=0 ; j<ART_RADIAL ; j++)
	if(i!=0 || j!= 0)
	{
		SetElement((char)i, (char)j, (char)V[n++]);
	}

	return 0;
}
*/
