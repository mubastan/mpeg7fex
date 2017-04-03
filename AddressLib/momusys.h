/****** CopyRightBegin *******************************************************/
/*                                                                           */
/* Copyright (C) 1996 TU-Muenchen LIS All Rights Reserved.                   */
/*                                                                           */
/****** CopyRightEnd *********************************************************/

/*****************************************************************************/
/* CREATED BY :  S. Herrmann                                                 */
/*               TU Muenchen-LIS                                             */
/* based on   :  Momusys VM                                                  */
/*****************************************************************************/

/**** LISSCCSInfo ************************************************************/
/* Filename :    mymomusys.h
   Version  :    1.4
   Last Edit:    99/10/15 15:20:07
   Released :    99/10/15 15:49:59   */
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
// Applicable File Name:  mymomusys.h
//
*/


#ifndef _MOMUSYS_
#define _MOMUSYS_
#define _MYMOMUSYS_

/******* INCLUDES ************************************************************/

/******* DEFINES *************************************************************/
#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

// commented out by mb, clash with MinGW 4.5.0 (12 June 2010)
//#define Char char
//#define UChar unsigned char
//#define Short short
//#define UShort unsigned short
//#define SInt short
//#define Int int
//#define UInt unsigned int
//#define Long long
//#define ULong unsigned long
//#define Float float
//#define UFloat unsigned float
//#define Double double
//#define UDouble unsigned double

#define ImageI MomImage

/******* MAKROS  *************************************************************/

/******* TYPES ***************************************************************/

//enum momimage_type {SHORT_TYPE,FLOAT_TYPE,UCHAR_TYPE};
//modified by mb: added NONE_TYPE (channel not existing)
enum momimage_type {SHORT_TYPE,FLOAT_TYPE,UCHAR_TYPE,NONE_TYPE};
typedef enum momimage_type MomImageType;

union momimage_data
{
  short *s;              /* SHORT data */
  float *f;             /* FLOAT data */
  unsigned char *u;             /* UCHAR data */
};
typedef union momimage_data MomImageData;

struct momimage
{
  int version;          /* Version number */
  unsigned int x,y;             /* Image size */
  char upperodd;        /* Flag to tell if the top line is considered
                           as even or odd (used for hex grids) */
  char grid;            /* Grid type: s = square, h = hexagonal */
  short *f;              /* Image data with short int values */
  MomImageData *data;      /* NEW: pointer to image data */
  MomImageType type;       /* NEW: type of the image */
};

typedef struct momimage MomImage;

struct momvop_extend		/* User definable VOP extension */
{				/* Put in your personally required stuff */
  int dummy;			/* Just a dummy entry */
};
typedef struct momvop_extend MomVopExtend;

struct momvop
{
  int ident;					/* VOL identifier (for tracking) */
  int width;                    /* VOP height (smallest rectangle) */
  int height;                   /* VOP width  (smallest rectangle) */
  int hor_spat_ref;             /* VOP horizontal ref. (for composition) */
  int ver_spat_ref;             /* VOP vertical ref.   (for composition) */

  /* Old entries maintained for old source code compatibility */
  int scaling;                  /* VOP scaling factor  (for composition) */
  int visibility;               /* VOP visibility      (for composition) */
  int binary_shape;             /* Key for alpha plane (1-binary,0-blending) */
  int comp_order;               /* VOP order           (for composition) */
  int time_increment_resolution;
  int arbitrary_shape;		/* VOL shape: 0-rect., 1-bin., 2-greyl. */

  /* Pointers to the images (YUVA) and to related VOPs */
  MomImage *a_chan;                /* The alpha plane */
/*  MomImage *a_uv_chan; */            /* The alpha  plane for uv */
  MomImage *y_chan;                /* Y component of the VOP texture */
  MomImage *u_chan;                /* U component of the VOP texture */
  MomImage *v_chan;                /* V component of the VOP texture */
  struct momvop *pnext_spat;       /* List of VOPs within same time instance */
  struct momvop *pnext_temp;       /* List of VOPs with same ID along
				      time-axis */
/*  MomVop *pprev_temp;*/       /* added by Minhua Zhou on 24.07.97 */
  MomVopExtend *pextend;   /* VOP extension pointer (user definable) */

};
typedef struct momvop MomVop;


#if defined(__cplusplus) || defined(c_plusplus)
}
#endif


#endif

