///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// University of California Santa Barbara, Kapil Chhabra, Yining Deng
// Mitsubishi Electric ITE-VIL, Leszek Cieplinski, LG-Elite.
// (contributing organizations names)
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
// Copyright (c) 1998-2000.
//
// This notice must be included in all copies or derivative works.
//
// Applicable File Name:  DominantColorExtraction.h
//
// modified by mb - remove unnecessary parts,interface
// date: 2009-11-13

#ifndef __DOMINANTCOLOREXTRACTION_H__
#define __DOMINANTCOLOREXTRACTION_H__

#include "Descriptors/DominantColor.h"

#include "AddressLib/vopio.h"

namespace XM
{

class DominantColorExtractionTool
{
public:

    DominantColorExtractionTool(bool normalize = true, bool variance = true, bool spatial = true, int bin1 = 32, int bin2 = 32, int bin3 = 32);
    ~DominantColorExtractionTool();

    DominantColorDescriptor* GetDescriptor();
    int SetDescriptor(DominantColorDescriptor* aDominantColorDescriptor);

    // RGB image is assumed, can be modified
    // modifications in Extract() VariancePresent, etc, input parameters

    // set source image whose descriptor is to be computed
    // alpha channel (a_chan) for arbtirary shape
    int setSourceImage(MomVop* img);

    // set bin numbers to the specified values - to quantize the centroids to these values
    void setBinNumbers( int bin1 = 32, int bin2 = 32, int bin3 = 32 );

    // extract the descriptor
    // varFlag: calculate variance ?
    // spatFlag: calculate spatial variance ?
    unsigned long extract( bool normalize = true, bool varFlag = true, bool spatFlag = true);
    unsigned long extract(MomVop* img, bool normalize = true, bool varFlag = true, bool spatFlag = true);


private:

    void Extract( float *imdata, int imsize, unsigned char *quantImageAlpha, MomVop *ImageMedia );
    double Cluster( int *closest, float *imdata, int imsize, unsigned char *quantImageAlpha );
    void Centroids( int *closest, float *imdata, int imsize, unsigned char *quantImageAlpha );
    double Dist( int *closest, float *imdata, int imsize, unsigned char *quantImageAlpha );
    void Vars( int *closest, float *imdata, int imsize, unsigned char *quantImageAlpha );
    void Split( int *closest, float *imdata, int imsize, double factor, unsigned char *quantImageAlpha );
    void Agglom( double distthr );

    int QuantizeSC(double sc);
    int GetSpatialCoherency(float *ColorData, int dim, int N, float **col_float,
              unsigned char *quantImageAlpha, MomVop *ImageMedia);
    double GetCoherencyWithColorAllow(float *ColorData, int dim, bool *IVisit,
                    float l, float u, float v, float Allow,
                    int NeighborRange,
                    unsigned int *OUTPUT_Corres_Pixel_Count,
                    MomVop *ImageMedia);

    DominantColorDescriptor* m_Descriptor;

    // image whose descriptor is to be computed
    MomVop* srcImage;

    int     m_MaxSize;		// Maximun DominantColor Number (initial value DCNMAX = 8)
    int     m_CurrSize;		// current (&final) size(number) of the dominant colors
    float*  m_Weights;		// Percentage of DominantColor
    float** m_Centroids;		// ColorValue of DominantColor (LUV color space, float)
    float** m_Variances;		// ColorVariance of DominantColor (float)
    float   m_Coherency;		// Spatial Coherency of DominantColor proposed by LG

    // added by mb
    bool spatialCoherFlag;	// compute spatial coherency? (default: true)
    bool normalizeFlag;		// normalize the descriptor values to MPEG-7 ranges?
    //bin values to quantize the colors
    int binNum1;				// default: 32
    int binNum2;				// default: 32
    int binNum3;				// default: 32

};

};

#endif
