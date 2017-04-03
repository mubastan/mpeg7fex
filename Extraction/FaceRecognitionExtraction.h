///////////////////////////////////////////////////////////////////////////////
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
// Applicable File Name:  FaceRecognitionExtraction.h
//
#ifndef __FaceRecognitionEXTRACTION_H__
#define __FaceRecognitionEXTRACTION_H__

#include "AddressLib/vopio.h"

//#include "face_detect.h"

#include "Descriptors/FaceRecognition.h"

namespace XM
{

class FaceRecognitionExtractionTool
{
public:

	FaceRecognitionExtractionTool();
    ~FaceRecognitionExtractionTool();

	int round(double x);

	FRD* GetFaceRecognitionDescriptor(){ return this->faceDescriptor;}

    // face image of size w = 46, height = 56;
    // y_chan is used
	FRD* extract( MomVop* faceImage );

	char eigenface[2576*48];
	unsigned char meanface[2576];

private:

    FRD* faceDescriptor;

	// Added for hard coded FaceRecognition eigenface and meanface parameters
	static char EigenfaceDataTable[2576*48];
	static unsigned char MeanfaceDataTable[2576];

	// Modified according to ISO/IEC 15938-3/FCD and XM visual part 10.0 (2001.05)
	int qstep[48];
};

};

#endif
