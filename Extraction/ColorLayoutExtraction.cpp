//////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// NEC Corp., Akio Yamada, Eiji Kasutani
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
// Copyright (c) 1998-1999.
// modified by authors to handle small pictures on Jan. 06, 2000.
// modified by authors for compatibility with Visual C++ Compiler Jan.20, 2000
//
// This notice must be included in all copies or derivative works.
//
// Applicable File Name:  ColorLayoutExtraction.cpp
//

#include <cstdio>
#include <cmath>
#include <cstdlib>

#include <cstring>
#include <cfloat>

#include "Descriptors/ColorLayout.h"                     /* Naming Convention */
#include "ColorLayoutExtraction.h"

#ifndef M_PI
#define M_PI (3.14159265358979323846)
#endif

#ifndef FLT_MAX
#define FLT_MAX 10000000000.0
#endif

static int initextractionwascalled = 0;

using namespace XM;

ColorLayoutExtractionTool::ColorLayoutExtractionTool(int numYCoef, int numCCoef):
m_Descriptor(0),
srcImage(0)
{
	unsigned char scan[64]={/* Zig-Zag scan pattern  */
		0,1,8,16,9,2,3,10,17,24,32,25,18,11,4,5,
		12,19,26,33,40,48,41,34,27,20,13,6,7,14,21,28,
		35,42,49,56,57,50,43,36,29,22,15,23,30,37,44,51,
		58,59,52,45,38,31,39,46,53,60,61,54,47,55,62,63
	};

	init_fdct();
	for(int i=0; i<64; i++)
		zigzag_scan[i]=scan[i];

    m_Descriptor = new ColorLayoutDescriptor(numYCoef, numCCoef);
}


//----------------------------------------------------------------------------
ColorLayoutExtractionTool::~ColorLayoutExtractionTool()
{

	// will be released by the clients!
	//if (m_Descriptor)
	//  m_Descriptor->release();
}


//----------------------------------------------------------------------------
int ColorLayoutExtractionTool::SetSourceMedia(MomVop* img)
{
  if (img==NULL) return -1;
  srcImage = img;
  return 0;
}

//----------------------------------------------------------------------------
ColorLayoutDescriptor* ColorLayoutExtractionTool::GetDescriptor(void)
{
        return m_Descriptor;
}

//----------------------------------------------------------------------------
int ColorLayoutExtractionTool::SetDescriptor(ColorLayoutDescriptor* aColorLayoutDescriptor)

{
	/* check if new value is different from old value */
	if (m_Descriptor == aColorLayoutDescriptor) return 0;
	/* release old interface */
	if (m_Descriptor) delete m_Descriptor;
	/* add new interface */
	m_Descriptor = aColorLayoutDescriptor;

	return 0;
}


//----------------------------------------------------------------------------
unsigned long ColorLayoutExtractionTool::extract(MomVop *img, int NumberOfYCoeff, int NumberOfCCoeff)
{
	initextractionwascalled = 1;

    if(!m_Descriptor) return (unsigned long)-1;


    if( NumberOfYCoeff < 3)
		NumberOfYCoeff = 3;
    else if( NumberOfYCoeff > 64 )
		NumberOfYCoeff = 64;

    if( NumberOfCCoeff < 3 )
		NumberOfCCoeff = 3;
    else if( NumberOfCCoeff > 64 )
		NumberOfCCoeff = 64;

    if (!img) return (unsigned long)-1;
    MomVop *ImageMedia = img;

    m_Descriptor->SetNumberOfYCoeff(NumberOfYCoeff);
    m_Descriptor->SetNumberOfCCoeff(NumberOfCCoeff);

    short small_img[3][64];
    CreateSmallImage(ImageMedia, small_img);
    fdct(small_img[0]);
    fdct(small_img[1]);
    fdct(small_img[2]);

    // authors recommendation;
    // if you need more accuracy, you had better extract the first
    // coefficient (DC components) directly from original image

    int YCoeff[64], CbCoeff[64], CrCoeff[64];

    YCoeff[0]=quant_ydc(small_img[0][0]/8)>>1;
    CbCoeff[0]=quant_cdc(small_img[1][0]/8);
    CrCoeff[0]=quant_cdc(small_img[2][0]/8);
    for(int i=1;i<64;i++){
	YCoeff[i]=quant_ac((small_img[0][(zigzag_scan[i])])/2)>>3;
	CbCoeff[i]=quant_ac(small_img[1][(zigzag_scan[i])])>>3;
	CrCoeff[i]=quant_ac(small_img[2][(zigzag_scan[i])])>>3;
    }
    m_Descriptor->SetYCoeff(YCoeff);
    m_Descriptor->SetCbCoeff(CbCoeff);
    m_Descriptor->SetCrCoeff(CrCoeff);


    return 0;
}

//
// 8x8 DCT
// init_fdct and fdct are developed by MPEG Software Simulation Group
// The followings are the Copyright information about these two routines
//
/* Copyright (C) 1996, MPEG Software Simulation Group. All Rights Reserved. */
/*
 * Disclaimer of Warranty
 *
 * These software programs are available to the user without any license fee or
 * royalty on an "as is" basis.  The MPEG Software Simulation Group disclaims
 * any and all warranties, whether express, implied, or statuary, including any
 * implied warranties or merchantability or of fitness for a particular
 * purpose.  In no event shall the copyright-holder be liable for any
 * incidental, punitive, or consequential damages of any kind whatsoever
 * arising from the use of these programs.
 *
 * This disclaimer of warranty extends to the user of these programs and user's
 * customers, employees, agents, transferees, successors, and assigns.
 *
 * The MPEG Software Simulation Group does not represent or warrant that the
 * programs furnished hereunder are free of infringement of any third-party
 * patents.
 *
 * Commercial implementations of MPEG-1 and MPEG-2 video, including shareware,
 * are subject to royalty fees to patent holders.  Many of these patents are
 * general enough such that they are unavoidable regardless of implementation
 * design.
 *
 */

//----------------------------------------------------------------------------
void ColorLayoutExtractionTool::init_fdct()
{
//#define COMPUTE_COS
    int i, j;
#ifdef COMPUTE_COS
    double s;
#else // for memory debugging!!
    double d_cos [8][8] =
    {{3.535534e-01, 3.535534e-01, 3.535534e-01, 3.535534e-01,
	3.535534e-01, 3.535534e-01, 3.535534e-01, 3.535534e-01},
     {4.903926e-01, 4.157348e-01, 2.777851e-01, 9.754516e-02,
	-9.754516e-02, -2.777851e-01, -4.157348e-01, -4.903926e-01},
     {4.619398e-01, 1.913417e-01, -1.913417e-01, -4.619398e-01,
	-4.619398e-01, -1.913417e-01, 1.913417e-01, 4.619398e-01},
     {4.157348e-01, -9.754516e-02, -4.903926e-01, -2.777851e-01,
	2.777851e-01, 4.903926e-01, 9.754516e-02, -4.157348e-01},
     {3.535534e-01, -3.535534e-01, -3.535534e-01, 3.535534e-01,
	3.535534e-01, -3.535534e-01, -3.535534e-01, 3.535534e-01},
     {2.777851e-01, -4.903926e-01, 9.754516e-02, 4.157348e-01,
	-4.157348e-01, -9.754516e-02, 4.903926e-01, -2.777851e-01},
     {1.913417e-01, -4.619398e-01, 4.619398e-01, -1.913417e-01,
	-1.913417e-01, 4.619398e-01, -4.619398e-01, 1.913417e-01},
     {9.754516e-02, -2.777851e-01, 4.157348e-01, -4.903926e-01,
	4.903926e-01, -4.157348e-01, 2.777851e-01, -9.754516e-02}};
#endif

    for (i=0; i<8; i++){
#ifdef COMPUTE_COS
	s = (i==0) ? sqrt(0.125) : 0.5;
#endif
	for (j=0; j<8; j++) {
#ifdef COMPUTE_COS
	  c[i][j] = s * cos((M_PI/8.0)*i*(j+0.5));
	  fprintf(stderr,"%le ", c[i][j]);
#else
	  c[i][j] = d_cos [i][j];

#endif
	}
    }
}

//----------------------------------------------------------------------------
void ColorLayoutExtractionTool::fdct(short *block)
{
    int i, j, k;
    double s;
    double tmp[64];
    for (i=0; i<8; i++){
		for (j=0; j<8; j++){
		    s = 0.0;
		    for (k=0; k<8; k++) s += c[j][k] * block[8*i+k];
		    tmp[8*i+j] = s;
		}
    }
    for (j=0; j<8; j++){
		for (i=0; i<8; i++){
		    s = 0.0;
		    for (k=0; k<8; k++) s += c[i][k] * tmp[8*k+j];
		    block[8*i+j] = (int)floor(s+0.499999);
		}
    }
}
//
//  End of routines those are developed by MPEG Software Simulation Group
//

//----------------------------------------------------------------------------
void ColorLayoutExtractionTool::CreateSmallImage(MomVop *src, short small_img[3][64])
{
	int y_axis, x_axis;
	int i, j, k ;
	int x, y;
	long small_block_sum[3][64];
	int cnt[64];

	for(i = 0; i < (8 * 8) ; i++){
		cnt[i]=0;
		for(j=0;j<3;j++){
			small_block_sum[j][i]=0;
			small_img[j][i] = 0;
		}
	}

	unsigned char *pR, *pG, *pB;
	// arbitrary shape
	unsigned char *pA = 0;

	// shape
	if(src->a_chan)
	    pA=src->a_chan->data->u;

	// end modification
	pR=src->v_chan->data->u;
	pG=src->y_chan->data->u;
	pB=src->u_chan->data->u;


	// upsampling for small pictures (less than 8x8) to avoid
	// floating point exception
	int rep_width=(src->width<8)?  7:0;
	int rep_height =(src->height<8)? 7:0;
	int width=(src->width<8)? 8*src->width:src->width;
	int height=(src->height<8)? 8*src->height:src->height;
	unsigned char *buffer, *ptrR, *ptrG, *ptrB, *ptrA = 0;

	// modify the code to use new operation to alloc memory for 3D array
	// arbitrary shape
	buffer = new unsigned char[4*width*height];
    // buffer = new unsigned char[3*width*height];
	// end modification

	ptrR=buffer;
	ptrG=buffer+width*height;
	ptrB=buffer+2*width*height;

	// arbitrary shape
	if(src->a_chan)
	    ptrA=buffer+3*width*height;
	// end modification

	for(y=0; y<src->height; y++){
	    for(x=0; x<src->width; x++){
		*ptrR++ =*pR; *ptrG++ =*pG; *ptrB++ =*pB;
		// arbitrary shape
		if(src->a_chan)
		  *ptrA++ =*pA;
		// end modification

		for(i=0; i<rep_width; i++){
		  *ptrR++ =*pR; *ptrG++ =*pG; *ptrB++ =*pB;
		  // arbitrary shape
		  if(src->a_chan)
		    *ptrA++ = *pA;
		  // end modification
		}
	    pR++; pG++; pB++;
	    // arbitrary shape
	    if(src->a_chan)
	      pA++;
	    // end modification
	    }

	    for(j=0; j<rep_height; j++) {
		memcpy(buffer+0*(width*height)+(8*y+j+1)*width+0,
			buffer+0*(width*height)+(8*y)*width+0,
			width);
		memcpy(buffer+1*(width*height)+(8*y+j+1)*width+0,
			buffer+1*(width*height)+(8*y)*width+0,
			width);
		memcpy(buffer+2*(width*height)+(8*y+j+1)*width+0,
			buffer+2*(width*height)+(8*y)*width+0,
			width);
		// arbitrary shape
		if(src->a_chan)
		  memcpy(buffer+3*(width*height)+(8*y+j+1)*width+0,
			 buffer+3*(width*height)+(8*y)*width+0,
			 width);
		// end modification
		ptrR+= width; ptrG+= width; ptrB+= width;
		// arbitrary shape
		if(src->a_chan)
		  ptrA+= width;
		// end modification
	    }
	}
	pR=buffer;
	pG=buffer+width*height;
	pB=buffer+2*width*height;
	// arbitrary shape
	if(src->a_chan)
	pA=buffer+3*width*height;
	// end modification
	// end of upsampling

	short R, G, B;
	// arbitrary shape
	short A;
	// end modification
	double yy;

	for(y=0; y<height; y++){
	    for(x=0; x<width; x++){
	        y_axis = (int)(y/(height/8.0));
		x_axis = (int)(x/(width/8.0));
		k = y_axis * 8 + x_axis;

		G = *pG++;
		B = *pB++;
		R = *pR++;
		// arbitrary shape
		if(src->a_chan){
		    A = *pA++;
		    if(A==0) continue;
		}
		// end modification

		// RGB to YCbCr
		yy = ( 0.299*R + 0.587*G + 0.114*B)/256.0;
		small_block_sum[0][k] += (int)(219.0 * yy + 16.5); // Y
		small_block_sum[1][k] += (int)(224.0 * 0.564 * (B/256.0*1.0 - yy) + 128.5); // Cb
		small_block_sum[2][k] += (int)(224.0 * 0.713 * (R/256.0*1.0 - yy) + 128.5); // Cr

		cnt[k]++;
	    }
	}


	// arbitrary shape
	double total_sum[3]={0.0, 0.0, 0.0};
	int valid_cell=0;
	// end modification

	// create 8x8 image
	for(i=0; i<8; i++){
	    for(j=0; j<8; j++){
	        for(k=0; k<3; k++){
		    if(cnt[i*8+j])
		        small_img[k][i*8+j] = (small_block_sum[k][i*8+j] / cnt[i*8+j]);
		    else
		      small_img[k][i*8+j] = 0;
		    // arbitrary shape
		    total_sum[k]+=small_img[k][i*8+j];
		    // end modification
		}
		// arbitrary shape
		if(cnt[i*8+j]!=0) valid_cell++;
		// end modification
	    }
	}

	// arbitrary shape
	if(src->a_chan) {
	    for(k=0; k<3; k++){
	        if(valid_cell)
		    total_sum[k]=total_sum[k]/valid_cell;
		else
		  total_sum[k]=0;
	    }
	    for(i=0; i<8; i++){
	        for(j=0; j<8; j++){
		    for(k=0; k<3; k++){
		        if(small_img[k][i*8+j]==0){
			    small_img[k][i*8+j]=(short)total_sum[k];
			}
		    }
		}
	    }
	}
	// end modification

	delete [] buffer;
}

//----------------------------------------------------------------------------
int ColorLayoutExtractionTool::quant_ydc(int i)
{
    int j;
    if(i>191) j=112+(i-192)/4;
    else if(i>159) j=96+(i-160)/2;
    else if(i>95) j=32+(i-96);
    else if(i>63) j=16+(i-64)/2;
    else j=i/4;
    return j;
}

//----------------------------------------------------------------------------
int ColorLayoutExtractionTool::quant_cdc(int i)
{
    int j;
    if(i>191) j=63;
    else if(i>159) j=56+(i-160)/4;
    else if(i>143) j=48+(i-144)/2;
    else if(i>111) j=16+(i-112);
    else if(i>95) j=8+(i-96)/2;
    else if(i>63) j=(i-64)/4;
    else j=0;
    return j;
}

//----------------------------------------------------------------------------
int ColorLayoutExtractionTool::quant_ac(int i)
{
    int j;
    // change for WD 2.0
    // if(i>255) i= 255;
    if(i>239) i= 239;
    if(i<-256) i= -256;
    if ((abs(i)) > 127) j= 64 + (abs(i))/4;
    else if ((abs(i)) > 63) j=32+(abs(i))/2;
    else j=abs(i);
    j = (i<0)?-j:j;
    // change for WD 2.0
    // j +=128;
    j+=132;
    return j;
}
