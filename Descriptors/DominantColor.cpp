///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// University of California Santa Barbara, Kapil Chhabra, Yining Deng.
// Mitsubishi Electric ITE-VIL, Leszek Cieplinski.
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
// Applicable File Name:  DominantColor.cpp
//
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <vector>
#include <iostream>

#include "DominantColor.h"

using namespace XM;


DominantColorDescriptor::DominantColorDescriptor():
m_DominantColorsNumber(0),
m_ColorSpacePresent(0),
m_ColorQuantizationPresent(0),
m_VariancePresent(1),
m_SpatialCoherency(0),
m_dc(0),
m_ColorSpace(0),
m_Quantizer(0)
{
    m_ColorSpace = new ColorSpaceDescriptor();;
    m_Quantizer = new ColorQuantizerDescriptor();

}


//----------------------------------------------------------------------------
DominantColorDescriptor::~DominantColorDescriptor()
{
  // will be released by the clients!
}


/*
//----------------------------------------------------------------------------
unsigned long DominantColorDescriptor::
ExportDDL(GenericDSInterfaceABC *aParentDescription)
{
  int i, j;
  vector<int> cvi(3);
  vector<int> var(3);
  DOMCOL *DC;

  GenericDS l_DDLDescription;
  GenericDSInterfaceABC *l_DDLDescriptionInterface;
  GenericDS SpatCoher, V, Per, ColValInd, Var;

  if(!aParentDescription) return (unsigned long) -1;

  l_DDLDescription = aParentDescription->CreateChild("Descriptor");
  l_DDLDescription.SetAttribute("xsi:type","DominantColorType");
  int dc_number = GetDominantColorsNumber();
  //l_DDLDescription.SetAttribute("size", dc_number);

  l_DDLDescriptionInterface = l_DDLDescription.GetInterface();

  bool colspcpres = GetColorSpacePresent();
  if(colspcpres) m_ColorSpace->ExportDDL(l_DDLDescriptionInterface);

  bool colquantpres = GetColorQuantizationPresent();
  if(colquantpres) m_Quantizer->ExportDDL(l_DDLDescriptionInterface);

  bool varpres = GetVariancePresent();

  SpatCoher = l_DDLDescription.CreateChild("SpatialCoherency");
  int spatcoher = GetSpatialCoherency();
  SpatCoher.SetValue(spatcoher);

  DC = GetDominantColors();

  for(i=0; i<dc_number; i++) {
    V = l_DDLDescription.CreateChild("Value");

    Per = V.CreateChild("Percentage");
    Per.SetValue(DC[i].m_Percentage);

    ColValInd = V.CreateChild("Index");
    for(j=0; j<3; j++) cvi[j] = DC[i].m_ColorValue[j];
    ColValInd.SetValue(cvi);

    if(varpres==true) {
      Var = V.CreateChild("ColorVariance");
      for(j=0; j<3; j++) var[j] = DC[i].m_ColorVariance[j];
      Var.SetValue(var);
    }
  }

  return 0;
}

//----------------------------------------------------------------------------
unsigned long DominantColorDescriptor::
ImportDDL(GenericDSInterfaceABC *aDescription)
{
  int i, j, dc_number, spatcoher, per;
  string xsitype, colspcpres, colquantpres, varpres;

  GenericDS l_DDLDescription;
  GenericDSInterfaceABC *l_DDLDescriptionInterface = NULL;

  GenericDS V, Per, ColValInd, Var;
  int *percents, **colors, **variances;

  if(!aDescription) return (unsigned long) -1;

  // if aDescription is of correct type
  if(aDescription->GetDSName() == "Descriptor") {
    aDescription->GetTextAttribute("xsi:type", xsitype);
    if(xsitype == "DominantColorType") {
      l_DDLDescriptionInterface = aDescription;
    }
  }

  // else search for DominantColorType as a child
  if(!l_DDLDescriptionInterface) {
    l_DDLDescription = aDescription->GetDescription("Descriptor");

    // search for correct xsi type
    while(!l_DDLDescription.isNull()) {
      l_DDLDescription.GetTextAttribute("xsi:type", xsitype);
      if(xsitype == "DominantColorType") break;
      l_DDLDescription.GetNextSibling("Descriptor");
    }

    // DominantColorType not found
    if(!l_DDLDescription.isNull()) return (unsigned long) -1;
    // DominantColorType found
    l_DDLDescriptionInterface = l_DDLDescription.GetInterface();
  }

  l_DDLDescriptionInterface->GetIntAttribute("size",dc_number);
  SetDominantColorsNumber(dc_number);

  GenericDS ColSpc = l_DDLDescriptionInterface->GetDescription("ColorSpace");
  if(!ColSpc.isNull()) {
    SetColorSpacePresent(true);
    m_ColorSpace->ImportDDL(l_DDLDescriptionInterface);
  }
  else SetColorSpacePresent(false);

  GenericDS ColQuant = l_DDLDescriptionInterface->GetDescription("ColorQuantization");
  if(!ColQuant.isNull()) {
    SetColorQuantizationPresent(true);
    m_Quantizer->ImportDDL(l_DDLDescriptionInterface);
  }
  else SetColorQuantizationPresent(false);

  GenericDS VarPres = l_DDLDescriptionInterface->GetDescription("VariancePresent");
  VarPres.GetTextValue(varpres);
  if(varpres=="true") SetVariancePresent(true);
  else SetVariancePresent(false);

  GenericDS SpatCoher = l_DDLDescriptionInterface->GetDescription("SpatialCoherency");
  SpatCoher.GetIntValue(spatcoher);
  SetSpatialCoherency(spatcoher);

  // dominant colors
  percents = new int[dc_number];
  colors = new int*[dc_number];
  for(i=0;i<dc_number;i++) colors[i]= new int[3];
  variances = new int*[dc_number];
  for(i=0;i<dc_number;i++) variances[i]= new int[3];

  for(i=0; i<dc_number; i++) {
    vector<int> cvi, var;

    V = l_DDLDescriptionInterface->GetDescription("Values");

    Per = V.GetDescription("Percentage");
    Per.GetIntValue(per);
    percents[i] = per;

    ColValInd = V.GetDescription("ColorValueIndex");
    ColValInd.GetIntVector(cvi);
    for(j=0; j<3; j++) colors[i][j] = cvi[j];

    Var = V.GetDescription("ColorVariance");
    if(!Var.isNull()) {
      SetVariancePresent(true);
      Var.GetIntVector(var);
      for(j=0; j<3; j++) variances[i][j] = var[j];
    }
    else SetVariancePresent(false);

    l_DDLDescriptionInterface->RemoveChild(V);
  }

  SetDominantColors(percents, colors, variances);

  delete [] percents;
  for(i=0;i<dc_number;i++) delete [] colors[i];
  delete [] colors;
  for(i=0;i<dc_number;i++) delete [] variances[i];
  delete [] variances;

  return 0;
}
*/


//----------------------------------------------------------------------------
ColorQuantizerDescriptor * DominantColorDescriptor::
GetColorQuantizerDescriptor(void)
{
  return m_Quantizer;
}

//----------------------------------------------------------------------------
unsigned long DominantColorDescriptor::SetColorQuantizerDescriptor
        (ColorQuantizerDescriptor *quantizer)
{
  if (!quantizer) return (unsigned long) -1;
  if (m_Quantizer == quantizer) return 0;
  if (m_Quantizer) delete m_Quantizer;
  m_Quantizer = quantizer;

  return 0;
}

//----------------------------------------------------------------------------
ColorSpaceDescriptor* DominantColorDescriptor::GetColorSpaceDescriptor()
{
  return m_ColorSpace;
}

//----------------------------------------------------------------------------
unsigned long DominantColorDescriptor::
SetColorSpaceDescriptor(ColorSpaceDescriptor *colorspace)
{
  if (m_ColorSpace == colorspace) return 0;
  if (m_ColorSpace) delete m_ColorSpace;
  m_ColorSpace = colorspace;

  return 0;
}

//----------------------------------------------------------------------------
int DominantColorDescriptor::GetDominantColorsNumber()
{
  return (m_DominantColorsNumber);
}

//----------------------------------------------------------------------------
int DominantColorDescriptor::GetSpatialCoherency()
{
  return (m_SpatialCoherency);
}

//----------------------------------------------------------------------------
DOMCOL* DominantColorDescriptor::GetDominantColors()
{
  return (m_dc);
}

//----------------------------------------------------------------------------
void DominantColorDescriptor::SetDominantColorsNumber(int dc_number)
{
  if (dc_number != m_DominantColorsNumber) {
    if (m_dc)
      delete [] m_dc;
    m_dc = new DOMCOL[dc_number];

	// set values to zero - by mb - may not be crucial here since they are assigned in SetDominantColors() below (no aggregation, etc)
	this->resetDescriptor();

	m_DominantColorsNumber = dc_number;
  }
}

//----------------------------------------------------------------------------
void DominantColorDescriptor::SetSpatialCoherency(int sc)
{
  m_SpatialCoherency = sc;
}
#include <iostream>
//----------------------------------------------------------------------------
void DominantColorDescriptor::SetDominantColors(int *percents, int **colors, int **variances)
{
    int j;

    // count the colors with nonzero weight
    // to cancel the DCs with zero weight
    int index = 0;
    for( int i = 0; i < m_DominantColorsNumber; i++ )
    {
        if(percents[i] > 0)
        {
            m_dc[index].m_Percentage = percents[i];

            for(j=0;j<3;j++)
              m_dc[index].m_ColorValue[j] = colors[i][j];

            if( m_VariancePresent )
                for(j=0;j<3;j++)
                    m_dc[index].m_ColorVariance[j] = variances[i][j];
            index++;
        }
    }

    if(index < m_DominantColorsNumber)
    {
        //std::cerr << "index < mdc" << std::endl;
        m_DominantColorsNumber = index;
    }
}

void DominantColorDescriptor::resetDescriptor()
{
	for( int i = 0; i < m_DominantColorsNumber; i++ ){

		this->m_dc[i].m_Percentage = 0;
		memset( this->m_dc[i].m_ColorValue, 0, 3*sizeof(int) );
		memset( this->m_dc[i].m_ColorVariance, 0, 3*sizeof(int) );
	}

}

//----------------------------------------------------------------------------
void DominantColorDescriptor::Print()
{
  int i;

  std::cerr << "size:        " << m_DominantColorsNumber << " (m_DominantColorsNumber)" << std::endl;
  std::cerr << "sc:          " << m_SpatialCoherency << " (m_SpatialCoherency)" <<std::endl;
  std::cerr << "percentage   |||  color values  |||  color variances" << std::endl;
  for(i=0; i<m_DominantColorsNumber; i++)
    std::cerr << "value: " << m_dc[i].m_Percentage << "  |||  "
         << m_dc[i].m_ColorValue[0] << " "
         << m_dc[i].m_ColorValue[1] << " "
         << m_dc[i].m_ColorValue[2] << "   |||   "
		 << m_dc[i].m_ColorVariance[0] << " "
         << m_dc[i].m_ColorVariance[1] << " "
         << m_dc[i].m_ColorVariance[2] << std::endl;

}
