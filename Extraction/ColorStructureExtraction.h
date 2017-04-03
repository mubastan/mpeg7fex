////////////////////////////////////////////////////////////////////////
//
// ColorStructureExtraction.h
//
// This software module was originally developed by
//
// Jim Errico, Sharp Laboratories of America, Camas, WA
// Peter van Beek, Sharp Laboratories of America, Camas, WA
//
//
// in the course of development of the MPEG-7 standard (ISO/IEC 15938).
// This software module is an implementation of a part of one or more
// MPEG-7 tools as specified by the MPEG-7 standard (ISO/IEC 15938).
// ISO/IEC gives users of the MPEG-7 standard free license to this
// software module or modifications thereof for use in hardware or
// software products claiming conformance to the MPEG-7 standard.
// Those intending to use this software module in hardware or software
// products are advised that its use may infringe existing patents.
// The original developer of this software module and his/her company,
// the subsequent editors and their companies, and ISO/IEC have no
// liability for use of this software module or modifications thereof
// in an application. No license to this software is granted for non
// MPEG-7 conforming products.
// Sharp Laboratories of America retains full right to use the software
// module for their own purpose, assign or donate the software module
// to a third party and to inhibit third parties from using the software
// module for non MPEG-7 conforming products.
//
// Copyright (c) 2000
//
// This copyright notice must be included in all copies or derivative
// works of this software module.
//
////////////////////////////////////////////////////////////////////////

#ifndef __COLORSTRUCTUREEXTRACTION_H__
#define __COLORSTRUCTUREEXTRACTION_H__

#include "Descriptors/ColorStructure.h"		  // all headers in Descriptors

#include "AddressLib/vopio.h"

namespace XM
{


//=============================================================================
class ColorStructureExtractionTool
{
public:

	ColorStructureExtractionTool();
	virtual ~ColorStructureExtractionTool();

	ColorStructureDescriptor* GetDescriptor();
	int SetDescriptor(ColorStructureDescriptor* aColorStructureDescriptor);

	///////////////////////////////////////////////////////////////
	/// source ýmage: RGB (converted to HMMD within the function)
	/// band order: RGB <==> VYU, openCV: BGR <==> UYV
	///////////////////////////////////////////////////////////////

	// set source image whose descriptor is to be computed
	int setSourceImage(MomVop* img);

	/// input image must have been set using SetSourceImage function
	/// descriptorSize: BASE_QUANT_SPACE = 256
	unsigned long extract();
	/// img: input image whose descriptor is to be computed
	/// descriptorSize: BASE_QUANT_SPACE = 256
	unsigned long extract( MomVop *img);
	unsigned long extract( int descriptorSize );
	unsigned long extract( MomVop *img, int descriptorSize );


	int DownQuantHMMD(int nNewSize);

	static int BuildColorQuantTable(int iColorQuantSpace = 3);


private:

	static int GetColorQuantSpace(int size);
	static int GetBinSize(int iColorQuantSpace);

	// Single pixel/bin operations
	static void RGB2HMMD(int R, int G, int B, int &H, int &S, int &D);
	static int QuantHMMD(int H, int S, int D, int iColorQuantSpace);
	static int TransformBinIndex(int iOrig, int iOrigColorQuantSpace,
											int iNewColorQuantSpace);

	// Full descriptor operations
	int QuantAmplNonLinear(unsigned long OldNorm);
	int ConvertAmplLinear(unsigned long NewNorm);
	int UnifyBins(unsigned long Norm, int nNewSize);

	// Three ways to transform color quant spaces:
	// 1) hard-coded transform table (define PRINT_TRANSFORM_TABLE to generate)
	// 2) build transform table from colorspace table dynamically
	// 3) build transform table using QuantHMMD() repeatedly (very inefficient)
	static int BuildTransformTable(int iOrigColorQuantSpace, int nNewSize);

	enum {	NUM_COLOR_QUANT_SPACE = 4,
			MAX_SUB_SPACE = 5,
			BASE_QUANT_SPACE = 256,
			BASE_QUANT_SPACE_INDEX = 3 };

	static const int diffThresh[NUM_COLOR_QUANT_SPACE][MAX_SUB_SPACE+1];
	static const int nHueLevels[NUM_COLOR_QUANT_SPACE][MAX_SUB_SPACE];
	static const int nSumLevels[NUM_COLOR_QUANT_SPACE][MAX_SUB_SPACE];
	static const int nCumLevels[NUM_COLOR_QUANT_SPACE][MAX_SUB_SPACE];
	static unsigned char *colorQuantTable[NUM_COLOR_QUANT_SPACE];
	static unsigned char *colorQuantTransform[NUM_COLOR_QUANT_SPACE]
											 [NUM_COLOR_QUANT_SPACE];

	static const double amplThresh[];
	static const int nAmplLevels[];


	ColorStructureDescriptor *m_Descriptor;

	// image whose descriptor is to be computed
	MomVop* srcImage;
};

};

#endif
