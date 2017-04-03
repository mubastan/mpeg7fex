#include <cstdio>
#include <cstring>
#include <cstdlib>

#include "EdgeHist.h"

using namespace XM;

// Quantization quantization tables
double EdgeHistogramDescriptor::QuantTable[5][8] = {
  {0.010867,0.057915,0.099526,0.144849,0.195573,0.260504,0.358031,0.530128},
  {0.012266,0.069934,0.125879,0.182307,0.243396,0.314563,0.411728,0.564319},
  {0.004193,0.025852,0.046860,0.068519,0.093286,0.123490,0.161505,0.228960},
  {0.004174,0.025924,0.046232,0.067163,0.089655,0.115391,0.151904,0.217745},
  {0.006778,0.051667,0.108650,0.166257,0.224226,0.285691,0.356375,0.450972},
};

EdgeHistogramDescriptor::EdgeHistogramDescriptor()
{
  m_pEdge_Histogram = 0;
  m_pEdge_HistogramElement = 0;
  m_pEdge_Histogram = new EHD[1];
  m_pEdge_HistogramElement = new char[80];

  // set values to zero
  this->resetDescriptor();

}

//----------------------------------------------------------------------------
EdgeHistogramDescriptor::~EdgeHistogramDescriptor()
{
  if( m_pEdge_Histogram )
    delete [] m_pEdge_Histogram;
  if( m_pEdge_HistogramElement )
    delete [] m_pEdge_HistogramElement;
}


//----------------------------------------------------------------------------
// size of descriptor is fixed at 80
unsigned long EdgeHistogramDescriptor::GetSize(void)
{ return (unsigned long)80; }

//----------------------------------------------------------------------------
EHD* EdgeHistogramDescriptor::GetEdgeHistogram()
{
	return	m_pEdge_Histogram;
}

//----------------------------------------------------------------------------
double*	EdgeHistogramDescriptor::GetEdgeHistogramD()
{
	double* ehd = m_pEdge_Histogram->Local_Edge;
	return	ehd;
}



//----------------------------------------------------------------------------
char*	EdgeHistogramDescriptor::GetEdgeHistogramElement()
{
	return	m_pEdge_HistogramElement;
}

//----------------------------------------------------------------------------
void EdgeHistogramDescriptor::SetEdgeHistogramElement(char*	pEdgeHistogram)
{
	for( int i = 0; i < 80; i++ )
		m_pEdge_Histogram->Local_Edge[i] = QuantTable[ i%5 ][ (int)pEdgeHistogram[i] ];

	memcpy(m_pEdge_HistogramElement, pEdgeHistogram, 80);
}

//----------------------------------------------------------------------------
void EdgeHistogramDescriptor::SetEdgeHistogram(EHD*	pEdge_Histogram)
{
  int i, j;
  double iQuantValue;

  for( i=0; i < 80; i++ ) {
    j=0;
    while(1){
      if( j < 7 ) // SIZI-1
        iQuantValue = (QuantTable[i%5][j]+QuantTable[i%5][j+1])/2.0;
      else
        iQuantValue = 1.0;
      if(pEdge_Histogram->Local_Edge[i] <= iQuantValue)
        break;
      j++;
    }
    m_pEdge_HistogramElement[i] = j;
  }
  for( i = 0; i < 80; i++ ){
    m_pEdge_Histogram->Local_Edge[i] = QuantTable[ i%5 ][ (int)m_pEdge_HistogramElement[i] ];
  }
}

// reset descriptor values to zero - by mb
void EdgeHistogramDescriptor::resetDescriptor()
{
	memset( m_pEdge_HistogramElement, 0, 80 );						// char* m_pEdge_HistogramElement;
	memset( m_pEdge_Histogram->Local_Edge, 0, 80*sizeof(double) );	//	double Local_Edge[80];
}

float EdgeHistogramDescriptor::distance(EdgeHistogramDescriptor* ehd)
{
	char* query = this->m_pEdge_HistogramElement;
	char* test = ehd->m_pEdge_HistogramElement;

    int size = 80;

	float dist = 0.0f;
	int i, j;
	for( i = 0; i < size; i++ ){
		dist += (float)abs(query[i] - test[i]);
	}

	// global: 5 directions 4x4 = 16 blocks => 80
	int gvalues1[5];
	memset(gvalues1, 0, 5*sizeof(int));

	for( j = 0; j < 80; j += 5 ) {
	  for( i = 0; i < 5; i++) {
	    gvalues1[i] += query[ j + i ];
	  }
	}
	for( i = 0; i < 5; i++) {
	    gvalues1[i] = gvalues1[i]*5.0/16;
	}

	int gvalues2[5];
	memset(gvalues2, 0, 5*sizeof(int));
	for( j = 0; j < 80; j += 5 ) {
	  for( i = 0; i < 5; i++) {
	    gvalues2[i] += test[ j + i ];
	  }
	}
	for( i = 0; i < 5; i++) {
	    gvalues2[i] = gvalues2[i]*5.0f/16;
	}

	// add to distance
	for( i = 0; i < 5; i++) {
	    dist += abs( gvalues1[i] - gvalues2[i] );
	}

	return dist;
}

#define EHD_MAX_DIST 554.0f
float EdgeHistogramDescriptor::distanceNorm(EdgeHistogramDescriptor* ehd)
{
    return distance(ehd)/EHD_MAX_DIST;
}


/*
//----------------------------------------------------------------------------
unsigned long EdgeHistogramDescriptor::
ExportDDL(GenericDSInterfaceABC *aParentDescription)
{
  GenericDS l_DDLDescription;
  int i;

  if(!aParentDescription) return (unsigned long) -1;

  l_DDLDescription=aParentDescription->CreateChild("Descriptor");
  l_DDLDescription.SetAttribute("xsi:type","EdgeHistogramType");

  vector<int> BinCounts(80);
  for(i=0; i<80; i++)
    BinCounts[i]=m_pEdge_HistogramElement[i];
  GenericDS EdgeHistogramDDLDescription=l_DDLDescription.CreateChild("BinCounts");
  EdgeHistogramDDLDescription.SetValue(BinCounts);

  return 0;
}

//-----------------------------------------------------------------------------
unsigned long EdgeHistogramDescriptor::
ImportDDL(GenericDSInterfaceABC *aDescription)
{
  string xsitype;

  GenericDS l_DDLDescription;
  GenericDSInterfaceABC *l_DDLDescriptionInterface    = NULL;

  if (!aDescription) return (unsigned long) -1;

  // if aDescriptor is of correct type
  if (aDescription->GetDSName() == "Descriptor") {
    aDescription->GetTextAttribute("xsi:type",xsitype);
    if (xsitype == "EdgeHistogramType") {
      l_DDLDescriptionInterface=aDescription;
    }
  }

  // else search for DummyType as a child
  if (!l_DDLDescriptionInterface) {
    l_DDLDescription=aDescription->GetDescription("Descriptor");

    // search for correct xsi type
    while (!l_DDLDescription.isNull()) {
      l_DDLDescription.GetTextAttribute("xsi:type", xsitype);
      if(xsitype=="EdgeHistogramType") break;
      l_DDLDescription=
	l_DDLDescription.GetNextSibling("Descriptor");
    }


    // DummyType not found
    if (l_DDLDescription.isNull()) return (unsigned long)-1;
    // DummyType found
    l_DDLDescriptionInterface=
      l_DDLDescription.GetInterface();
  }

  char tempBins[80];
  vector<int> BinCounts;

  GenericDS EdgeHistogramDDLDescription=l_DDLDescriptionInterface->GetDescription("BinCounts");
  EdgeHistogramDDLDescription.GetIntVector(BinCounts);
  for(int i=0; i<80; i++) tempBins[i] = (char)BinCounts[i];
    SetEdgeHistogramElement(tempBins);
  return 0;
}
*/
