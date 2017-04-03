/*
 * address.h   provides basic types and defines
 *
 */

/****** CopyRightBegin *******************************************************/
/*                                                                           */
/* Copyright (C) 1996 TU-Muenchen LIS All Rights Reserved.                   */
/*                                                                           */
/****** CopyRightEnd *********************************************************/

/*****************************************************************************/
/* CREATED BY :  S. Herrmann                                                 */
/*               TU Muenchen-LIS                                             */
/*****************************************************************************/

/**** LISSCCSInfo ************************************************************/
/* Filename :    $RCSfile: address.h,v $
   Version  :    Revision: 1.13
   Last Edit:    Date: 2002/01/15 17:35:16
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
// Applicable File Name:  address.h
//
*/

#ifndef _ADDRESS_
#define _ADDRESS_

/******* DEFINES *************************************************************/
//#define ADDPROF   /* to force mini-profiling*/

#ifndef _COLORFORMATS_
#define _COLORFORMATS_
#define _444_ 1
#define _422_ 2
#define _420_ 4
#define _400_ 0
#endif /*_COLORFORMATS_*/

#define A_CH 1
#define Y_CH 2
#define U_CH 4
#define V_CH 8
#define AX_CH 16
#define M_CH 32

#define CON_0 0 /* center only*/
#define CON_4 1 /* center + 4 neighbours*/
#define CON_8 2 /* center + 8 neighbours*/
#define CON_12 3 /*center + 12 nearest pixels*/
#define CON_24 4 /*center + 24 nearest pixels*/
#define CON_H2 5 /*center + horizontal neighbours*/
#define CON_H4 6 /*center + 2 horizontal neighbours*/
#define CON_V2 7 /*center + vertical neighbours*/
#define CON_V4 8 /*center + 2 vertical neighbours*/
#define CON_H6 9 /*center + 3 horizontal neighbours*/
#define CON_H8 10 /*center + 4 horizontal neighbours*/
#define CON_V6 11 /*center + 3 vertical neighbours*/
#define CON_V8 12 /*center + 4 vertical neighbours*/

#define SCAN_HO 0 /* from top to buttom, from left to right*/
#define SCAN_VE 1 /* from left to right, from top to buttom*/
#define SCAN_HM 2 /* horizontal meander*/
#define SCAN_VM 3 /* vertical meander*/
#define SCAN_LOOP 4 /* loop with decrising radius*/
#define SCAN_LIST 5 /* read positions from list*/
#define SCAN_COLLIST 21 /* read positions from collected list*/
#define SCAN_COLLECT 16 /* collect start pixels for fifo mode*/
#define SCAN_MULIN 32 /* multi queue in mode for fifo*/
#define TRANS_NO_INTERPOLATE 0 /*perfrom not pixel interpolation in
				 inter_trans*/
#define TRANS_INT_INTERPOLATE 16 /*perfrom linear pixel interpolation in
				 inter_trans in integer range*/
#define TRANS_FLOAT_INTERPOLATE 32 /*perfrom linear pixel interpolation in
				 inter_trans in floating point range*/
#define TRANS_INTERPOLATE_MODES (TRANS_INT_INTERPOLATE|TRANS_FLOAT_INTERPOLATE)
         /* value range for interpoation mode in scan scheme*/

#define QIN_PROC 0 /* process on queue in*/
#define QOUT_PROC 1 /* process on queue out*/

#define	DIR_LEFT 0
#define	DIR_UP_LEFT 1
#define	DIR_UP 2
#define	DIR_UP_RIGHT 3
#define	DIR_RIGHT 4
#define	DIR_DOWN_RIGHT 5
#define	DIR_DOWN 6
#define	DIR_DOWN_LEFT 7

#define MARKER_INVALID 0
#define MARKER_NOT_LOADED 0
#define MARKER_LOADED 1
#define MARKER_CRIT_TRUE 2
#define MARKER_PUSHED 3
#define MARKER_QUEUED 3
#define MARKER_POPED 4
#define MARKER_DEQUEUED 4
#define MARKER_PROCESSED 5


/******* MAKROS  *************************************************************/
#define TCHAN_TO_FLOAT(a) (*((float *)(&(a))))
#define FLOAT_TO_TCHAN(a) (*((TChan *)(&(a))))

/******* TYPES ***************************************************************/
typedef long TChan;
/* typedef short TAChan; */
/* typedef short TYChan; */
/* typedef short TUChan; */
/* typedef short TVChan; */
/* typedef short TMChan; */
/* typedef short TAXChan; */

struct pixel_struct{
  union {
    TChan i;
    float f;
  } a;
  union {
    TChan i;
    float f;
  } y;
  union {
    TChan i;
    float f;
  } u;
  union {
    TChan i;
    float f;
  } v;
  TChan m;
  union {
    TChan i;
    float f;
  } ax;
/*   TAChan a; */
/*   TYChan y; */
/*   TUChan u; */
/*   TVChan v; */
/*   TMChan m; */
/*   TAXChan ax; */
};
typedef struct pixel_struct TPixel;

typedef short TCoor;

struct pos
{
  TCoor x;
  TCoor y;
};

typedef struct pos TPos;

/*struct emptylist
{
  long  empty;
  long  previous;
};

typedef struct emptylist EmptyList;

struct hlist
{
  TCoor x;
  TCoor y;
  short level;
};

typedef struct hlist THList;*/


struct hfifoentry
{
  TCoor x;
  TCoor y;
  struct hfifoentry *next;
};

typedef struct hfifoentry THFifoEntry;


/******* VARIABLES ***********************************************************/

/******* FUNCTIONS ***********************************************************/
#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

float tchan_to_float(TChan);
TChan float_to_tchan(float);

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif



#endif
