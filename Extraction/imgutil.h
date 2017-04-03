///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Kapil Chhabra, Yining Deng, Xinding Sun, Dr. Hyundoo Shin, Prof. B.S. Manjunath
// University of California, Santa Barbara.
// (contributing organizations names)
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
//
// This notice must be included in all copies or derivative works.
//
// Applicable File Name:  DominantColorAppl.cpp
//
#ifndef __IMGUTIL_H
#define __IMGUTIL_H

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif



#ifndef MAX
  #define MAX(x, y) (((x) > (y)) ? (x) : (y))
#endif
#ifndef MIN
  #define MIN(x, y) (((x) < (y)) ? (x) : (y))
#endif
#define sqr(x) ((x)*(x))

void rgb2luv2(unsigned char *RGB,float *LUV,int size);
void rgb2luv(int *RGB,float *LUV,int size);
void luv2rgb(int *RGB,float *LUV,int size);

void inputimgraw(char *fname,unsigned char *img,int size,/*int ny,int nx,*/int dim);
void outputimgraw(char *fname,unsigned char *img,int ny,int nx,int dim);

float **fmatrix(int nr, int nc);
void free_fmatrix(float **m, int nr);

//#define LROUNDF_DEFINED_IN_STDHEADERS
//#ifndef LROUNDF_DEFINED_IN_STDHEADERS
//int lroundf(float x);
//#endif

void piksrt(int n, float *num, int *index);
float distance(float *a,float *b,int dim);
float distance2(float *a,float *b,int dim);

int getsize( );
unsigned char *getpixels( int size);

int mergecb(float *B,float **cb,unsigned char *P,int npt,int N,float thresh,int dim);
int mergecb1(float **dist,float *B,float **cb,int newN,float thresh,int dim,int *count);int gla(float *A,int nvec,int ndim,int N,float **codebook,float t,unsigned char *P);
int greedy(float *A,int nvec,int ndim,int N,float **codebook,float t,unsigned char *P);
void getcmap(float *B,unsigned char *cmap,float **cb,int npt,int dim,int N);

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif
