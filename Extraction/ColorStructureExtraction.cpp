////////////////////////////////////////////////////////////////////////
//
// ColorStructureExtraction.cpp
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

#ifndef MAX
#define MAX(a,b) ((a) < (b) ? (b) : (a))
#endif

#include <cassert>
#include <cstring>
#include <cstdio>
#include <cmath>
#include <cstdlib>


#include "AddressLib/momusys.h"

#include "Extraction/ColorStructureExtraction.h"


//=============================================================================


using namespace XM;


// Hard coded ColorQuantization parameters
const int ColorStructureExtractionTool::
diffThresh[NUM_COLOR_QUANT_SPACE][MAX_SUB_SPACE+1] = {
	{0,6,60,110,256,-1},
	{0,6,20,60,110,256},
	{0,6,20,60,110,256},
	{0,6,20,60,110,256}};
const int ColorStructureExtractionTool::
nHueLevels[NUM_COLOR_QUANT_SPACE][MAX_SUB_SPACE] = {
	{1,4,4,4,0},
	{1,4,4,8,8},
	{1,4,8,8,8},
	{1,4,16,16,16}};
const int ColorStructureExtractionTool::
nSumLevels[NUM_COLOR_QUANT_SPACE][MAX_SUB_SPACE] = {
	{8,4,1,1,0},
	{8,4,4,2,1},
	{16,4,4,4,4},
	{32,8,4,4,4}};
// The following could be derived implicitly from nHue and nSum
// Reverse these to order quantization from inside to outside
const int ColorStructureExtractionTool::
nCumLevels[NUM_COLOR_QUANT_SPACE][MAX_SUB_SPACE] = {
//	{0,8,24,28,0},
//	{0,8,24,40,56},
//	{0,16,32,64,96},
//	{0,32,64,128,192}};
	{24,8,4,0,0},
	{56,40,24,8,0},
	{112,96,64,32,0},
	{224,192,128,64,0}};

const double ColorStructureExtractionTool::
amplThresh[] = {0.0, 0.000000000001, 0.037, 0.08, 0.195, 0.32};
const int ColorStructureExtractionTool::
nAmplLevels[] = {1, 25, 20, 35, 35, 140};

unsigned char *ColorStructureExtractionTool::
colorQuantTable[NUM_COLOR_QUANT_SPACE] = {0,0,0,0};

unsigned char cqt256_128[] = {
  0,   1,   2,   3,   0,   1,   2,   3,   4,   5,   6,   7,   4,   5,   6,   7,
  8,   9,  10,  11,   8,   9,  10,  11,  12,  13,  14,  15,  12,  13,  14,  15,
 16,  17,  18,  19,  16,  17,  18,  19,  20,  21,  22,  23,  20,  21,  22,  23,
 24,  25,  26,  27,  24,  25,  26,  27,  28,  29,  30,  31,  28,  29,  30,  31,
 32,  33,  34,  35,  32,  33,  34,  35,  36,  37,  38,  39,  36,  37,  38,  39,
 40,  41,  42,  43,  40,  41,  42,  43,  44,  45,  46,  47,  44,  45,  46,  47,
 48,  49,  50,  51,  48,  49,  50,  51,  52,  53,  54,  55,  52,  53,  54,  55,
 56,  57,  58,  59,  56,  57,  58,  59,  60,  61,  62,  63,  60,  61,  62,  63,
 64,  65,  66,  67,  64,  65,  66,  67,  68,  69,  70,  71,  68,  69,  70,  71,
 72,  73,  74,  75,  72,  73,  74,  75,  76,  77,  78,  79,  76,  77,  78,  79,
 80,  81,  82,  83,  80,  81,  82,  83,  84,  85,  86,  87,  84,  85,  86,  87,
 88,  89,  90,  91,  88,  89,  90,  91,  92,  93,  94,  95,  92,  93,  94,  95,
 96,  96,  97,  97,  98,  98,  99,  99, 100, 100, 101, 101, 102, 102, 103, 103,
104, 104, 105, 105, 106, 106, 107, 107, 108, 108, 109, 109, 110, 110, 111, 111,
112, 112, 113, 113, 114, 114, 115, 115, 116, 116, 117, 117, 118, 118, 119, 119,
120, 120, 121, 121, 122, 122, 123, 123, 124, 124, 125, 125, 126, 126, 127, 127 };
unsigned char cqt256_064[] = {
 0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,  1,  1,  1,  1,
 2,  2,  2,  2,  2,  2,  2,  2,  3,  3,  3,  3,  3,  3,  3,  3,
 4,  4,  4,  4,  4,  4,  4,  4,  5,  5,  5,  5,  5,  5,  5,  5,
 6,  6,  6,  6,  6,  6,  6,  6,  7,  7,  7,  7,  7,  7,  7,  7,
 8,  8,  9,  9,  8,  8,  9,  9, 10, 10, 11, 11, 10, 10, 11, 11,
12, 12, 13, 13, 12, 12, 13, 13, 14, 14, 15, 15, 14, 14, 15, 15,
16, 16, 17, 17, 16, 16, 17, 17, 18, 18, 19, 19, 18, 18, 19, 19,
20, 20, 21, 21, 20, 20, 21, 21, 22, 22, 23, 23, 22, 22, 23, 23,
24, 25, 26, 27, 24, 25, 26, 27, 24, 25, 26, 27, 24, 25, 26, 27,
28, 29, 30, 31, 28, 29, 30, 31, 28, 29, 30, 31, 28, 29, 30, 31,
32, 33, 34, 35, 32, 33, 34, 35, 32, 33, 34, 35, 32, 33, 34, 35,
36, 37, 38, 39, 36, 37, 38, 39, 36, 37, 38, 39, 36, 37, 38, 39,
40, 40, 41, 41, 42, 42, 43, 43, 44, 44, 45, 45, 46, 46, 47, 47,
48, 48, 49, 49, 50, 50, 51, 51, 52, 52, 53, 53, 54, 54, 55, 55,
56, 56, 56, 56, 57, 57, 57, 57, 58, 58, 58, 58, 59, 59, 59, 59,
60, 60, 60, 60, 61, 61, 61, 61, 62, 62, 62, 62, 63, 63, 63, 63 };
unsigned char cqt256_032[] = {
 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
 2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,
 3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,
 4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,
 5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,
 6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,
 7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,
 8,  9, 10, 11,  8,  9, 10, 11,  8,  9, 10, 11,  8,  9, 10, 11,
12, 13, 14, 15, 12, 13, 14, 15, 12, 13, 14, 15, 12, 13, 14, 15,
16, 17, 18, 19, 16, 17, 18, 19, 16, 17, 18, 19, 16, 17, 18, 19,
20, 21, 22, 23, 20, 21, 22, 23, 20, 21, 22, 23, 20, 21, 22, 23,
 8,  8,  9,  9, 10, 10, 11, 11, 12, 12, 13, 13, 14, 14, 15, 15,
16, 16, 17, 17, 18, 18, 19, 19, 20, 20, 21, 21, 22, 22, 23, 23,
24, 24, 24, 24, 25, 25, 25, 25, 26, 26, 26, 26, 27, 27, 27, 27,
28, 28, 28, 28, 29, 29, 29, 29, 30, 30, 30, 30, 31, 31, 31, 31 };
unsigned char cqt128_064[] = {
 0,  0,  0,  0,  1,  1,  1,  1,  2,  2,  2,  2,  3,  3,  3,  3,
 4,  4,  4,  4,  5,  5,  5,  5,  6,  6,  6,  6,  7,  7,  7,  7,
 8,  8,  9,  9, 10, 10, 11, 11, 12, 12, 13, 13, 14, 14, 15, 15,
16, 16, 17, 17, 18, 18, 19, 19, 20, 20, 21, 21, 22, 22, 23, 23,
24, 25, 26, 27, 24, 25, 26, 27, 28, 29, 30, 31, 28, 29, 30, 31,
32, 33, 34, 35, 32, 33, 34, 35, 36, 37, 38, 39, 36, 37, 38, 39,
40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55,
56, 56, 57, 57, 58, 58, 59, 59, 60, 60, 61, 61, 62, 62, 63, 63 };
unsigned char cqt128_032[] = {
 0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,  1,  1,  1,  1,
 2,  2,  2,  2,  2,  2,  2,  2,  3,  3,  3,  3,  3,  3,  3,  3,
 4,  4,  4,  4,  4,  4,  4,  4,  5,  5,  5,  5,  5,  5,  5,  5,
 6,  6,  6,  6,  6,  6,  6,  6,  7,  7,  7,  7,  7,  7,  7,  7,
 8,  9, 10, 11,  8,  9, 10, 11, 12, 13, 14, 15, 12, 13, 14, 15,
16, 17, 18, 19, 16, 17, 18, 19, 20, 21, 22, 23, 20, 21, 22, 23,
 8,  9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23,
24, 24, 25, 25, 26, 26, 27, 27, 28, 28, 29, 29, 30, 30, 31, 31 };
unsigned char cqt064_032[] = {
 0,  0,  1,  1,  2,  2,  3,  3,  4,  4,  4,  4,  5,  5,  5,  5,
 6,  6,  6,  6,  7,  7,  7,  7,  8,  9, 10, 11, 12, 13, 14, 15,
16, 17, 18, 19, 20, 21, 22, 23,  8,  9, 10, 11, 12, 13, 14, 15,
16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31 };

#ifndef REGENERATE_TRANSFORM_TABLE
unsigned char *ColorStructureExtractionTool::
colorQuantTransform[NUM_COLOR_QUANT_SPACE][NUM_COLOR_QUANT_SPACE] =
{{0,		0,		0,		0},
 {cqt064_032,	0,		0,		0},
 {cqt128_032,	cqt128_064,	0,		0},
 {cqt256_032,	cqt256_064,	cqt256_128,	0}};

#else
unsigned char *ColorStructureExtractionTool::
colorQuantTransform[NUM_COLOR_QUANT_SPACE][NUM_COLOR_QUANT_SPACE] =
{{0,0,0,0},
 {0,0,0,0},
 {0,0,0,0},
 {0,0,0,0}};
#endif




//=============================================================================
ColorStructureExtractionTool::ColorStructureExtractionTool():
m_Descriptor(NULL),
srcImage(NULL)
{
    m_Descriptor = new ColorStructureDescriptor();
#ifdef REGENERATE_TRANSFORM_TABLE
	int iOrig, iNew;
	for(iOrig = 3; iOrig >= 0; iOrig--)
		BuildColorQuantTable(iOrig);

	for(iOrig = 3; iOrig >= 0; iOrig--)
		for(iNew = iOrig - 1; iNew >= 0; iNew--)
			BuildTransformTable(iOrig, iNew);
#endif
}


//----------------------------------------------------------------------------
ColorStructureExtractionTool::~ColorStructureExtractionTool()
{
	// release descriptor

	// commented out - mb - 07.07.2008
	// descriptor will be released by the clients
	//if (m_Descriptor)
	//	m_Descriptor->release();
}


// set source image whose descriptor is to be computed
int ColorStructureExtractionTool::setSourceImage(MomVop* img)

{
	if( img == NULL ) return -1;
	srcImage = img;
	return 0;

}

//----------------------------------------------------------------------------
ColorStructureDescriptor* ColorStructureExtractionTool::GetDescriptor()

{
	return m_Descriptor;
}

//----------------------------------------------------------------------------
int ColorStructureExtractionTool::
SetDescriptor( ColorStructureDescriptor* aColorStructureDescriptor)

{
	/* check if new value is different from old value*/
	if (m_Descriptor == aColorStructureDescriptor)
        return 0;
	if (m_Descriptor)
        delete m_Descriptor;
	m_Descriptor = aColorStructureDescriptor;

	return 0;
}

//----------------------------------------------------------------------------
// img: input image whose descriptor is to be computed
// descriptorSize: final descriptor size
unsigned long ColorStructureExtractionTool::extract( MomVop *img, int descriptorSize )
{
	this->setSourceImage( img );
	return this->extract( descriptorSize );

}


//----------------------------------------------------------------------------
// img: input image whose descriptor is to be computed
// descriptorSize: BASE_QUANT_SPACE = 256
unsigned long ColorStructureExtractionTool::extract( MomVop *img)
{
	this->setSourceImage( img );
	return this->extract( BASE_QUANT_SPACE );

}

//----------------------------------------------------------------------------
// input image must have been set using SetSourceImage function
// descriptorSize: fBASE_QUANT_SPACE = 256
unsigned long ColorStructureExtractionTool::extract( void )
{
	return this->extract( BASE_QUANT_SPACE );

}


//----------------------------------------------------------------------------
// source ýmage: RGB (converted to HMMD within the function)
// descriptorSize: final descriptor size
unsigned long ColorStructureExtractionTool::extract( int descriptorSize )
{
	MomVop *ImageMedia;
	unsigned long	*quantImageBuffer;


	// First check that it's all correctly initialised
	if (m_Descriptor==NULL) return (unsigned long)-1;

	//if(strcmp(m_Descriptor->GetName(),
	//	"ColorStructureDescriptionInterface") != 0) return (unsigned long)-1;


	///////////////////////////////////////////////////
	// Initial extraction is always to Base size     //
	///////////////////////////////////////////////////
	if (!m_Descriptor->SetSize(BASE_QUANT_SPACE))		// BASE_QUANT_SPACE = 256
	{
		for (int i=0; i < BASE_QUANT_SPACE; i++)
			m_Descriptor->SetElement(i,0);
	}


	// source image
	if ( srcImage == NULL ) return (unsigned long)-1;
	ImageMedia = srcImage;

	// Allocate space for quantized image
	int imageSize = ImageMedia->width * ImageMedia->height;
	quantImageBuffer = new unsigned long[imageSize];
	if (!quantImageBuffer)
	{
		//std::cerr << "Allocation error - CSD:extract()" << std::endl;
		return (unsigned long)-1;
	}


	///////////////////////////////////////////
	// Convert color space and quantize      //
	///////////////////////////////////////////
	int i;
	int H, S, D;
	for (i = 0; i < imageSize; i++)
	{
		// rgb is vyu in MomVop
		RGB2HMMD( ImageMedia->v_chan->data->u[i],
				  ImageMedia->y_chan->data->u[i],
				  ImageMedia->u_chan->data->u[i],
				  H, S, D );

		quantImageBuffer[i] = QuantHMMD(H, S, D, BASE_QUANT_SPACE_INDEX);
	}


	///////////////////////////////////////////
	// Extract histogram of color structure  //
	///////////////////////////////////////////
	unsigned long	col, row, index;
	unsigned long	Norm;
	unsigned long	modifiedImageWidth, modifiedImageHeight, moduloSlideHeight;
	unsigned long	*slideHist;
	unsigned long	*pAdd, *pDel, *pAddStop;
	unsigned char	*pAddAlpha, *pDelAlpha, *quantImageAlpha = 0;

	if( ImageMedia->a_chan && ImageMedia->a_chan->data )
		quantImageAlpha = ImageMedia->a_chan->data->u;

	// determine working dimensions
	//const unsigned long slideWidth = 15, slideHeight = 15, subSample = 1;

	double logArea =  log(ImageMedia->width * ImageMedia->height) / log(2);
	int scalePower = (int)floor(0.5 * logArea - 8 + 0.5);
	scalePower = MAX(0, scalePower);
	unsigned long subSample = 1 << scalePower;
	unsigned long slideWidth = 8 * subSample;
	unsigned long slideHeight = 8 * subSample;

	modifiedImageWidth  = ImageMedia->width  - (slideWidth  - 1);
	modifiedImageHeight = ImageMedia->height - (slideHeight - 1);
	//moduloSlideHeight = (int)
	//	((slideHeight + subSample - 1)/subSample) * subSample;
	// How many actual rows from the first row of a slideHist
	//	to the first row of the next slideHist
	// If slideHeight is not a multiple of subSample, and n is on
	//	a subSample, then n+slideHeight won't be on a subSample
	if(slideHeight % subSample == 0) {
		moduloSlideHeight = slideHeight;
	}
	else {
		moduloSlideHeight = slideHeight +
		  (subSample - slideHeight % subSample);
	}

	//assert( slideWidth & 1);	// odd
	//assert( slideHeight & 1);	// odd

	// allocate local sliding window histogram
	slideHist = new unsigned long[BASE_QUANT_SPACE];
	assert( slideHist );


	// loop through columns
	for( col = 0; col < modifiedImageWidth; col += subSample )
	{
		// reset and fill in the first (top of image) full sliding window histograms
		memset( (void *) slideHist, 0, BASE_QUANT_SPACE * sizeof(unsigned long) );
		for( row = 0; row < slideHeight; row += subSample )
		{
			pAdd  =    &quantImageBuffer[row * ImageMedia->width + col];
			pAddAlpha = &quantImageAlpha[row * ImageMedia->width + col];	// May be illegal pointer
			pAddStop = pAdd + slideWidth;
			for( ; pAdd < pAddStop; pAdd += subSample, pAddAlpha += subSample )
				if( !quantImageAlpha || *pAddAlpha )
					slideHist[*pAdd] ++;
		}

		// update histogram from first sliding window histograms
		for ( index = 0; index < BASE_QUANT_SPACE; index ++)
		{
			if( slideHist[ index ] )
				m_Descriptor->
				SetElement( index, m_Descriptor->
							GetElement(index) + 1 );
		}

		// slide the window down the rest of the rows
		for( row = subSample; row < modifiedImageHeight; row += subSample )
		{
			pDel  =    &quantImageBuffer[(row - subSample) * ImageMedia->width + col];
			pDelAlpha = &quantImageAlpha[(row - subSample) * ImageMedia->width + col];
			pAdd  =    &quantImageBuffer[(row + moduloSlideHeight - subSample) * ImageMedia->width + col];
			pAddAlpha = &quantImageAlpha[(row + moduloSlideHeight - subSample) * ImageMedia->width + col];
			pAddStop = pAdd + slideWidth;
			for( ; pAdd < pAddStop; pDel += subSample, pAdd += subSample,
									pDelAlpha += subSample, pAddAlpha += subSample )
			{
				if( !quantImageAlpha || *pDelAlpha )
					slideHist[*pDel] --;
				if( !quantImageAlpha || *pAddAlpha )
					slideHist[*pAdd] ++;
			}

			// update histogram from sliding window histogram
			for ( index = 0; index < BASE_QUANT_SPACE; index ++)
			{
				if( slideHist[ index ] )
					m_Descriptor->
						SetElement( index, m_Descriptor->
											GetElement(index) + 1 );
			}
		}
	}

	// Free memory
	delete [] slideHist;
	delete [] quantImageBuffer;


	Norm = ((ImageMedia->height - slideHeight)/subSample + 1)
		 * ((ImageMedia->width  - slideWidth )/subSample + 1);


	///////////////////////////////////////////
	// Requantize color space to Target size //
	///////////////////////////////////////////
	UnifyBins(Norm, descriptorSize);


	///////////////////////////////////////////
	// Quantize the Bin Amplitude			 //
	///////////////////////////////////////////
	QuantAmplNonLinear(Norm);


	return 0;
}

//----------------------------------------------------------------------------
int ColorStructureExtractionTool::DownQuantHMMD(int nNewSize)
{
	int nOrigSize = m_Descriptor->GetSize();

	if (nOrigSize == nNewSize)
		return 0;

	assert(nOrigSize > nNewSize);


	// Linearize, Unify, and Nonlinearize; all at 2^20
	ConvertAmplLinear	((1 << 20) - 1);
	UnifyBins		((1 << 20) - 1, nNewSize);
	QuantAmplNonLinear	((1 << 20) - 1);

	return 0;
}


//----------------------------------------------------------------------------
void ColorStructureExtractionTool::RGB2HMMD(int R, int G, int B,
										   int &H, int &S, int &D)
{
	int max, min;
	float hue;

	max=R;
	if(max<G) {max=G;}
	if(max<B) {max=B;}

	min=R;
	if(min>G) {min=G;}
	if(min>B) {min=B;}

	if (max == min) // ( R == G == B )//exactly gray
		hue = -1; //hue is undefined

	else
	{   //solve Hue
		if(R==max)
			hue=((G-B)/(float)(max-min));

		else if(G==max)
			hue=(2.0+(B-R)/(float)(max-min));

		else if(B==max)
			hue=(4.0+(R-G)/(float)(max-min));

		hue*=60;
		if(hue<0.0) hue+=360;
	}

	H = (long)(hue + 0.5);				//range [0,360]
	S = (long)((max + min)/2.0 + 0.5);	//range [0,255]
	D = (long)(max - min + 0.5);		//range [0,255]
	return;
}

//----------------------------------------------------------------------------
int ColorStructureExtractionTool::QuantHMMD(int H, int S, int D, int N)
{
	// Test bounds of input
	assert(H >= 0 && H <= 360);
	assert(D >= 0 && D < 256);
	assert(S >= (D+1)>>1 && S < 256 - (D>>1));

	if (colorQuantTable[N])
		return colorQuantTable[N][(H<<16) + (S<<8) + D];

	// Note: lower threshold boundary is inclusive,
	// i.e. diffThresh[..][m] <= (D of subspace m) < diffThresh[..][m+1]

	// Quantize the Difference component, find the Subspace
	int iSub = 0;
	while( diffThresh[N][iSub + 1] <= D )
		iSub ++;

	// Valid subspace ?
	assert(diffThresh[N][iSub + 1] >= 0);

	// Quantize the Hue component
	int Hindex = (int)((H / 360.0) * nHueLevels[N][iSub]);
	if ( H == 360 )
		Hindex = 0;

	// Quantize the Sum component
	// The min value of Sum in a subspace is 0.5*diffThresh (see HMMD slice)
	int Sindex = (int)floor((S - 0.5*diffThresh[N][iSub])
							* nSumLevels[N][iSub]
							/ (255 - diffThresh[N][iSub]));
	if ( Sindex >= nSumLevels[N][iSub] )
		Sindex   = nSumLevels[N][iSub] - 1;

/* The following quantization of Sum is more uniform and doesn't require the bounds check
	int Sindex = (int)floor((S - 0.5*diffThresh[N][iSub])
							* nSumLevels[N][iSub]
							/ (256 - diffThresh[N][iSub]));
*/

	return nCumLevels[N][iSub] + Hindex*nSumLevels[N][iSub] + Sindex;
}

//----------------------------------------------------------------------------
int ColorStructureExtractionTool::BuildColorQuantTable(int iColorQuantSpace)
{
	const int nSize = 361*256*256;
	// TODO cut size in half, using triangular properties

	if (colorQuantTable[iColorQuantSpace])
		return 1;

	unsigned char *tbl = new unsigned char[nSize];
	memset(tbl,0,nSize);

	if (!tbl)
	{
		//std::cerr << "Could not allocate " << nSize << " for Color Quant Table!" << std::endl;
		return 0;
	}

	//std::cerr << "Building Color Quant Table (" << (32 << iColorQuantSpace) << ")" << std::endl;

	int H, S, D;
	for ( H = 0; H < 361; H++)
	{
		for ( D = 0; D < 256; D++ )
		{
			int beginS = (D+1)>>1;
			int endS = 256 - (D>>1);	// Color Space extraction rounds up (i.e. max=255,min=254 => sum=255)
			for ( S = beginS; S < endS; S++ )
			{
				tbl[(H<<16) + (S<<8) + D] = QuantHMMD(H, S, D, iColorQuantSpace);
			}
		}
	}

	colorQuantTable[iColorQuantSpace] = tbl;
	return 1;
}


//----------------------------------------------------------------------------
int ColorStructureExtractionTool::TransformBinIndex(int iOrig,
													int iOrigColorQuantSpace,
													int iNewColorQuantSpace)
{
	// ColorQuantSpace enums are in ascending order
	assert(iOrigColorQuantSpace > iNewColorQuantSpace);

	// Build transform table if not already present
	if (!colorQuantTransform[iOrigColorQuantSpace][iNewColorQuantSpace])
		BuildTransformTable(iOrigColorQuantSpace, iNewColorQuantSpace);

	if (!colorQuantTransform[iOrigColorQuantSpace][iNewColorQuantSpace])
	{
		//std::cerr << "Unable to BuildTransformTable" << std::endl;
		exit(1);
	}

	return colorQuantTransform[iOrigColorQuantSpace][iNewColorQuantSpace][iOrig];
}


//----------------------------------------------------------------------------
int ColorStructureExtractionTool::BuildTransformTable(int iOrigColorQuantSpace,
													  int iNewColorQuantSpace)
{
	const int maxMatchTest = 27;	// Allow deviation +/- 1 in 3D
	int iMatch[maxMatchTest], nMatch[maxMatchTest], nUniqueMatch = 0;
	int iOrig, iNew;
	int H, S, D;
	int iTest;

	// Allocate
	unsigned char *tbl = new unsigned char[GetBinSize(iOrigColorQuantSpace)];

	if (!tbl)
	{
		//std::cerr << "Could not allocate " << GetBinSize(iOrigColorQuantSpace) <<
		//	" for Color Space Transform!" << std::endl;
		return 0;
	}

	//std::cerr << "Building Transform Table (" << (32 << iOrigColorQuantSpace) <<
	//	" => " << (32 << iNewColorQuantSpace) << ")" << std::endl;

	// Loop through originating cells
	for (iOrig = 0; iOrig < GetBinSize(iOrigColorQuantSpace); iOrig++ )
	{

		// Loop through 24 bit HMMD values
		for (H = 0; H < 361; H++)
		{
			for (D = 0; D < 256; D++)
			{
				int beginS = (D+1)>>1;
				int endS = 256 - (D>>1);
				for (S = beginS; S < endS; S++)
				{
					// Check match and get new space
					if (QuantHMMD(H, S, D, iOrigColorQuantSpace) == iOrig)
					{
						iNew = QuantHMMD(H, S, D, iNewColorQuantSpace);

						// Find result in Match array
						for (iTest = 0; iTest < nUniqueMatch; iTest++)
						{
							if ( iMatch[iTest] == iNew )
								break;
						}

						// Accumulate stats on best match
						if (iTest == nUniqueMatch)
						{
							assert( nUniqueMatch < maxMatchTest);
							iMatch[nUniqueMatch] = iNew;
							nMatch[nUniqueMatch] = 1;
							nUniqueMatch ++;
						}
						else
							nMatch[iTest] ++;
					}
				}
			}
		}

		// Report on inexact matches
		if (nUniqueMatch > 1)
		{
			int nTotal = 0;
			fprintf(stdout,"%d to %d duplicate mapping: %d => ", GetBinSize(iOrigColorQuantSpace),
														 GetBinSize(iNewColorQuantSpace), iOrig);
			for(iTest = 0; iTest < nUniqueMatch; iTest++)
				nTotal += nMatch[iTest];
			for(iTest = 0; iTest < nUniqueMatch; iTest++)
				fprintf(stdout, "%d (%2d%%), ", iMatch[iTest], 100*nMatch[iTest]/nTotal);
			fprintf(stdout,"\n");
		}
		if (!nUniqueMatch)
		{
			fprintf(stderr, "No match found in BuildTransformTable(%d,%d)",
				iOrigColorQuantSpace,iNewColorQuantSpace);
			exit(1);
		}


		// Select the best match
		int iBest = -1, nBest = 0;
		for ( iTest = 0; iTest < nUniqueMatch; iTest ++)
		{
			if ( nMatch[iTest] > nBest )
			{
				nBest = nMatch[iTest];
				iBest = iMatch[iTest];
			}
		}

		// Update table and reset stats
		tbl[iOrig] = iBest;
		nUniqueMatch = 0;

	}	// Loop through orig indices


	// Print out transform table (then, hard-code into this module)
	fprintf(stdout, "Color Space Transform: %d to %d\n", GetBinSize(iOrigColorQuantSpace),
														 GetBinSize(iNewColorQuantSpace));

	int nPrec = GetBinSize(iNewColorQuantSpace)>100?3:2;
	for (iOrig = 0; iOrig < GetBinSize(iOrigColorQuantSpace); iOrig ++)
	{
		fprintf(stdout, "%*d", nPrec, tbl[iOrig]);
		if (iOrig != GetBinSize(iOrigColorQuantSpace)-1)
			fprintf(stdout, ",");
		if ((iOrig+1) % 16 == 0)
			fprintf(stdout, "\n");
		else
			fprintf(stdout, " ");
	}
	fflush(stdout);


	if (colorQuantTransform[iOrigColorQuantSpace][iNewColorQuantSpace])
		delete colorQuantTransform[iOrigColorQuantSpace][iNewColorQuantSpace];

	colorQuantTransform[iOrigColorQuantSpace][iNewColorQuantSpace] = tbl;
	return 1;
}

//----------------------------------------------------------------------------
int ColorStructureExtractionTool::QuantAmplNonLinear(unsigned long Norm)
{
	unsigned long iBin, TotalNoOfBins, iQuant;
	const int nAmplLinearRegions = sizeof(nAmplLevels)/sizeof(nAmplLevels[0]);
	int nTotalLevels = 0;

	// Calculate total levels
	for (iQuant = 0; iQuant < nAmplLinearRegions; iQuant++)
		nTotalLevels += nAmplLevels[iQuant];

	// Get size
	TotalNoOfBins = m_Descriptor->GetSize();

	// Loop through bins
	for ( iBin = 0; iBin < TotalNoOfBins; iBin ++)
	{
		// Get bin amplitude
		double val = m_Descriptor->GetElement(iBin);

		// Normalize
		val /= Norm;
		assert (val>=0.0); assert (val<= 1.0);

		// Find quantization boundary and base value
		int quantValue = 0;
		for (iQuant = 0; iQuant+1 < nAmplLinearRegions; iQuant++ )
		{
			if (val < amplThresh[iQuant+1])
				break;
			quantValue += nAmplLevels[iQuant];
		}

		// Quantize
		double nextThresh = (iQuant+1 < nAmplLinearRegions) ? amplThresh[iQuant+1] : 1.0;
		val = floor(quantValue +
					(val - amplThresh[iQuant]) *
					(nAmplLevels[iQuant] / (nextThresh - amplThresh[iQuant])));

		// Limit (and alert), one bin contains all of histogram
		if (val == nTotalLevels)
		{
			// Possible (though rare) case
//			cerr << "Degenerate case, histogram bin " << iBin << " has value " <<
//				GetHistogramDescriptorInterface()->GetElement(iBin) << " of " << Norm << " Norm" << endl;
			val = nTotalLevels - 1;
		}
		assert(val >= 0.0); assert(val < nTotalLevels);

		// Set value into histogram
		m_Descriptor->SetElement( iBin, (int)val );
	}

	return 0;
}


//----------------------------------------------------------------------------
int ColorStructureExtractionTool::UnifyBins(unsigned long Norm, int nTargSize)
{
	double *pBin;
	int iTargBin, iOrigBin;
	int nOrigSize = m_Descriptor->GetSize();

	if (nTargSize == nOrigSize)
		return 0;

	// Only down quantize
	assert ( nTargSize < nOrigSize );

	// Grab temp space
	pBin = new double [nTargSize];
	assert(pBin);
	for (iTargBin = 0; iTargBin < nTargSize; iTargBin++)
		pBin[iTargBin] = 0.0;

	int iTargSpace = GetColorQuantSpace(nTargSize);
	int iOrigSpace = GetColorQuantSpace(nOrigSize);


	// Unify
	for (iOrigBin = 0; iOrigBin < nOrigSize; iOrigBin++)
	{
		iTargBin = TransformBinIndex(iOrigBin, iOrigSpace, iTargSpace);
		pBin[iTargBin] += m_Descriptor->GetElement(iOrigBin);
	}

	// Resize the descriptor
	m_Descriptor->SetSize(nTargSize);

	// Clip and insert
	for (iTargBin = 0; iTargBin < nTargSize; iTargBin++)
	{
		if (pBin[iTargBin] > Norm)
			pBin[iTargBin] = Norm;

		m_Descriptor->SetElement(iTargBin, pBin[iTargBin]);
	}

	return 0;
}

//----------------------------------------------------------------------------
int ColorStructureExtractionTool::ConvertAmplLinear(unsigned long NewNorm)
{
	unsigned long iBin, TotalNoOfBins;
	const unsigned long OldNorm = 255;
	const int nAmplLinearRegions = sizeof(nAmplLevels)/sizeof(nAmplLevels[0]);
	int iQuant;

	// Get histogram size
	TotalNoOfBins = m_Descriptor->GetSize();

#ifdef _DEBUG
	// Assume that OldNorm is equal to the maximum quant level
	// Test assumption
	int cumQuant = 0;
	for( iQuant = 0; iQuant < nAmplLinearRegions; iQuant++)
		cumQuant += nAmplLevels[iQuant];
	assert(cumQuant == OldNorm + 1);
#endif

	// Loop through bins
	for ( iBin = 0; iBin < TotalNoOfBins; iBin ++)
	{
		// Get bin amplitude
		double val = m_Descriptor->GetElement(iBin);
		assert(val <= OldNorm);

		// Find quantization boundary and base value
		int quantBdry = 0;
		for( iQuant = 0; iQuant < nAmplLinearRegions; iQuant++ )
		{
			if (val < quantBdry + nAmplLevels[iQuant])
				break;
			quantBdry += nAmplLevels[iQuant];
		}
		assert(iQuant < nAmplLinearRegions);

		// UnQuantize, find central point of this quantization level
		double nextQuantBdry = quantBdry + nAmplLevels[iQuant];
		double nextThresh = iQuant+1 < nAmplLinearRegions ? amplThresh[iQuant+1] : 1.0;
		val = amplThresh[iQuant] +
			  (nextThresh - amplThresh[iQuant]) *
			  (val + 0.5 - quantBdry) / (nextQuantBdry - quantBdry);

		// UnNormalize (new)
		assert(val <= 1.0);
		val = floor(val * NewNorm + 0.5);

		// Set value into histogram
		m_Descriptor->SetElement( iBin, (int)val );
	}

	return 0;
}






//----------------------------------------------------------------------------
int ColorStructureExtractionTool::GetColorQuantSpace(int size)
{
	if		(size == 256)
		return 3;
	else if (size == 128)
		return 2;
	else if (size == 64)
		return 1;
	else if (size == 32)
		return 0;
	else
	{
		assert("Out of bounds GetColorQuantSpace");
		return -1;
	}
}

//----------------------------------------------------------------------------
int ColorStructureExtractionTool::GetBinSize(int iColorQuantSpace)
{
	if (iColorQuantSpace <= 3 &&
		iColorQuantSpace >= 0)
		return 32 << iColorQuantSpace;
	else
	{
		assert("Out of bounds GetBinSize");
		return 0;
	}
}
