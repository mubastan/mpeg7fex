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
// Applicable File Name: ColorLayout.cpp
//
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <vector>

#include "ColorLayout.h"

using namespace XM;


ColorLayoutDescriptor::ColorLayoutDescriptor(int numYCoef, int numCCoef)
{
    m_y_coeff = 0;
    m_cb_coeff = 0;
    m_cr_coeff = 0;

	m_NumberOfYCoeff = numYCoef;
	m_NumberOfCCoeff = numCCoef;

	m_y_coeff = new int[ m_NumberOfYCoeff ];
	memset( m_y_coeff, 0, m_NumberOfYCoeff * sizeof(int) );		// added by mb

	m_cb_coeff = new int[ m_NumberOfCCoeff ];
	memset( m_cb_coeff, 0, m_NumberOfCCoeff * sizeof(int) );		// added by mb

	m_cr_coeff = new int[ m_NumberOfCCoeff ];
	memset( m_cr_coeff, 0, m_NumberOfCCoeff * sizeof(int) );		// added by mb

}

//----------------------------------------------------------------------------
ColorLayoutDescriptor::~ColorLayoutDescriptor()
{
	if(m_y_coeff) delete [] m_y_coeff;
	if(m_cb_coeff) delete [] m_cb_coeff;
	if(m_cr_coeff) delete [] m_cr_coeff;
}


/*
//----------------------------------------------------------------------------
unsigned long ColorLayoutDescriptor::
ExportDDL(GenericDSInterfaceABC *aParentDescription)
{
	if(!aParentDescription) return (unsigned long) -1;

	GenericDS DescriptorRoot;
	DescriptorRoot=aParentDescription->CreateChild("Descriptor"); // Yamada's memo: Why you can say Descriptor?

 	DescriptorRoot.SetAttribute("xsi:type","ColorLayoutType");

	int num;
	int *coeff, i;
	char name[256];

	GenericDS  YDCCoeff_element = DescriptorRoot.CreateChild("YDCCoeff");
	coeff=GetYCoeff();
	YDCCoeff_element.SetValue(coeff[0]);
	GenericDS  CbDCCoeff_element = DescriptorRoot.CreateChild("CbDCCoeff");
	coeff=GetCbCoeff();
	CbDCCoeff_element.SetValue(coeff[0]);
	GenericDS  CrDCCoeff_element = DescriptorRoot.CreateChild("CrDCCoeff");
	coeff=GetCrCoeff();
	CrDCCoeff_element.SetValue(coeff[0]);


	num=GetNumberOfYCoeff();
	coeff=GetYCoeff();
	vector<int> AC_Y(num-1);
	for(i=1; i<num; i++)
		AC_Y[i-1]=coeff[i];
	sprintf(name, "YACCoeff%d", num-1);
	GenericDS  YACCoeff_element = DescriptorRoot.CreateChild(name);
	YACCoeff_element.SetValue(AC_Y);

	num=GetNumberOfCCoeff();
	vector<int> AC_C(num-1);

	coeff=GetCbCoeff();
	for(i=1; i<num; i++)
		AC_C[i-1]=coeff[i];
	sprintf(name, "CbACCoeff%d", num-1);
	GenericDS  CbACCoeff_element = DescriptorRoot.CreateChild(name);
	CbACCoeff_element.SetValue(AC_C);

	coeff=GetCrCoeff();
	for(i=1; i<num; i++)
		AC_C[i-1]=coeff[i];
	sprintf(name, "CrACCoeff%d", num-1);
	GenericDS  CrACCoeff_element = DescriptorRoot.CreateChild(name);
	CrACCoeff_element.SetValue(AC_C);

	return 0;
}

//----------------------------------------------------------------------------
unsigned long ColorLayoutDescriptor::ImportDDL(GenericDSInterfaceABC *DescriptorRoot)
{
	int i;
	int val;
	string text;
	if(!DescriptorRoot) return (unsigned long)-1;


	//DescriptorRoot->GetTextAttribute("xsi:type", text);
	//if(text!="ColorLayoutType") return (unsigned long)-2;


	GenericDS l_DLLDescription;
	GenericDSInterfaceABC *l_DDLDescriptionInterface=NULL;

	if(DescriptorRoot->GetDSName()=="Descriptor"){
		// memo again: Why you can restrict the name only to Descriptor?
		DescriptorRoot->GetTextAttribute("xsi:type", text);
		if(text=="ColorLayoutType"){
			l_DDLDescriptionInterface=DescriptorRoot;
		}
	}
	if(!l_DDLDescriptionInterface){
		l_DLLDescription=DescriptorRoot->GetDescription("Descriptor");
		// search for correct xsi type
		while(!l_DLLDescription.isNull()){
			l_DLLDescription.GetTextAttribute("xsi:type", text);
			if(text=="ColorLayoutType") break;
			l_DLLDescription=l_DLLDescription.GetNextSibling("Descriptor");
		}
		// DummyType not found
		if(l_DLLDescription.isNull()) return (unsigned long)-1;
		// DummyType found
		l_DDLDescriptionInterface=l_DLLDescription.GetInterface();
	}

//sth if parent node gives itself check if an apropriate child node exists

	GenericDS YDCCoeff_element  = l_DDLDescriptionInterface->GetDescription("YDCCoeff");
	GenericDS CbDCCoeff_element  = l_DDLDescriptionInterface->GetDescription("CbDCCoeff");
	GenericDS CrDCCoeff_element  = l_DDLDescriptionInterface->GetDescription("CrDCCoeff");

	GenericDS YACCoeff2_element  = l_DDLDescriptionInterface->GetDescription("YACCoeff2");
	GenericDS YACCoeff5_element  = l_DDLDescriptionInterface->GetDescription("YACCoeff5");
	GenericDS YACCoeff9_element  = l_DDLDescriptionInterface->GetDescription("YACCoeff9");
	GenericDS YACCoeff14_element  = l_DDLDescriptionInterface->GetDescription("YACCoeff14");
	GenericDS YACCoeff20_element  = l_DDLDescriptionInterface->GetDescription("YACCoeff20");
	GenericDS YACCoeff27_element  = l_DDLDescriptionInterface->GetDescription("YACCoeff27");
	GenericDS YACCoeff63_element  = l_DDLDescriptionInterface->GetDescription("YACCoeff63");
	GenericDS *YACCoeff_element = &YACCoeff2_element;
	int numOfYCoeff=3;
	if(YACCoeff_element->isNull()){ YACCoeff_element=&YACCoeff5_element; numOfYCoeff=6; }
	if(YACCoeff_element->isNull()){ YACCoeff_element=&YACCoeff9_element; numOfYCoeff=10; }
	if(YACCoeff_element->isNull()){ YACCoeff_element=&YACCoeff14_element; numOfYCoeff=15; }
	if(YACCoeff_element->isNull()){ YACCoeff_element=&YACCoeff20_element; numOfYCoeff=21; }
	if(YACCoeff_element->isNull()){ YACCoeff_element=&YACCoeff27_element; numOfYCoeff=28; }
	if(YACCoeff_element->isNull()){ YACCoeff_element=&YACCoeff63_element; numOfYCoeff=64; }

	GenericDS CbACCoeff2_element  = l_DDLDescriptionInterface->GetDescription("CbACCoeff2");
	GenericDS CbACCoeff5_element  = l_DDLDescriptionInterface->GetDescription("CbACCoeff5");
	GenericDS CbACCoeff9_element  = l_DDLDescriptionInterface->GetDescription("CbACCoeff9");
	GenericDS CbACCoeff14_element  = l_DDLDescriptionInterface->GetDescription("CbACCoeff14");
	GenericDS CbACCoeff20_element  = l_DDLDescriptionInterface->GetDescription("CbACCoeff20");
	GenericDS CbACCoeff27_element  = l_DDLDescriptionInterface->GetDescription("CbACCoeff27");
	GenericDS CbACCoeff63_element  = l_DDLDescriptionInterface->GetDescription("CbACCoeff63");
	GenericDS *CbACCoeff_element = &CbACCoeff2_element;
	int numOfCCoeff=3;
	if(CbACCoeff_element->isNull()){ CbACCoeff_element=&CbACCoeff5_element; numOfCCoeff=6; }
	if(CbACCoeff_element->isNull()){ CbACCoeff_element=&CbACCoeff9_element; numOfCCoeff=10; }
	if(CbACCoeff_element->isNull()){ CbACCoeff_element=&CbACCoeff14_element; numOfCCoeff=15; }
	if(CbACCoeff_element->isNull()){ CbACCoeff_element=&CbACCoeff20_element; numOfCCoeff=21; }
	if(CbACCoeff_element->isNull()){ CbACCoeff_element=&CbACCoeff27_element; numOfCCoeff=28; }
	if(CbACCoeff_element->isNull()){ CbACCoeff_element=&CbACCoeff63_element; numOfCCoeff=64; }

	GenericDS CrACCoeff2_element  = l_DDLDescriptionInterface->GetDescription("CrACCoeff2");
	GenericDS CrACCoeff5_element  = l_DDLDescriptionInterface->GetDescription("CrACCoeff5");
	GenericDS CrACCoeff9_element  = l_DDLDescriptionInterface->GetDescription("CrACCoeff9");
	GenericDS CrACCoeff14_element  = l_DDLDescriptionInterface->GetDescription("CrACCoeff14");
	GenericDS CrACCoeff20_element  = l_DDLDescriptionInterface->GetDescription("CrACCoeff20");
	GenericDS CrACCoeff27_element  = l_DDLDescriptionInterface->GetDescription("CrACCoeff27");
	GenericDS CrACCoeff63_element  = l_DDLDescriptionInterface->GetDescription("CrACCoeff63");
	GenericDS *CrACCoeff_element;
	switch(numOfCCoeff){
	case  3: CrACCoeff_element = &CrACCoeff2_element; break;
	case  6: CrACCoeff_element = &CrACCoeff5_element; break;
	case 10: CrACCoeff_element = &CrACCoeff9_element; break;
	case 15: CrACCoeff_element = &CrACCoeff14_element; break;
	case 21: CrACCoeff_element = &CrACCoeff20_element; break;
	case 28: CrACCoeff_element = &CrACCoeff27_element; break;
	case 63: CrACCoeff_element = &CrACCoeff63_element; break;
	default: fprintf(stderr, "Invalid number of C Coeff.[%d]\n", numOfCCoeff);
	}
	if(CrACCoeff_element->isNull()){
		fprintf(stderr, "Invalid instance. The number of Cb and Cr coefficients should be synchronized.\n");
		exit(-1);
	}

	int *y_coeff=new int[numOfYCoeff];
	YDCCoeff_element.GetIntValue(val);
	y_coeff[0]=val;
	if(!YACCoeff_element->isNull()){
		vector<int> AC;
		if( YACCoeff_element->GetIntVector(AC)==0 ){
			for(i=1; i<numOfYCoeff; i++) y_coeff[i]=AC[i-1];
		}
	}
	SetNumberOfYCoeff(numOfYCoeff);
	SetYCoeff(y_coeff);
	delete [] y_coeff;

	int *c_coeff=new int[numOfCCoeff];

	CbDCCoeff_element.GetIntValue(val);
	c_coeff[0]=val;
	if(!CbACCoeff_element->isNull()){
		vector<int> AC;
		if( CbACCoeff_element->GetIntVector(AC)==0 ){
			for(i=1; i<numOfCCoeff; i++) c_coeff[i]=AC[i-1];
		}
	}
	SetNumberOfCCoeff(numOfCCoeff);
	SetCbCoeff(c_coeff);

	CrDCCoeff_element.GetIntValue(val);
	c_coeff[0]=val;
	if(!CrACCoeff_element->isNull()){
		vector<int> AC;
		if( CrACCoeff_element->GetIntVector(AC)==0 ){
			for(i=1; i<numOfCCoeff; i++) c_coeff[i]=AC[i-1];
		}
	}
	SetCrCoeff(c_coeff);

	delete [] c_coeff;

	return 0;
}
*/


//----------------------------------------------------------------------------
int ColorLayoutDescriptor::GetNumberOfYCoeff()
{
	return (m_NumberOfYCoeff);
}

//----------------------------------------------------------------------------
int ColorLayoutDescriptor::GetNumberOfCCoeff()
{
	return (m_NumberOfCCoeff);
}

//----------------------------------------------------------------------------
int* ColorLayoutDescriptor::GetYCoeff()
{
	return (m_y_coeff);
}

//----------------------------------------------------------------------------
int* ColorLayoutDescriptor::GetCbCoeff()
{
	return (m_cb_coeff);
}

//----------------------------------------------------------------------------
int* ColorLayoutDescriptor::GetCrCoeff()
{
	return (m_cr_coeff);
}

//----------------------------------------------------------------------------
void ColorLayoutDescriptor::SetNumberOfYCoeff(int NumberOfYCoeff)
{
    if( NumberOfYCoeff == m_NumberOfYCoeff)
        return;

    switch(NumberOfYCoeff){
		case 3: case 6: case 10: case 15: case 21: case 28: case 64:
			m_NumberOfYCoeff = NumberOfYCoeff;
			if(m_y_coeff) delete [] m_y_coeff;
			m_y_coeff = new int[ m_NumberOfYCoeff ];
			memset(m_y_coeff, 0, m_NumberOfYCoeff * sizeof(int) );		// added by mb
			break;

		default:
			fprintf(stderr, "Invalid Number of Y Coeff. [%d]\n", NumberOfYCoeff);
    }
}

//----------------------------------------------------------------------------
void ColorLayoutDescriptor::SetNumberOfCCoeff(int NumberOfCCoeff)
{
    if( m_NumberOfCCoeff == NumberOfCCoeff )
        return;

    switch(NumberOfCCoeff){
		case 3: case 6: case 10: case 15: case 21: case 28: case 64:
            m_NumberOfCCoeff=NumberOfCCoeff;
            if(m_cb_coeff) delete [] m_cb_coeff;
            if(m_cr_coeff) delete [] m_cr_coeff;
            m_cb_coeff=new int[m_NumberOfCCoeff];
            memset(m_cb_coeff, 0, m_NumberOfCCoeff * sizeof(int) );		// added by mb

            m_cr_coeff=new int[m_NumberOfCCoeff];
            memset(m_cr_coeff, 0, m_NumberOfCCoeff * sizeof(int) );		// added by mb

            break;
		default:
			fprintf(stderr, "Invalid Number of C Coeff. [%d]\n", NumberOfCCoeff);
    }
}

//----------------------------------------------------------------------------
void ColorLayoutDescriptor::SetYCoeff(int *YCoeff)
{
	for( int i = 0; i < m_NumberOfYCoeff; i++ )
		m_y_coeff[i] = YCoeff[i];

}

//----------------------------------------------------------------------------
void ColorLayoutDescriptor::SetCbCoeff(int *CbCoeff)
{
	for(int i = 0; i < m_NumberOfCCoeff; i++)
		m_cb_coeff[i]=CbCoeff[i];
}

//----------------------------------------------------------------------------
void ColorLayoutDescriptor::SetCrCoeff(int *CrCoeff)
{
	for( int i = 0; i < m_NumberOfCCoeff; i++ )
		m_cr_coeff[i] = CrCoeff[i];
}

/// compute the distance between 2 CLDs
float ColorLayoutDescriptor::distance(ColorLayoutDescriptor* cld)
{
    int NY2, NC2, NY, NC;

    NY2 = cld->GetNumberOfYCoeff();
    NC2 = cld->GetNumberOfCCoeff();

    NY = ( m_NumberOfYCoeff < NY2) ? m_NumberOfYCoeff : NY2;
    NC = (m_NumberOfCCoeff < NC2) ? m_NumberOfCCoeff : NC2;

    int *Y2, *Cb2, *Cr2;
    Y2 = cld->GetYCoeff();
    Cb2 = cld->GetCbCoeff();
    Cr2 = cld->GetCrCoeff();

    int sum1, sum2, sum3;
    int diff;

    /// -- Y --

    sum1 = 0;
    int j;
    // first 3 values have higher weight
    for( j = 0; j < 3; j++ )
    {
		diff = m_y_coeff[j] - Y2[j];
		sum1 += 2*diff*diff;
    }
    for( j = 3; j < NY; j++ )
    {
		diff = m_y_coeff[j] - Y2[j];
		sum1 += diff*diff;
    }

    /// -- Cb --
    sum2 = 0;
    diff = m_cb_coeff[0] - Cb2[0];
    sum2 += 2*diff*diff;
    for( j = 1; j < NC; j++ )
    {
		diff = m_cb_coeff[j] - Cb2[j];
		sum2 += diff*diff;
    }

    /// -- Cr --
    sum3 = 0;
    diff = m_cr_coeff[0] - Cr2[0];
    sum3 += 4*diff*diff;
    diff = m_cr_coeff[1] - Cr2[1];
	sum3 += 2*diff*diff;
    diff = m_cr_coeff[2] - Cr2[2];
	sum3 += 2*diff*diff;
    for( j = 3; j < NC; j++ )
    {
		diff = m_cr_coeff[j] - Cr2[j];
		sum3 += diff*diff;
    }

    return sqrt((float)sum1) + sqrt((float)sum2) + sqrt((float)sum3);
}

/// normalized distance
#define CLD_MAX_DIST 130.0f	// approximate (for CLD120, max distance is ~ 260)
float ColorLayoutDescriptor::distanceNorm(ColorLayoutDescriptor* cld)
{
    return distance(cld)/CLD_MAX_DIST;
}


// Print the descriptor values on screen
// added by mb - 27.06.2008
void ColorLayoutDescriptor::Print()
{
	printf("Color Layout Descriptor (CLD):\n");
	int i;

	//Y DC coeff
	printf("YDCCoeff  : %3d\n", m_y_coeff[0]);
	//Cb DC Coeff
	printf("CbDCCoeff : %3d\n", m_cb_coeff[0]);
	//Cr DC Coeff
	printf("CrDCCoeff : %3d\n", m_cr_coeff[0]);

	printf("YACCoeff[%d] : ", m_NumberOfYCoeff-1);
	for(i = 1; i<m_NumberOfYCoeff; i++)
		printf(" %3d", m_y_coeff[i]);
	printf("\n");

	printf("CbACCoeff[%d]: ", m_NumberOfCCoeff-1);
	for(i=1; i<m_NumberOfCCoeff; i++)
		printf(" %3d", m_cb_coeff[i]);
	printf("\n");

	printf("CrACCoeff[%d]: ", m_NumberOfCCoeff-1);
	for(i=1; i<m_NumberOfCCoeff; i++)
		printf(" %3d", m_cr_coeff[i]);
	printf("\n");
};

