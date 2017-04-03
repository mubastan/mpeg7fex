/*
 * inter.h
 *
 * inter  provides the addressing function for frame based operations
 *        which compute the result from two input frames
 *        (e.g. diffrence frames)
 *
 */

/****** CopyRightBegin *******************************************************/
/*                                                                           */
/* Copyright (C) 1996 TU-Muenchen LIS All Rights Reserved.                   */
/*                                                                           */
/****** CopyRightEnd *********************************************************/

/*****************************************************************************/
/* CREATED BY :  S. Herrmann  26/2/96                                        */
/*               TU Muenchen-LIS                                             */
/* based on   :  MPEG-4 Momusys VM Data Structure                            */
/*****************************************************************************/

/**** LISSCCSInfo ************************************************************/
/* Filename :    $RCSfile: inter.h,v $
   Version  :    Revision: 1.12
   Last Edit:    Date: 2000/11/10 15:36:35
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
// Applicable File Name:  inter.h
//
*/

#ifndef _INTER_
#define _INTER_

/******* INCLUDES ************************************************************/
#include "momusys.h"
#include "address.h"

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
extern TChan _inter_regin1,_inter_regin2,_inter_regin3,_inter_regin4;
extern TChan _inter_regin5;
extern TChan _inter_regout1,_inter_regout2,_inter_regout3,_inter_regout4;
/* global variable to fill the gap between high level algorithm and pixel
   processing function */
extern void *_inter_infopt;
/* pointer to a database with segment features*/

/******* FUNCTIONS ***********************************************************/
void inter_proc(MomVop *result, MomVop *input1, MomVop *input2,
			  int reschannels,int in1channels,int in2channels,
			  int scanmode,
			  TCoor winx,TCoor winy,TCoor resposx,TCoor resposy,
			  TCoor in1posx,TCoor in1posy,
			  TCoor in2posx,TCoor in2posy,
			  void (*operation)(TPixel *res, TPixel *in1,
					    TPixel *in2));

void intertrans_proc(MomVop *result, MomVop *input1, MomVop *input2,
			       int reschanels,int in1chanels,int in2chanels,
			       int scanmode,
			       TCoor winx,TCoor winy,
			       TCoor resposx,TCoor resposy,
			       TCoor in1posx, TCoor in1posy,
			       void in2trans(TCoor width,TCoor height,
					     TCoor inx,TCoor iny,
					     float *outx,float *outy),
			       void (*operation)(TPixel *res, TPixel *in1,
						 TPixel *in2));


/* address routine for inter addressing mode
        a result at each pixel position is computet from 2 input frames or
	windows  (res = f(in1,in2))*/

/* result  = reference (pointer) to result Vop
                 NULL pointer is allowewed*/
/* input1, = references to input Vops
   input2        NULL pointer is allowewed*/
/* reschannels = ored list of channels to be written to the result Vop
                e.g. A_CH | Y_CH | U_CH | V_CH when all channels are
                           computed*/
/* in1channels,= ored list of channels to be read from image buffers
   in2chanles              unused channels are set to zero values*/
/* scanmode= scheme in which order the pixels are processed
             SCAN_HO = scaning line by line from left to right
             SCAN_HM = scaning first line from left to right, second line
                       from right to left ... (horizontal meander)
             SCAN_VE = scaning culomns from top to bottom (vertical)
             SCAN_VM = scaning first culomn from top to bottom, second culomn
                       from buttom to top ... (vertical meander)*/
/* winx,winy= size of processed window
          = -1,-1 means total frame*/
/* resposx,resposy= top left corner of window location in Vop of
   in1posx,in1posy,           result, input1, input2
   in2posx,in2posy,                                             */
/* in2trans    = pointer to transfomation function for coordinates*/
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
/* #define CON_H3 5 *//*center + horizontal neighbours*/
/* #define CON_H5 6 *//*center + 2 horizontal neighbours*/
/* #define CON_V3 7 *//*center + vertical neighbours*/
/* #define CON_V5 8 *//*center + 2 vertical neighbours*/

/* #define SCAN_HO 0      *//* from top to buttom, from left to right*/
/* #define SCAN_VE 1      *//* from left to right, from top to buttom*/
/* #define SCAN_HM 2      *//* horizontal meander*/
/* #define SCAN_VM 3      *//* vertical meander*/
/* #define SCAN_LOOP 4    *//* loop with decrising radius*/
/* #define SCAN_LIST 5    *//* read positions from list*/
/* #define SCAN_COLLIST 6 *//* read positions from collected list*/
/* #define TRANS_NO_INTERPOLATE 0*//*perfrom not pixel interpolation in
				 inter_trans*/
/* #define TRANS_INT_INTERPOLATE 16*//*perfrom linear pixel interpolation in
				 inter_trans in integer range*/
/* #define TRANS_FLOAT_INTERPOLATE 32*//*perfrom linear pixel interpolation in
				 inter_trans in floating point range*/

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif
