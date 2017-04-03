/***************************************************************
 * Name:      Feature.h
 * Purpose:   Extract low-level MPEG-7 descriptors
 * Author:    Muhammet Bastan (mubastan@gmail.com, bastan@cs.bilkent.edu.tr)
 * Created:   2008
 * Copyright: Muhammet Bastan (http://www.cs.bilkent.edu.tr/~bastan)
 * License:
 * Update: 2009-11-08
 * Update: 2009-11-18
 * Update: 2011-02-02 (upgrade to OpenCV 2.2 -- Mat)
 * TODO: modify the Feature class or provide a new class which
 *       does not use static functions, for efficiency in memory management
 **************************************************************/

/************************************************************************************
					How to use this class
					---------------------
1. include this header:		#include "Feature.h"
2. intantiate a Frame object: Frame* frame = new Frame( width, height, true, false, false );			// width-height of the input image/frame
   set the image			  frame->setImage( img );								// Mat& img // input image
					or use this: Frame* frame = new Frame( img );					// Mat& img // input image

3.a) Extract feature from the whole frame:
		XM::ColorStructureDescriptor* csd = Feature::getColorStructureD(frame);
		use the descriptor values in csd ( have a look at the descriptor classes under ./Descriptors/ directory)

NOTE: use the namespace XM to refer to the descriptors rather than declaring "using namespace XM",
      it may mess up with other namespaces!

3.b) Extract features from an arbitrary region - need to set the region mask
		set the mask for the region: frame->setMaskAll( Mat& mask, int regionID, 255, 0 );	// regionID: ID/value of the region in the mask image (1,2,3,..)
		compute the feature:
		XM::ColorStructureDescriptor* csd = Feature::getColorStructureD(frame);

4. when you are done with the computed descriptor, relase it: delete csd;		(avoid memory leakage)

IMPORTANT NOTES:

1. If you set a mask for the frame to extract features from arbitrary regions, and after that, if you want to extract features
   from the whole frame you need to reset the mask! Use: frame->resetMaskAll();
   Therefore, it is better (more efficient) to extract the global features from the whole frame first, and then set mask for arbitrary regions..

2. Homogeneous Texture Descriptor, Feature::getHomogeneousTextureD, uses grayscale image as input, therefore you need to convert your color image to gray first
   Suppose you have the RGB color image 'Mat img',
			// create a single channel image 'Mat gray' to store the grayscale image
			Mat gray = Mat( img.rows, img.cols, CV_8UC1 );
			// convert the input image from RGB to gray
			cvtColor(img, gray, CV_BGR2GRAY);
			// Create the frame (with only the gray image)
			Frame* frame = new Frame(width, height, false, true, false);
			// set the grayscale image of the frame
			frame->setGray( gray );
			// extract the descriptor
			XM::HomogeneousTextureDescriptor* htd = Feature::getHomogeneousTextureD( frame, true );	// true:full layer
            // do whatever you like with `htd`
            // ...
            // release the descriptor
            delete htd;

 ************************************************************************************/

#ifndef _FEATURE_H_
#define _FEATURE_H_

#include <vector>

#include "Frame.h"

#include "Descriptors.h"

typedef enum
{
    MP7_CSD, MP7_SCD, MP7_DCD, MP7_CLD,
    MP7_EHD, MP7_HTD, MP7_GOF, MP7_FRD, MP7_MAc
} MP7FeatureType;


class Feature
{
public:

	///--------------------------------------------------
	/// MPEG-7 Color Features
	///--------------------------------------------------

	// Color Structure Descriptor (CSD), default descriptor size:  64
	// descSize: (32, 64, 128, 256)
	static XM::ColorStructureDescriptor* getColorStructureD( Frame* f, int descSize = 64 );

	// Scalable Color Descriptor (SCD)
	// Frame f shoud have mask allocated
	// note: if maskFlag = true, it sets all the mask to value 255 (whole image), at the end detaches the mask from f->image  again
	//       otherwise you need to do it before you call this functions - better use maskFlag = true to extract from the whole image/frame
	//       if the description is to be extracted from a region, set the mask explicitly using a foreground value of 255, and use maskFlag = false
	// numCoeff = desc size: (16, 32, 64, 128, 256)
	static XM::ScalableColorDescriptor* getScalableColorD( Frame* f,  bool maskFlag = true, int numCoeff = 256, int bitPlanesDiscarded = 0);

	// GoFGoP color descriptor (for a group of frames, such as shots) (GoFGoP)
	// instead of this function use the GoF class defined below!
	static XM::ScalableColorDescriptor* getGoFColorD( std::vector<Mat> frames, int numCoeff = 256, int bitPlanesDiscarded = 0 );

	// Dominant Color Descriptor (CLD)
	// normalize: normalize values to MPEG-7 range (color:0-32, variance:0,1, weight:0-32)?
	//            if false, (color:RGB values[0-255], variance:as computed, weight:0-100 percent)
	// variance: compute the variance?
	// spatial: compute the spatial coherency
	// bin1, bin2, bin3: bin numbers to quantize the dominat color values
	static XM::DominantColorDescriptor* getDominantColorD( Frame* f, bool normalize = true, bool variance = true, bool spatial = true, int bin1 = 32, int bin2 = 32, int bin3 = 32 );

	// Color Layout Descriptor (CLD)
	// numberOfYCoeff/numberOfCCoeff: (3, 6, 10, 15, 21, 28, 64)	( DC(1) + AC )
	static XM::ColorLayoutDescriptor* getColorLayoutD( Frame* f, int numberOfYCoeff = 64, int numberOfCCoeff = 28 );


	///--------------------------------------------------
	/// MPEG-7 Texture Features
	///--------------------------------------------------

	// Edge Histogram Descriptor (EHD)
	// input image: color - converted to grayscale within the extractor
	static XM::EdgeHistogramDescriptor* getEdgeHistogramD( Frame* f );

	// Homogeneous Texture Descriptor (HTD)
	// input: single band grayscale image; image size must be > 128x128, arbitrary shape, uses htd->extract( f->gray, layerFlag);
	// use f->setGray(grayImg); before calling this function
	// layerFlag: base-layer 32-components,  true:full-layer 62 components (both energy and deviation)
	static XM::HomogeneousTextureDescriptor* getHomogeneousTextureD( Frame* f, bool layerFlag = true );


	///--------------------------------------------------
	/// MPEG-7 Shape Features
	///--------------------------------------------------
	// Contour Shape descriptor (CShD)
	static XM::ContourShapeDescriptor* getContourShapeD( Frame* f );

	// Region Shape Descriptor (RSD),
	// uses f->mask,  set f->mask before calling this function (f->setMask(mask))
	// foreground pixels > 0 => 0:backgground, 255:foreground
	static XM::RegionShapeDescriptor* getRegionShapeD( Frame* f );

};


///*********************************************************************************
///			FRD: Face Recognition Descriptor (PCA-based)
///*********************************************************************************
#include "Extraction/FaceRecognitionExtraction.h"
class FaceRecognitionFeature
{
    public:
    FaceRecognitionFeature();
    ~FaceRecognitionFeature();

    // uses grayscale image of f (f->gray)
    // input: face image of size w = 46, height = 56, normalized
    // do not delete FRD (48) returned!
    XM::FRD* getFaceRecognitionD( Frame* f );

    XM::FaceRecognitionExtractionTool* facerexTool;
};


///*********************************************************************************
///			GoF = Group of Frame Descriptor
///*********************************************************************************

#include "Extraction/GoFGoPColorExtraction.h"
/*
Howto use GoF
GoF* gofd = new GoF( 1, video->getWidth(), video->getHeight());
while(...){
	// read/get frame

	// add the frame to the group
	gofd->addFrame1( frame );
}
// end
gofd->end();

// get the descriptor
XM::ScalableColorDescriptor* scdi = gofd->scdi;

//release gofd (descriptor is not released by the destructor, release it when you are done wit it)
delete gofd;

*/

class GoF
{
public:

	// for type 1: from whole frame, type 2: from arbitrary regions
	GoF( int type, int w = 0, int h = 0, int aggregationTye = 0, int numCoeff = 256, int bitPlanesDiscarded = 0 );

	// descriptor scd is not released, it should be released by the client
	~GoF();

	void restart( int aggregationType, int numCoeff, int bitPlanesDiscarded );
	// reset to initial state
	void reset();
	void start( int aggregation );

	// for type 1 (whole frame)
	void addFrame1( Mat& img );

	// for type 2 (arbitrary region), mask should be set before calling this function
	void addFrame2( Frame* f );

	// post processing - aggregation at the end of processing all the frames in the group
	void end();

	// Data

	// number of frames in the group
	int numFrames;

	// type of input, 1: whole frame, 2: from an arbitrary region
	int type;

	Frame* f;

	XM::GoFGoPColorExtractionTool* gof;
	XM::ScalableColorDescriptor* scd;

};


#endif
