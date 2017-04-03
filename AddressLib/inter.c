/*
 * inter.c
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
/* Filename :    $RCSfile: inter.c,v $
   Version  :    Revision: 1.13
   Last Edit:    Date: 2002/01/15 17:37:06
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
// Applicable File Name:  inter.c
//
*/

/******* INCLUDES ************************************************************/
#include <stdio.h>
#include <math.h>

#include "vopio.h"
#include "nhood.h"
#include "inter.h"

#ifdef EVAL
  #include "evaladdr.h"
  #include "eval.h"
#endif
#ifdef PPTRACE
  #include "pptrace.h"
#endif


/******* DEFINES *************************************************************/

/******* TYPES ***************************************************************/

/******* VARIABLES ***********************************************************/
/*static int _typesize[5]={0,0,1,1,2};*/
TChan _inter_regin1=0,_inter_regin2=0,_inter_regin3=0,_inter_regin4=0;
TChan _inter_regin5=0;
TChan _inter_regout1=0,_inter_regout2=0,_inter_regout3=0,_inter_regout4=0;
void *_inter_infopt=0;



/*static TCoor _i_icntx=0,_i_icnty=0; */
                                  /* curent pixel position in input frame*/
/*static TCoor _i_icntx=0,_i_icnty=0; */
                                  /* curent pixel position in input frame*/
static TCoor _i_in1width=0,_i_in1height=0; /* input image size*/
static TCoor _i_in2width=0,_i_in2height=0; /* input image size*/
static TCoor _i_maxx=0,_i_maxy=0;          /* endpositions of window*/

static MomImageData  _i_ra={0},  _i_ry={0},_i_ru={0},_i_rv={0};
static MomImageData  _i_i1a={0},_i_i1y={0},_i_i1u={0},_i_i1v={0};
static MomImageData  _i_i2a={0},_i_i2y={0},_i_i2u={0},_i_i2v={0};

static int _i_ras=0,_i_rys=0,_i_rus=0,_i_rvs=0;
static int _i_i1as=0,_i_i1ys=0,_i_i1us=0,_i_i1vs=0;
static int _i_i2as=0,_i_i2ys=0,_i_i2us=0,_i_i2vs=0;

static int _i_rals=0,_i_ryls=0,_i_ruls=0,_i_rvls=0;
static int _i_i1als=0,_i_i1yls=0,_i_i1uls=0,_i_i1vls=0;
static int _i_i2als=0,_i_i2yls=0,_i_i2uls=0,_i_i2vls=0;

static MomImageData _i_sti1a,_i_sti1y,_i_sti1u,_i_sti1v;
static MomImageData _i_sti2a,_i_sti2y,_i_sti2u,_i_sti2v;
static MomImageData _i_stra,_i_stry,_i_stru,_i_strv;

static float _i_in2x,_i_in2y;        /* transformed coordiantes for input2*/
static TPixel _i_in2;                /* interpolated 2nd input pixel*/

/* references to read-write functions*/
static TChan (*_i_i1aget)(MomImageData data);
static TChan (*_i_i1yget)(MomImageData data);
static TChan (*_i_i1uget)(MomImageData data);
static TChan (*_i_i1vget)(MomImageData data);
static TChan (*_i_i2aget)(MomImageData data);
static TChan (*_i_i2yget)(MomImageData data);
static TChan (*_i_i2uget)(MomImageData data);
static TChan (*_i_i2vget)(MomImageData data);
static void (*_i_raset)(MomImageData data,TChan value);
static void (*_i_ryset)(MomImageData data,TChan value);
static void (*_i_ruset)(MomImageData data,TChan value);
static void (*_i_rvset)(MomImageData data,TChan value);

/*static MomImageType _i_ratype=UCHAR_TYPE,_i_rytype=UCHAR_TYPE,
  _i_rutype=UCHAR_TYPE,_i_rvtype=UCHAR_TYPE;
static MomImageType _i_i1atype=UCHAR_TYPE,_i_i1ytype=UCHAR_TYPE,
  _i_i1utype=UCHAR_TYPE,_i_i1vtype=UCHAR_TYPE;
static MomImageType _i_i2atype=UCHAR_TYPE,_i_i2ytype=UCHAR_TYPE,
  _i_i2utype=UCHAR_TYPE,_i_i2vtype=UCHAR_TYPE;
*/

static int _i_in1channels=0;
static int _i_in2channels=0;

/******* FUNCTIONS ***********************************************************/
static void _t_notrans(TCoor width,TCoor height,TCoor cntx,TCoor cnty,
		       float *x,float *y);
static TChan _t_no_interpolation();
static TChan _t_int_interpolation();
static TChan _t_float_interpolation();


/******* Code ****************************************************************/

/* central function for area related functions*/
void inter_proc(MomVop *result, MomVop *input1, MomVop *input2,int reschannels,
	       int in1channels,int in2channels,
	       int scanmode,
	       TCoor winx,TCoor winy,TCoor resposx,TCoor resposy,
	       TCoor in1posx,TCoor in1posy,TCoor in2posx,TCoor in2posy,
	       void (*operation)(TPixel *res, TPixel *in1, TPixel *in2))

{
  TPixel res,in1,in2;
  TCoor cntx,cnty;        /* current centerposition in resultimage*/
  TCoor width,height;     /* result image size*/
  int error;
  int i;
  MomImage *res_ch=0,*in1_ch=0,*in2_ch=0;
  /* references to read-write functions*/
  TChan	(*_i_i1aget)(MomImageData data);
  TChan	(*_i_i1yget)(MomImageData data);
  TChan	(*_i_i1uget)(MomImageData data);
  TChan	(*_i_i1vget)(MomImageData data);
  TChan	(*_i_i2aget)(MomImageData data);
  TChan	(*_i_i2yget)(MomImageData data);
  TChan	(*_i_i2uget)(MomImageData data);
  TChan	(*_i_i2vget)(MomImageData data);
  void (*_i_raset)(MomImageData data,TChan	value);
  void (*_i_ryset)(MomImageData data,TChan	value);
  void (*_i_ruset)(MomImageData data,TChan	value);
  void (*_i_rvset)(MomImageData data,TChan	value);

#ifdef EVAL
  eval_BeginMeasureDur();
  eval_BeginAddrMode(SECTION_INTER);
  eval_BeginRecMode(input1,result,in1channels,reschannels);
  eval_PictSize(input1,input2,result,NULL);
  eval_BeginScan(scanmode);
  eval_ScanWindow(winx,winy,in1posx,in1posy,in2posx,in2posy,resposx,resposy);
  eval_BeginConSC(CON_0);
  eval_BeginOpr((void*)operation);
  eval_StartAddr();
#endif

#ifdef PPTRACE
  PPTrace_StartOfTool();
  PPTrace_LogType((void*)operation,reschannels,CON_0);
  PPTrace_LogInterReg(1);
#endif

  /* inital values*/
  _i_stra.u=_i_stry.u=_i_stru.u=_i_strv.u=0;
  _i_sti1a.u=_i_sti1y.u=_i_sti1u.u=_i_sti1v.u=0;
  _i_sti2a.u=_i_sti2y.u=_i_sti2u.u=_i_sti2v.u=0;

  _i_ra.u=_i_ry.u=_i_ru.u=_i_rv.u=0;;
  _i_i1a.u=_i_i1y.u=_i_i1u.u=_i_i1v.u=0;
  _i_i2a.u=_i_i2y.u=_i_i2u.u=_i_i2v.u=0;

  _i_ras=_i_rys=_i_rus=_i_rvs=0;
  _i_i1as=_i_i1ys=_i_i1us=_i_i1vs=0;
  _i_i2as=_i_i2ys=_i_i2us=_i_i2vs=0;

  _i_rals=_i_ryls=_i_ruls=_i_rvls=0;
  _i_i1als=_i_i1yls=_i_i1uls=_i_i1vls=0;
  _i_i2als=_i_i2yls=_i_i2uls=_i_i2vls=0;

/*  _i_ratype=_i_rytype=_i_rutype=_i_rvtype=UCHAR_TYPE;
  _i_i1atype=_i_i1ytype=_i_i1utype=_i_i1vtype=UCHAR_TYPE;
  _i_i2atype=_i_i2ytype=_i_i2utype=_i_i2vtype=UCHAR_TYPE;*/

  width=height=_i_in1width=_i_in1height=_i_in2width=_i_in2height=0;
  _i_in1channels=in1channels;
  _i_in2channels=in2channels;

  _i_i1aget= &getunused;
  _i_i1yget= &getunused;
  _i_i1uget= &getunused;
  _i_i1vget= &getunused;
  _i_i2aget= &getunused;
  _i_i2yget= &getunused;
  _i_i2uget= &getunused;
  _i_i2vget= &getunused;
  _i_raset= &setunused;
  _i_ryset= &setunused;
  _i_ruset= &setunused;
  _i_rvset= &setunused;

  i=1;
  error=0;
  while (i<16) {
    if (input1) {
      if (_i_in1channels & i) {
  	switch (i) {
  	case A_CH:
  	  in1_ch=input1->a_chan;
  	  break;
  	case Y_CH:
  	  in1_ch=input1->y_chan;
  	  break;
  	case U_CH:
  	  in1_ch=input1->u_chan;
  	  break;
  	case V_CH:
  	  in1_ch=input1->v_chan;
  	  break;
  	default:
  	  addlib_stop_here("ERROR : Internal error in inter_proc\n",0,0);
  	  return;
  	}

  	if (!in1_ch) {
  	  addlib_stop_here("WARNING : color component does not exist in "
  		  "inter_proc\n",0,0);
  	  _i_in1channels-=i;
  	}
  	else if (!(in1_ch->x && in1_ch->y)) {
  	  addlib_stop_here("WARNING : color component has 0-size in "
  		  "inter_proc\n",0,0);
  	  _i_in1channels-=i;
  	}
  	else {
  	  switch (i) {
  	  case A_CH:
  	    _i_i1as=datasize(in1_ch);
  	    _i_i1als=_i_i1as*in1_ch->x;
  	    _i_sti1a.u=in1_ch->data->u;
  	    iref(in1_ch->type,&_i_i1aget);
  	    break;
  	  case Y_CH:
  	    _i_i1ys=datasize(in1_ch);
  	    _i_i1yls=_i_i1ys*in1_ch->x;
  	    _i_sti1y.u=in1_ch->data->u;
  	    iref(in1_ch->type,&_i_i1yget);
  	    break;
  	  case U_CH:
  	    _i_i1us=datasize(in1_ch);
  	    _i_i1uls=_i_i1us*in1_ch->x;
  	    _i_sti1u.u=in1_ch->data->u;
  	    iref(in1_ch->type,&_i_i1uget);
  	    break;
  	  case V_CH:
  	    _i_i1vs=datasize(in1_ch);
  	    _i_i1vls=_i_i1vs*in1_ch->x;
  	    _i_sti1v.u=in1_ch->data->u;
  	    iref(in1_ch->type,&_i_i1vget);
  	    break;
  	  }
  	  if (in1_ch) {
  	    if (!_i_in1width) _i_in1width=in1_ch->x;
  	    else {
  	      if (_i_in1width != in1_ch->x) {
  		addlib_stop_here("Warning can only process colorformat 444\n",
				 0,0);
  		return;
  	      }
  	    }
  	    if (!_i_in1height) _i_in1height=in1_ch->y;
  	    else {
  	      if (_i_in1height != in1_ch->y) {
  		addlib_stop_here("Warning can only process colorformat 444\n",
				 0,0);
  		return;
  	      }
  	    }
  	  }
  	}
      }
    }
    else {
      _i_in1channels=0;
    }


    if (input2) {
      if (_i_in2channels & i) {
  	switch (i) {
  	case A_CH:
  	  in2_ch=input2->a_chan;
  	  break;
  	case Y_CH:
  	  in2_ch=input2->y_chan;
  	  break;
  	case U_CH:
  	  in2_ch=input2->u_chan;
  	  break;
  	case V_CH:
  	  in2_ch=input2->v_chan;
  	  break;
  	default:
  	  addlib_stop_here("ERROR : Internal error in inter_proc\n",0,0);
  	  return;
  	}

  	if (!in2_ch) {
  	  addlib_stop_here("WARNING : color component does not exist in "
  		  "inter_proc\n",0,0);
  	  _i_in2channels-=i;
  	}
  	else if (!(in2_ch->x && in2_ch->y)) {
  	  addlib_stop_here("WARNING : color component has 0-size in "
  		  "inter_proc\n",0,0);
  	  _i_in2channels-=i;
  	}
  	else {
  	  switch (i) {
  	  case A_CH:
  	    _i_i2as=datasize(in2_ch);
  	    _i_i2als=_i_i2as*in2_ch->x;
  	    _i_sti2a.u=in2_ch->data->u;
  	    iref(in2_ch->type,&_i_i2aget);
  	    break;
  	  case Y_CH:
  	    _i_i2ys=datasize(in2_ch);
  	    _i_i2yls=_i_i2ys*in2_ch->x;
  	    _i_sti2y.u=in2_ch->data->u;
  	    iref(in2_ch->type,&_i_i2yget);
  	    break;
  	  case U_CH:
  	    _i_i2us=datasize(in2_ch);
  	    _i_i2uls=_i_i2us*in2_ch->x;
  	    _i_sti2u.u=in2_ch->data->u;
  	    iref(in2_ch->type,&_i_i2uget);
  	    break;
  	  case V_CH:
  	    _i_i2vs=datasize(in2_ch);
  	    _i_i2vls=_i_i2vs*in2_ch->x;
  	    _i_sti2v.u=in2_ch->data->u;
  	    iref(in2_ch->type,&_i_i2vget);
  	    break;
  	  }
  	  if (in2_ch) {
  	    if (!_i_in2width) _i_in2width=in2_ch->x;
  	    else {
  	      if (_i_in2width != in2_ch->x) {
  		addlib_stop_here("Warning can only process colorformat 444\n",
				 0,0);
  		return;
  	      }
  	    }
  	    if (!_i_in2height) _i_in2height=in2_ch->y;
  	    else {
  	      if (_i_in2height != in2_ch->y) {
  		addlib_stop_here("Warning can only process colorformat 444\n",
				 0,0);
  		return;
  	      }
  	    }
  	  }
  	}
      }
    }
    else {
      _i_in2channels=0;
    }

    if (result) {
      if (reschannels & i) {
  	switch (i) {
  	case A_CH:
  	  res_ch=result->a_chan;
  	  break;
  	case Y_CH:
  	  res_ch=result->y_chan;
  	  break;
  	case U_CH:
  	  res_ch=result->u_chan;
  	  break;
  	case V_CH:
  	  res_ch=result->v_chan;
  	  break;
  	default:
  	  addlib_stop_here("ERROR : Internal error in inter_proc\n",0,0);
  	  return;
  	}

  	if (!res_ch) {
  	  addlib_stop_here("WARNING : color component does not exist in "
  		  "inter_proc\n",0,0);
  	  reschannels-=i;
  	}
  	else  if (!(res_ch->x && res_ch->y)) {
  	  addlib_stop_here("WARNING : color component has 0-size in "
  		  "inter_proc\n",0,0);
  	  reschannels-=i;
  	}
  	else {
  	  switch (i) {
  	  case A_CH:
  	    _i_ras=datasize(res_ch);
  	    _i_rals=_i_ras*res_ch->x;
  	    _i_stra.u=res_ch->data->u;
  	    oref(res_ch->type,&_i_raset);
  	    break;
  	  case Y_CH:
  	    _i_rys=datasize(res_ch);
  	    _i_ryls=_i_rys*res_ch->x;
  	    _i_stry.u=res_ch->data->u;
  	    oref(res_ch->type,&_i_ryset);
  	    break;
  	  case U_CH:
  	    _i_rus=datasize(res_ch);
  	    _i_ruls=_i_rus*res_ch->x;
  	    _i_stru.u=res_ch->data->u;
  	    oref(res_ch->type,&_i_ruset);
  	    break;
  	  case V_CH:
  	    _i_rvs=datasize(res_ch);
  	    _i_rvls=_i_rvs*res_ch->x;
  	    _i_strv.u=res_ch->data->u;
  	    oref(res_ch->type,&_i_rvset);
  	    break;
  	  }

  	  if (res_ch) {
  	    if (!width) width=res_ch->x;
  	    else {
  	      if (width != res_ch->x) {
  		addlib_stop_here("Warning can only process colorformat 444\n",
				 0,0);
  		return;
  	      }
  	    }
  	    if (!height) height=res_ch->y;
  	    else {
  	      if (height != res_ch->y) {
  		addlib_stop_here("Warning can only process colorformat 444\n",
				 0,0);
  		return;
  	      }
  	    }
  	  }
  	}
      }
    }
    else {
      reschannels=0;
    }

    /* next colorcomponent to check*/
    i<<=1;
  }

  if (!reschannels) {
    if (_i_in1channels) {
      width=_i_in1width-in1posx;
      height=_i_in1height-in1posy;
    }
    else if (_i_in1channels) {
      width=_i_in2width-in1posx;
      height=_i_in2height-in1posy;
    }
    else {
      addlib_stop_here("Warning : no image componenet given\n",0,0);
      return;
    }
    resposx=0;
    resposy=0;
  }

  /*check for window size*/
  if (winx == -1) {
    winx=width;
  }
  if (winy == -1) {
    winy=height;
  }

  /*check window placments*/
  if (reschannels) {
    if ((resposx +winx > width) || (resposy +winy > height) ||
	(resposx < 0) || (resposy < 0)) {
      error=1;
    }
  }
  if (_i_in1channels) {
    if ((in1posx +winx > _i_in1width) || (in1posy +winy > _i_in1height) ||
	(in1posx < 0) || (in1posy < 0)) {
      error=1;
    }
  }
  if (_i_in2channels) {
    if ((in2posx +winx > _i_in2width) || (in2posy +winy > _i_in2height) ||
	(in2posx < 0) || (in2posy < 0)) {
      error=1;
    }
  }
  if (error) {
    addlib_stop_here("WARNING : Window not in Image in inter_proc\n",0,0);
    return;
  }

  if (in1posx) {
    if (_i_sti1a.u) _i_sti1a.u+=in1posx*_i_i1as;
    if (_i_sti1y.u) _i_sti1y.u+=in1posx*_i_i1ys;
    if (_i_sti1u.u) _i_sti1u.u+=in1posx*_i_i1us;
    if (_i_sti1v.u) _i_sti1v.u+=in1posx*_i_i1vs;
  }
  if (in2posx) {
    if (_i_sti2a.u) _i_sti2a.u+=in2posx*_i_i2as;
    if (_i_sti2y.u) _i_sti2y.u+=in2posx*_i_i2ys;
    if (_i_sti2u.u) _i_sti2u.u+=in2posx*_i_i2us;
    if (_i_sti2v.u) _i_sti2v.u+=in2posx*_i_i2vs;
  }
  if (resposx) {
    if (_i_stra.u) _i_stra.u+=resposx*_i_ras;
    if (_i_stry.u) _i_stry.u+=resposx*_i_rys;
    if (_i_stru.u) _i_stru.u+=resposx*_i_rus;
    if (_i_strv.u) _i_strv.u+=resposx*_i_rvs;
  }
  _i_maxx=resposx+winx-1;

  if (in1posy) {
    if (_i_sti1a.u) _i_sti1a.u+=in1posy*_i_i1als;
    if (_i_sti1y.u) _i_sti1y.u+=in1posy*_i_i1yls;
    if (_i_sti1u.u) _i_sti1u.u+=in1posy*_i_i1uls;
    if (_i_sti1v.u) _i_sti1v.u+=in1posy*_i_i1vls;
  }
  if (in2posy) {
    if (_i_sti2a.u) _i_sti2a.u+=in2posy*_i_i2als;
    if (_i_sti2y.u) _i_sti2y.u+=in2posy*_i_i2yls;
    if (_i_sti2u.u) _i_sti2u.u+=in2posy*_i_i2uls;
    if (_i_sti2v.u) _i_sti2v.u+=in2posy*_i_i2vls;
  }
  if (resposy) {
    if (_i_stra.u) _i_stra.u+=resposy*_i_rals;
    if (_i_stry.u) _i_stry.u+=resposy*_i_ryls;
    if (_i_stru.u) _i_stru.u+=resposy*_i_ruls;
    if (_i_strv.u) _i_strv.u+=resposy*_i_rvls;
  }
  _i_maxy=resposy+winy-1;

  in1.a.i=in1.y.i=in1.u.i=in1.v.i=in1.ax.i=
    in2.a.i=in2.y.i=in2.u.i=in2.v.i=in2.ax.i=0;

  /* loops for imagescan*/
  switch (scanmode) {
  case SCAN_HO:


    for (cnty=resposy; cnty<=_i_maxy; cnty++) {

      _i_i1a.u=_i_sti1a.u;
      _i_i1y.u=_i_sti1y.u;
      _i_i1u.u=_i_sti1u.u;
      _i_i1v.u=_i_sti1v.u;
      _i_i2a.u=_i_sti2a.u;
      _i_i2y.u=_i_sti2y.u;
      _i_i2u.u=_i_sti2u.u;
      _i_i2v.u=_i_sti2v.u;
      _i_ra.u=_i_stra.u;
      _i_ry.u=_i_stry.u;
      _i_ru.u=_i_stru.u;
      _i_rv.u=_i_strv.u;


#ifdef EVAL
      eval_LogPreload();
#endif
      for (cntx=resposx; cntx<=_i_maxx; cntx++) {

	if (_i_i1as) {
	  in1.a.i=_i_i1aget(_i_i1a);
	  _i_i1a.u+=_i_i1as;
	}
	if (_i_i1ys) {
	  in1.y.i=_i_i1yget(_i_i1y);
	  _i_i1y.u+=_i_i1ys;
	}
	if (_i_i1us) {
	  in1.u.i=_i_i1uget(_i_i1u);
	  _i_i1u.u+=_i_i1us;
	}
	if (_i_i1vs) {
	  in1.v.i=_i_i1vget(_i_i1v);
	  _i_i1v.u+=_i_i1vs;
	}

	if (_i_i2as) {
	  in2.a.i=_i_i2aget(_i_i2a);
	  _i_i2a.u+=_i_i2as;
	}
	if (_i_i2ys) {
	  in2.y.i=_i_i2yget(_i_i2y);
	  _i_i2y.u+=_i_i2ys;
	}
	if (_i_i2us) {
	  in2.u.i=_i_i2uget(_i_i2u);
	  _i_i2u.u+=_i_i2us;
	}
	if (_i_i2vs) {
	  in2.v.i=_i_i2vget(_i_i2v);
	  _i_i2v.u+=_i_i2vs;
	}


#ifdef EVAL
  eval_LogOpr(cntx,cnty);
#endif
	/* execute embedded function*/
#ifdef PPTRACE
  PPTrace_LogInterIn(&in1, &in2, cntx, cnty);
#endif
	operation(&res,&in1,&in2);
#ifdef PPTRACE
  PPTrace_LogInterOut(res);
#endif


	if (_i_ras) {
	  _i_raset(_i_ra,res.a.i);
	  _i_ra.u+=_i_ras;
	}
	if (_i_rys) {
	  _i_ryset(_i_ry,res.y.i);
	  _i_ry.u+=_i_rys;
	}
	if (_i_rus) {
	  _i_ruset(_i_ru,res.u.i);
	  _i_ru.u+=_i_rus;
	}
	if (_i_rvs) {
	  _i_rvset(_i_rv,res.v.i);
	  _i_rv.u+=_i_rvs;
	}

      }

      _i_sti1a.u+=_i_i1als;
      _i_sti1y.u+=_i_i1yls;
      _i_sti1u.u+=_i_i1uls;
      _i_sti1v.u+=_i_i1vls;
      _i_sti2a.u+=_i_i2als;
      _i_sti2y.u+=_i_i2yls;
      _i_sti2u.u+=_i_i2uls;
      _i_sti2v.u+=_i_i2vls;
      _i_stra.u+=_i_rals;
      _i_stry.u+=_i_ryls;
      _i_stru.u+=_i_ruls;
      _i_strv.u+=_i_rvls;

    }
    break;
  case SCAN_VE:
    for (cntx=resposx; cntx<=_i_maxx; cntx++) {


      _i_i1a.u=_i_sti1a.u;
      _i_i1y.u=_i_sti1y.u;
      _i_i1u.u=_i_sti1u.u;
      _i_i1v.u=_i_sti1v.u;
      _i_i2a.u=_i_sti2a.u;
      _i_i2y.u=_i_sti2y.u;
      _i_i2u.u=_i_sti2u.u;
      _i_i2v.u=_i_sti2v.u;
      _i_ra.u=_i_stra.u;
      _i_ry.u=_i_stry.u;
      _i_ru.u=_i_stru.u;
      _i_rv.u=_i_strv.u;


#ifdef EVAL
      eval_LogPreload();
#endif
      for (cnty=resposy; cnty<=_i_maxy; cnty++) {

	if (_i_i1as) {
	  in1.a.i=_i_i1aget(_i_i1a);
	  _i_i1a.u+=_i_i1als;
	}
	if (_i_i1ys) {
	  in1.y.i=_i_i1yget(_i_i1y);
	  _i_i1y.u+=_i_i1yls;
	}
	if (_i_i1us) {
	  in1.u.i=_i_i1uget(_i_i1u);
	  _i_i1u.u+=_i_i1uls;
	}
	if (_i_i1vs) {
	  in1.v.i=_i_i1vget(_i_i1v);
	  _i_i1v.u+=_i_i1vls;
	}

	if (_i_i2as) {
	  in2.a.i=_i_i2aget(_i_i2a);
	  _i_i2a.u+=_i_i2als;
	}
	if (_i_i2ys) {
	  in2.y.i=_i_i2yget(_i_i2y);
	  _i_i2y.u+=_i_i2yls;
	}
	if (_i_i2us) {
	  in2.u.i=_i_i2uget(_i_i2u);
	  _i_i2u.u+=_i_i2uls;
	}
	if (_i_i2vs) {
	  in2.v.i=_i_i2vget(_i_i2v);
	  _i_i2v.u+=_i_i2vls;
	}


#ifdef EVAL
  eval_LogOpr(cntx,cnty);
#endif
	/* execute embedded function*/
#ifdef PPTRACE
  PPTrace_LogInterIn(&in1, &in2, cntx, cnty);
#endif
	operation(&res,&in1,&in2);
#ifdef PPTRACE
  PPTrace_LogInterOut(res);
#endif

	if (_i_ras) {
	  _i_raset(_i_ra,res.a.i);
	  _i_ra.u+=_i_rals;
	}
	if (_i_rys) {
	  _i_ryset(_i_ry,res.y.i);
	  _i_ry.u+=_i_ryls;
	}
	if (_i_rus) {
	  _i_ruset(_i_ru,res.u.i);
	  _i_ru.u+=_i_ruls;
	}
	if (_i_rvs) {
	  _i_rvset(_i_rv,res.v.i);
	  _i_rv.u+=_i_rvls;
	}

      }

      _i_sti1a.u+=_i_i1as;
      _i_sti1y.u+=_i_i1ys;
      _i_sti1u.u+=_i_i1us;
      _i_sti1v.u+=_i_i1vs;
      _i_sti2a.u+=_i_i2as;
      _i_sti2y.u+=_i_i2ys;
      _i_sti2u.u+=_i_i2us;
      _i_sti2v.u+=_i_i2vs;
      _i_stra.u+=_i_ras;
      _i_stry.u+=_i_rys;
      _i_stru.u+=_i_rus;
      _i_strv.u+=_i_rvs;


    }

    break;
  case SCAN_HM:

    _i_i1a.u=_i_sti1a.u;
    _i_i1y.u=_i_sti1y.u;
    _i_i1u.u=_i_sti1u.u;
    _i_i1v.u=_i_sti1v.u;
    _i_i2a.u=_i_sti2a.u;
    _i_i2y.u=_i_sti2y.u;
    _i_i2u.u=_i_sti2u.u;
    _i_i2v.u=_i_sti2v.u;
    _i_ra.u=_i_stra.u;
    _i_ry.u=_i_stry.u;
    _i_ru.u=_i_stru.u;
    _i_rv.u=_i_strv.u;

#ifdef EVAL
    eval_LogPreload();
#endif
    for (cnty=resposy; cnty<=_i_maxy; cnty++) {

      for (cntx=resposy; cntx<=_i_maxx; cntx++) {

	if (_i_i1as) {
	  in1.a.i=_i_i1aget(_i_i1a);
	}
	if (_i_i1ys) {
	  in1.y.i=_i_i1yget(_i_i1y);
	}
	if (_i_i1us) {
	  in1.u.i=_i_i1uget(_i_i1u);
	}
	if (_i_i1vs) {
	  in1.v.i=_i_i1vget(_i_i1v);
	}
	if (_i_i2as) {
	  in2.a.i=_i_i2aget(_i_i2a);
	}
	if (_i_i2ys) {
	  in2.y.i=_i_i2yget(_i_i2y);
	}
	if (_i_i2us) {
	  in2.u.i=_i_i2uget(_i_i2u);
	}
	if (_i_i2vs) {
	  in2.v.i=_i_i2vget(_i_i2v);
	}

#ifdef EVAL
  eval_LogOpr(cntx,cnty);
#endif
	/* execute embedded function*/
#ifdef PPTRACE
  PPTrace_LogInterIn(&in1, &in2, cntx, cnty);
#endif
	operation(&res,&in1,&in2);
#ifdef PPTRACE
  PPTrace_LogInterOut(res);
#endif

	if (_i_ras) {
	  _i_raset(_i_ra,res.a.i);
	}
	if (_i_rys) {
	  _i_ryset(_i_ry,res.y.i);
	}
	if (_i_rus) {
	  _i_ruset(_i_ru,res.u.i);
	}
	if (_i_rvs) {
	  _i_rvset(_i_rv,res.v.i);
	}


	/*if not end of line*/
/*	if (dir == 1) {
	  if (cntx==_i_maxx) {
	    break;
	  }
	}
	else {
	  if (cntx==resposx) {
	    break;
	  }
	}*/

	/*next pixel (size value is inverted at each line)*/
/*	cntx+=dir;*/
	_i_i1a.u+=_i_i1as;
	_i_i1y.u+=_i_i1ys;
	_i_i1u.u+=_i_i1us;
	_i_i1v.u+=_i_i1vs;
	_i_i2a.u+=_i_i2as;
	_i_i2y.u+=_i_i2ys;
	_i_i2u.u+=_i_i2us;
	_i_i2v.u+=_i_i2vs;
	_i_ra.u+=_i_ras;
	_i_ry.u+=_i_rys;
	_i_ru.u+=_i_rus;
	_i_rv.u+=_i_rvs;
      }


      /* next row*/
      _i_i1a.u+=_i_i1als-_i_i1as;
      _i_i1y.u+=_i_i1yls-_i_i1ys;
      _i_i1u.u+=_i_i1uls-_i_i1us;
      _i_i1v.u+=_i_i1vls-_i_i1vs;
      _i_i2a.u+=_i_i2als-_i_i2as;
      _i_i2y.u+=_i_i2yls-_i_i2ys;
      _i_i2u.u+=_i_i2uls-_i_i2us;
      _i_i2v.u+=_i_i2vls-_i_i2vs;
      _i_ra.u+=_i_rals-_i_ras;
      _i_ry.u+=_i_ryls-_i_rys;
      _i_ru.u+=_i_ruls-_i_rus;
      _i_rv.u+=_i_rvls-_i_rvs;

      /*invert horizontal direction*/
/*      dir=-dir;*/
      _i_i1as=-_i_i1as;
      _i_i1ys=-_i_i1ys;
      _i_i1us=-_i_i1us;
      _i_i1vs=-_i_i1vs;
      _i_i2as=-_i_i2as;
      _i_i2ys=-_i_i2ys;
      _i_i2us=-_i_i2us;
      _i_i2vs=-_i_i2vs;
      _i_ras=-_i_ras;
      _i_rys=-_i_rys;
      _i_rus=-_i_rus;
      _i_rvs=-_i_rvs;

    }
    break;
  case SCAN_VM:

    _i_i1a.u=_i_sti1a.u;
    _i_i1y.u=_i_sti1y.u;
    _i_i1u.u=_i_sti1u.u;
    _i_i1v.u=_i_sti1v.u;
    _i_i2a.u=_i_sti2a.u;
    _i_i2y.u=_i_sti2y.u;
    _i_i2u.u=_i_sti2u.u;
    _i_i2v.u=_i_sti2v.u;
    _i_ra.u=_i_stra.u;
    _i_ry.u=_i_stry.u;
    _i_ru.u=_i_stru.u;
    _i_rv.u=_i_strv.u;

#ifdef EVAL
    eval_LogPreload();
#endif
    for (cntx=resposx; cntx<=_i_maxx; cntx++) {

      for (cnty=resposy; cnty<=_i_maxy; cnty++) {

	if (_i_i1as) {
	  in1.a.i=_i_i1aget(_i_i1a);
	}
	if (_i_i1ys) {
	  in1.y.i=_i_i1yget(_i_i1y);
	}
	if (_i_i1us) {
	  in1.u.i=_i_i1uget(_i_i1u);
	}
	if (_i_i1vs) {
	  in1.v.i=_i_i1vget(_i_i1v);
	}
	if (_i_i2as) {
	  in2.a.i=_i_i2aget(_i_i2a);
	}
	if (_i_i2ys) {
	  in2.y.i=_i_i2yget(_i_i2y);
	}
	if (_i_i2us) {
	  in2.u.i=_i_i2uget(_i_i2u);
	}
	if (_i_i2vs) {
	  in2.v.i=_i_i2vget(_i_i2v);
	}

#ifdef EVAL
  eval_LogOpr(cntx,cnty);
#endif
	/* execute embedded function*/
#ifdef PPTRACE
  PPTrace_LogInterIn(&in1, &in2, cntx, cnty);
#endif
	operation(&res,&in1,&in2);
#ifdef PPTRACE
  PPTrace_LogInterOut(res);
#endif

	if (_i_ras) {
	  _i_raset(_i_ra,res.a.i);
	}
	if (_i_rys) {
	  _i_ryset(_i_ry,res.y.i);
	}
	if (_i_rus) {
	  _i_ruset(_i_ru,res.u.i);
	}
	if (_i_rvs) {
	  _i_rvset(_i_rv,res.v.i);
	}

	/*if not end of culomn*/
/*	if (dir == 1) {
	  if (cnty==_i_maxy) {
	    break;
	  }
	}
	else {
	  if (cnty==resposy) {
	    break;
	  }
	}*/

	/*next pixel (sizevalue is invertet at each column)*/
/*	cnty+=dir;*/
	_i_i1a.u+=_i_i1als;
	_i_i1y.u+=_i_i1yls;
	_i_i1u.u+=_i_i1uls;
	_i_i1v.u+=_i_i1vls;
	_i_i2a.u+=_i_i2als;
	_i_i2y.u+=_i_i2yls;
	_i_i2u.u+=_i_i2uls;
	_i_i2v.u+=_i_i2vls;
	_i_ra.u+=_i_rals;
	_i_ry.u+=_i_ryls;
	_i_ru.u+=_i_ruls;
	_i_rv.u+=_i_rvls;

      }

      /* next row*/
      _i_i1a.u+=_i_i1as-_i_i1als;
      _i_i1y.u+=_i_i1ys-_i_i1yls;
      _i_i1u.u+=_i_i1us-_i_i1uls;
      _i_i1v.u+=_i_i1vs-_i_i1vls;
      _i_i2a.u+=_i_i2as-_i_i1als;
      _i_i2y.u+=_i_i2ys-_i_i1yls;
      _i_i2u.u+=_i_i2us-_i_i1uls;
      _i_i2v.u+=_i_i2vs-_i_i1vls;
      _i_ra.u+=_i_ras-_i_rals;
      _i_ry.u+=_i_rys-_i_ryls;
      _i_ru.u+=_i_rus-_i_ruls;
      _i_rv.u+=_i_rvs-_i_rvls;

      /*invert horizontal direction*/
/*      dir=-dir;*/
      _i_i1als=-_i_i1als;
      _i_i1yls=-_i_i1yls;
      _i_i1uls=-_i_i1uls;
      _i_i1vls=-_i_i1vls;
      _i_i2als=-_i_i2als;
      _i_i2yls=-_i_i2yls;
      _i_i2uls=-_i_i2uls;
      _i_i2vls=-_i_i2vls;
      _i_rals=-_i_rals;
      _i_ryls=-_i_ryls;
      _i_ruls=-_i_ruls;
      _i_rvls=-_i_rvls;

    }
    break;
  case SCAN_LOOP:
  default :
    addlib_stop_here("Warning : scanmode not implemented\n",0,0);
    return;
  }

#ifdef PPTRACE
  PPTrace_LogInterReg(0);
#endif

#ifdef EVAL
  eval_EndMeasureDur();
  eval_EndAddr();
  eval_EndOpr();
  eval_EndConSC();
  eval_EndScan();
  eval_EndRecMode();
  eval_EndAddrMode();
#endif
}



/***************************************************************************/
/* inter_proc with coordnite transformation on input2*/

void intertrans_proc(MomVop *result, MomVop *input1, MomVop *input2,
	       int reschannels,int in1channels,int in2channels,
	       int scanmode,
	       TCoor winx,TCoor winy,TCoor resposx,TCoor resposy,
	       TCoor in1posx, TCoor in1posy,
	       void in2trans(TCoor width,TCoor height, TCoor inx,TCoor iny,
			     float *outx,float *outy),
	       void (*operation)(TPixel *res, TPixel *in1, TPixel *in2))

{
  TPixel res,in1;
  TCoor cntx,cnty;        /* current centerposition in resultimage*/
  TCoor width,height;     /* result image size*/
  int error;
  int i;
  short dir;               /* scandirection for meander modes*/
  MomImage *res_ch=0,*in1_ch=0,*in2_ch=0;

  /* references to interpolation fuctions*/
  TChan (*_i2_interpolation)();

#ifdef EVAL
  eval_BeginMeasureDur();
  eval_BeginAddrMode(SECTION_INTER_TRANS);
  eval_BeginRecMode(input1,result,in1channels,reschannels);
  eval_PictSize(input1,input2,result,NULL);
  eval_BeginScan(scanmode);
  eval_ScanWindow(winx,winy,in1posx,in1posy,0,0,resposx,resposy);
  eval_BeginConSC(CON_0);
  eval_BeginOpr((void*)operation);
  eval_BeginTrafo((void*)in2trans);
  eval_StartAddr();
#endif

#ifdef PPTRACE
  PPTrace_StartOfTool();
  PPTrace_LogType((void*)operation,reschannels,CON_0);
  PPTrace_LogInterReg(1);
#endif

  /* inital values*/
  _i_stra.u=_i_stry.u=_i_stru.u=_i_strv.u=0;
  _i_sti1a.u=_i_sti1y.u=_i_sti1u.u=_i_sti1v.u=0;
  _i_sti2a.u=_i_sti2y.u=_i_sti2u.u=_i_sti2v.u=0;

  _i_ra.u=_i_ry.u=_i_ru.u=_i_rv.u=0;;
  _i_i1a.u=_i_i1y.u=_i_i1u.u=_i_i1v.u=0;
  _i_i2a.u=_i_i2y.u=_i_i2u.u=_i_i2v.u=0;

  _i_ras=_i_rys=_i_rus=_i_rvs=0;
  _i_i1as=_i_i1ys=_i_i1us=_i_i1vs=0;
  _i_i2as=_i_i2ys=_i_i2us=_i_i2vs=0;

  _i_rals=_i_ryls=_i_ruls=_i_rvls=0;
  _i_i1als=_i_i1yls=_i_i1uls=_i_i1vls=0;
  _i_i2als=_i_i2yls=_i_i2uls=_i_i2vls=0;

/*  _i_ratype=_i_rytype=_i_rutype=_i_rvtype=UCHAR_TYPE;
  _i_i1atype=_i_i1ytype=_i_i1utype=_i_i1vtype=UCHAR_TYPE;
  _i_i2atype=_i_i2ytype=_i_i2utype=_i_i2vtype=UCHAR_TYPE;*/

  width=height=_i_in1width=_i_in1height=_i_in2width=_i_in2height=0;
  _i_in1channels=in1channels;
  _i_in2channels=in2channels;

  _i_i1aget= &getunused;
  _i_i1yget= &getunused;
  _i_i1uget= &getunused;
  _i_i1vget= &getunused;
  _i_i2aget= &getunused;
  _i_i2yget= &getunused;
  _i_i2uget= &getunused;
  _i_i2vget= &getunused;
  _i_raset= &setunused;
  _i_ryset= &setunused;
  _i_ruset= &setunused;
  _i_rvset= &setunused;

  switch (scanmode & (TRANS_INTERPOLATE_MODES)) {
  case (TRANS_INT_INTERPOLATE):
    _i2_interpolation= &_t_int_interpolation;
    break;
  case (TRANS_FLOAT_INTERPOLATE):
    _i2_interpolation= &_t_float_interpolation;
    break;
  default:
    _i2_interpolation= &_t_no_interpolation;
  }

  i=1;
  error=0;
  while (i<16) {
    if (_i_in1channels & i) {
      switch (i) {
      case A_CH:
	in1_ch=input1->a_chan;
	break;
      case Y_CH:
	in1_ch=input1->y_chan;
	break;
      case U_CH:
	in1_ch=input1->u_chan;
	break;
      case V_CH:
	in1_ch=input1->v_chan;
	break;
      default:
	addlib_stop_here("ERROR : Internal error in inter_proc\n",0,0);
	return;
      }

      if (!in1_ch) {
	addlib_stop_here("WARNING : color component does not exist in "
		"inter_proc\n",0,0);
	_i_in1channels-=i;
      }
      else if (!(in1_ch->x && in1_ch->y)) {
	addlib_stop_here("WARNING : color component has 0-size in "
		"inter_proc\n",0,0);
	_i_in1channels-=i;
      }
      else {
	switch (i) {
	case A_CH:
	  _i_i1as=datasize(in1_ch);
	  _i_i1als=_i_i1as*in1_ch->x;
/* 	  _i_i1atype=in1_ch->type; */
	  _i_sti1a.u=in1_ch->data->u;
	  iref(in1_ch->type,&_i_i1aget);
	  break;
	case Y_CH:
	  _i_i1ys=datasize(in1_ch);
	  _i_i1yls=_i_i1ys*in1_ch->x;
/* 	  _i_i1ytype=in1_ch->type; */
	  _i_sti1y.u=in1_ch->data->u;
	  iref(in1_ch->type,&_i_i1yget);
	  break;
	case U_CH:
	  _i_i1us=datasize(in1_ch);
	  _i_i1uls=_i_i1us*in1_ch->x;
/* 	  _i_i1utype=in1_ch->type; */
	  _i_sti1u.u=in1_ch->data->u;
	  iref(in1_ch->type,&_i_i1uget);
	  break;
	case V_CH:
	  _i_i1vs=datasize(in1_ch);
	  _i_i1vls=_i_i1vs*in1_ch->x;
/* 	  _i_i1vtype=in1_ch->type; */
	  _i_sti1v.u=in1_ch->data->u;
	  iref(in1_ch->type,&_i_i1vget);
	  break;
	}
	if (in1_ch) {
	  if (!_i_in1width) _i_in1width=in1_ch->x;
	  else {
	    if (_i_in1width != in1_ch->x) {
	      addlib_stop_here("Warning can only process colorformat 444\n",
			       0,0);
	      return;
	    }
	  }
	  if (!_i_in1height) _i_in1height=in1_ch->y;
	  else {
	    if (_i_in1height != in1_ch->y) {
	      addlib_stop_here("Warning can only process colorformat 444\n",
			       0,0);
	      return;
	    }
	  }
	}
      }
    }

    if (_i_in2channels & i) {
      switch (i) {
      case A_CH:
	in2_ch=input2->a_chan;
	break;
      case Y_CH:
	in2_ch=input2->y_chan;
	break;
      case U_CH:
	in2_ch=input2->u_chan;
	break;
      case V_CH:
	in2_ch=input2->v_chan;
	break;
      default:
	addlib_stop_here("ERROR : Internal error in inter_proc\n",0,0);
	return;
      }

      if (!in2_ch) {
	addlib_stop_here("WARNING : color component does not exist in "
		"inter_proc\n",0,0);
	_i_in2channels-=i;
      }
      else if (!(in2_ch->x && in2_ch->y)) {
	addlib_stop_here("WARNING : color component has 0-size in "
		"inter_proc\n",0,0);
	_i_in2channels-=i;
      }
      else {
	switch (i) {
	case A_CH:
	  _i_i2as=datasize(in2_ch);
	  _i_i2als=_i_i2as*in2_ch->x;
/* 	  _i_i2atype=in2_ch->type; */
	  _i_sti2a.u=in2_ch->data->u;
	  iref(in2_ch->type,&_i_i2aget);
	  break;
	case Y_CH:
	  _i_i2ys=datasize(in2_ch);
	  _i_i2yls=_i_i2ys*in2_ch->x;
/* 	  _i_i2ytype=in2_ch->type; */
 	  _i_sti2y.u=in2_ch->data->u;
	  iref(in2_ch->type,&_i_i2yget);
	  break;
	case U_CH:
	  _i_i2us=datasize(in2_ch);
	  _i_i2uls=_i_i2us*in2_ch->x;
/* 	  _i_i2utype=in2_ch->type; */
	  _i_sti2u.u=in2_ch->data->u;
	  iref(in2_ch->type,&_i_i2uget);
	  break;
	case V_CH:
	  _i_i2vs=datasize(in2_ch);
	  _i_i2vls=_i_i2vs*in2_ch->x;
/* 	  _i_i2vtype=in2_ch->type; */
	  _i_sti2v.u=in2_ch->data->u;
	  iref(in2_ch->type,&_i_i2vget);
	  break;
	}
	if (in2_ch) {
	  if (!_i_in2width) _i_in2width=in2_ch->x;
	  else {
	    if (_i_in2width != in2_ch->x) {
	      addlib_stop_here("Warning can only process colorformat 444\n",
			       0,0);
	      return;
	    }
	  }
	  if (!_i_in2height) _i_in2height=in2_ch->y;
	  else {
	    if (_i_in2height != in2_ch->y) {
	      addlib_stop_here("Warning can only process colorformat 444\n",
			       0,0);
	      return;
	    }
	  }
	}
      }
    }


    if (reschannels & i) {
      switch (i) {
      case A_CH:
	res_ch=result->a_chan;
	break;
      case Y_CH:
	res_ch=result->y_chan;
	break;
      case U_CH:
	res_ch=result->u_chan;
	break;
      case V_CH:
	res_ch=result->v_chan;
	break;
      default:
	addlib_stop_here("ERROR : Internal error in inter_proc\n",0,0);
	return;
      }

      if (!res_ch) {
	addlib_stop_here("WARNING : color component does not exist in "
		"inter_proc\n",0,0);
	reschannels-=i;
      }
      else  if (!(res_ch->x && res_ch->y)) {
	addlib_stop_here("WARNING : color component has 0-size in "
		"inter_proc\n",0,0);
	reschannels-=1;
      }
      else {
	switch (i) {
	case A_CH:
	  _i_ras=datasize(res_ch);
	  _i_rals=_i_ras*res_ch->x;
/* 	  _i_ratype=res_ch->type; */
	  _i_stra.u=res_ch->data->u;
	  oref(res_ch->type,&_i_raset);
	  break;
	case Y_CH:
	  _i_rys=datasize(res_ch);
	  _i_ryls=_i_rys*res_ch->x;
/* 	  _i_rytype=res_ch->type; */
	  _i_stry.u=res_ch->data->u;
	  oref(res_ch->type,&_i_ryset);
	  break;
	case U_CH:
	  _i_rus=datasize(res_ch);
	  _i_ruls=_i_rus*res_ch->x;
/* 	  _i_rutype=res_ch->type; */
	  _i_stru.u=res_ch->data->u;
	  oref(res_ch->type,&_i_ruset);
	  break;
	case V_CH:
	  _i_rvs=datasize(res_ch);
	  _i_rvls=_i_rvs*res_ch->x;
/* 	  _i_rvtype=res_ch->type; */
	  _i_strv.u=res_ch->data->u;
	  oref(res_ch->type,&_i_rvset);
	  break;
	}

	if (res_ch) {
	  if (!width) width=res_ch->x;
	  else {
	    if (width != res_ch->x) {
	      addlib_stop_here("Warning can only process colorformat 444\n",
			       0,0);
	      return;
	    }
	  }
	  if (!height) height=res_ch->y;
	  else {
	    if (height != res_ch->y) {
	      addlib_stop_here("Warning can only process colorformat 444\n",
			       0,0);
	      return;
	    }
	  }
	}
      }
    }

    /* next colorcomponent to check*/
    i<<=1;
  }

  if (!(width || _i_in1width) ||
      !(height || _i_in1height)) {
    error=1;
  }

  if (!width) {
    width=_i_in1width;
  }
  if (!height) {
    height=_i_in1height;
  }

  if (_i_in1width || _i_in1height) {
    if ((width != _i_in1width) || (height != _i_in1height)) error=1;
  }

  if (!in2trans) in2trans=&_t_notrans;

  if (error) {
    addlib_stop_here("WARNING : Images size does not match in inter_proc\n",
		     0,0);
    return;
  }

  if (!(_i_in1channels || _i_in2channels)) {
    addlib_stop_here("WARNING : No channel component in inter_proc\n",0,0);
    return;
  }

  /*check window placments*/
  if (winx == -1) winx=width;
  if (winy == -1) winy=height;
  if ((resposx +winx > width) || (resposy +winy > height)) error=1;
  if ((resposx < 0) || (resposy < 0)) error=1;
  if ((in1posx +winx > width) || (in1posy +winy > height)) error=1;
  if ((in1posx < 0) || (in1posy < 0)) error=1;
/*  if ((in2posx +winx > width) || (in2posy +winy > height)) error=1;
  if ((in2posx < 0) || (in2posy < 0)) error=1;*/
  if (error) {
    addlib_stop_here("WARNING : Window not in Image in inter_proc\n",0,0);
    return;
  }

  if (winx != -1) {
    if (_i_sti1a.u) _i_sti1a.u+=in1posx*_i_i1as;
    if (_i_sti1y.u) _i_sti1y.u+=in1posx*_i_i1ys;
    if (_i_sti1u.u) _i_sti1u.u+=in1posx*_i_i1us;
    if (_i_sti1v.u) _i_sti1v.u+=in1posx*_i_i1vs;
/*    if (_i_sti2a.u) _i_sti2a.u+=in2posx*_i_i2as;
    if (_i_sti2y.u) _i_sti2y.u+=in2posx*_i_i2ys;
    if (_i_sti2u.u) _i_sti2u.u+=in2posx*_i_i2us;
    if (_i_sti2v.u) _i_sti2v.u+=in2posx*_i_i2vs;*/
/*    if (stax.u) stax.u+=resposx*_i_axs;*/
    if (_i_stra.u) _i_stra.u+=resposx*_i_ras;
    if (_i_stry.u) _i_stry.u+=resposx*_i_rys;
    if (_i_stru.u) _i_stru.u+=resposx*_i_rus;
    if (_i_strv.u) _i_strv.u+=resposx*_i_rvs;
    _i_maxx=resposx+winx-1;
/*    _i_icntx=inposx;
    stx=cntx=resposx;*/
  }
  else _i_maxx=width-1;

  if (winy != -1) {
    if (_i_sti1a.u) _i_sti1a.u+=in1posy*_i_i1als;
    if (_i_sti1y.u) _i_sti1y.u+=in1posy*_i_i1yls;
    if (_i_sti1u.u) _i_sti1u.u+=in1posy*_i_i1uls;
    if (_i_sti1v.u) _i_sti1v.u+=in1posy*_i_i1vls;
/*    if (_i_sti2a.u) _i_sti2a.u+=in2posy*_i_i2als;
    if (_i_sti2y.u) _i_sti2y.u+=in2posy*_i_i2yls;
    if (_i_sti2u.u) _i_sti2u.u+=in2posy*_i_i2uls;
    if (_i_sti2v.u) _i_sti2v.u+=in2posy*_i_i2vls;*/
/*    if (stax.u) stax.u+=resposy*_i_axls;*/
    if (_i_stra.u) _i_stra.u+=resposy*_i_rals;
    if (_i_stry.u) _i_stry.u+=resposy*_i_ryls;
    if (_i_stru.u) _i_stru.u+=resposy*_i_ruls;
    if (_i_strv.u) _i_strv.u+=resposy*_i_rvls;
    _i_maxy=resposy+winy-1;
/*    _i_icnty=inposy;
    sty=cnty=resposy;*/
  }
  else _i_maxy=height-1;



  /* loops for imagescan*/
  switch (scanmode & ~(TRANS_INTERPOLATE_MODES)) {
  case SCAN_HO:


    for (cnty=resposy; cnty<=_i_maxy; cnty++) {

      _i_i1a.u=_i_sti1a.u;
      _i_i1y.u=_i_sti1y.u;
      _i_i1u.u=_i_sti1u.u;
      _i_i1v.u=_i_sti1v.u;
/*      _i_i2a.u=_i_sti2a.u;
      _i_i2y.u=_i_sti2y.u;
      _i_i2u.u=_i_sti2u.u;
      _i_i2v.u=_i_sti2v.u;*/
      _i_ra.u=_i_stra.u;
      _i_ry.u=_i_stry.u;
      _i_ru.u=_i_stru.u;
      _i_rv.u=_i_strv.u;


#ifdef EVAL
      eval_LogPreload();
#endif
      for (cntx=resposx; cntx<=_i_maxx; cntx++) {

	if (_i_i1as) {
	  in1.a.i=_i_i1aget(_i_i1a);
	  _i_i1a.u+=_i_i1as;
	}
	if (_i_i1ys) {
	  in1.y.i=_i_i1yget(_i_i1y);
	  _i_i1y.u+=_i_i1ys;
	}
	if (_i_i1us) {
	  in1.u.i=_i_i1uget(_i_i1u);
	  _i_i1u.u+=_i_i1us;
	}
	if (_i_i1vs) {
	  in1.v.i=_i_i1vget(_i_i1v);
	  _i_i1v.u+=_i_i1vs;
	}

	/* transform coordinates for input2*/
	in2trans(_i_in2width,_i_in2height,cntx,cnty,&_i_in2x,&_i_in2y);
	_i_in2.m=_i2_interpolation();

#ifdef EVAL
  eval_LogOpr(cntx,cnty);
#endif
	/* execute embedded function*/
#ifdef PPTRACE
  PPTrace_LogInterIn(&in1, &_i_in2, cntx, cnty);
#endif
	operation(&res,&in1,&_i_in2);
#ifdef PPTRACE
  PPTrace_LogInterOut(res);
#endif

	if (_i_ras) {
	  _i_raset(_i_ra,res.a.i);
	  _i_ra.u+=_i_ras;
	}
	if (_i_rys) {
	  _i_ryset(_i_ry,res.y.i);
	  _i_ry.u+=_i_rys;
	}
	if (_i_rus) {
	  _i_ruset(_i_ru,res.u.i);
	  _i_ru.u+=_i_rus;
	}
	if (_i_rvs) {
	  _i_rvset(_i_rv,res.v.i);
	  _i_rv.u+=_i_rvs;
	}

      }

      _i_sti1a.u+=_i_i1als;
      _i_sti1y.u+=_i_i1yls;
      _i_sti1u.u+=_i_i1uls;
      _i_sti1v.u+=_i_i1vls;
/*      _i_sti2a.u+=_i_i2als;
      _i_sti2y.u+=_i_i2yls;
      _i_sti2u.u+=_i_i2uls;
      _i_sti2v.u+=_i_i2vls;*/
      _i_stra.u+=_i_rals;
      _i_stry.u+=_i_ryls;
      _i_stru.u+=_i_ruls;
      _i_strv.u+=_i_rvls;

    }
    break;
  case SCAN_VE:
    for (cntx=resposx; cntx<=_i_maxx; cntx++) {


      _i_i1a.u=_i_sti1a.u;
      _i_i1y.u=_i_sti1y.u;
      _i_i1u.u=_i_sti1u.u;
      _i_i1v.u=_i_sti1v.u;
/*      _i_i2a.u=_i_sti2a.u;
      _i_i2y.u=_i_sti2y.u;
      _i_i2u.u=_i_sti2u.u;
      _i_i2v.u=_i_sti2v.u;*/
      _i_ra.u=_i_stra.u;
      _i_ry.u=_i_stry.u;
      _i_ru.u=_i_stru.u;
      _i_rv.u=_i_strv.u;


#ifdef EVAL
      eval_LogPreload();
#endif
      for (cnty=resposy; cnty<=_i_maxy; cnty++) {

	if (_i_i1as) {
	  in1.a.i=_i_i1aget(_i_i1a);
	  _i_i1a.u+=_i_i1als;
	}
	if (_i_i1ys) {
	  in1.y.i=_i_i1yget(_i_i1y);
	  _i_i1y.u+=_i_i1yls;
	}
	if (_i_i1us) {
	  in1.u.i=_i_i1uget(_i_i1u);
	  _i_i1u.u+=_i_i1uls;
	}
	if (_i_i1vs) {
	  in1.v.i=_i_i1vget(_i_i1v);
	  _i_i1v.u+=_i_i1vls;
	}

	/* transform coordinates for input2*/
	in2trans(_i_in2width,_i_in2height,cntx,cnty,&_i_in2x,&_i_in2y);
	_i_in2.m=_i2_interpolation();

#ifdef EVAL
  eval_LogOpr(cntx,cnty);
#endif
	/* execute embedded function*/
#ifdef PPTRACE
  PPTrace_LogInterIn(&in1, &_i_in2, cntx, cnty);
#endif
	operation(&res,&in1,&_i_in2);
#ifdef PPTRACE
  PPTrace_LogInterOut(res);
#endif

	if (_i_ras) {
	  _i_raset(_i_ra,res.a.i);
	  _i_ra.u+=_i_rals;
	}
	if (_i_rys) {
	  _i_ryset(_i_ry,res.y.i);
	  _i_ry.u+=_i_ryls;
	}
	if (_i_rus) {
	  _i_ruset(_i_ru,res.u.i);
	  _i_ru.u+=_i_ruls;
	}
	if (_i_rvs) {
	  _i_rvset(_i_rv,res.v.i);
	  _i_rv.u+=_i_rvls;
	}

      }

      _i_sti1a.u+=_i_i1as;
      _i_sti1y.u+=_i_i1ys;
      _i_sti1u.u+=_i_i1us;
      _i_sti1v.u+=_i_i1vs;
/*      _i_sti2a.u+=_i_i2as;
      _i_sti2y.u+=_i_i2ys;
      _i_sti2u.u+=_i_i2us;
      _i_sti2v.u+=_i_i2vs;*/
      _i_stra.u+=_i_ras;
      _i_stry.u+=_i_rys;
      _i_stru.u+=_i_rus;
      _i_strv.u+=_i_rvs;


    }

    break;
  case SCAN_HM:

    _i_i1a.u=_i_sti1a.u;
    _i_i1y.u=_i_sti1y.u;
    _i_i1u.u=_i_sti1u.u;
    _i_i1v.u=_i_sti1v.u;
/*    _i_i2a.u=_i_sti2a.u;
    _i_i2y.u=_i_sti2y.u;
    _i_i2u.u=_i_sti2u.u;
    _i_i2v.u=_i_sti2v.u;*/
    _i_ra.u=_i_stra.u;
    _i_ry.u=_i_stry.u;
    _i_ru.u=_i_stru.u;
    _i_rv.u=_i_strv.u;

    cntx=resposx;
    dir=1;

#ifdef EVAL
      eval_LogPreload();
#endif
    for (cnty=resposy; cnty<=_i_maxy; cnty++) {

      while (1) {

	if (_i_i1as) {
	  in1.a.i=_i_i1aget(_i_i1a);
	}
	if (_i_i1ys) {
	  in1.y.i=_i_i1yget(_i_i1y);
	}
	if (_i_i1us) {
	  in1.u.i=_i_i1uget(_i_i1u);
	}
	if (_i_i1vs) {
	  in1.v.i=_i_i1vget(_i_i1v);
	}


	/* transform coordinates for input2*/
	in2trans(_i_in2width,_i_in2height,cntx,cnty,&_i_in2x,&_i_in2y);
	_i_in2.m=_i2_interpolation();

#ifdef EVAL
  eval_LogOpr(cntx,cnty);
#endif
	/* execute embedded function*/
#ifdef PPTRACE
  PPTrace_LogInterIn(&in1, &_i_in2, cntx, cnty);
#endif
	operation(&res,&in1,&_i_in2);
#ifdef PPTRACE
  PPTrace_LogInterOut(res);
#endif

	if (_i_ras) {
	  _i_raset(_i_ra,res.a.i);
	}
	if (_i_rys) {
	  _i_ryset(_i_ry,res.y.i);
	}
	if (_i_rus) {
	  _i_ruset(_i_ru,res.u.i);
	}
	if (_i_rvs) {
	  _i_rvset(_i_rv,res.v.i);
	}

	/*if not end of line*/
	if (dir == 1) {
	  if (cntx==_i_maxx) {
	    break;
	  }
	}
	else {
	  if (cntx==resposx) {
	    break;
	  }
	}

	/*next pixel (size value is inverted at each line)*/
	cntx+=dir;
	_i_i1a.u+=_i_i1as;
	_i_i1y.u+=_i_i1ys;
	_i_i1u.u+=_i_i1us;
	_i_i1v.u+=_i_i1vs;
/*	_i_i2a.u+=_i_i2as;
	_i_i2y.u+=_i_i2ys;
	_i_i2u.u+=_i_i2us;
	_i_i2v.u+=_i_i2vs;*/
	_i_ra.u+=_i_ras;
	_i_ry.u+=_i_rys;
	_i_ru.u+=_i_rus;
	_i_rv.u+=_i_rvs;
      }


      /* next row*/
      _i_i1a.u+=_i_i1als;
      _i_i1y.u+=_i_i1yls;
      _i_i1u.u+=_i_i1uls;
      _i_i1v.u+=_i_i1vls;
/*      _i_i2a.u+=_i_i2als;
      _i_i2y.u+=_i_i2yls;
      _i_i2u.u+=_i_i2uls;
      _i_i2v.u+=_i_i2vls;*/
      _i_ra.u+=_i_rals;
      _i_ry.u+=_i_ryls;
      _i_ru.u+=_i_ruls;
      _i_rv.u+=_i_rvls;

      /*invert horizontal direction*/
      dir=-dir;
      _i_i1as=-_i_i1as;
      _i_i1ys=-_i_i1ys;
      _i_i1us=-_i_i1us;
      _i_i1vs=-_i_i1vs;
/*      _i_i2as=-_i_i2as;
      _i_i2ys=-_i_i2ys;
      _i_i2us=-_i_i2us;
      _i_i2vs=-_i_i2vs;*/
      _i_ras=-_i_ras;
      _i_rys=-_i_rys;
      _i_rus=-_i_rus;
      _i_rvs=-_i_rvs;

    }
    break;
  case SCAN_VM:

    _i_i1a.u=_i_sti1a.u;
    _i_i1y.u=_i_sti1y.u;
    _i_i1u.u=_i_sti1u.u;
    _i_i1v.u=_i_sti1v.u;
/*    _i_i2a.u=_i_sti2a.u;
    _i_i2y.u=_i_sti2y.u;
    _i_i2u.u=_i_sti2u.u;
    _i_i2v.u=_i_sti2v.u;*/
    _i_ra.u=_i_stra.u;
    _i_ry.u=_i_stry.u;
    _i_ru.u=_i_stru.u;
    _i_rv.u=_i_strv.u;


    cnty=resposy;
    dir=1;

#ifdef EVAL
    eval_LogPreload();
#endif
    for (cntx=resposx; cntx<=_i_maxx; cntx++) {

      while (1) {

	if (_i_i1as) {
	  in1.a.i=_i_i1aget(_i_i1a);
	}
	if (_i_i1ys) {
	  in1.y.i=_i_i1yget(_i_i1y);
	}
	if (_i_i1us) {
	  in1.u.i=_i_i1uget(_i_i1u);
	}
	if (_i_i1vs) {
	  in1.v.i=_i_i1vget(_i_i1v);
	}

	/* transform coordinates for input2*/
	in2trans(_i_in2width,_i_in2height,cntx,cnty,&_i_in2x,&_i_in2y);
	_i_in2.m=_i2_interpolation();

#ifdef EVAL
  eval_LogOpr(cntx,cnty);
#endif
	/* execute embedded function*/
#ifdef PPTRACE
  PPTrace_LogInterIn(&in1, &_i_in2, cntx, cnty);
#endif
	operation(&res,&in1,&_i_in2);
#ifdef PPTRACE
  PPTrace_LogInterOut(res);
#endif

	if (_i_ras) {
	  _i_raset(_i_ra,res.a.i);
	}
	if (_i_rys) {
	  _i_ryset(_i_ry,res.y.i);
	}
	if (_i_rus) {
	  _i_ruset(_i_ru,res.u.i);
	}
	if (_i_rvs) {
	  _i_rvset(_i_rv,res.v.i);
	}

	/*if not end of culomn*/
	if (dir == 1) {
	  if (cnty==_i_maxy) {
	    break;
	  }
	}
	else {
	  if (cnty==resposy) {
	    break;
	  }
	}


	/*next pixel (sizevalue is invertet at each column)*/
	cnty+=dir;
	_i_i1a.u+=_i_i1als;
	_i_i1y.u+=_i_i1yls;
	_i_i1u.u+=_i_i1uls;
	_i_i1v.u+=_i_i1vls;
/*	_i_i2a.u+=_i_i2als;
	_i_i2y.u+=_i_i2yls;
	_i_i2u.u+=_i_i2uls;
	_i_i2v.u+=_i_i2vls;*/
	_i_ra.u+=_i_rals;
	_i_ry.u+=_i_ryls;
	_i_ru.u+=_i_ruls;
	_i_rv.u+=_i_rvls;

      }

      /* next row*/
      _i_i1a.u+=_i_i1as;
      _i_i1y.u+=_i_i1ys;
      _i_i1u.u+=_i_i1us;
      _i_i1v.u+=_i_i1vs;
/*      _i_i2a.u+=_i_i2as;
      _i_i2y.u+=_i_i2ys;
      _i_i2u.u+=_i_i2us;
      _i_i2v.u+=_i_i2vs;*/
      _i_ra.u+=_i_ras;
      _i_ry.u+=_i_rys;
      _i_ru.u+=_i_rus;
      _i_rv.u+=_i_rvs;

      /*invert horizontal direction*/
      dir=-dir;
      _i_i1als=-_i_i1als;
      _i_i1yls=-_i_i1yls;
      _i_i1uls=-_i_i1uls;
      _i_i1vls=-_i_i1vls;
/*      _i_i2als=-_i_i2als;
      _i_i2yls=-_i_i2yls;
      _i_i2uls=-_i_i2uls;
      _i_i2vls=-_i_i2vls;*/
      _i_rals=-_i_rals;
      _i_ryls=-_i_ryls;
      _i_ruls=-_i_ruls;
      _i_rvls=-_i_rvls;

    }
    break;
  case SCAN_LOOP:
  default :
    addlib_stop_here("Warning : scanmode not implemented\n",0,0);
    return;
  }

#ifdef PPTRACE
  PPTrace_LogInterReg(0);
#endif

#ifdef EVAL
  eval_EndMeasureDur();
  eval_EndAddr();
  eval_EndTrafo();
  eval_EndOpr();
  eval_EndConSC();
  eval_EndScan();
  eval_EndRecMode();
  eval_EndAddrMode();
#endif

}

/***************************************************************************/
/* default coordinate transformation*/
static void _t_notrans(TCoor width,TCoor height,TCoor cntx,TCoor cnty,
		       float *x,float *y)

{
  *x=(float)cntx;
  *y=(float)cnty;
}

/***************************************************************************/
static TChan _t_no_interpolation()
{
  TCoor in2x,in2y;
  TChan returnvalue;

  returnvalue=MARKER_LOADED;

  in2x=(int)(_i_in2x+0.5);
  in2y=(int)(_i_in2y+0.5);

  if (in2x<0) {
    in2x=0;
    returnvalue=MARKER_INVALID;
  }
  if (in2x>=_i_in2width) {
    in2x=_i_in2width-1;
    returnvalue=MARKER_INVALID;
  }
  if (in2y<0) {
    in2y=0;
    returnvalue=MARKER_INVALID;
  }
  if (in2y>=_i_in2height) {
    in2y=_i_in2height-1;
    returnvalue=MARKER_INVALID;
  }

  {
    unsigned long in2offset;

    in2offset=in2x+in2y*_i_in2width;

    if (_i_i2as) {
      _i_i2a.u=_i_sti2a.u+(in2offset<<_typesize[_i_i2as]);
      _i_in2.a.i=_i_i2aget(_i_i2a);
    }
    if (_i_i2ys) {
      _i_i2y.u=_i_sti2y.u+(in2offset<<_typesize[_i_i2ys]);
      _i_in2.y.i=_i_i2yget(_i_i2y);
    }
    if (_i_i2us) {
      _i_i2u.u=_i_sti2u.u+(in2offset<<_typesize[_i_i2us]);
      _i_in2.u.i=_i_i2uget(_i_i2u);
    }
    if (_i_i2vs) {
      _i_i2v.u=_i_sti2v.u+(in2offset<<_typesize[_i_i2vs]);
      _i_in2.v.i=_i_i2vget(_i_i2v);
    }
  }
  return returnvalue;
}

/***************************************************************************/
static TChan _t_int_interpolation()
{
  TChan returnvalue;

  returnvalue=MARKER_LOADED;


  if ((int)_i_in2x<0) {
    _i_in2x=0.0;
    returnvalue=MARKER_INVALID;
  }
  if ((((int)_i_in2x)+1.0)>=_i_in2width) {
    _i_in2x=((float)_i_in2width)-1.000001;
    returnvalue=MARKER_INVALID;
  }
  if ((int)_i_in2y<0) {
    _i_in2y=0.0;
    returnvalue=MARKER_INVALID;
  }
  if ((((int)_i_in2y)+1.0)>=_i_in2height) {
    _i_in2y=((float)_i_in2height)-1.00001;
    returnvalue=MARKER_INVALID;
  }

  { /* interpoalte pixel*/
    unsigned long in2offset;
    float wheight1,wheight2,wheight3,wheight4;
    float inperpolated;

    in2offset=((int)_i_in2x)+((int)_i_in2y)*_i_in2width;

    { /* compute wheights*/
      float frac1,frac2;

      frac1=_i_in2x-floor(_i_in2x);
      frac2=_i_in2y-floor(_i_in2y);
      wheight1=(1.0-frac1)*(1.0-frac2);
      wheight2=frac1*(1.0-frac2);
      wheight3=(1.0-frac1)*frac2;
      wheight4=frac1*frac2;
    }

    if (_i_i2as) {
      _i_i2a.u=_i_sti2a.u+(in2offset<<_typesize[_i_i2as]);
      inperpolated=wheight1*((float)_i_i2aget(_i_i2a));
      _i_i2a.u+=1<<_typesize[_i_i2as];
      inperpolated+=wheight2*((float)_i_i2aget(_i_i2a));
      _i_i2a.u+=(_i_in2width<<_typesize[_i_i2as]);
      inperpolated+=wheight4*((float)_i_i2aget(_i_i2a));
      _i_i2a.u-=1<<_typesize[_i_i2as];
      inperpolated+=wheight3*((float)_i_i2aget(_i_i2a));
      _i_in2.a.i=(int)(inperpolated+0.5);
    }
    if (_i_i2ys) {
      _i_i2y.u=_i_sti2y.u+(in2offset<<_typesize[_i_i2ys]);
      inperpolated=wheight1*((float)_i_i2yget(_i_i2y));
      _i_i2y.u+=1<<_typesize[_i_i2ys];
      inperpolated+=wheight2*((float)_i_i2yget(_i_i2y));
      _i_i2y.u+=(_i_in2width<<_typesize[_i_i2ys]);
      inperpolated+=wheight4*((float)_i_i2yget(_i_i2y));
      _i_i2y.u-=1<<_typesize[_i_i2ys];
      inperpolated+=wheight3*((float)_i_i2yget(_i_i2y));
      _i_in2.y.i=(int)(inperpolated+0.5);
    }
    if (_i_i2us) {
      _i_i2u.u=_i_sti2u.u+(in2offset<<_typesize[_i_i2us]);
      inperpolated=wheight1*((float)_i_i2uget(_i_i2u));
      _i_i2u.u+=1<<_typesize[_i_i2us];
      inperpolated+=wheight2*((float)_i_i2uget(_i_i2u));
      _i_i2u.u+=(_i_in2width<<_typesize[_i_i2us]);
      inperpolated+=wheight4*((float)_i_i2uget(_i_i2u));
      _i_i2u.u-=1<<_typesize[_i_i2us];
      inperpolated+=wheight3*((float)_i_i2uget(_i_i2u));
      _i_in2.u.i=(int)(inperpolated+0.5);
    }
    if (_i_i2vs) {
      _i_i2v.u=_i_sti2v.u+(in2offset<<_typesize[_i_i2vs]);
      inperpolated=wheight1*((float)_i_i2vget(_i_i2v));
      _i_i2v.u+=1<<_typesize[_i_i2vs];
      inperpolated+=wheight2*((float)_i_i2vget(_i_i2v));
      _i_i2v.u+=(_i_in2width<<_typesize[_i_i2vs]);
      inperpolated+=wheight4*((float)_i_i2vget(_i_i2v));
      _i_i2v.u-=1<<_typesize[_i_i2vs];
      inperpolated+=wheight3*((float)_i_i2vget(_i_i2v));
      _i_in2.v.i=(int)(inperpolated+0.5);
    }
  }
  return returnvalue;
}

/***************************************************************************/
static TChan _t_float_interpolation()
{
  TChan returnvalue;
  TChan readval;

  returnvalue=MARKER_LOADED;

  if ((int)_i_in2x<0) {
    _i_in2x=0.0;
    returnvalue=MARKER_INVALID;
  }
  if ((((int)_i_in2x)+1.0)>=_i_in2width) {
    _i_in2x=((float)_i_in2width)-1.000001;
    returnvalue=MARKER_INVALID;
  }
  if ((int)_i_in2y<0) {
    _i_in2y=0.0;
    returnvalue=MARKER_INVALID;
  }
  if ((((int)_i_in2y)+1.0)>=_i_in2height) {
    _i_in2y=((float)_i_in2height)-1.00001;
    returnvalue=MARKER_INVALID;
  }

  { /* interpoalte pixel*/
    unsigned long in2offset;
    float wheight1,wheight2,wheight3,wheight4;

    in2offset=((int)_i_in2x)+((int)_i_in2y)*_i_in2width;

    { /* compute wheights*/
      float frac1,frac2;

      frac1=_i_in2x-floor(_i_in2x);
      frac2=_i_in2y-floor(_i_in2y);
      wheight1=(1.0-frac1)*(1.0-frac2);
      wheight2=frac1*(1.0-frac2);
      wheight3=(1.0-frac1)*frac2;
      wheight4=frac1*frac2;
    }

    if (_i_i2as) {
      _i_i2a.u=_i_sti2a.u+(in2offset<<_typesize[_i_i2as]);
      readval=_i_i2aget(_i_i2a);
      _i_in2.a.f=wheight1*TCHAN_TO_FLOAT(readval);
      _i_i2a.u+=1<<_typesize[_i_i2as];
      readval=_i_i2aget(_i_i2a);
      _i_in2.a.f+=wheight2*TCHAN_TO_FLOAT(readval);
      _i_i2a.u+=(_i_in2width<<_typesize[_i_i2as]);
      readval=_i_i2aget(_i_i2a);
      _i_in2.a.f+=wheight4*TCHAN_TO_FLOAT(readval);
      _i_i2a.u-=1<<_typesize[_i_i2as];
      readval=_i_i2aget(_i_i2a);
      _i_in2.a.f+=wheight3*TCHAN_TO_FLOAT(readval);
    }
    if (_i_i2ys) {
      _i_i2y.u=_i_sti2y.u+(in2offset<<_typesize[_i_i2ys]);
      readval=_i_i2yget(_i_i2y);
      _i_in2.y.f=wheight1*TCHAN_TO_FLOAT(readval);
      _i_i2y.u+=1<<_typesize[_i_i2ys];
      readval=_i_i2yget(_i_i2y);
      _i_in2.y.f+=wheight2*TCHAN_TO_FLOAT(readval);
      _i_i2y.u+=(_i_in2width<<_typesize[_i_i2ys]);
      readval=_i_i2yget(_i_i2y);
      _i_in2.y.f+=wheight4*TCHAN_TO_FLOAT(readval);
      _i_i2y.u-=1<<_typesize[_i_i2ys];
      readval=_i_i2yget(_i_i2y);
      _i_in2.y.f+=wheight3*TCHAN_TO_FLOAT(readval);
    }
    if (_i_i2us) {
      _i_i2u.u=_i_sti2u.u+(in2offset<<_typesize[_i_i2us]);
      readval=_i_i2uget(_i_i2u);
      _i_in2.u.f=wheight1*TCHAN_TO_FLOAT(readval);
      _i_i2u.u+=1<<_typesize[_i_i2us];
      readval=_i_i2uget(_i_i2u);
      _i_in2.u.f+=wheight2*TCHAN_TO_FLOAT(readval);
      _i_i2u.u+=(_i_in2width<<_typesize[_i_i2us]);
      readval=_i_i2uget(_i_i2u);
      _i_in2.u.f+=wheight4*TCHAN_TO_FLOAT(readval);
      _i_i2u.u-=1<<_typesize[_i_i2us];
      readval=_i_i2uget(_i_i2u);
      _i_in2.u.f+=wheight3*TCHAN_TO_FLOAT(readval);
    }
    if (_i_i2vs) {
      _i_i2v.u=_i_sti2v.u+(in2offset<<_typesize[_i_i2vs]);
      readval=_i_i2vget(_i_i2v);
      _i_in2.v.f=wheight1*TCHAN_TO_FLOAT(readval);
      _i_i2v.u+=1<<_typesize[_i_i2vs];
      readval=_i_i2vget(_i_i2v);
      _i_in2.v.f+=wheight2*TCHAN_TO_FLOAT(readval);
      _i_i2v.u+=(_i_in2width<<_typesize[_i_i2vs]);
      readval=_i_i2vget(_i_i2v);
      _i_in2.v.f+=wheight4*TCHAN_TO_FLOAT(readval);
      _i_i2v.u-=1<<_typesize[_i_i2vs];
      readval=_i_i2vget(_i_i2v);
      _i_in2.v.f+=wheight3*TCHAN_TO_FLOAT(readval);
    }
  }
  return returnvalue;
}
