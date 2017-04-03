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
// Applicable File Name:  ContourShape.cpp
//
///////////////////////////////////////////////////////////////////////////////
#include <cstdio>
#include <cstring>

#include "ContourShape.h"

using namespace XM;


ContourShapeDescriptor::ContourShapeDescriptor():
m_cPeaksCount(0),
m_iHighestPeakY(0),
m_pPeak(0)
{
  m_piGlobalCurvatureVector[0] = m_piGlobalCurvatureVector[1] = 0;
  m_piPrototypeCurvatureVector[0] = m_piPrototypeCurvatureVector[1] = 0;

}

//-----------------------------------------------------------------------------
ContourShapeDescriptor::~ContourShapeDescriptor()
{
  // Free peak memory (if allocated)
  if (m_cPeaksCount > 0)
  {
    delete [] m_pPeak;
  }
}

/*
//-----------------------------------------------------------------------------
unsigned long ContourShapeDescriptor::ExportDDL(GenericDSInterfaceABC *aParentDescription)
{
  if (!aParentDescription)
    return (unsigned long)-1;

  GenericDS l_DDLDescription;
  l_DDLDescription = aParentDescription->CreateChild("Descriptor");
  l_DDLDescription.SetAttribute("xsi:type", "ContourShapeType");

  unsigned int num = GetNoOfPeaks();

  GenericDS GlobalCurvature_element = l_DDLDescription.CreateChild("GlobalCurvature");
  unsigned long lgcv[2];
  GetGlobalCurvature(lgcv[0], lgcv[1]);
  vector<int> gcv(2);
  gcv[0] = (int)lgcv[0];
  gcv[1] = (int)lgcv[1];
  GlobalCurvature_element.SetValue(gcv);

  if (num > 0)
  {
    GenericDS PrototypeCurvature_element = l_DDLDescription.CreateChild("PrototypeCurvature");
    unsigned long lpcv[2];
    GetPrototypeCurvature(lpcv[0], lpcv[1]);
    vector<int> pcv(2);
    pcv[0] = (int)lpcv[0];
    pcv[1] = (int)lpcv[1];
    PrototypeCurvature_element.SetValue(pcv);
  }

  GenericDS HighestPeak_element = l_DDLDescription.CreateChild("HighestPeakY");
  HighestPeak_element.SetValue((int)GetHighestPeakY());

  for (unsigned int i = 1; i < num; i++)
  {
    unsigned short xp, yp;

    GenericDS Peak_element = l_DDLDescription.CreateChild("Peak");
    GetPeak(i, xp, yp);
    Peak_element.SetAttribute("peakX", xp);
    Peak_element.SetAttribute("peakY", yp);
  }

  return 0;
}

//-----------------------------------------------------------------------------
unsigned long ContourShapeDescriptor::ImportDDL(GenericDSInterfaceABC *aDescription)
{
  if (!aDescription)
    return (unsigned long)-1;

  GenericDS l_DDLDescription;
  GenericDSInterfaceABC *l_DDLDescriptionInterface = NULL;

  string xsitype;
  if (aDescription->GetDSName() == "Descriptor")
  {
    aDescription->GetTextAttribute("xsi:type", xsitype);
    if (xsitype == "ContourShapeType")
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
      if (xsitype == "ContourShapeType")
        break;
      l_DDLDescription = l_DDLDescription.GetNextSibling("Descriptor");
    }

    if (l_DDLDescription.isNull())
      return (unsigned long)-1;

    l_DDLDescriptionInterface = l_DDLDescription.GetInterface();
  }

  GenericDS GlobalCurvature_element = l_DDLDescriptionInterface->GetDescription("GlobalCurvature");
  vector<int> gcv;
  GlobalCurvature_element.GetIntVector(gcv);
  SetGlobalCurvature((unsigned long)gcv[0], (unsigned long)gcv[1]);

  GenericDS PrototypeCurvature_element = l_DDLDescriptionInterface->GetDescription("PrototypeCurvature");
  if (!PrototypeCurvature_element.isNull())
  {
    vector<int> pcv;
    PrototypeCurvature_element.GetIntVector(pcv);
    SetPrototypeCurvature((unsigned long)pcv[0], (unsigned long)pcv[1]);
  }
  else
    SetPrototypeCurvature(0, 0);

  GenericDS HighestPeak_element = l_DDLDescriptionInterface->GetDescription("HighestPeakY");
  int peak0;
  HighestPeak_element.GetIntValue(peak0);
  SetHighestPeakY(peak0);

  // Now that the 'numberOfPeaks' attribute has been removed, we need this
  // special case to check for the case where there are zero peaks.

  if (peak0 > 0)
  {
    // The SetNoOfPeaks() call must be done BEFORE reading the peak data, in
    // order to allocate the memory, so we step through the Peak elements
    // here first...

    int numPeaks = 1;
    GenericDS Peak_element = l_DDLDescriptionInterface->GetDescription("Peak");
    while (!Peak_element.isNull())
    {
      Peak_element = Peak_element.GetNextSibling("Peak");
      numPeaks++;
    }
    SetNoOfPeaks(numPeaks);

    // And now step through properly to actually read and store the data...

    int i = 1;
    Peak_element = l_DDLDescriptionInterface->GetDescription("Peak");
    while (!Peak_element.isNull())
    {
      int xp, yp;
      Peak_element.GetIntAttribute("peakX", xp);
      Peak_element.GetIntAttribute("peakY", yp);
      SetPeak(i++, (unsigned short)xp, (unsigned short)yp);
      Peak_element = Peak_element.GetNextSibling("Peak");
    }

    // And finally, set the zeroth peak (same as the highest one)

    SetPeak(0, 0, peak0);
  }

  // If the highest peak has a height of zero, this implies NO peaks

  else
    SetNoOfPeaks(0);

  return 0;
}
*/

// print the descriptor
void ContourShapeDescriptor:: Print()
{

	  printf("Contour Shape Descriptor\n");

	  unsigned long lgcv[2];
	  GetGlobalCurvature(lgcv[0], lgcv[1]);
	  printf("Global Curvature: %d  %d\n", (int)lgcv[0], (int)lgcv[1]);

	  unsigned int num = GetNoOfPeaks();
	  if (num > 0)
	  {
		unsigned long lpcv[2];
		GetPrototypeCurvature(lpcv[0], lpcv[1]);
		printf("Prototype Curvature: %d  %d\n", (int)lpcv[0], (int)lpcv[1]);

	  }

	  printf("Highest Peak Y: %d\n", (int)GetHighestPeakY());

	  if (num > 0)
		printf("Peaks:\n");

	  for (unsigned int i = 1; i < num; i++)
	  {
		unsigned short xp, yp;

		GetPeak(i, xp, yp);
		printf("Peak X: %d\n", xp );
		printf("Peak Y: %d\n", yp );
	  }

}

//-----------------------------------------------------------------------------
void ContourShapeDescriptor::SetNoOfPeaks(unsigned char cPeaks)
{
  unsigned char cOldPeaks = m_cPeaksCount;

  // Only 5 bits used so mask out rest

  if (cPeaks > CONTOURSHAPE_CSSPEAKMASK)
    m_cPeaksCount = CONTOURSHAPE_CSSPEAKMASK;
  else
    m_cPeaksCount = cPeaks;

  // Manage peak memory
  if (m_cPeaksCount != cOldPeaks)
  {
    if (m_pPeak)
      delete [] m_pPeak;
    m_pPeak = NULL;
    if (m_cPeaksCount > 0)
    {
      m_pPeak = (unsigned short*) new unsigned short[m_cPeaksCount * 2];
	  memset( m_pPeak, 0, m_cPeaksCount * 2 * sizeof(unsigned short) );		// added by mb
    }
  }
  else
  {
    memset(m_pPeak, 0, m_cPeaksCount * sizeof(unsigned short) * 2);
  }
}

//-----------------------------------------------------------------------------
unsigned char ContourShapeDescriptor::GetNoOfPeaks() const
{
  return m_cPeaksCount;
}

//-----------------------------------------------------------------------------
void ContourShapeDescriptor::SetGlobalCurvature(unsigned long lC, unsigned long lE)
{
  m_piGlobalCurvatureVector[0] = lC;
  m_piGlobalCurvatureVector[1] = lE;
}

//-----------------------------------------------------------------------------
void ContourShapeDescriptor::GetGlobalCurvature(unsigned long &lC, unsigned long &lE) const
{
  lC = m_piGlobalCurvatureVector[0];
  lE = m_piGlobalCurvatureVector[1];
}

//-----------------------------------------------------------------------------
void ContourShapeDescriptor::SetPrototypeCurvature(unsigned long lC, unsigned long lE)
{
  if (m_cPeaksCount > 0)
  {
    m_piPrototypeCurvatureVector[0] = lC;
    m_piPrototypeCurvatureVector[1] = lE;
  }
}

//-----------------------------------------------------------------------------
void ContourShapeDescriptor::GetPrototypeCurvature(unsigned long &lC, unsigned long &lE) const
{
  if (m_cPeaksCount > 0)
  {
    lC = m_piPrototypeCurvatureVector[0];
    lE = m_piPrototypeCurvatureVector[1];
  }
  else
  {
    lC = lE = 0;
  }
}

//-----------------------------------------------------------------------------
void ContourShapeDescriptor::SetHighestPeakY(unsigned short iHigh)
{
  m_iHighestPeakY = iHigh;
}

//-----------------------------------------------------------------------------
unsigned short ContourShapeDescriptor::GetHighestPeakY() const
{
  return m_iHighestPeakY;
}

//-----------------------------------------------------------------------------
void ContourShapeDescriptor::SetPeak(unsigned char cIndex,
				     unsigned short iX, unsigned short iY)
{
  unsigned char cOffset = cIndex * 2;
  if (cIndex < m_cPeaksCount)
  {
    m_pPeak[cOffset    ] = iX;
    m_pPeak[cOffset + 1] = iY;
  }
}

//-----------------------------------------------------------------------------
void ContourShapeDescriptor::GetPeak(unsigned char cIndex,
				     unsigned short &iX,unsigned  short &iY)
{
  unsigned char cOffset = cIndex * 2;
  if (cIndex < m_cPeaksCount)
  {
    iX = m_pPeak[cOffset    ];
    iY = m_pPeak[cOffset + 1];
  }
  else
  {
    iX = iY = 0;
  }
}


