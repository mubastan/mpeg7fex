/****** CopyRightBegin *******************************************************/
/*                                                                           */
/* Copyright (C) 1996 TU-Muenchen LIS All Rights Reserved.                   */
/*                                                                           */
/****** CopyRightEnd *********************************************************/

/*****************************************************************************/
/* CREATED BY :  S. Herrmann  26/2/96                                        */
/*               TU Muenchen-LIS  und  FhG-IFT                               */
/* based on   :  Momusys VM                                                  */
/*****************************************************************************/

/**** LISSCCSInfo ************************************************************/
/* Filename :    $RCSfile: vopio.h,v $
   Version  :    Revision: 1.10
   Last Edit:    Date: 2001/08/02 07:56:54
   Released :    %D% %T%   */
/**** LISSCCSInfo ************************************************************/

/*
///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// S. Herrmann TU-Munich, Institute for Integrated Circuits
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
//
// This notice must be included in all copies or derivative works.
//
// Applicable File Name:  vopio.h
//
*/


#ifndef _VOPIO_
#define _VOPIO_


/******* INCLUDES ************************************************************/
#include "momusys.h"
#include "address.h"

/******* DEFINES *************************************************************/
#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif
// modifying the width and height of image in initvop() function according to these flags
#ifndef _COLORFORMATS_
#define _COLORFORMATS_
#define _444_ 1				// use _444_ for normal operation, don@t know what others are for!
#define _422_ 2
#define _420_ 4
#define _400_ 0				// for single band(?) -- initimg
#endif /*_COLORFORMATS_*/

/* Treat naming conflict of MoMuSys and ImageMagick*/
/*
#ifndef _MYMOMUSYS_
#ifndef _NAMECONFICTDEFS_
#define _NAMECONFICTDEFS_
#define MomImageType ImageType
#define MomImageData ImageData
#define MomImage Image
#define MomVopExtend VopExtend
#define MomVop Vop
#define momvop vop
#endif
#endif
*/

/******* MAKROS  *************************************************************/

/******* TYPES ***************************************************************/

/******* VARIABLES ***********************************************************/
#ifdef ADDPROF
typedef unsigned long long framebuffercounter;
extern framebuffercounter *readcnt,*writecnt;
#endif

/******* FUNCTIONS ***********************************************************/
/* Momusys specific functions*/
MomVop *initvop(TCoor x, TCoor y, int colorformat,
	     MomImageType atype, MomImageType ytype,
	     MomImageType utype, MomImageType vtype);
/* mallocs and initilizes memory for a Vop*/

/* x, y    = width and height*/
/* *type   = datatype for corresonding buffer (UCHAR_TYPE,SHORT_TYPE)*/



/**
 * allocate memory for a MomVop image
 * If a type is NONE_TYPE that channel will not be allocated, and will be set to NULL
 * added by mb - 28.06.2008
 **/
MomVop *initVop(int width, int height, MomImageType atype, MomImageType ytype, MomImageType utype, MomImageType vtype);

MomVop *freevop(MomVop *vop);
/* frees a complete Vop-structure malloced by initvop*/


void setvop(MomVop *vop, TCoor x, TCoor y);
/* initilizes a Vop (variable not pointer to) the 4 Images are set
              to a NULL pointer*/

/* vop     = reference to Vop variable*/
/* x, y    = width and height*/

MomImage *initimg(TCoor x, TCoor y, MomImageType type);
/* mallocs and initilizes memory for an Image*/

/* x, y    = width and height*/
/* type    = datatype of buffer (UCHAR_TYPE,SHORT_TYPE)*/

MomImage *freeimg(MomImage *img);
/* frees a complete Image structure malloced by initimg*/

int datasize(MomImage *image);
/* returns the element size of the ImageData buffer
            e.g. image->typee = UCHAR_TYPE => result = 1*/

int typesize(MomImageType type);
/* returns element size form ImageType*/

unsigned long typemax(MomImageType type);
/* returns maximum value of ImageType
            e.g. image->typee = SHORT_TYPE => result = 32767*/

void iorefs(MomImageType type,TChan (**readfunc)(MomImageData),
	     void (**writefunc)(MomImageData,TChan));
/* decodes references to image data access functions for type
            e.g. type = UCHAR_TYPE will be decoded to
                 *readfunc = &getuchardata
                 *writefunc = &setuchardata*/
void iref(MomImageType type,TChan (**readfunc)(MomImageData));
/* decodes only *readfunc*/
void oref(MomImageType type,void (**writefunc)(MomImageData,TChan));
/* decodes only *writefunc*/


void setdata(MomImageData data,MomImageType type,TChan value);
TChan getdata(MomImageData data,MomImageType type);
/* reads or write data from/to image buffer*/
/* data    = union with pointer to data position in memory*/
/* type    = datatype of buffer (UCHAR_TYPE,SHORT_TYPE)*/
/* value   = value which is written to memory*/


void setuchardata(MomImageData data,TChan value);
void setshortdata(MomImageData data,TChan value);
void setfloatdata(MomImageData data,TChan value);
void setunused(MomImageData data,TChan value);
TChan getuchardata(MomImageData data);
TChan getshortdata(MomImageData data);
TChan getfloatdata(MomImageData data);
TChan getunused(MomImageData data);
/* typespecific read and write buffer functions*/

void incdata(MomImageData data,MomImageType type);
/* increments data in image buffer*/

unsigned char *xydata(MomImage *image,TCoor x,TCoor y);
/* gets position of data in image buffer by its x and y position*/

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif /*ifndef _VOPIO_*/
