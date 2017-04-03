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
/* Filename :    $RCSfile: intra.h,v $
   Version  :    Revision: 1.10
   Last Edit:    Date: 2000/11/10 15:36:49
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
// Applicable File Name:  intra.h
//
*/

#ifndef _INTRA_
#define _INTRA_

/******* INCLUDES ************************************************************/
#include "momusys.h"
#include "address.h"
#include "nhood.h"

/******* DEFINES *************************************************************/
#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

/* Treat naming conflict of MoMuSys and ImageMagick*/
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
/******* MAKROS  *************************************************************/

/******* TYPES ***************************************************************/

/******* VARIABLES ***********************************************************/
extern TChan _intra_regin1,_intra_regin2,_intra_regin3,_intra_regin4;
extern TChan _intra_regin5;
extern TChan _intra_regout1,_intra_regout2,_intra_regout3,_intra_regout4;
/* global variable to fill the gap between high level algorithm and pixel
   processing function */
extern void *_intra_infopt;
/* pointer to a database with segment features*/



/******* FUNCTIONS ***********************************************************/
void intra_proc(MomVop *resvop, MomVop *invop,MomImage *aux,
			  int reschannels,int inchannels,
			  int connect,int scanmode,
			  TChan ahold,TChan yhold,TChan uhold,
			  TChan vhold,TChan axhold,
			  TCoor winx,TCoor winy,TCoor resposx,TCoor resposy,
			  TCoor inposx,TCoor inposy,TSizeConv conv,
			  void (*operation)(TPixel *, TPixel *, TCoor, TCoor));

/* address routine for intra addressing mode
        a result at each pixel position is computed from the pixel
	itself and its neighbors
	windows  (res = f(in,N(in))*/
/* result  = reference (pointer) to result Vop
                 NULL pointer is allowewed*/
/* input   = references to input Vops
                 NULL pointer is allowewed*/
/* aux     = auxillary Image. If this image is used, it is an IO
             buffer (in oposite to result or input). I.e. results
	     are also written directly to the internal input matrix*/
/* reschannels= ored list of channels to be written to the result Vop
                e.g. A_CH | Y_CH | U_CH | V_CH when all channels are
                           computed*/
/* in1channels= ored list of channels to be read from image buffers
                           unused channels are set to zero values*/
/* connect= type of neighborhood
             CON_0 0  = center only
             CON_4 1  = center + 4 neighbours
             CON_8 2  = center + 8 neighbours
             CON_12 3 = center + 12 nearest pixels
             CON_24 4 = center + 24 nearest pixels
             CON_H3 5 = center + horizontal neighbours
             CON_H5 6 = center + 2 horizontal neighbours
             CON_V3 7 = center + vertical neighbours
             CON_V5 8 = center + 2 vertical neighbours
           CON_4 and CON_8 are treated in the same way, since
           this information is only used to preload the pixel matrix
           The connectivity for pixel processing is then defined by the
           pixel "operation"*/

/* scanmode= scheme in which order the pixels are processed
             SCAN_HO = scaning line by line from left to right
             SCAN_HM = scaning first line from left to right, second line
                       from right to left ... (horizontal meander)
             SCAN_VE = scaning culomns from top to bottom (vertical)
             SCAN_VM = scaning first culomn from top to bottom, second culomn
                       from buttom to top ... (vertical meander)*/
/* *hold   = values which are set to invalid pixel positions
           =  -1 doublicate pixel values from previous valid pixel
                 (or if no right or upper neighbour then take the value
                  from left or lower neighbor
           =  else take the given value   */
/* winx,winy= size of processed window
           = -1,-1 means total frame*/
/* resposx,resposy= top left corner of window location in Vop of
   inposx,inposy,             result and input*/
/* operation= pointer to pixel processing function
              this function is called for every pixel in the result
              frame or window*/

/* complete list of defines for addressing routines*/
/* #define A_CH 1 */
/* #define Y_CH 2 */
/* #define U_CH 4 */
/* #define V_CH 8 */
/* #define AX_CH 16 */
/* #define M_CH 32 */

/* #define CON_0 0  *//* center only*/
/* #define CON_4 1  *//* center + 4 neighbours*/
/* #define CON_8 2  *//* center + 8 neighbours*/
/* #define CON_12 3 *//*center + 12 nearest pixels*/
/* #define CON_24 4 *//*center + 24 nearest pixels*/
/* #define CON_H2 5 *//*center + horizontal neighbours*/
/* #define CON_H4 6 *//*center + 2 horizontal neighbours*/
/* #define CON_V2 7 *//*center + vertical neighbours*/
/* #define CON_V4 8 *//*center + 2 vertical neighbours*/

/* #define SCAN_HO 0      *//* from top to buttom, from left to right*/
/* #define SCAN_VE 1      *//* from left to right, from top to buttom*/
/* #define SCAN_HM 2      *//* horizontal meander*/
/* #define SCAN_VM 3      *//* vertical meander*/
/* #define SCAN_LOOP 4    *//* loop with decrising radius*/
/* #define SCAN_LIST 5    *//* read positions from list*/
/* #define SCAN_COLLIST 6 *//* read positions from collected list*/

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif
