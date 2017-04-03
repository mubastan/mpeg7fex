//////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Lei Wang, Zhongyang Huang
// (Panasonic Singapore Laboratories Pte Ltd)
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
// Copyright (c) 1998-2001.
//
// This notice must be included in all copies or derivative works.
//
// Applicable File Name:  FaceRecognitionExtraction.cpp
//

#include <cmath>


#include "FaceRecognitionExtraction.h"
#include "EigenFaceData.h"

using namespace XM;

FaceRecognitionExtractionTool::FaceRecognitionExtractionTool()
{
    this->faceDescriptor = 0;
    int i,j,k;

    // MeanfaceDataTable is in file EigenFaceData.h
	for ( i = 0; i < 56; i++ )			// height
		for ( j = 0; j < 46; j++ )		// width
			meanface[ i * 46 + j ] = MeanfaceDataTable[ i + j * 56 ];

	// EigenfaceDataTable is in file EigenFaceData.h
	for( k = 0; k < 48; k++ )
		for ( i = 0; i < 56; i++ )
			for ( j = 0; j < 46; j++ )
				eigenface[ i * 46 + j + k * 2576 ] = EigenfaceDataTable[ i + j * 56 + k * 2576 ];


    for ( i = 0; i < 48; i++ )
		qstep[i] = 8;
}

//----------------------------------------------------------------------------
FaceRecognitionExtractionTool::~FaceRecognitionExtractionTool()
{
    // will be released by the clients
    //if( this->faceDescriptor )
    //    delete this->faceDescriptor;
}


//----------------------------------------------------------------------------
FRD* FaceRecognitionExtractionTool::extract( MomVop* faceImage )
{
	int i,j;
	double tmpfeature;
	int featuresize = 48;
	int facesize = 2576;

	// Mark Pickering's proposal
	double step1st = 1.6384e+004;

	unsigned char* Face = faceImage->y_chan->data->u;
    int image_width = faceImage->width;
    int image_height = faceImage->height;

	if ( ( image_width != 46 )||( image_height != 56 ) )
		return 0;
	else
	{
	    this->faceDescriptor = new FRD;

		//computing the eigenfeatures
		for ( i = 0; i < 48; i++ )
		{
			tmpfeature = 0;
			for ( j = 0; j < 2576; j++ )
				tmpfeature += ( Face[j] - meanface[j] ) * eigenface[ i * facesize + j ];
			this->faceDescriptor->eigenfeature[i] = round( tmpfeature / step1st / qstep[i]) + 128 / qstep[i];
		}

		for ( i = 0; i < featuresize; i++ )
		{
			if (this->faceDescriptor->eigenfeature[i] < 0)
				this->faceDescriptor->eigenfeature[i] = 0;

			if (this->faceDescriptor->eigenfeature[i] >= 256/qstep[i])
				this->faceDescriptor->eigenfeature[i] = 256/qstep[i]-1;
		}

		return this->faceDescriptor;
	}
}


int FaceRecognitionExtractionTool::round(double x)
{
	if (x>0)
		return (int)(x+0.5);
	else if (x<0)
		return (int)(x-0.5);
	else
		return 0;
}
