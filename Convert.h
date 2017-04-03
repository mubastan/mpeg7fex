#ifndef __CONVERT__H__
#define __CONVERT__H__

#include <opencv2/core/core.hpp>

#include "AddressLib/vopio.h"

using namespace cv;
class Convert {

public:
	static void ipl2momvop( Mat& src, MomVop* dst );
	static void setShape( Mat& mask, MomVop* img, int val, int fval = 255, Rect* roi = 0 );
	static void setShape( Mat& mask, MomVop* img, int val, int bval = 0, int fval = 255, Rect* roi = 0 );

	// Set all the values of the mask (a_chan) to fval
	static void setMaskValue( MomVop* img, int fval = 255 );
};

#endif

