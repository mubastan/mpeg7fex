#include <vector>
#include <iostream>
#include <cstdio>
#include <cstring>
#include <cstdlib>

#include "ScalableColor.h"

using namespace XM;


ScalableColorDescriptor::ScalableColorDescriptor():
m_ScalableHistogram(0),
m_NumberOfCoefficients(0),
m_NumberOfBitplanesDiscarded(0)
{
}

//----------------------------------------------------------------------------
ScalableColorDescriptor::~ScalableColorDescriptor()
{

	if (m_ScalableHistogram) delete [] m_ScalableHistogram;

}

//----------------------------------------------------------------------------
unsigned long* ScalableColorDescriptor::GetScalableHistogram(void)
{
	return m_ScalableHistogram;
}

//----------------------------------------------------------------------------
unsigned long ScalableColorDescriptor::GetNumberOfCoefficients(void)
{
	return m_NumberOfCoefficients;
}

//----------------------------------------------------------------------------
unsigned long ScalableColorDescriptor::SetNumberOfCoefficients(unsigned long newSize)
{
	if (m_NumberOfCoefficients == newSize) return 0;

	if (m_ScalableHistogram) {
	  delete [] m_ScalableHistogram;
	  delete [] m_Sign;
	}
	m_ScalableHistogram = new unsigned long [newSize];
	//memset( m_ScalableHistogram, 0,  newSize*sizeof(unsigned long));

	m_Sign = new unsigned long [newSize];
	//memset( m_Sign, 0,  newSize*sizeof(unsigned long));

	m_NumberOfCoefficients = newSize;

	this->resetDescriptor();

	return 0;
}

//----------------------------------------------------------------------------
unsigned long ScalableColorDescriptor::SetNumberOfBitplanesDiscarded(unsigned long aRenorm)
{
	if(aRenorm<0 || aRenorm==5 || aRenorm==7 || aRenorm>8)
	  return (unsigned long)-1;
	m_NumberOfBitplanesDiscarded = aRenorm;

	return 0;
}

//----------------------------------------------------------------------------
unsigned long ScalableColorDescriptor::GetNumberOfBitplanesDiscarded()
{
	return m_NumberOfBitplanesDiscarded;
}

//----------------------------------------------------------------------------
unsigned long ScalableColorDescriptor::SetCoefficient(unsigned long index, int value)
{
	if (index < 0 || index >= m_NumberOfCoefficients)
	  return (unsigned long)-1;
	//m_ScalableHistogram[index] = value;
	m_ScalableHistogram[index] = value >=0 ? value : -value;		// by mb
	m_Sign[index] = value<0?0:1;

	return 0;
}

//----------------------------------------------------------------------------
unsigned long ScalableColorDescriptor::SetCoeffValue(unsigned long index, int value)
{
	if (index < 0 || index >= m_NumberOfCoefficients)
	  return (unsigned long)-1;
	m_ScalableHistogram[index] = value;

	return 0;
}

//----------------------------------------------------------------------------
unsigned long ScalableColorDescriptor::GetCoefficient(unsigned long index)
{
	if (index < 0 || index >= m_NumberOfCoefficients)
	  return (unsigned long)-1;

	return m_ScalableHistogram[index];
}

//----------------------------------------------------------------------------
unsigned long ScalableColorDescriptor::GetCoeffSign(unsigned long index)
{
	if (index < 0 || index >= m_NumberOfCoefficients)
	  return (unsigned long)-1;

	return m_Sign[index];
}

//----------------------------------------------------------------------------
unsigned long ScalableColorDescriptor::SetCoeffSign(unsigned long index, int value)
{
	if (index < 0 || index >= m_NumberOfCoefficients)
	  return (unsigned long)-1;
	m_Sign[index]=value<0?0:1;
	return 0;
}

// set descriptor values to zero
void ScalableColorDescriptor::resetDescriptor()
{
	memset( m_ScalableHistogram, 0,  this->m_NumberOfCoefficients*sizeof(unsigned long));
	memset( m_Sign, 0,  this->m_NumberOfCoefficients*sizeof(unsigned long));
}

float ScalableColorDescriptor::distance(ScalableColorDescriptor* scd)
{
    float dist = 0.0f;
	for( int i = 0; i < m_NumberOfCoefficients; i++ )
	{
		dist += abs( m_Sign[i]*m_ScalableHistogram[i] - scd->m_Sign[i]*scd->m_ScalableHistogram[i] );
	}
    return dist;
}

#define SCD_MAX_DIST 1549.0f
float ScalableColorDescriptor::distanceNorm(ScalableColorDescriptor* scd)
{
    return distance(scd)/SCD_MAX_DIST;
}

/*
//-----------------------------------------------------------------------------
unsigned long ScalableColorDescriptor::
ExportDDL(GenericDSInterfaceABC *aParentDescription)
{
  if (!aParentDescription)
    return (unsigned long)-1;

  GenericDS l_DDLDescription;
  l_DDLDescription = aParentDescription->CreateChild("Descriptor");
  l_DDLDescription.SetAttribute("xsi:type", "ScalableColorType");
//////////////////////////

	int NumberOfCoefficients=atoi(getpara("NumberOfCoefficients","256"));

	if((NumberOfCoefficients!=16)&&(NumberOfCoefficients!=32)&&
	   (NumberOfCoefficients!=64)&&(NumberOfCoefficients!=128)&&
		 (NumberOfCoefficients!=256))
	return (unsigned long)-1;

	int anzkoeff = 0;
	//if(NumberOfCoefficients==16) anzkoeff=0;
	if(NumberOfCoefficients==32) anzkoeff=1;
	if(NumberOfCoefficients==64) anzkoeff=2;
	if(NumberOfCoefficients==128) anzkoeff=3;
	if(NumberOfCoefficients==256) anzkoeff=4;
  l_DDLDescription.SetAttribute("NumberOfCoefficients", anzkoeff);

	//put NumberOfRedBits
	int NumberOfBitplanesDiscarded = GetNumberOfBitplanesDiscarded();
	int anzredbits = -1;
	if((NumberOfBitplanesDiscarded>=0)&&(NumberOfBitplanesDiscarded<5))
		anzredbits=NumberOfBitplanesDiscarded;
	if(NumberOfBitplanesDiscarded==6) anzredbits=5;
	if(NumberOfBitplanesDiscarded>6) anzredbits=6;
  l_DDLDescription.SetAttribute("NumberOfBitplanesDiscarded", anzredbits);


  GenericDS Coeff_element = l_DDLDescription.CreateChild("Coefficients");

	vector<int> element(NumberOfCoefficients);
	for (int i=0; i<NumberOfCoefficients; i++)
	{
		int no_of_bits=XM::scalableColorQuantValues[i][1]-1-NumberOfBitplanesDiscarded;

		if(no_of_bits<=0) no_of_bits = 1;

		element[i] = GetCoefficient(i);// >> (sizeof(long) - no_of_bits)
	}

	Coeff_element.SetValue(element);

  return 0;
}

//-----------------------------------------------------------------------------
unsigned long ScalableColorDescriptor::ImportDDL(GenericDSInterfaceABC
																								 *aDescription)
{
  if (!aDescription)
    return (unsigned long)-1;

  GenericDS l_DDLDescription;
  GenericDSInterfaceABC *l_DDLDescriptionInterface = NULL;

  string xsitype;
  if (aDescription->GetDSName() == "Descriptor")
  {
    aDescription->GetTextAttribute("xsi:type", xsitype);
    if (xsitype == "ScalableColorType")
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
      if (xsitype == "ScalableColorType")
        break;
      l_DDLDescription = l_DDLDescription.GetNextSibling("Descriptor");
    }

    if (l_DDLDescription.isNull())
      return (unsigned long)-1;

    l_DDLDescriptionInterface = l_DDLDescription.GetInterface();
  }

  int anzkoeff;
  l_DDLDescriptionInterface->GetIntAttribute("NumberOfCoefficients",
					     anzkoeff);

  int NumberOfCoefficients=-1;
  if(anzkoeff==0) NumberOfCoefficients=16;
  if(anzkoeff==1) NumberOfCoefficients=32;
  if(anzkoeff==2) NumberOfCoefficients=64;
  if(anzkoeff==3) NumberOfCoefficients=128;
  if(anzkoeff==4) NumberOfCoefficients=256;
  if (!NumberOfCoefficients)
    return 0;
  SetNumberOfCoefficients(NumberOfCoefficients);

  //get normalization
  int anzredbits;
  l_DDLDescriptionInterface->GetIntAttribute("NumberOfBitplanesDiscarded",
					     anzredbits);

  int NumberOfBitplanesDiscarded=-1;
  if(anzredbits==0) NumberOfBitplanesDiscarded=0;
  if(anzredbits==1) NumberOfBitplanesDiscarded=1;
  if(anzredbits==2) NumberOfBitplanesDiscarded=2;
  if(anzredbits==3) NumberOfBitplanesDiscarded=3;
  if(anzredbits==4) NumberOfBitplanesDiscarded=4;
  if(anzredbits==5) NumberOfBitplanesDiscarded=6;
  if(NumberOfBitplanesDiscarded==-1) NumberOfBitplanesDiscarded=8;

  SetNumberOfBitplanesDiscarded(NumberOfBitplanesDiscarded);

  GenericDS Coeff_element = l_DDLDescriptionInterface->
    GetDescription("Coefficients");
  vector<int> element;
  Coeff_element.GetIntVector(element);

  for (int i=0; i<NumberOfCoefficients; i++)
    {
//		int no_of_bits =
//			XM::scalableColorQuantValues[i][1]-1-NumberOfBitplanesDiscarded;
      int coeff = element[i]; // << (sizeof(long) - no_of_bits)
      SetCoefficient(i,coeff);
    }

  return 0;
}
*/

// added by mb - 26.06.2008
void ScalableColorDescriptor::Print()
{

	std::cerr << "\nScalable Color Descriptor"<< std::endl;
	std::cerr << "\nm_NumberOfCoefficients: "<< this->GetNumberOfCoefficients()<<std::endl;
	std::cerr << "m_NumberOfBitplanesDiscarded: " << this->GetNumberOfBitplanesDiscarded() << std::endl;

	int NumberOfCoefficients = this->GetNumberOfCoefficients();

	//vector<int> element(NumberOfCoefficients);
	std::cerr<<"Coefficients: ";
	for (int i=0; i<NumberOfCoefficients; i++)
	{
		//int no_of_bits=XM::scalableColorQuantValues[i][1]-1-NumberOfBitplanesDiscarded;

		//if(no_of_bits<=0) no_of_bits = 1;

		std::cerr << (int) GetCoefficient(i) <<" ";	// >> (sizeof(long) - no_of_bits)
	}
	std::cerr<<std::endl;
}


