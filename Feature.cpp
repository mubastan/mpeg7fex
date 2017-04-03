/***************************************************************
 * Name:      Feature.cpp
 * Purpose:   Extract low-level MPEG-7 descriptors
 * Author:    Muhammet Bastan (bastan@cs.bilkent.edu.tr)
 * Created:   2008
 * Copyright: Muhammet Bastan (http://www.cs.bilkent.edu.tr/~bastan)
 * License:
 * Update: 2009-11-08
 **************************************************************/

#include <iostream>
#include "Feature.h"
#include "Convert.h"

//color
#include "Extraction/ColorStructureExtraction.h"
#include "Extraction/DominantColorExtraction.h"
#include "Extraction/ColorLayoutExtraction.h"
#include "Extraction/ScalableColorExtraction.h"
#include "Extraction/GoFGoPColorExtraction.h"

//texture
#include "Extraction/EdgeHistExtraction.h"
#include "Extraction/HomoTextureExtraction.h"

//shape
#include "Extraction/ContourShapeExtraction.h"
#include "Extraction/RegionShapeExtraction.h"


//============================================================================
///				Color Structure Descriptor
//============================================================================
// descSize: = 32 || size == 64 || size == 128 || size == 256
// note: tested, working for 3-band RGB images


// Frame f should be prepared properly prior to calling this function (image, mask, rid, etc.)
// Note: the descriptor returned should be released by the user (csdi->release())
XM::ColorStructureDescriptor* Feature::getColorStructureD( Frame* f, int descSize )
{
	//std::cout<< "\nColor Structure Descriptor (CSD):" <<std::endl;


	XM::ColorStructureExtractionTool* csdt = new  XM::ColorStructureExtractionTool();
	XM::ColorStructureDescriptor* csd = csdt->GetDescriptor();
	csdt->extract(f->image, descSize);
	// print values
	/*
	for(int i = 0; i < csdi->GetSize(); i++)
		std::cout << (int)csdi->GetElement(i) << " ";
	std::cout << std::endl;
	*/
	delete csdt;

	return csd;
}


//------------------------------------------------------------------------------------------------------------------

//============================================================================
//				Scalable Color Descriptor
//============================================================================
// note:sets all the mask to value 255 (whole image), at the end detaches the mask from f->image and f->gray again
// size == 16 || size == 32 || size == 64 || size == 128 || size == 256
XM::ScalableColorDescriptor* Feature::getScalableColorD( Frame* f, bool maskFlag, int numCoeff, int bitPlanesDiscarded )
{
	//std::cout<< "\nScalable Color Descriptor (SCD):" <<std::endl;

	if(maskFlag)
		f->setMaskValue( 255 );

	XM::ScalableColorExtractionTool* scd = new  XM::ScalableColorExtractionTool( numCoeff, bitPlanesDiscarded );
	XM::ScalableColorDescriptor* scdi = scd->GetDescriptor();
	scd->extract( f->image );

	//scdi->Print();

	//std::cout << std::endl;

	if(maskFlag)
		f->resetMaskAll();

	delete scd;

	return scdi;

}


//------------------------------------------------------------------------------------------------------------------

//============================================================================
//				GoFGoP Color Descriptor
//============================================================================

XM::ScalableColorDescriptor* Feature::getGoFColorD( std::vector<Mat> frames, int numCoeff, int bitPlanesDiscarded )
{
	if( frames.size() < 1 ){
		std::cerr << "getGoFColorD - no frame in the list" << std::endl;
		return 0;
	}

	Frame* f = new Frame( frames[0].cols, frames[0].rows );
	f->setMaskValue(255);

	XM::GoFGoPColorExtractionTool* gof = new XM::GoFGoPColorExtractionTool();

	// initialize extraction, set aggregation 0 (average)
	gof->initExtraction( 0 );

	for( unsigned int i = 0; i < frames.size(); i++ ){
		f->setImage(frames[i]);
		gof->extract(f->image);
	}

	// do the aggregation
	gof->postExtraction();

	XM::ScalableColorDescriptor* scdi = gof->GetDescriptor();

	// delete f;
	delete gof;

	return scdi;
}

//------------------------------------------------------------------------------------------------------------------

//============================================================================
//				Dominant Color Descriptor
//============================================================================
// normalize: normalize values to MPEG-7 range (color:0-32, variance:0,1, weight:0-32)?
//            if false, (color:RGB values[0-255], variance:as computed, weight:0-100 percent)
// variance: compute the variance?
// spatial: compute the spatial coherency
// bin1, bin2, bin3: bin numbers to quantize the dominat color values
XM::DominantColorDescriptor* Feature::getDominantColorD( Frame* f, bool normalize, bool variance, bool spatial, int bin1, int bin2, int bin3 )
{
	//std::cout<< "\nDominant Color Descriptor (DCD):" <<std::endl;

	XM::DominantColorExtractionTool* dcd = new  XM::DominantColorExtractionTool(normalize, variance, spatial);
	XM::DominantColorDescriptor* dcdi = dcd->GetDescriptor();

	// set bin numbers if to be normalized
	if(normalize)
		dcd->setBinNumbers( bin1, bin2, bin3 );

	// extract(MomVop* img, bool normalize = true, bool varFlag = true, bool spatFlag = true);
	dcd->extract( f->image, normalize, variance, spatial );
	// print values
	//dcdi->Print();

	delete dcd;

	return dcdi;

}


//============================================================================
///				Color Layout Descriptor
//============================================================================
// numberOfYCoeff, numberOfCCoeff: case 3: case 6: case 10: case 15: case 21: case 28: case 64:
XM::ColorLayoutDescriptor* Feature::getColorLayoutD( Frame* f, int numberOfYCoeff, int numberOfCCoeff )
{
	//std::cout << "\nColor Layout Descriptor (CLD):" << std::endl;

	XM::ColorLayoutExtractionTool* cldt = new  XM::ColorLayoutExtractionTool(numberOfYCoeff, numberOfCCoeff);
	XM::ColorLayoutDescriptor* cld = cldt->GetDescriptor();
	cldt->extract( f->image, numberOfYCoeff, numberOfCCoeff);

	//cldi->Print();
	//std::cout << std::endl;

	delete cldt;

	return cld;
}


//==========================================================================================================
///									TEXTURE DESCRIPTORS
//==========================================================================================================



//============================================================================
///				Edge Histogram Descriptor
//============================================================================
// input image: color - converted to grayscale within the extractor
XM::EdgeHistogramDescriptor* Feature::getEdgeHistogramD( Frame* f )
{
	//std::cout << "\nEdge Histogram Descriptor (EHD):" << std::endl;

	XM::EdgeHistogramExtractionTool* ehd = new  XM::EdgeHistogramExtractionTool();
	XM::EdgeHistogramDescriptor* ehdi = ehd->GetDescriptor();
	ehd->extract(f->image);
	// print the values
	/*
	char* de = ehdi->GetEdgeHistogramElement();
	for( int i = 0; i < ehdi->GetSize(); i++)
		std::cout << (int)de[i] << " ";
	std::cout << std::endl;
	*/
	delete ehd;

	return ehdi;
}


//============================================================================
///				Homogeneous Texture Descriptor
//============================================================================
// This function uses a single-band grayscale image, ( f->gray ), image size must be > 128x128
// Use f->setGray( Mat& gray ); to set the grayscale image before calling this function
// layerFlag: 0:base-layer 32-components  1:full-layer 62 components (both energy and deviation)
XM::HomogeneousTextureDescriptor* Feature::getHomogeneousTextureD( Frame* f, bool layerFlag )
{
	//std::cout << "\nHomogeneous Texture Descriptor (HTD):" << std::endl;

	XM::HomogeneousTextureExtractionTool* htdt = new  XM::HomogeneousTextureExtractionTool(true);
	XM::HomogeneousTextureDescriptor* htd = htdt->GetDescriptor();
	htdt->extract( f->gray, layerFlag);
	//htdi->Print();
    delete htdt;

	return htd;
}



//==========================================================================================================
///									SHAPE DESCRIPTORS
//==========================================================================================================


//============================================================================
///				Contour Shape Descriptor
//============================================================================
// input image: object mask, y_chan or a_chan of MomVop image is used in extraction
//

// before calling this function using set mask by f->setMask(mask, rid, 255, 0); or f->setMaskAll(mask, rid, 255, 0);
XM::ContourShapeDescriptor* Feature::getContourShapeD( Frame* f )
{
	//std::cout << "\nContour Shape Descriptor (CSD):" << std::endl;

	XM::ContourShapeExtractionTool* csdt = new  XM::ContourShapeExtractionTool();
	XM::ContourShapeDescriptor* csd = csdt->GetDescriptor();
	csdt->extract(f->mask);
	//csdi->Print();

	//std::cout << std::endl;

	delete csdt;

	return csd;
}

//============================================================================
///				Region Shape Descriptor
//============================================================================
XM::RegionShapeDescriptor* Feature::getRegionShapeD( Frame* f )
{
	//std::cout << "\nRegion Shape Descriptor (RSD):" << std::endl;

	//Set shape (alpha channel) if arbitrary shape
	// foreground pixels > 0 => 0:bcgk, 255:foreground
	//Frame :: setMask( Mat& mask, int rid, int fg, int bg )
	//if(mask)
	//	f->setMask(mask, rid, 255, 0);

	XM::RegionShapeExtractionTool* rsdt = new  XM::RegionShapeExtractionTool();
	XM::RegionShapeDescriptor* rsd = rsdt->GetDescriptor();
	rsdt->extract(f->mask);
	//rsdi->Print();

    /*
	int i,j;
	int n = 0;
	for(i=0 ; i<ART_ANGULAR ; i++)
		for(j=0 ; j<ART_RADIAL ; j++)
			if(i!=0 || j!= 0)
			{
				std::cout<< (int)rsdi->GetElement(i, j) << " ";
			}

	std::cout << std::endl;
    */
	delete rsdt;

	return rsd;
}
///**********************************************************
///			FRD: Face Recognition Descriptor (PCA-based)
///**********************************************************
FaceRecognitionFeature::FaceRecognitionFeature()
{
    this->facerexTool = 0;

    this->facerexTool = new XM::FaceRecognitionExtractionTool();
}

FaceRecognitionFeature::~FaceRecognitionFeature()
{
    if(this->facerexTool)
        delete this->facerexTool;
}

XM::FRD* FaceRecognitionFeature::getFaceRecognitionD( Frame* f )
{
    if(!f)
        return 0;

    return this->facerexTool->extract( f->gray );
}

///**********************************************************
///			    GoF: Group of Frame/Picture
///**********************************************************

// for type 1: from whole frame, type 2: from arbitrary regions
GoF::GoF(int type,  int w, int h, int aggregationType, int numCoeff, int bitPlanesDiscarded )
{
	if( type == 1 || type == 2 )
		this->type = type;
	else
		this->type = 1;	// from whole frame

	if( this->type == 1) {
		if( w > 0 && h > 0 ) {
			f = new Frame( w, h, true, false, true );
			f->setMaskValue(255);	// required by Scalable Color Extraction Tool
		}
		else{
			this->f = 0;
			std::cerr << "GoF::GoF - w or h nonpositive!" << std::endl;
		}
	} else
		this->f = 0;

	this->gof = 0;
	this->scd = 0;

	this->restart( aggregationType, numCoeff, bitPlanesDiscarded );

}

// descriptor should be released by the client
GoF::~GoF()
{
	if(gof)
		delete gof;

	// descriptor will be released by the clients
	//if(scd)
	//	delete scd;

	if(f)
		delete f;
}

// reset and start again
void GoF::restart( int aggregationType, int numCoeff, int bitPlanesDiscarded )
{
	if( gof )
		delete gof;
	if(scd)
		delete scd;

	this->numFrames = 0;

	this->gof = new XM::GoFGoPColorExtractionTool( numCoeff, bitPlanesDiscarded );
	this->scd = gof->GetDescriptor();
	this->start(aggregationType);
}

// reset to initial state, set descriptor values to 0
void GoF::reset()
{
	this->gof->reset();
}

// only reset the descriptor, start again with the same parameters (numCoeff, etc.)
void GoF::start( int aggregationType )
{
	if(gof){
		gof->initExtraction( aggregationType );
	}

}

void GoF::addFrame1( Mat& img )
{
	if(this->type != 1 ){
		std::cerr<<"GoF::addFrame1 - This function is for whole frames!" << std::endl;
		return;
	}

	if( !gof || !img.empty() ){
		std::cerr << "GoF::addFrame1 - gof = 0 or img.empty() !" << std::endl;
		return;
	}

	this->f->setImage( img );
	gof->extract( this->f->image );

	this->numFrames++;
}

void GoF::addFrame2( Frame* frame )
{
	if(this->type != 2 ){
		std::cerr<<"GoF::addFrame2 - This function is for arbitrary regions!" << std::endl;
		return;
	}


	if( gof && frame ){
		gof->extract(frame->image);
		this->numFrames++;
	}
	else
		std::cerr << "GoF::addFrame2 - gof = 0 or frame = 0 !" << std::endl;
}

void GoF::end()
{
	if(gof)
		gof->postExtraction();
}
