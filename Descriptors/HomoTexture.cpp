#include <cstdio>
#include <cstring>
#include <cstdlib>

#include "HomoTexture.h"

using namespace XM;

HomogeneousTextureDescriptor::HomogeneousTextureDescriptor(bool endevFlag )
{
    energydeviationflag = endevFlag;
}
//----------------------------------------------------------------------------
HomogeneousTextureDescriptor::~HomogeneousTextureDescriptor()
{
	this->resetDescriptor();
}

//----------------------------------------------------------------------------------------------
void HomogeneousTextureDescriptor ::
SetHomogeneousTextureFeature( bool endevFlag, int* pHomogeneousTextureFeature )
{
	energydeviationflag = endevFlag;
	// sizeof (feature): total number of bytes in the array `feature`
	memcpy(feature, pHomogeneousTextureFeature, sizeof (feature));
}

//----------------------------------------------------------------------------
int* HomogeneousTextureDescriptor::GetHomogeneousTextureFeature()
{
	return feature;
}

//----------------------------------------------------------------------------
int HomogeneousTextureDescriptor::GetHomogeneousTextureFeatureFlag()
{
	return energydeviationflag;
}

// reset feature vector values to 0
void HomogeneousTextureDescriptor::resetDescriptor()
{
	memset( feature, 0, sizeof(feature) );

}

#define RadialDivision  5
#define AngularDivision 6
#define HTD_FULL_MAX_DIST	9237.0f
float HomogeneousTextureDescriptor::distance(HomogeneousTextureDescriptor* htd)
{
    float dist = 0.0f;
	static double wm[RadialDivision] = {0.42,1.00,1.00,0.08,1.00};
	static double wd[RadialDivision] = {0.32,1.00,1.00,1.00,1.00};
	//double wdc=0.28;
	//double wstd=0.22;

	int m, n;
	for( n = 0; n < RadialDivision; n++ )
	    for( m = 0; m < AngularDivision; m++ ) {
			dist += (float)( wm[n]*abs( this->feature[n*AngularDivision+m+2] - htd->feature[n*AngularDivision+m+2] ) )
					   + ( wd[n]*abs( this->feature[n*AngularDivision+m+30+2] - htd->feature[n*AngularDivision+m+30+2] ) );
		}

    return dist/HTD_FULL_MAX_DIST;

}

#define HTD_BASE_MAX_DIST	4600.0f
float HomogeneousTextureDescriptor::distanceBase(HomogeneousTextureDescriptor* htd)
{
    float dist = 0.0f;
	static double wm[RadialDivision] = {0.42,1.00,1.00,0.08,1.00};
	//double wd[RadialDivision] = {0.32,1.00,1.00,1.00,1.00};
	//double wdc=0.28;
	//double wstd=0.22;

	int m, n;
	for( n = 0; n < RadialDivision; n++ )
	    for( m = 0; m < AngularDivision; m++ ) {
			dist += (float)( wm[n]*abs( this->feature[n*AngularDivision+m+2] - htd->feature[n*AngularDivision+m+2] ) );

		}

    return dist/HTD_BASE_MAX_DIST;
}

//----------------------------------------------------------------------------
void HomogeneousTextureDescriptor::Print(void)
{

  printf("Homogeneous Texture Descriptor (HTD)\n");
  int* values = GetHomogeneousTextureFeature();
  printf("average: %3d\n", values[0]);
  printf("Standard Deviation: %3d\n",values[1]);

  printf("Energy: ");
  for (int i = 2; i < 32; i++ )
	  printf("%d ", values[i] );			// GetHomogeneousTextureFeature()[i+2]
  printf("\n");


  if (GetHomogeneousTextureFeatureFlag() == 1)
  {
	  printf("Energy Deviation: ");

	  for ( int i = 32; i < 62; i++)
		printf("%d ", values[i]);

      printf("\n");
  }

}


/*
//----------------------------------------------------------------------------
unsigned long HomogeneousTextureDescriptor::ExportDDL(GenericDSInterfaceABC *aParentDescription)
{
  GenericDS l_DDLDescription;

  if (!aParentDescription) return (unsigned long) -1;

  l_DDLDescription=aParentDescription->CreateChild("Descriptor");
  l_DDLDescription.SetAttribute("xsi:type","HomogeneousTextureType");

  vector<int> energy(30), energydeviation(30);

  GenericDS AverageDescription = l_DDLDescription.CreateChild("Average");
  AverageDescription.SetValue(GetHomogeneousTextureFeature()[0]);

  GenericDS StandardDeviationDescription = l_DDLDescription.CreateChild("StandardDeviation");
  StandardDeviationDescription.SetValue(GetHomogeneousTextureFeature()[1]);

  GenericDS EnergyDescription = l_DDLDescription.CreateChild("Energy");
  for (int i=0;i<30;i++)
	  energy[i] = GetHomogeneousTextureFeature()[i+2];
  EnergyDescription.SetValue(energy);

  if (GetHomogeneousTextureFeatureFlag() == 1)
  {
		GenericDS EnergyDeviationDescription = l_DDLDescription.CreateChild("EnergyDeviation");
		for (int i=0;i<30;i++)
			energydeviation[i] = GetHomogeneousTextureFeature()[i+32];
        EnergyDeviationDescription.SetValue(energydeviation);
  }
  return 0;
}

//----------------------------------------------------------------------------
unsigned long HomogeneousTextureDescriptor::
ImportDDL(GenericDSInterfaceABC *aDescription)
{
  string xsitype;

  GenericDS l_DLLDescription;
  GenericDSInterfaceABC *l_DDLDescriptionInterface    = NULL;

  if (!aDescription) return (unsigned long) -1;

  // if aDescriptor is of correct type
  if (aDescription->GetDSName() == "Descriptor") {
    aDescription->GetTextAttribute("xsi:type",xsitype);
    if (xsitype == "HomogeneousTextureType") {
      l_DDLDescriptionInterface=aDescription;
    }
  }

  // else search for HomogeneousTextureType as a child
  if (!l_DDLDescriptionInterface) {
    l_DLLDescription=aDescription->GetDescription("Descriptor");

    // search for correct xsi type
    while (!l_DLLDescription.isNull()) {
      l_DLLDescription.GetTextAttribute("xsi:type", xsitype);
      if(xsitype=="HomogeneousTextureType") break;
      l_DLLDescription=
	l_DLLDescription.GetNextSibling("Descriptor");
    }


    // HomogeneousTextureType not found
    if (l_DLLDescription.isNull()) return (unsigned long)-1;
    // HomogeneousTextureType found
    l_DDLDescriptionInterface=
      l_DLLDescription.GetInterface();
  }

  int energydeviationflag, average, standarddeviation;
  int feature[62];
  vector<int> energy, energydeviation;

  GenericDS AverageDescription = l_DDLDescriptionInterface->GetDescription("Average");
  AverageDescription.GetIntValue(average);
  feature[0] = average;

  GenericDS StandardDeviationDescription = l_DDLDescriptionInterface->GetDescription("StandardDeviation");
  StandardDeviationDescription.GetIntValue(standarddeviation);
  feature[1] = standarddeviation;

  GenericDS EnergyDescription = l_DDLDescriptionInterface->GetDescription("Energy");
  EnergyDescription.GetIntVector(energy);
  for (int i=0;i<30;i++)
	  feature[i+2] = energy[i];

  GenericDS EnergyDeviationDescription = l_DDLDescriptionInterface->GetDescription("EnergyDeviation");
  if (!EnergyDeviationDescription.isNull()) {
	 energydeviationflag = 1;
     EnergyDeviationDescription.GetIntVector(energydeviation);
	 for (int i=0;i<30;i++) feature[i+32] = energydeviation[i];
  }
  else {
	  energydeviationflag = 0;
	  for (int i=0;i<30;i++) feature[i+32] = 0;
  }
  SetHomogeneousTextureFeature(energydeviationflag, feature);
  return 0;
}
*/




