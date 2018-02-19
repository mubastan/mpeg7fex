/***************************************************************
 * Name:      FexWrite.cpp
 * Purpose:   Code for : extract and write the descriptors according to the input parameters
 * Author:    Muhammet Bastan (mubastan@gmail.com)
 * Created:   16.06.2009
 * Copyright: Muhammet Bastan (https://sites.google.com/site/mubastan)
 * License:
 **************************************************************/

#include <iostream>

//#include "cxcore.h"
//#include "cv.h"


#include "FexWrite.h"

// Color Structure
void FexWrite::computeWriteCSD( Frame* frame, int descSize )
{
	if(!frame)
		return;

    // compute the descriptor
	XM::ColorStructureDescriptor* csd = Feature::getColorStructureD(frame, descSize);

	// write to screen
	for(unsigned int i = 0; i < csd->GetSize(); i++)
		std::cout << (int)csd->GetElement(i) << " " ;
	std::cout << std::endl;

    // release descriptor
	delete csd;
}


// Scalable Color
void FexWrite::computeWriteSCD( Frame* frame, bool maskFlag, int descSize )
{
	if(!frame)
		return;

    // compute descriptor
	XM::ScalableColorDescriptor* scd = Feature::getScalableColorD( frame, maskFlag, descSize );

	// write to screen
	for(unsigned int i = 0; i < scd->GetNumberOfCoefficients(); i++)
		std::cout << (int)  scd->GetCoefficient(i) << " ";
	std::cout << std::endl;

    // release descriptor
	delete scd;
}


// Color Layout
void FexWrite::computeWriteCLD( Frame* frame, int numYCoef, int numCCoef )
{
	if(!frame)
		return;

    // compute the descriptor
	XM::ColorLayoutDescriptor* cld = Feature::getColorLayoutD( frame, numYCoef, numCCoef );

	// write to screen

	// number of coefficients
	int numberOfYCoeff = cld->GetNumberOfYCoeff();
	int numberOfCCoeff = cld->GetNumberOfCCoeff();


	// values
	int *y_coeff, *cb_coeff, *cr_coeff;
	y_coeff = cld->GetYCoeff();
	cb_coeff = cld->GetCbCoeff();
	cr_coeff = cld->GetCrCoeff();


	int i = 0;
	// Y coeff (DC and AC)
	for ( i = 0; i < numberOfYCoeff; i++ )
		std::cout << y_coeff[i] << " " ;

	//Cb coeff  (DC and AC)
	for ( i = 0; i < numberOfCCoeff; i++ )
		std::cout << cb_coeff[i] << " ";

	//Cr coeff  (DC and AC)
	for ( i = 0; i < numberOfCCoeff; i++ )
		std::cout << cr_coeff[i] << " ";

	std::cout << std::endl;

    // release the descriptor
	delete cld;
}


// Dominant Color
void FexWrite::computeWriteDCD( Frame* frame,
                                bool normalize, bool variance, bool spatial,
                                int bin1, int bin2, int bin3 )
{
	if(!frame)
		return;

    // compute the descriptor
	XM::DominantColorDescriptor* dcd = Feature::getDominantColorD( frame, normalize, variance, spatial, bin1, bin2, bin3 );


	// write to screen

	// number of dominant colors
	int ndc = dcd->GetDominantColorsNumber();
	//std::cout << ndc << " " ;

	// spatial coherency
	if(spatial)
        std::cout << dcd->GetSpatialCoherency();

	// dominant colors: percentage(1) centroid value (3) color variance (3)
	XM::DOMCOL* domcol = dcd->GetDominantColors();
	for( int i = 0; i < ndc; i++ )
	{
		std::cout << " " << domcol[i].m_Percentage
                  << " " << domcol[i].m_ColorValue[0]
                  << " " << domcol[i].m_ColorValue[1]
                  << " " << domcol[i].m_ColorValue[2];
        if(variance)
        std::cout << " " << domcol[i].m_ColorVariance[0]
                  << " " << domcol[i].m_ColorVariance[1]
                  << " " << domcol[i].m_ColorVariance[2];
	}

	std::cout << std::endl;

    // release the descriptor
	delete dcd;
}

// Homogeneous Texture
void FexWrite::computeWriteHTD( Frame* frame, int layerFlag )
{
	if(!frame)
		return;

	XM::HomogeneousTextureDescriptor* htd = Feature::getHomogeneousTextureD( frame, layerFlag );

    // get a pointer to the values
	int* values = htd->GetHomogeneousTextureFeature();

	// write to screen

	// values[0]: mean, values[1]: std, values[2-31] base layer (energy)
	int i;
	for(i = 0; i < 32; i++)
		std::cout << values[i] << " " ;

    // if full layer, print values[32-61] (energy deviation)
    if(layerFlag)
        for(i = 32; i < 62; i++)
            std::cout << values[i] << " " ;

	std::cout << std::endl;

    // release the descriptor
	delete htd;
}


// Edge Histogram
void FexWrite::computeWriteEHD( Frame* frame )
{
	if(!frame)
		return;

    // compute the descriptor
	XM::EdgeHistogramDescriptor* ehd = Feature::getEdgeHistogramD( frame );

    // get a pointer to the values
	char* de = ehd->GetEdgeHistogramElement();

	// write to screen
	for( unsigned int i = 0; i < ehd->GetSize(); i++)
		std::cout << (int)de[i] << " ";
	std::cout  << std::endl;

    // release the descriptor
	delete ehd;
}


// Region Shape
void FexWrite::computeWriteRSD( Frame* frame )
{
    if(!frame)
		return;

    // compute the descriptor
    XM::RegionShapeDescriptor* rsd = Feature::getRegionShapeD( frame );

    int i,j;
	for(i=0; i<ART_ANGULAR; i++)
		for(j=0; j<ART_RADIAL; j++)
			if(i != 0 || j != 0)
				std::cout << (int)rsd->GetElement(i, j) << " ";

	std::cout << std::endl;

    // release the descriptor
    delete rsd;
}

// Contour Shape
void FexWrite::computeWriteCShD( Frame* frame )
{
    if(!frame)
		return;

}
