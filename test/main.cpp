/***************************************************************
 * Name:      main.cpp
 * Purpose:   Illustrate the usage of the MPEG-7 Feature Extraction library
 *            uses OpenCV for image handling (as does the library)
 * Author:    Muhammet Bastan (mubastan@gmail.com)
 * Created:   2010-02-19
 * Update: 2011-02-04 (update to OpenCV 2.2, Mat)
 * Copyright: Muhammet Bastan (https://sites.google.com/site/mubastan/)
 * License:
 **************************************************************/

#include <iostream>
#include "FexWrite.h"

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

int main( int argc, char* argv[] )
{
    Mat image;

    // load an image
    if( argc < 2 )
        image = imread("motor.jpg");
    else
        image = imread( argv[1] );

    // display the image
    //namedWindow("image");
    //imshow("image", image);
    //waitKey();

    ///-- extract global image descriptors --

    // create a Frame object (see include/Frame.h)
    // allocate memory for 3-channel color and 1-channel gray image and mask
    Frame* frame = new Frame( image.cols, image.rows, true, true, true);

    // set the image of the frame
    frame->setImage(image);

    // compute and display the descriptors for the 'image'
    // CSD of size 32
    FexWrite::computeWriteCSD(frame, 32);

    // SCD of size 128
    FexWrite::computeWriteSCD( frame, true, 128 );

    // CLD with numberOfYCoeff (28), numberOfCCoeff (15)
    FexWrite::computeWriteCLD( frame, 28, 15 );

    // DCD with normalization (to MPEG-7 ranges), without variance, without spatial coherency
    FexWrite::computeWriteDCD( frame, true, false, false );

    // EHD
    FexWrite::computeWriteEHD( frame );

    // HTD: we need to compute and set the grayscale image of the frame
    // create the grayscale image
    Mat gray;
    cvtColor( image, gray, CV_BGR2GRAY );
    frame->setGray( gray );

    // full layer (both energy and deviation)
    FexWrite::computeWriteHTD( frame, 1 );


    // load another image
    image = imread("timsah.jpg");
    //imshow("image", image);
    //waitKey();

    // set the image of 'frame'
    // we should resize the 'frame' first, the dimensions may be different
    // therefore, reallocation may be required
    frame->resize( image.cols, image.rows);

    // set the image
    frame->setImage(image);

    // here, we can compute the descriptors as above..
    // ..

    ///--- extract region-based descriptos ---

    // we need a foreground mask for the region
    // lets first create a dummy foreground mask to use as the region mask

    Mat mask = Mat( image.rows, image.cols, CV_8UC1, Scalar(0) );

    // draw a filled rectangle/circle, with foreground value 200
    int regVal = 200;   // (1...255)
    circle(mask, Point(120,100), 50, Scalar(regVal), -1);
    circle(mask, Point(120,200), 60, Scalar(regVal), -1);

    // display the mask
    //namedWindow("mask");
    //imshow("mask", mask);
    //waitKey();
    imwrite("mask.png", mask);

    // set the mask of 'frame', pixels having regVal are set to 255, background pixels to 0
    frame->setMaskAll( mask, regVal, 255, 0 );

    // SCD of the region
    FexWrite::computeWriteSCD( frame, false, 128 );

    // RSD (region shape)
    FexWrite::computeWriteRSD( frame );

    FexWrite::computeWriteCSD(frame, 64);

    // at this point, if you want to extract descriptors
    // from the whole image, reset the mask
    frame->resetMaskAll();
    FexWrite::computeWriteCSD(frame, 64);
    FexWrite::computeWriteEHD( frame );

    // ...
    // ...

    // release frame
    delete frame;

    // destroy the windows
    //destroyWindow("image");
    //destroyWindow("mask");

    return 0;
}
