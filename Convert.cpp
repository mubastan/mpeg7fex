#include <iostream>
#include "Convert.h"


// convert Mat of OpenCV to MomVop image of XM sotfware
void Convert :: ipl2momvop(Mat& src, MomVop* dst)
{
    assert( !src.empty() );
	assert( dst );	// none should be NULL or abort will be called
	assert( dst->y_chan );

	//single band
	if(src.channels() == 1 && dst->y_chan )
	{
		for ( int i = 0; i < src.rows; i++)
			for( int j = 0; j < src.cols; j++)
				dst->y_chan->data->u[i*src.cols + j] = (unsigned char)src.at<uchar>(i,j);

	}
	// 3-channel color image: in openCV BGR (blue green red);  RGB is VYU in MomVop => r:v, g:y, b:u
	else if( src.channels() == 3 && dst->y_chan && dst->u_chan && dst->v_chan ){

		int index;
		for ( int i = 0; i < src.rows; i++)
		{
			for( int j = 0; j < src.cols; j++)
			{
				Vec3b& pbgr = src.at<Vec3b>(i,j);
				index = i*src.cols + j;
				dst->v_chan->data->u[index] = (unsigned char)pbgr[2];	// red
				dst->y_chan->data->u[index] = (unsigned char)pbgr[1];	// green
				dst->u_chan->data->u[index] = (unsigned char)pbgr[0];	// blue


			}
		}
	}else{
		std::cerr<<"ipl2momvop:Input and output images should have 1 or 3 channel(s)"<<std::endl;
		exit(0);
	}

}

/**
 * set alpha channel of MomVop image according to the shape in mask image
 * val: value of the region in mask
 * fval (destination foreground pixel value): foreground pixels are set to destVal in MomVop img (default 1)
 * Background pixels are 0
 * note: output image should have been allocated, alpha channel allocated if it does not exist
 * note: image sizes should be compatible (not checked)
 */
void Convert :: setShape( Mat& mask, MomVop* img, int val, int fval, Rect* roi )
{
    assert( !mask.empty() );
	assert( img );	// none should be NULL or abort will be called

	if(!img->a_chan){
		img->a_chan = initimg (img->width, img->height, UCHAR_TYPE);	// sets data to 0
		std::cerr << "\nWarning: alpha channel does not exist! Allocated." << std::endl;
	}

	int x1, x2, y1, y2;

	// if no ROI is specified consider the whole image
	if(!roi){
		x1 = y1 = 0;
		x2 = mask.cols;
		y2 = mask.rows;
	}
	else
	{
	    x1 = roi->x;
        y1 = roi->y;
		x2 = roi->x + roi->width - 1;
		y2 = roi->y + roi->height - 1;
	}


	for ( int i = y1; i < y2; i++){
		for( int j = x1; j < x2; j++){
				int pval = mask.at<uchar>(i,j);
				//if this pixel belongs to the foreground region represented by val
				if( pval == val )
					img->a_chan->data->u[i*img ->width+j] = (unsigned char)fval;
		}
	}

}


/**
 * set alpha channel of MomVop image according to the shape in mask image
 * val: value of the region in mask
 * bval: background pixels (default 0)
 * fval: foreground pixels are set to destVal in MomVop img (default 1)
 * note: output image should have been allocated, alpha channel allocated if it does not exist
 * note: image sizes should be compatible (not checked)
 */
void Convert :: setShape( Mat& mask, MomVop* img, int val, int bval, int fval, Rect* roi )
{
    assert( !mask.empty() );
	assert( img );	// none should be NULL or abort will be called


	if(!img->a_chan){
		img->a_chan = initimg (img->width, img->height, UCHAR_TYPE);
		std::cerr<<"\nWarning: alpha channel does not exist! Allocated."<<std::endl;
	}

	int x1, x2, y1, y2;

	// if no ROI is specified consider the whole image
	if(!roi){
		x1 = y1 = 0;
		x2 = mask.cols;
		y2 = mask.rows;
	}
	else
	{
        x1 = roi->x;
        y1 = roi->y;
		x2 = roi->x + roi->width - 1;
		y2 = roi->y + roi->height - 1;
	}

	CvScalar s;

	for ( int i = y1; i < y2; i++){
		for( int j = x1; j < x2; j++){
            int pval = mask.at<uchar>(i,j);
            //if this pixel belongs to the foreground region represented by val
            if( pval == val )
                img->a_chan->data->u[i*img ->width+j] = (unsigned char)fval;
            else
                img->a_chan->data->u[i*img ->width+j] = (unsigned char)bval;
		}
	}

}

// Set all the values of the mask (a_chan) to fval
void Convert :: setMaskValue( MomVop* img, int fval )
{

	if(!img->a_chan){
		std::cerr<<"\nWarning: alpha channel does not exist!"<<std::endl;
		return;
	}

	for ( int i = 0; i < img->height; i++){
		for( int j = 0; j < img->width; j++){
				img->a_chan->data->u[i*img ->width+j] = (unsigned char)fval;

		}
	}

}
