/***************************************************************
 * Name:      FaceRecognition.h
 * Purpose:   Defines
 * Author:    Muhammet Bastan (bastan@cs.bilkent.edu.tr)
 * Created:   05.02.2009
 * Copyright: Muhammet Bastan (http:://www.cs.bilkent.edu.tr/~bastan)
 * License:
 **************************************************************/
#ifndef FACERECOGNITION_H_INCLUDED
#define FACERECOGNITION_H_INCLUDED

namespace XM
{
/*
typedef	struct FaceRecognition_Descriptor
{
	int eigenfeature[48];

} FRD;
*/
class FRD
{
    public:

        void reset();

        float distance( FRD* frd );

        int eigenfeature[48];
};

};
#endif
