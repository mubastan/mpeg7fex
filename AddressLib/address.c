/* address.c
*
* address  provides lock up table to translate direction from center
*          pixel to index of corresponding neighbor
*          (should be in nhood.c)
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
/* Filename :    $RCSfile: address.c,v $
   Version  :    Revision: 1.7 
   Last Edit:    Date: 2000/06/07 10:58:01
   Released :    %D% %T%   */
/**** LISSCCSInfo ************************************************************/
 
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
// Applicable File Name:  address.c
//

/******* INCLUDES ************************************************************/
#include "address.h"

/******* DEFINES *************************************************************/

/******* MAKROS  *************************************************************/

/******* TYPES ***************************************************************/

/******* VARIABLES ***********************************************************/

/******* FUNCTIONS ***********************************************************/
/*****************************************************************************/
float tchan_to_float(TChan value)

{
  return TCHAN_TO_FLOAT(value);
}

/*****************************************************************************/
TChan float_to_tchan(float value)

{
  return FLOAT_TO_TCHAN(value);
}

/******* Code ****************************************************************/

