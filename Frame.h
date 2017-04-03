/*************************************************************
Represent a video frame or image with MomVop structure used in MPEG-7 XM
aim: decrease the memory allocation deallocation for each frame during video processing
	 allocation, deallocation, setting the image is done only once and the Frame object can be
	 used to extract all the features (color/shape/texture)

author : mb
date: 07.07.2008
update: 02.02.2011 (upgrade to OpenCV 2.2, Mat)
**************************************************************/

#ifndef __FRAME__H__
#define __FRAME__H__

#include <opencv2/core/core.hpp>

#include "AddressLib/vopio.h"

using namespace cv;

class Frame
{
public:

	Frame( int width, int height, bool imgFlag = true, bool grayFlag = true, bool maskFlag = true );
	/// allocates and sets the image (this->image <= src)
	Frame( Mat& src, bool imgFlag = true, bool grayFlag = true, bool maskFlag = true  );
	~Frame();

    // allocate this->image, this->gray, this->mask (depending on the flags)
	void allocate(int width, int height, bool imgFlag = true, bool grayFlag = true, bool maskFlag = true );
    // resize the images in the frame, if the new size is different
    // mask is reset (ResetMaskAll()) if it exists
    void resize(int width, int height);

	/// this->image <= src (if this->image != 0 && src != 0 )
	void setImage( Mat& src );
	/// this->gray <= gray (if this->gray != 0 && gray != 0 )
	void setGray( Mat& gray );

	// set the alpha channel (mask) to the arbitrary shape
	void setMask( Mat& mask, int rid, int fg = 255, int bg = 0 );

	// set all the values in the mask to val, and attach this mask to image and gray as their a_chan
	// this  is required by Scalable Color Descriptor extraction!
	void setMaskValue( int val = 255);

	// set the alpha channel (mask) to the arbitrary shape AND point the alpha channels (a_chan) of image and gray to a_chan of mask
	// use this to extract color and grayscale features of arbitrary shapes
	void setMaskAll( Mat& mask, int rid, int fg = 255, int bg = 0 );
    // set the mask only for this->image
    void setMaskImage( Mat& mask, int rid, int fg = 255, int bg = 0 );
    // set the mask only for this->gray
    void setMaskGray( Mat& mask, int rid, int fg, int bg);

	//point a_chan of image (mask) and gray to 0 again (as initiated) - to extract feature from the whole frame
	void resetMaskAll();
	void resetMaskImage();
	void resetMaskGray();

	/// data
	// width-height of the frame
	int width;
	int height;

    /// MomVop is the data structure to store image data in MPEG-7 Reference software
    /// see AddressLib/vopio.h

	// 3-channel color image (3x8 bit)
	MomVop* image;

	// single channel grayscale image (8 bit)
	MomVop* gray;

	// single channel mask for arbitrary shape (8 bit)
	MomVop* mask;

};

#endif
