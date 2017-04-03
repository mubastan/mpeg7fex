/***************************************************************
 * Name:      FaceRecognition.cpp
 * Purpose:   Code for
 * Author:    Muhammet Bastan (bastan@cs.bilkent.edu.tr)
 * Created:   2009-12-06
 * Copyright: Muhammet Bastan (http://www.cs.bilkent.edu.tr/~bastan)
 * License:
 **************************************************************/


#include "FaceRecognition.h"

#include <cstring>

using namespace XM;

void FRD::reset()
{
    memset( eigenfeature, 0, 48*sizeof(int) );
}

int weight[48] = {8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
                  16,16,16,16,16,16,16,16,
                  32,32,32,32,32,32,32,32,32,32,32,
                  32,32,32,32,32,32,32,32,32,32,32,32,32};

float FRD::distance( FRD* frd )
{
    float dist = 0.0f;
    int dif;
    for( int i = 0; i < 48; i++ )
    {
        dif = eigenfeature[i] - frd->eigenfeature[i];
        dist += weight[i]*dif*dif;
    }
    return dist;
}
