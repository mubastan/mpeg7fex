/*
 * nhood.c
 * 
 * nhood   provides the kernel for intra, fifo and lifo addrssing functions
 *         and performs the complex pixel load/store mechanisms implemented
 *         by pointer arithmetics
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
/* Filename :    $RCSfile: nhood.c,v $
   Version  :    Revision: 1.16
   Last Edit:    Date: 2003/10/23 14:31:02
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
// Applicable File Name:  nhood.c
//
*/

/******* INCLUDES ************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "nhood.h"
#include "vopio.h"


/******* DEFINES *************************************************************/

/******* MAKROS  *************************************************************/

/******* TYPES ***************************************************************/

/******* VARIABLES ***********************************************************/
TSizeConv size1to1={1,0,1,1,0,1};
TSizeConv uphv1TO2={2,0,1,2,0,1};
TSizeConv uph1TO2={2,0,1,1,0,1};
TSizeConv upv1TO2={1,0,1,2,0,1};
TSizeConv downhv2TO1={1,0,2,1,0,2};
TSizeConv downv2TO1={1,0,1,1,0,2};
TSizeConv downh2TO1={1,0,2,1,0,1};
short _typesize[5]={0,0,1,1,2};
char dirtopos[8]={11,6,7,8,13,18,17,16};

static int *_my;
static int _m5x5[5]={0,5,10,15,20};
static int _m1x25[25]={0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,
		       17,18,19,20,21,22,23,24};
static int _m25x1[2]={0,0};

/*static TNhood regs;*/

/******* FUNCTIONS ***********************************************************/

/******* CLASES **************************************************************/


/******* Code ****************************************************************/
/******* Constructor *********************************************************/
void ConstructNhood(TNhood *nhood,MomVop *result, MomVop *in,MomImage *aux,
		    MomImage *marker,int usemarker,
		    char reschanels,char inchanels,char connect,char scanmode,
		    TChan ahold,TChan yhold,TChan uhold,TChan vhold,
		    TChan axhold,TCoor winx,TCoor winy,TCoor resposx,
		    TCoor resposy,TCoor inposx,TCoor inposy,TSizeConv inconv)

{
  int i;
  MomImage *in_ch,*res_ch;
  short matrix_xcenter,matrix_ycenter;
  short maxblkdelta;

  matrix_xcenter=0;   /* avoid warnings */
  matrix_ycenter=0;

  /* set m-chanel not malloced in constructor*/
  nhood->_n_mintern=0;

  nhood->stra.u=nhood->stry.u=nhood->stru.u=nhood->strv.u=0;
  nhood->stia.u=nhood->stiy.u=nhood->stiu.u=nhood->stiv.u=
    nhood->stm.u=nhood->stax.u=0;

  nhood->fstra.u=nhood->fstry.u=nhood->fstru.u=nhood->fstrv.u=0;
  nhood->fstia.u=nhood->fstiy.u=nhood->fstiu.u=nhood->fstiv.u=
    nhood->fstm.u=nhood->fstax.u=0;

  nhood->_n_ra.u=nhood->_n_ry.u=nhood->_n_ru.u=nhood->_n_rv.u=0;
  nhood->_n_ia.u=nhood->_n_iy.u=nhood->_n_iu.u=nhood->_n_iv.u=
    nhood->_n_m.u=nhood->_n_ax.u=0;

  nhood->_n_raes=nhood->_n_ryes=nhood->_n_rues=nhood->_n_rves=0;
  nhood->_n_iaes=nhood->_n_iyes=nhood->_n_iues=nhood->_n_ives=0;

  nhood->_n_ras=nhood->_n_rys=nhood->_n_rus=nhood->_n_rvs=0;
  nhood->_n_ias=nhood->_n_iys=nhood->_n_ius=nhood->_n_ivs=
    nhood->_n_ms=nhood->_n_axs=0;

  nhood->_n_rals=nhood->_n_ryls=nhood->_n_ruls=nhood->_n_rvls=0;
  nhood->_n_ials=nhood->_n_iyls=nhood->_n_iuls=nhood->_n_ivls=
    nhood->_n_mls=nhood->_n_axls=0;

  nhood->_n_ratype=nhood->_n_rytype=nhood->_n_rutype=
    nhood->_n_rvtype=UCHAR_TYPE;
/*  nhood->_n_iatype=nhood->_n_iytype=nhood->_n_iutype=nhood->_n_ivtype=
    nhood->_n_mtype=nhood->_n_axtype=UCHAR_TYPE;*/

  nhood->width=nhood->height=nhood->_n_inwidth=nhood->_n_inheight=0;
  nhood->_n_istepx=inconv.stepx;
  nhood->_n_istepy=inconv.stepy;
  nhood->_n_irepeatx=inconv.repeatx;
  nhood->_n_irepeaty=inconv.repeaty;
  nhood->_n_irepcntx=nhood->_n_stirepcntx=inconv.strepx%inconv.repeatx;
  nhood->_n_irepcnty=nhood->_n_stirepcnty=inconv.strepy%inconv.repeaty;
  nhood->_n_firstx=inposx%inconv.stepx;
  nhood->_n_firsty=inposy%inconv.stepy;


  nhood->_n_reschanels=reschanels;
  nhood->_n_inchanels=inchanels;
  
  nhood->_n_stx=nhood->_n_sty=0;
  nhood->_n_cntx=nhood->_n_cnty=0;
  nhood->_n_icntx=nhood->_n_icnty=0;



  nhood->_n_amaska=nhood->_n_ymaska=nhood->_n_umaska=nhood->_n_vmaska=
    nhood->_n_axmaska=0;
  nhood->_n_amasko=nhood->_n_ymasko=nhood->_n_umasko=nhood->_n_vmasko=
    nhood->_n_axmasko=0;
  if (ahold == -1) nhood->_n_amaska= -1;
  else if (ahold)  nhood->_n_amasko= ahold;
  if (yhold == -1) nhood->_n_ymaska= -1;
  else if (yhold)  nhood->_n_ymasko= yhold;
  if (uhold == -1) nhood->_n_umaska= -1;
  else if (uhold)  nhood->_n_umasko= uhold;
  if (vhold == -1) nhood->_n_vmaska= -1;
  else if (vhold)  nhood->_n_vmasko= vhold;
  if (axhold == -1) nhood->_n_axmaska= -1;
  else if (axhold)  nhood->_n_axmasko= axhold;


  /* references to read-write functions*/
  nhood->_n_iaget= &getunused;
  nhood->_n_iyget= &getunused;
  nhood->_n_iuget= &getunused;
  nhood->_n_ivget= &getunused;
  nhood->_n_axget= &getunused;
/*  nhood->_n_mget = &getunused;*/
/*  nhood->_n_raget= &getunused;
  nhood->_n_ryget= &getunused;
  nhood->_n_ruget= &getunused;
  nhood->_n_rvget= &getunused;*/

/*  nhood->_n_iaset= &setunused;
  nhood->_n_iyset= &setunused;
  nhood->_n_iuset= &setunused;
  nhood->_n_ivset= &setunused;*/
  nhood->_n_axset= &setunused;
/*  nhood->_n_mset = &setunused;*/
  nhood->_n_raset= &setunused;
  nhood->_n_ryset= &setunused;
  nhood->_n_ruset= &setunused;
  nhood->_n_rvset= &setunused;


  if ((winx ==0) || (winy ==0)) {
    addlib_stop_here("WARNING : window size is zero\n",0,0);
    nhood->error=1;
    return;
  }

  /* check for right inputformat*/
  in_ch=res_ch=0;
  i=1;
  nhood->error=0;
  while (i<16) {
    if (nhood->_n_inchanels & i) {
      switch (i) {
      case A_CH:
	in_ch=in->a_chan;
	break;
      case Y_CH:
	in_ch=in->y_chan;
	break;
      case U_CH:
	in_ch=in->u_chan;
	break;
      case V_CH:
	in_ch=in->v_chan;
	break;
      default:
	addlib_stop_here("ERROR : Internal error in nhood\n",0,0);
	nhood->error=1;
	return;
      }

      if (!in_ch) {
	addlib_stop_here("WARNING : color component does not exist in nhood\n",0,0);
	nhood->_n_inchanels-=i;
      }
      else if (!(in_ch->x && in_ch->y)) {
	addlib_stop_here("WARNING : color component has 0-size in "
			 "nhood\n",0,0);
	nhood->_n_inchanels-=i;
      }
      else {
	switch (i) {
	case A_CH:
	  nhood->_n_iaes=nhood->_n_ias=datasize(in_ch);
	  nhood->_n_ials=nhood->_n_ias*in_ch->x;
/*	  nhood->_n_iatype=in_ch->type;*/
	  nhood->fstia.u=in_ch->data->u;
	  iref(in_ch->type,&nhood->_n_iaget);
	  break;
	case Y_CH:
	  nhood->_n_iyes=nhood->_n_iys=datasize(in_ch);
	  nhood->_n_iyls=nhood->_n_iys*in_ch->x;
/*	  nhood->_n_iytype=in_ch->type;*/
	  nhood->fstiy.u=in_ch->data->u;
	  iref(in_ch->type,&nhood->_n_iyget);
	  break;
	case U_CH:
	  nhood->_n_iues=nhood->_n_ius=datasize(in_ch);
	  nhood->_n_iuls=nhood->_n_ius*in_ch->x;
/*	  nhood->_n_iutype=in_ch->type;*/
	  nhood->fstiu.u=in_ch->data->u;
	  iref(in_ch->type,&nhood->_n_iuget);
	  break;
	case V_CH:
	  nhood->_n_ives=nhood->_n_ivs=datasize(in_ch);
	  nhood->_n_ivls=nhood->_n_ivs*in_ch->x;
/*	  nhood->_n_ivtype=in_ch->type;*/
	  nhood->fstiv.u=in_ch->data->u;
	  iref(in_ch->type,&nhood->_n_ivget);
	  break;
	}
	if (in_ch) {
	  if (!nhood->_n_inwidth) nhood->_n_inwidth=in_ch->x;
	  else {
	    if (nhood->_n_inwidth != in_ch->x) {
	      addlib_stop_here("WARNING: Can only process colorformat 444\n",
			       0,0);
	      nhood->error=1;
	      return;
	    }
	  }
	  if (!nhood->_n_inheight) nhood->_n_inheight=in_ch->y;
	  else {
	    if (nhood->_n_inheight != in_ch->y) {
	      addlib_stop_here("WARNING: Can only process colorformat 444\n",
			       0,0);
	      nhood->error=1;
	      return;
	    }
	  }
	}
      }
    }


    if (nhood->_n_reschanels & i) {
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
	addlib_stop_here("ERROR : Internal error in nhood\n",0,0);
	nhood->error=1;
	return;
      }

      if (!res_ch) {
	addlib_stop_here("WARNING : color component does not exist in "
		"nhood\n",0,0);
	nhood->_n_reschanels-=i;
      }
      else  if (!(res_ch->x && res_ch->y)) {
	addlib_stop_here("WARNING : color component has 0-size in "
		"nhood\n",0,0);
	nhood->_n_reschanels-=i;
      }
      else {
	switch (i) {
	case A_CH:
	  nhood->_n_raes=nhood->_n_ras=datasize(res_ch);
	  nhood->_n_rals=nhood->_n_ras*res_ch->x;
	  nhood->_n_ratype=res_ch->type;
	  nhood->fstra.u=res_ch->data->u;
	  oref(res_ch->type,&nhood->_n_raset);
	  break;
	case Y_CH:
	  nhood->_n_ryes=nhood->_n_rys=datasize(res_ch);
	  nhood->_n_ryls=nhood->_n_rys*res_ch->x;
	  nhood->_n_rytype=res_ch->type;
	  nhood->fstry.u=res_ch->data->u;
	  oref(res_ch->type,&nhood->_n_ryset);
	  break;
	case U_CH:
	  nhood->_n_rues=nhood->_n_rus=datasize(res_ch);
	  nhood->_n_ruls=nhood->_n_rus*res_ch->x;
	  nhood->_n_rutype=res_ch->type;
	  nhood->fstru.u=res_ch->data->u;
	  oref(res_ch->type,&nhood->_n_ruset);
	  break;
	case V_CH:
	  nhood->_n_rves=nhood->_n_rvs=datasize(res_ch);
	  nhood->_n_rvls=nhood->_n_rvs*res_ch->x;
	  nhood->_n_rvtype=res_ch->type;
	  nhood->fstrv.u=res_ch->data->u;
	  oref(res_ch->type,&nhood->_n_rvset);
	  break;
	}

	if (res_ch) {
	  if (!nhood->width) nhood->width=res_ch->x;
	  else {
	    if (nhood->width != res_ch->x) {
	      addlib_stop_here("WARNING: Can only process colorformat 444\n",
			       0,0);
	      nhood->error=1;
	      return;
	    }
	  }
	  if (!nhood->height) nhood->height=res_ch->y;
	  else {
	    if (nhood->height != res_ch->y) {
	      addlib_stop_here("WARNING: Can only process colorformat 444\n",
			       0,0);
	      nhood->error=1;
	      return;
	    }
	  }
	}
      }
    }
  
    /* next colorcomponent to check*/
    i<<=1;
  }

  /* correct 0 repeat and step values*/
  if (!nhood->_n_istepx) nhood->_n_istepx=1;
  if (!nhood->_n_irepeatx) nhood->_n_irepeatx=1;
  if (!nhood->_n_istepy) nhood->_n_istepy=1;
  if (!nhood->_n_irepeaty) nhood->_n_irepeaty=1;

  /* automatic set of unset frame and window sizes*/
  /*if only aux channel is given*/
  if (((nhood->_n_reschanels | nhood->_n_inchanels) == AX_CH) && aux) {
    nhood->width=nhood->_n_inwidth=aux->x;
    nhood->height=nhood->_n_inheight=aux->y;
  }
  if (winx == -1) {
    if (!nhood->width) {
      nhood->width=(nhood->_n_inwidth/nhood->_n_istepx)*nhood->_n_irepeatx - 
	nhood->_n_stirepcntx;
    }
    winx=nhood->width;
    resposx=0;
  }
  else {
    if (!nhood->width) {
      nhood->width=(nhood->_n_inwidth/nhood->_n_istepx)*nhood->_n_irepeatx - 
	nhood->_n_stirepcntx+resposx;
    }
  }

  if (winy == -1) {
    if (!nhood->height) {
      nhood->height=(nhood->_n_inheight/nhood->_n_istepy)*nhood->_n_irepeaty - 
	nhood->_n_stirepcnty;
    }
    winy=nhood->height;
    resposy=0;
  }
  else {
    if (!nhood->height) {
      nhood->height=(nhood->_n_inheight/nhood->_n_istepy)*nhood->_n_irepeaty - 
	nhood->_n_stirepcnty+resposy;
    }
  }

  if (!nhood->_n_inwidth) {
    nhood->_n_inwidth=((nhood->width+nhood->_n_stirepcntx)/
		       nhood->_n_irepeatx) * nhood->_n_istepx +inposx;
  }
  if (!nhood->_n_inheight) {
    nhood->_n_inheight=((nhood->height+nhood->_n_stirepcnty)/
			nhood->_n_irepeaty) * nhood->_n_istepy + inposy;
  }


  

  /* prepare aux-chanel*/
  if ((nhood->_n_reschanels | nhood->_n_inchanels) & AX_CH) {
    /* if aux image is given*/
    if (aux && aux->data) {

      /* if 1to1 size conversion*/
      if ((nhood->_n_irepeatx==1) && (nhood->_n_irepeaty==1) &&
	  (nhood->_n_istepx==1) && (nhood->_n_istepy==1) &&
	  (resposx == inposx) && (resposy == inposy)) {

	/* if processed window is in aux*/
	if ((aux->x >= (winx + resposx)) && 
	    (aux->y >= (winy + resposy))) {
	  nhood->_n_axs=datasize(aux);
	  nhood->_n_axls=nhood->_n_axs*aux->x;
	  nhood->_n_axtype=aux->type;
	  nhood->fstax.u=aux->data->u;
	  if (nhood->_n_inchanels & AX_CH) iref(aux->type,&nhood->_n_axget);
	  if (nhood->_n_reschanels & AX_CH) oref(aux->type,&nhood->_n_axset);
	}
	else {
	  addlib_stop_here("WARNING: Incorrect Aux field in nhood\n",0,0);
	  nhood->_n_reschanels&=~AX_CH;
	  nhood->_n_inchanels&=~AX_CH;
	}
      }
      else {
	addlib_stop_here("WARNING: Aux-Chanel is invalid with size conversion "
		"or window offset\n",0,0);
	nhood->_n_reschanels&=~AX_CH;
	nhood->_n_inchanels&=~AX_CH;
      }
    }
    else {
      addlib_stop_here("WARNING : color component aux does not exist in "
		"nhood\n",0,0);
      nhood->_n_reschanels&=~AX_CH;
      nhood->_n_inchanels&=~AX_CH;
    }
  }

  /* check size*/
  if (!nhood->width || !nhood->_n_inwidth || !nhood->height ||
      !nhood->_n_inheight) {
    nhood->error=1;
  }

  /* check channels and ranges*/
  if (nhood->error) {
    addlib_stop_here("WARNING : Images size does not match in nhood\n",0,0);
    nhood->error=1;
    return;
  }

  /* prepare marker*/
  if (usemarker) {
    if (marker) {
      if(!(marker->data && marker->data->u && (marker->x == nhood->width) &&
	 (marker->y == nhood->height))) {
	addlib_stop_here("WARNING: Incorrect marker filed in connected "
			 "nhood\n",0,0);
	nhood->error=1;
	return;
      }
      nhood->_n_marker=marker;
    }
    else {
      nhood->_n_marker=initimg(nhood->width,nhood->height,UCHAR_TYPE);
      if (!nhood->_n_marker) {
	addlib_stop_here("WARNING: Out of memory, Skipping nhood\n",0,0);
	nhood->error=1;
	return;
      }
      nhood->_n_mintern=1;
      memset(nhood->_n_marker->data->u,0,
	     nhood->_n_marker->x * nhood->_n_marker->y *
	     datasize(nhood->_n_marker));
    }
    nhood->_n_ms=datasize(nhood->_n_marker);
    nhood->_n_mls=nhood->_n_ms*nhood->width;
    nhood->_n_mtype=nhood->_n_marker->type;
    nhood->fstm.u=nhood->_n_marker->data->u;
    iorefs(nhood->_n_marker->type,&nhood->_n_mget,&nhood->_n_mset);
    /* initialize markerfield*/
    for (i=0; i<25; nhood->matrix[i++].m=0); 
  }
  else {
    nhood->_n_ms=0;
    nhood->_n_mls=0;
    nhood->_n_mtype=UCHAR_TYPE;
    nhood->fstm.u=0;
    nhood->_n_mget = &getunused;
    nhood->_n_mset = &setunused;
  }


  /* check if windowoperation*/
  if (resposx+winx > nhood->width) {
    addlib_stop_here("Error: Window out of range in nhood (method skipped)\n",
		     0,0);
    nhood->error=1;
    return;
  }
  if (((inposx+(winx+nhood->_n_stirepcntx)/nhood->_n_irepeatx)*
       nhood->_n_istepx) > nhood->_n_inwidth) {
    addlib_stop_here("Error: Window out of range in nhood (method skipped)\n",
		     0,0);
    nhood->error=1;
    return;
  }
  if (nhood->fstia.u) nhood->stia.u=nhood->fstia.u+inposx*nhood->_n_iaes;
  if (nhood->fstiy.u) nhood->stiy.u=nhood->fstiy.u+inposx*nhood->_n_iyes;
  if (nhood->fstiu.u) nhood->stiu.u=nhood->fstiu.u+inposx*nhood->_n_iues;
  if (nhood->fstiv.u) nhood->stiv.u=nhood->fstiv.u+inposx*nhood->_n_ives;
  if (nhood->fstax.u) nhood->stax.u=nhood->fstax.u+resposx*nhood->_n_axs;
  if (nhood->fstm.u)  nhood->stm.u =nhood->fstm.u +resposx*nhood->_n_ms;
  if (nhood->fstra.u) nhood->stra.u=nhood->fstra.u+resposx*nhood->_n_raes;
  if (nhood->fstry.u) nhood->stry.u=nhood->fstry.u+resposx*nhood->_n_ryes;
  if (nhood->fstru.u) nhood->stru.u=nhood->fstru.u+resposx*nhood->_n_rues;
  if (nhood->fstrv.u) nhood->strv.u=nhood->fstrv.u+resposx*nhood->_n_rves;
  nhood->_n_maxx=resposx+winx-1;
  nhood->_n_icntx=inposx;
  nhood->_n_stx=nhood->_n_cntx=resposx;

  if (resposy+winy > nhood->height) {
    addlib_stop_here("Error: Window out of range in nhood (method skipped)\n",
		     0,0);
    nhood->error=1;
    return;
  }
  if (((inposy+(winy+nhood->_n_stirepcnty)/nhood->_n_irepeaty)*
       nhood->_n_istepy) > nhood->_n_inheight) {
    addlib_stop_here("Error: Window out of range in nhood (method skipped)\n",
		     0,0);
    nhood->error=1;
    return;
  }
  if (nhood->stia.u) nhood->stia.u+=inposy*nhood->_n_ials;
  if (nhood->stiy.u) nhood->stiy.u+=inposy*nhood->_n_iyls;
  if (nhood->stiu.u) nhood->stiu.u+=inposy*nhood->_n_iuls;
  if (nhood->stiv.u) nhood->stiv.u+=inposy*nhood->_n_ivls;
  if (nhood->stax.u) nhood->stax.u+=resposy*nhood->_n_axls;
  if (nhood->stm.u)  nhood->stm.u +=resposy*nhood->_n_mls;
  if (nhood->stra.u) nhood->stra.u+=resposy*nhood->_n_rals;
  if (nhood->stry.u) nhood->stry.u+=resposy*nhood->_n_ryls;
  if (nhood->stru.u) nhood->stru.u+=resposy*nhood->_n_ruls;
  if (nhood->strv.u) nhood->strv.u+=resposy*nhood->_n_rvls;
  nhood->_n_maxy=resposy+winy-1;
  nhood->_n_icnty=inposy;
  nhood->_n_sty=nhood->_n_cnty=resposy;

  nhood->_n_ia.u= nhood->stia.u;
  nhood->_n_iy.u= nhood->stiy.u;
  nhood->_n_iu.u= nhood->stiu.u;
  nhood->_n_iv.u= nhood->stiv.u;
  nhood->_n_ax.u= nhood->stax.u;
  nhood->_n_m.u= nhood->stm.u;
  nhood->_n_ra.u= nhood->stra.u;
  nhood->_n_ry.u= nhood->stry.u;
  nhood->_n_ru.u= nhood->stru.u;
  nhood->_n_rv.u= nhood->strv.u;

  if (nhood->_n_istepx != 1) {
    nhood->_n_ias*=nhood->_n_istepx;
    nhood->_n_iys*=nhood->_n_istepx;
    nhood->_n_ius*=nhood->_n_istepx;
    nhood->_n_ivs*=nhood->_n_istepx;
  }
  if (nhood->_n_istepy != 1) {
    nhood->_n_ials*=nhood->_n_istepy;
    nhood->_n_iyls*=nhood->_n_istepy;
    nhood->_n_iuls*=nhood->_n_istepy;
    nhood->_n_ivls*=nhood->_n_istepy;
  }


  /*determin blocksize*/
  nhood->_n_scanmode =scanmode;
  switch (connect) {
  case CON_0 : /* center only*/
    nhood->_n_xblksize=nhood->_n_yblksize=0;
    matrix_xcenter=2;
    matrix_ycenter=2;
    _my=_m5x5;
    break;
  case CON_4 : /* center + 4 neighbours*/
  case CON_8 : /* center + 8 neighbours*/
    nhood->_n_xblksize=nhood->_n_yblksize=1;
    matrix_xcenter=2;
    matrix_ycenter=2;
    _my=_m5x5;
    break;
  case CON_12 : /*center + 12 nearest pixels*/
  case CON_24 : /*center + 24 nearest pixels*/
    nhood->_n_xblksize=nhood->_n_yblksize=1;
    matrix_xcenter=2;
    matrix_ycenter=2;
    _my=_m5x5;
    break;
  case CON_H2 : /*center + horizontal neighbours*/
    nhood->_n_xblksize=1;
    nhood->_n_yblksize=0;
    if (nhood->_n_scanmode == SCAN_VE) nhood->_n_scanmode=SCAN_HO;
    if (nhood->_n_scanmode == SCAN_VM) nhood->_n_scanmode=SCAN_HM;
    if (nhood->_n_scanmode == SCAN_LOOP) nhood->_n_scanmode=SCAN_HM;
    matrix_xcenter=2;
    matrix_ycenter=2;
    _my=_m5x5;
    break;
  case CON_H4 : /*center + 2 horizontal neighbours*/
    nhood->_n_xblksize=2;
    nhood->_n_yblksize=0;
    if (nhood->_n_scanmode == SCAN_VE) nhood->_n_scanmode=SCAN_HO;
    if (nhood->_n_scanmode == SCAN_VM) nhood->_n_scanmode=SCAN_HM;
    if (nhood->_n_scanmode == SCAN_LOOP) nhood->_n_scanmode=SCAN_HM;
    matrix_xcenter=2;
    matrix_ycenter=2;
    _my=_m5x5;
    break;
  case CON_V2 : /*center + vertical neighbours*/
    nhood->_n_xblksize=0;
    nhood->_n_yblksize=1;
    if (nhood->_n_scanmode == SCAN_HO) nhood->_n_scanmode=SCAN_VE;
    if (nhood->_n_scanmode == SCAN_HM) nhood->_n_scanmode=SCAN_VM;
    if (nhood->_n_scanmode == SCAN_LOOP) nhood->_n_scanmode=SCAN_VM;
    matrix_xcenter=2;
    matrix_ycenter=2;
    _my=_m5x5;
    break;
  case  CON_V4 : /*center + 2 vertical neighbours*/
    nhood->_n_xblksize=0;
    nhood->_n_yblksize=2;
    if (nhood->_n_scanmode == SCAN_HO) nhood->_n_scanmode=SCAN_VE;
    if (nhood->_n_scanmode == SCAN_HM) nhood->_n_scanmode=SCAN_VM;
    if (nhood->_n_scanmode == SCAN_LOOP) nhood->_n_scanmode=SCAN_VM;
    matrix_xcenter=2;
    matrix_ycenter=2;
    _my=_m5x5;
    break;
  case CON_H6 : /*center + 3 horizontal neighbours*/
    if ((nhood->_n_irepeatx | nhood->_n_irepeaty | 
	 nhood->_n_istepx | nhood->_n_istepy) > 1) {
      addlib_stop_here("Error CON_H6 not allowd with scaling\n",0,0);
      return;
    }
    nhood->_n_xblksize=3;
    nhood->_n_yblksize=0;
    if (nhood->_n_scanmode == SCAN_VE) nhood->_n_scanmode=SCAN_HO;
    if (nhood->_n_scanmode == SCAN_VM) nhood->_n_scanmode=SCAN_HM;
    if (nhood->_n_scanmode == SCAN_LOOP) nhood->_n_scanmode=SCAN_HM;
    matrix_xcenter=12;
    matrix_ycenter=0;
    _my=_m25x1;
    break;
  case CON_H8 : /*center + 4 horizontal neighbours*/
    if ((nhood->_n_irepeatx | nhood->_n_irepeaty | 
	 nhood->_n_istepx | nhood->_n_istepy) > 1) {
      addlib_stop_here("Error CON_H8 not allowd with scaling\n",0,0);
      return;
    }
    nhood->_n_xblksize=4;
    nhood->_n_yblksize=0;
    if (nhood->_n_scanmode == SCAN_VE) nhood->_n_scanmode=SCAN_HO;
    if (nhood->_n_scanmode == SCAN_VM) nhood->_n_scanmode=SCAN_HM;
    if (nhood->_n_scanmode == SCAN_LOOP) nhood->_n_scanmode=SCAN_HM;
    matrix_xcenter=12;
    matrix_ycenter=0;
    _my=_m25x1;
    break;
  case CON_V6 : /*center + 3 vertical neighbours*/
    if ((nhood->_n_irepeatx | nhood->_n_irepeaty | 
	 nhood->_n_istepx | nhood->_n_istepy) > 1) {
      addlib_stop_here("Error CON_V6 not allowd with scaling\n",0,0);
      return;
    }
    nhood->_n_xblksize=3;
    nhood->_n_yblksize=0;
    if (nhood->_n_scanmode == SCAN_HO) nhood->_n_scanmode=SCAN_VE;
    if (nhood->_n_scanmode == SCAN_HM) nhood->_n_scanmode=SCAN_VM;
    if (nhood->_n_scanmode == SCAN_LOOP) nhood->_n_scanmode=SCAN_VM;
    matrix_xcenter=0;
    matrix_ycenter=12;
    _my=_m1x25;
    break;
  case CON_V8 : /*center + 4 vertical neighbours*/
    if ((nhood->_n_irepeatx | nhood->_n_irepeaty | 
	 nhood->_n_istepx | nhood->_n_istepy) > 1) {
      addlib_stop_here("Error CON_V8 not allowd with scaling\n",0,0);
      return;
    }
    nhood->_n_xblksize=4;
    nhood->_n_yblksize=0;
    if (nhood->_n_scanmode == SCAN_HO) nhood->_n_scanmode=SCAN_VE;
    if (nhood->_n_scanmode == SCAN_HM) nhood->_n_scanmode=SCAN_VM;
    if (nhood->_n_scanmode == SCAN_LOOP) nhood->_n_scanmode=SCAN_VM;
    matrix_xcenter=0;
    matrix_ycenter=12;
    _my=_m1x25;
    break;
  default :
    addlib_stop_here("WARNING: unsupported conectaivity\n",0,0);
    nhood->_n_xblksize=nhood->_n_yblksize=0;
  }
  nhood->_n_xblkstart=matrix_xcenter - nhood->_n_xblksize;
  nhood->_n_yblkstart=matrix_ycenter - nhood->_n_yblksize;
  nhood->_n_xblkstop=matrix_xcenter + nhood->_n_xblksize;
  nhood->_n_yblkstop=matrix_ycenter + nhood->_n_yblksize;
  maxblkdelta=nhood->_n_xblksize<<1;
  if (nhood->_n_irepeatx > maxblkdelta) nhood->_n_xblkdelta= maxblkdelta;
  else nhood->_n_xblkdelta= nhood->_n_irepeatx -1;
  maxblkdelta=nhood->_n_yblksize<<1;
  if (nhood->_n_irepeaty > maxblkdelta) nhood->_n_yblkdelta= maxblkdelta;
  else nhood->_n_yblkdelta= nhood->_n_irepeaty -1;

  /* reset matrix*/
  for (i=0; i<25; i++) {
    nhood->matrix[i].a.i=0;
    nhood->matrix[i].y.i=0;
    nhood->matrix[i].u.i=0;
    nhood->matrix[i].v.i=0;
    nhood->matrix[i].ax.i=0;
    nhood->matrix[i].m=0;
  }
}

/******* Destructor **********************************************************/
void DestructNhood(TNhood *nhood)

{
  if (nhood->_n_mintern) freeimg(nhood->_n_marker);
}
/******* Check for Error *****************************************************/
int CheckError(TNhood *nhood)

{
  return nhood->error;
}


/*****************************************************************************/
/******* FIFO_AREA ***********************************************************/
/*****************************************************************************/

/******* Goto Position *******************************************************/
void Goto(TNhood *nhood,TCoor x,TCoor y)

{
  long offset;

  offset=x+y*nhood->width;
  if (nhood->fstia.u) nhood->_n_ia.u=nhood->fstia.u +
		       (offset<<_typesize[nhood->_n_iaes]);
  if (nhood->fstiy.u) nhood->_n_iy.u=nhood->fstiy.u + 
		       (offset<<_typesize[nhood->_n_iyes]);
  if (nhood->fstiu.u) nhood->_n_iu.u=nhood->fstiu.u + 
		       (offset<<_typesize[nhood->_n_iues]);
  if (nhood->fstiv.u) nhood->_n_iv.u=nhood->fstiv.u + 
		       (offset<<_typesize[nhood->_n_ives]);
  if (nhood->fstm.u) nhood->_n_m.u=nhood->fstm.u + 
		      (offset<<_typesize[nhood->_n_ms]);
  if (nhood->fstax.u) nhood->_n_ax.u=nhood->fstax.u + 
		       (offset<<_typesize[nhood->_n_axs]);
  if (nhood->fstra.u) nhood->_n_ra.u=nhood->fstra.u + 
		       (offset<<_typesize[nhood->_n_raes]);
  if (nhood->fstry.u) nhood->_n_ry.u=nhood->fstry.u + 
		       (offset<<_typesize[nhood->_n_ryes]);
  if (nhood->fstru.u) nhood->_n_ru.u=nhood->fstru.u +
		       (offset<<_typesize[nhood->_n_rues]);
  if (nhood->fstrv.u) nhood->_n_rv.u=nhood->fstrv.u +
		       (offset<<_typesize[nhood->_n_rves]);
  nhood->_n_icntx=nhood->_n_cntx=x;
  nhood->_n_icnty=nhood->_n_cnty=y;
}


/******* Preload Matrix for Fifo *********************************************/
void Preload4(TNhood *nhood,TCoor x,TCoor y)

{
  MomImageData ia,iy,iu,iv,m,ax;
  long offset;

  offset=x+y*nhood->width;
  if (nhood->fstia.u) nhood->_n_ia.u=nhood->fstia.u +
		       (offset<<_typesize[nhood->_n_iaes]);
  if (nhood->fstiy.u) nhood->_n_iy.u=nhood->fstiy.u + 
		       (offset<<_typesize[nhood->_n_iyes]);
  if (nhood->fstiu.u) nhood->_n_iu.u=nhood->fstiu.u + 
		       (offset<<_typesize[nhood->_n_iues]);
  if (nhood->fstiv.u) nhood->_n_iv.u=nhood->fstiv.u + 
		       (offset<<_typesize[nhood->_n_ives]);
  if (nhood->fstm.u) nhood->_n_m.u=nhood->fstm.u + 
		      (offset<<_typesize[nhood->_n_ms]);
  if (nhood->fstax.u) nhood->_n_ax.u=nhood->fstax.u + 
		       (offset<<_typesize[nhood->_n_axs]);
  if (nhood->fstra.u) nhood->_n_ra.u=nhood->fstra.u + 
		       (offset<<_typesize[nhood->_n_raes]);
  if (nhood->fstry.u) nhood->_n_ry.u=nhood->fstry.u + 
		       (offset<<_typesize[nhood->_n_ryes]);
  if (nhood->fstru.u) nhood->_n_ru.u=nhood->fstru.u +
		       (offset<<_typesize[nhood->_n_rues]);
  if (nhood->fstrv.u) nhood->_n_rv.u=nhood->fstrv.u +
		       (offset<<_typesize[nhood->_n_rves]);
  nhood->_n_icntx=nhood->_n_cntx=x;
  nhood->_n_icnty=nhood->_n_cnty=y;


  /* load centerposition*/
  if (nhood->_n_iaes) nhood->matrix[12].a.i=nhood->_n_iaget(nhood->_n_ia);
  if (nhood->_n_iyes) nhood->matrix[12].y.i=nhood->_n_iyget(nhood->_n_iy);
  if (nhood->_n_iues) nhood->matrix[12].u.i=nhood->_n_iuget(nhood->_n_iu);
  if (nhood->_n_ives) nhood->matrix[12].v.i=nhood->_n_ivget(nhood->_n_iv);
  if (nhood->_n_axs) nhood->matrix[12].ax.i=nhood->_n_axget(nhood->_n_ax);
  nhood->matrix[12].m=nhood->_n_mget(nhood->_n_m);

  if (!nhood->matrix[12].m) nhood->matrix[12].m=1;

  /* load neighbours*/
  /* up*/
  if (nhood->_n_cnty > 0) {
    m.u=nhood->_n_m.u-nhood->_n_mls;
    nhood->matrix[7].m=nhood->_n_mget(m);
    if (nhood->matrix[7].m != MARKER_QUEUED) {
      if (!nhood->matrix[7].m) nhood->matrix[7].m=1;

      /* if not queued load all chanels*/
      if (nhood->_n_iaes) {
	ia.u=nhood->_n_ia.u-nhood->_n_ials;
	nhood->matrix[7].a.i=nhood->_n_iaget(ia);
      }
      if (nhood->_n_iyes) {
	iy.u=nhood->_n_iy.u-nhood->_n_iyls;
	nhood->matrix[7].y.i=nhood->_n_iyget(iy);
      }
      if (nhood->_n_iues) {
	iu.u=nhood->_n_iu.u-nhood->_n_iuls;
	nhood->matrix[7].u.i=nhood->_n_iuget(iu);
      }
      if (nhood->_n_ives) {
	iv.u=nhood->_n_iv.u-nhood->_n_ivls;
	nhood->matrix[7].v.i=nhood->_n_ivget(iv);
      }
      if (nhood->_n_axs) {
	ax.u=nhood->_n_ax.u-nhood->_n_axls;
	nhood->matrix[7].ax.i=nhood->_n_axget(ax);
      }
    }
  }
  else {
    nhood->matrix[7].m=0;
  }

  /* down*/
  if (nhood->_n_cnty < (nhood->height-1)) {
    m.u=nhood->_n_m.u+nhood->_n_mls;
    nhood->matrix[17].m=nhood->_n_mget(m);
    if (nhood->matrix[17].m != MARKER_QUEUED) {
      if (!nhood->matrix[17].m) nhood->matrix[17].m=1;

      /* if not queued load all chanels*/
      if (nhood->_n_iaes) {
	ia.u=nhood->_n_ia.u+nhood->_n_ials;
	nhood->matrix[17].a.i=nhood->_n_iaget(ia);
      }
      if (nhood->_n_iyes) {
	iy.u=nhood->_n_iy.u+nhood->_n_iyls;
	nhood->matrix[17].y.i=nhood->_n_iyget(iy);
      }
      if (nhood->_n_iues) {
	iu.u=nhood->_n_iu.u+nhood->_n_iuls;
	nhood->matrix[17].u.i=nhood->_n_iuget(iu);
      }
      if (nhood->_n_ives) {
	iv.u=nhood->_n_iv.u+nhood->_n_ivls;
	nhood->matrix[17].v.i=nhood->_n_ivget(iv);
      }
      if (nhood->_n_axs) {
	ax.u=nhood->_n_ax.u+nhood->_n_axls;
	nhood->matrix[17].ax.i=nhood->_n_axget(ax);
      }
    }
  }
  else {
    nhood->matrix[17].m=0;
  }
    
  /* left*/
  if (nhood->_n_cntx > 0) {
    m.u=nhood->_n_m.u-nhood->_n_ms;
    nhood->matrix[11].m=nhood->_n_mget(m);
    if (nhood->matrix[11].m != MARKER_QUEUED) {
      if (!nhood->matrix[11].m) nhood->matrix[11].m=1;

      /* if not queued load all chanels*/
      if (nhood->_n_iaes) {
	ia.u=nhood->_n_ia.u-nhood->_n_ias;
	nhood->matrix[11].a.i=nhood->_n_iaget(ia);
      }
      if (nhood->_n_iyes) {
	iy.u=nhood->_n_iy.u-nhood->_n_iys;
	nhood->matrix[11].y.i=nhood->_n_iyget(iy);
      }
      if (nhood->_n_iues) {
	iu.u=nhood->_n_iu.u-nhood->_n_ius;
	nhood->matrix[11].u.i=nhood->_n_iuget(iu);
      }
      if (nhood->_n_ives) {
	iv.u=nhood->_n_iv.u-nhood->_n_ivs;
	nhood->matrix[11].v.i=nhood->_n_ivget(iv);
      }
      if (nhood->_n_axs) {
	ax.u=nhood->_n_ax.u-nhood->_n_axs;
	nhood->matrix[11].ax.i=nhood->_n_axget(ax);
      }
    }
  }
  else {
    nhood->matrix[11].m=0;
  }
    
  /* right*/
  if (nhood->_n_cntx < (nhood->width-1)) {
    m.u=nhood->_n_m.u+nhood->_n_ms;
    nhood->matrix[13].m=nhood->_n_mget(m);
    if (nhood->matrix[13].m != MARKER_QUEUED) {
      if (!nhood->matrix[13].m) nhood->matrix[13].m=1;

      /* if not queued load all chanels*/
      if (nhood->_n_iaes) {
	ia.u=nhood->_n_ia.u+nhood->_n_ias;
	nhood->matrix[13].a.i=nhood->_n_iaget(ia);
      }
      if (nhood->_n_iyes) {
	iy.u=nhood->_n_iy.u+nhood->_n_iys;
	nhood->matrix[13].y.i=nhood->_n_iyget(iy);
      }
      if (nhood->_n_iues) {
	iu.u=nhood->_n_iu.u+nhood->_n_ius;
	nhood->matrix[13].u.i=nhood->_n_iuget(iu);
      }
      if (nhood->_n_ives) {
	iv.u=nhood->_n_iv.u+nhood->_n_ivs;
	nhood->matrix[13].v.i=nhood->_n_ivget(iv);
      }
      if (nhood->_n_axs) {
	ax.u=nhood->_n_ax.u+nhood->_n_axs;
	nhood->matrix[13].ax.i=nhood->_n_axget(ax);
      }
    }
  }
  else {
    nhood->matrix[13].m=0;
  }
}

/******* Preload Matrix for Fifo *********************************************/
void Preload8(TNhood *nhood,TCoor x,TCoor y)

{
  MomImageData ia,iy,iu,iv,m,ax;
  long offset;

  offset=x+y*nhood->width;
  if (nhood->fstia.u) nhood->_n_ia.u=nhood->fstia.u +
		       (offset<<_typesize[nhood->_n_iaes]);
  if (nhood->fstiy.u) nhood->_n_iy.u=nhood->fstiy.u + 
		       (offset<<_typesize[nhood->_n_iyes]);
  if (nhood->fstiu.u) nhood->_n_iu.u=nhood->fstiu.u + 
		       (offset<<_typesize[nhood->_n_iues]);
  if (nhood->fstiv.u) nhood->_n_iv.u=nhood->fstiv.u + 
		       (offset<<_typesize[nhood->_n_ives]);
  if (nhood->fstm.u) nhood->_n_m.u=nhood->fstm.u + 
		      (offset<<_typesize[nhood->_n_ms]);
  if (nhood->fstax.u) nhood->_n_ax.u=nhood->fstax.u + 
		       (offset<<_typesize[nhood->_n_axs]);
  if (nhood->fstra.u) nhood->_n_ra.u=nhood->fstra.u + 
		       (offset<<_typesize[nhood->_n_raes]);
  if (nhood->fstry.u) nhood->_n_ry.u=nhood->fstry.u + 
		       (offset<<_typesize[nhood->_n_ryes]);
  if (nhood->fstru.u) nhood->_n_ru.u=nhood->fstru.u +
		       (offset<<_typesize[nhood->_n_rues]);
  if (nhood->fstrv.u) nhood->_n_rv.u=nhood->fstrv.u +
		       (offset<<_typesize[nhood->_n_rves]);
  nhood->_n_icntx=nhood->_n_cntx=x;
  nhood->_n_icnty=nhood->_n_cnty=y;


  /* load centerposition*/
  if (nhood->_n_iaes) nhood->matrix[12].a.i=nhood->_n_iaget(nhood->_n_ia);
  if (nhood->_n_iyes) nhood->matrix[12].y.i=nhood->_n_iyget(nhood->_n_iy);
  if (nhood->_n_iues) nhood->matrix[12].u.i=nhood->_n_iuget(nhood->_n_iu);
  if (nhood->_n_ives) nhood->matrix[12].v.i=nhood->_n_ivget(nhood->_n_iv);
  if (nhood->_n_axs) nhood->matrix[12].ax.i=nhood->_n_axget(nhood->_n_ax);
  nhood->matrix[12].m=nhood->_n_mget(nhood->_n_m);

  if (!nhood->matrix[12].m) nhood->matrix[12].m=1;

  /* load neighbours*/
  /* up*/
  if (nhood->_n_cnty > 0) {
    m.u=nhood->_n_m.u-nhood->_n_mls;
    nhood->matrix[7].m=nhood->_n_mget(m);
    if (nhood->matrix[7].m != MARKER_QUEUED) {
      if (!nhood->matrix[7].m) nhood->matrix[7].m=1;

      /* if not queued load all chanels*/
      if (nhood->_n_iaes) {
	ia.u=nhood->_n_ia.u-nhood->_n_ials;
	nhood->matrix[7].a.i=nhood->_n_iaget(ia);
      }
      if (nhood->_n_iyes) {
	iy.u=nhood->_n_iy.u-nhood->_n_iyls;
	nhood->matrix[7].y.i=nhood->_n_iyget(iy);
      }
      if (nhood->_n_iues) {
	iu.u=nhood->_n_iu.u-nhood->_n_iuls;
	nhood->matrix[7].u.i=nhood->_n_iuget(iu);
      }
      if (nhood->_n_ives) {
	iv.u=nhood->_n_iv.u-nhood->_n_ivls;
	nhood->matrix[7].v.i=nhood->_n_ivget(iv);
      }
      if (nhood->_n_axs) {
	ax.u=nhood->_n_ax.u-nhood->_n_axls;
	nhood->matrix[7].ax.i=nhood->_n_axget(ax);
      }
    }
  }
  else {
    nhood->matrix[7].m=0;
  }

  /* down*/
  if (nhood->_n_cnty < (nhood->height-1)) {
    m.u=nhood->_n_m.u+nhood->_n_mls;
    nhood->matrix[17].m=nhood->_n_mget(m);
    if (nhood->matrix[17].m != MARKER_QUEUED) {
      if (!nhood->matrix[17].m) nhood->matrix[17].m=1;

      /* if not queued load all chanels*/
      if (nhood->_n_iaes) {
	ia.u=nhood->_n_ia.u+nhood->_n_ials;
	nhood->matrix[17].a.i=nhood->_n_iaget(ia);
      }
      if (nhood->_n_iyes) {
	iy.u=nhood->_n_iy.u+nhood->_n_iyls;
	nhood->matrix[17].y.i=nhood->_n_iyget(iy);
      }
      if (nhood->_n_iues) {
	iu.u=nhood->_n_iu.u+nhood->_n_iuls;
	nhood->matrix[17].u.i=nhood->_n_iuget(iu);
      }
      if (nhood->_n_ives) {
	iv.u=nhood->_n_iv.u+nhood->_n_ivls;
	nhood->matrix[17].v.i=nhood->_n_ivget(iv);
      }
      if (nhood->_n_axs) {
	ax.u=nhood->_n_ax.u+nhood->_n_axls;
	nhood->matrix[17].ax.i=nhood->_n_axget(ax);
      }
    }
  }
  else {
    nhood->matrix[17].m=0;
  }
    
  /* left*/
  if (nhood->_n_cntx > 0) {
    m.u=nhood->_n_m.u-nhood->_n_ms;
    nhood->matrix[11].m=nhood->_n_mget(m);
    if (nhood->matrix[11].m != MARKER_QUEUED) {
      if (!nhood->matrix[11].m) nhood->matrix[11].m=1;

      /* if not queued load all chanels*/
      if (nhood->_n_iaes) {
	ia.u=nhood->_n_ia.u-nhood->_n_ias;
	nhood->matrix[11].a.i=nhood->_n_iaget(ia);
      }
      if (nhood->_n_iyes) {
	iy.u=nhood->_n_iy.u-nhood->_n_iys;
	nhood->matrix[11].y.i=nhood->_n_iyget(iy);
      }
      if (nhood->_n_iues) {
	iu.u=nhood->_n_iu.u-nhood->_n_ius;
	nhood->matrix[11].u.i=nhood->_n_iuget(iu);
      }
      if (nhood->_n_ives) {
	iv.u=nhood->_n_iv.u-nhood->_n_ivs;
	nhood->matrix[11].v.i=nhood->_n_ivget(iv);
      }
      if (nhood->_n_axs) {
	ax.u=nhood->_n_ax.u-nhood->_n_axs;
	nhood->matrix[11].ax.i=nhood->_n_axget(ax);
      }
    }
  }
  else {
    nhood->matrix[11].m=0;
  }
    
  /* right*/
  if (nhood->_n_cntx < (nhood->width-1)) {
    m.u=nhood->_n_m.u+nhood->_n_ms;
    nhood->matrix[13].m=nhood->_n_mget(m);
    if (nhood->matrix[13].m != MARKER_QUEUED) {
      if (!nhood->matrix[13].m) nhood->matrix[13].m=1;

      /* if not queued load all chanels*/
      if (nhood->_n_iaes) {
	ia.u=nhood->_n_ia.u+nhood->_n_ias;
	nhood->matrix[13].a.i=nhood->_n_iaget(ia);
      }
      if (nhood->_n_iyes) {
	iy.u=nhood->_n_iy.u+nhood->_n_iys;
	nhood->matrix[13].y.i=nhood->_n_iyget(iy);
      }
      if (nhood->_n_iues) {
	iu.u=nhood->_n_iu.u+nhood->_n_ius;
	nhood->matrix[13].u.i=nhood->_n_iuget(iu);
      }
      if (nhood->_n_ives) {
	iv.u=nhood->_n_iv.u+nhood->_n_ivs;
	nhood->matrix[13].v.i=nhood->_n_ivget(iv);
      }
      if (nhood->_n_axs) {
	ax.u=nhood->_n_ax.u+nhood->_n_axs;
	nhood->matrix[13].ax.i=nhood->_n_axget(ax);
      }
    }
  }
  else {
    nhood->matrix[13].m=0;
  }

  /* up right*/
  if ((nhood->_n_cnty > 0) && (nhood->_n_cntx < (nhood->width-1))) {
    m.u=nhood->_n_m.u-nhood->_n_mls+nhood->_n_ms;
    nhood->matrix[8].m=nhood->_n_mget(m);
    if (nhood->matrix[8].m != MARKER_QUEUED) {
      if (!nhood->matrix[8].m) nhood->matrix[8].m=1;

      /* if not queued load all chanels*/
      if (nhood->_n_iaes) {
	ia.u=nhood->_n_ia.u-nhood->_n_ials+nhood->_n_ias;
	nhood->matrix[8].a.i=nhood->_n_iaget(ia);
      }
      if (nhood->_n_iyes) {
	iy.u=nhood->_n_iy.u-nhood->_n_iyls+nhood->_n_iys;
	nhood->matrix[8].y.i=nhood->_n_iyget(iy);
      }
      if (nhood->_n_iues) {
	iu.u=nhood->_n_iu.u-nhood->_n_iuls+nhood->_n_ius;
	nhood->matrix[8].u.i=nhood->_n_iuget(iu);
      }
      if (nhood->_n_ives) {
	iv.u=nhood->_n_iv.u-nhood->_n_ivls+nhood->_n_ivs;
	nhood->matrix[8].v.i=nhood->_n_ivget(iv);
      }
      if (nhood->_n_axs) {
	ax.u=nhood->_n_ax.u-nhood->_n_axls+nhood->_n_axs;
	nhood->matrix[8].ax.i=nhood->_n_axget(ax);
      }
    }
  }
  else {
    nhood->matrix[8].m=0;
  }
  /* up left*/
  if ((nhood->_n_cnty > 0) && (nhood->_n_cntx > 0)) {
    m.u=nhood->_n_m.u-nhood->_n_mls-nhood->_n_ms;
    nhood->matrix[6].m=nhood->_n_mget(m);
    if (nhood->matrix[6].m != MARKER_QUEUED) {
      if (!nhood->matrix[6].m) nhood->matrix[6].m=1;

      /* if not queued load all chanels*/
      if (nhood->_n_iaes) {
	ia.u=nhood->_n_ia.u-nhood->_n_ials-nhood->_n_ias;
	nhood->matrix[6].a.i=nhood->_n_iaget(ia);
      }
      if (nhood->_n_iyes) {
	iy.u=nhood->_n_iy.u-nhood->_n_iyls-nhood->_n_iys;
	nhood->matrix[6].y.i=nhood->_n_iyget(iy);
      }
      if (nhood->_n_iues) {
	iu.u=nhood->_n_iu.u-nhood->_n_iuls-nhood->_n_ius;
	nhood->matrix[6].u.i=nhood->_n_iuget(iu);
      }
      if (nhood->_n_ives) {
	iv.u=nhood->_n_iv.u-nhood->_n_ivls-nhood->_n_ivs;
	nhood->matrix[6].v.i=nhood->_n_ivget(iv);
      }
      if (nhood->_n_axs) {
	ax.u=nhood->_n_ax.u-nhood->_n_axls-nhood->_n_axs;
	nhood->matrix[6].ax.i=nhood->_n_axget(ax);
      }
    }
  }
  else {
    nhood->matrix[6].m=0;
  }
  /* down right*/
  if ((nhood->_n_cnty < (nhood->height-1)) && 
      (nhood->_n_cntx < (nhood->width-1))) {
    m.u=nhood->_n_m.u+nhood->_n_mls+nhood->_n_ms;
    nhood->matrix[18].m=nhood->_n_mget(m);
    if (nhood->matrix[18].m != MARKER_QUEUED) {
      if (!nhood->matrix[18].m) nhood->matrix[18].m=1;

      /* if not queued load all chanels*/
      if (nhood->_n_iaes) {
	ia.u=nhood->_n_ia.u+nhood->_n_ials+nhood->_n_ias;
	nhood->matrix[18].a.i=nhood->_n_iaget(ia);
      }
      if (nhood->_n_iyes) {
	iy.u=nhood->_n_iy.u+nhood->_n_iyls+nhood->_n_iys;
	nhood->matrix[18].y.i=nhood->_n_iyget(iy);
      }
      if (nhood->_n_iues) {
	iu.u=nhood->_n_iu.u+nhood->_n_iuls+nhood->_n_ius;
	nhood->matrix[18].u.i=nhood->_n_iuget(iu);
      }
      if (nhood->_n_ives) {
	iv.u=nhood->_n_iv.u+nhood->_n_ivls+nhood->_n_ivs;
	nhood->matrix[18].v.i=nhood->_n_ivget(iv);
      }
      if (nhood->_n_axs) {
	ax.u=nhood->_n_ax.u+nhood->_n_axls+nhood->_n_axs;
	nhood->matrix[18].ax.i=nhood->_n_axget(ax);
      }
    }
  }
  else {
    nhood->matrix[18].m=0;
  }

  /* down left*/
  if ((nhood->_n_cnty < (nhood->height-1)) && (nhood->_n_cntx > 0) ) {
    m.u=nhood->_n_m.u+nhood->_n_mls-nhood->_n_ms;
    nhood->matrix[16].m=nhood->_n_mget(m);
    if (nhood->matrix[16].m != MARKER_QUEUED) {
      if (!nhood->matrix[16].m) nhood->matrix[16].m=1;

      /* if not queued load all chanels*/
      if (nhood->_n_iaes) {
	ia.u=nhood->_n_ia.u+nhood->_n_ials-nhood->_n_ias;
	nhood->matrix[16].a.i=nhood->_n_iaget(ia);
      }
      if (nhood->_n_iyes) {
	iy.u=nhood->_n_iy.u+nhood->_n_iyls-nhood->_n_iys;
	nhood->matrix[16].y.i=nhood->_n_iyget(iy);
      }
      if (nhood->_n_iues) {
	iu.u=nhood->_n_iu.u+nhood->_n_iuls-nhood->_n_ius;
	nhood->matrix[16].u.i=nhood->_n_iuget(iu);
      }
      if (nhood->_n_ives) {
	iv.u=nhood->_n_iv.u+nhood->_n_ivls-nhood->_n_ivs;
	nhood->matrix[16].v.i=nhood->_n_ivget(iv);
      }
      if (nhood->_n_axs) {
	ax.u=nhood->_n_ax.u+nhood->_n_axls-nhood->_n_axs;
	nhood->matrix[16].ax.i=nhood->_n_axget(ax);
      }
    }
  }
  else {
    nhood->matrix[16].m=0;
  }

}

/******* MUpdate *************************************************************/
void MUpdate(TNhood *nhood, char pos)

{
  MomImageData m;

  m=nhood->_n_m;

  switch (pos) {
  case 11:
    m.u-=nhood->_n_ms;
  case 12:
    break;
  case 6:
    m.u-=(nhood->_n_mls+nhood->_n_ms);
    break;
  case 7:
    m.u-=nhood->_n_mls;
    break;
  case 8:
    m.u-=(nhood->_n_mls-nhood->_n_ms);
    break;
  case 13:
    m.u+=nhood->_n_ms;
    break;
  case 18:
    m.u+=(nhood->_n_mls+nhood->_n_ms);
    break;
  case 17:
    m.u+=nhood->_n_mls;
    break;
  case 16:
    m.u+=(nhood->_n_mls-nhood->_n_ms);
    break;
  default:
    addlib_stop_here("ERROR : Invalid position in MUpdate\n",0,0);
    return;
  }
  nhood->_n_mset(m,nhood->result.m);
}


/******* UpdatePos **********************************************************/
void UpdatePos(TNhood *nhood, char pos)

{
  MomImageData a,y,u,v,ax,m;

  a=nhood->_n_ra;
  y=nhood->_n_ry;
  u=nhood->_n_ru;
  v=nhood->_n_rv;
  ax=nhood->_n_ax;
  m=nhood->_n_m;

  switch (pos) {
  case 11:
    a.u -=nhood->_n_ras;
    y.u -=nhood->_n_rys;
    u.u -=nhood->_n_rus;
    v.u -=nhood->_n_rvs;
    ax.u-=nhood->_n_axs;
    m.u -=nhood->_n_ms;
  case 12:
    break;
  case 6:
    a.u -=(nhood->_n_rals+nhood->_n_ras);
    y.u -=(nhood->_n_ryls+nhood->_n_rys);
    u.u -=(nhood->_n_ruls+nhood->_n_rus);
    v.u -=(nhood->_n_rvls+nhood->_n_rvs);
    ax.u-=(nhood->_n_axls+nhood->_n_axs);
    m.u -=(nhood->_n_mls +nhood->_n_ms);
    break;
  case 7:
    a.u -=nhood->_n_rals;
    y.u -=nhood->_n_ryls;
    u.u -=nhood->_n_ruls;
    v.u -=nhood->_n_rvls;
    ax.u-=nhood->_n_axls;
    m.u -=nhood->_n_mls;
    break;
  case 8:
    a.u -=(nhood->_n_rals-nhood->_n_ras);
    y.u -=(nhood->_n_ryls-nhood->_n_rys);
    u.u -=(nhood->_n_ruls-nhood->_n_rus);
    v.u -=(nhood->_n_rvls-nhood->_n_rvs);
    ax.u-=(nhood->_n_axls-nhood->_n_axs);
    m.u -=(nhood->_n_mls -nhood->_n_ms);
    break;
  case 13:
    a.u +=nhood->_n_ras;
    y.u +=nhood->_n_rys;
    u.u +=nhood->_n_rus;
    v.u +=nhood->_n_rvs;
    ax.u+=nhood->_n_axs;
    m.u +=nhood->_n_ms;
    break;
  case 18:
    a.u +=(nhood->_n_rals+nhood->_n_ras);
    y.u +=(nhood->_n_ryls+nhood->_n_rys);
    u.u +=(nhood->_n_ruls+nhood->_n_rus);
    v.u +=(nhood->_n_rvls+nhood->_n_rvs);
    ax.u+=(nhood->_n_axls+nhood->_n_axs);
    m.u +=(nhood->_n_mls +nhood->_n_ms);
    break;
  case 17:
    a.u +=nhood->_n_rals;
    y.u +=nhood->_n_ryls;
    u.u +=nhood->_n_ruls;
    v.u +=nhood->_n_rvls;
    ax.u+=nhood->_n_axls;
    m.u +=nhood->_n_mls;
    break;
  case 16:
    a.u +=(nhood->_n_rals-nhood->_n_ras);
    y.u +=(nhood->_n_ryls-nhood->_n_rys);
    u.u +=(nhood->_n_ruls-nhood->_n_rus);
    v.u +=(nhood->_n_rvls-nhood->_n_rvs);
    ax.u+=(nhood->_n_axls-nhood->_n_axs);
    m.u +=(nhood->_n_mls -nhood->_n_ms);
    break;
  default:
    addlib_stop_here("ERROR : Invalid position in UpdatePos\n",0,0);
    return;
  }

  nhood->_n_mset(m,nhood->result.m);
  
  if (nhood->_n_raes) {
    nhood->_n_raset(a,nhood->result.a.i);
  }
  if (nhood->_n_ryes) {
    nhood->_n_ryset(y,nhood->result.y.i);
  }
  if (nhood->_n_rues) {
    nhood->_n_ruset(u,nhood->result.u.i);
  }
  if (nhood->_n_rves) {
    nhood->_n_rvset(v,nhood->result.v.i);
  }
  if (nhood->_n_axs) {
    nhood->_n_axset(ax,nhood->result.ax.i);
  }
  if (nhood->_n_axs) {
    nhood->_n_mset(m,nhood->result.m);
  }
}

/*****************************************************************************/
/******* REC_AREA ************************************************************/
/*****************************************************************************/

/******* Shift Operation Without Load ****************************************/
void Shift(TNhood *nhood,char dir)

{
  int regpos,regpos1;
  
  if  (((dir==1) || (dir==0) || (dir==7)) && (nhood->_n_cntx>0)) {
    nhood->_n_cntx--;
    nhood->_n_ia.u-=nhood->_n_ias;
    nhood->_n_iy.u-=nhood->_n_iys;
    nhood->_n_iu.u-=nhood->_n_ius;
    nhood->_n_iv.u-=nhood->_n_ivs;
    nhood->_n_m.u-=nhood->_n_ms;
    nhood->_n_ax.u-=nhood->_n_axs;
    nhood->_n_ra.u-=nhood->_n_ras;
    nhood->_n_ry.u-=nhood->_n_rys;
    nhood->_n_ru.u-=nhood->_n_rus;
    nhood->_n_rv.u-=nhood->_n_rvs;
    for (regpos=24, regpos1=23; regpos>0; regpos--, regpos1--) {
      nhood->matrix[regpos].m=nhood->matrix[regpos1].m;
      if (!nhood->matrix[regpos1].m) continue;
      nhood->matrix[regpos].a.i=nhood->matrix[regpos1].a.i;
      nhood->matrix[regpos].y.i=nhood->matrix[regpos1].y.i;
      nhood->matrix[regpos].u.i=nhood->matrix[regpos1].u.i;
      nhood->matrix[regpos].v.i=nhood->matrix[regpos1].v.i;
      nhood->matrix[regpos].ax.i=nhood->matrix[regpos1].ax.i;
    }
    for (regpos=0; regpos<25; regpos+=_m5x5[1]) {
/*      nhood->matrix[regpos].a=0;
      nhood->matrix[regpos].y=0;
      nhood->matrix[regpos].u=0;
      nhood->matrix[regpos].v=0;*/
      nhood->matrix[regpos].m=0;
/*      nhood->matrix[regpos].ax=0;*/
    }
  }
  if  (((dir==3) || (dir==4) || (dir==5)) &&
       (nhood->_n_cntx<(nhood->width-1))) {
    nhood->_n_cntx++;
    nhood->_n_ia.u+=nhood->_n_ias;
    nhood->_n_iy.u+=nhood->_n_iys;
    nhood->_n_iu.u+=nhood->_n_ius;
    nhood->_n_iv.u+=nhood->_n_ivs;
    nhood->_n_m.u+=nhood->_n_ms;
    nhood->_n_ax.u+=nhood->_n_axs;
    nhood->_n_ra.u+=nhood->_n_ras;
    nhood->_n_ry.u+=nhood->_n_rys;
    nhood->_n_ru.u+=nhood->_n_rus;
    nhood->_n_rv.u+=nhood->_n_rvs;
    for (regpos=0, regpos1=1; regpos<24; regpos++, regpos1++) {
      nhood->matrix[regpos].m=nhood->matrix[regpos1].m;
      if (!nhood->matrix[regpos1].m) continue;
      nhood->matrix[regpos].a=nhood->matrix[regpos1].a;
      nhood->matrix[regpos].y=nhood->matrix[regpos1].y;
      nhood->matrix[regpos].u=nhood->matrix[regpos1].u;
      nhood->matrix[regpos].v=nhood->matrix[regpos1].v;
      nhood->matrix[regpos].ax=nhood->matrix[regpos1].ax;
    }
    for (regpos=4; regpos<25; regpos+=_m5x5[1]) {
/*      nhood->matrix[regpos].a=0;
      nhood->matrix[regpos].y=0;
      nhood->matrix[regpos].u=0;
      nhood->matrix[regpos].v=0;*/
      nhood->matrix[regpos].m=0;
/*      nhood->matrix[regpos].ax=0;*/
    }
  }
  if  (((dir==1) || (dir==2) || (dir==3)) && (nhood->_n_cnty>0)) {
    nhood->_n_cnty--;
    nhood->_n_ia.u-=nhood->_n_ials;
    nhood->_n_iy.u-=nhood->_n_iyls;
    nhood->_n_iu.u-=nhood->_n_iuls;
    nhood->_n_iv.u-=nhood->_n_ivls;
    nhood->_n_m.u-=nhood->_n_mls;
    nhood->_n_ax.u-=nhood->_n_axls;
    nhood->_n_ra.u-=nhood->_n_rals;
    nhood->_n_ry.u-=nhood->_n_ryls;
    nhood->_n_ru.u-=nhood->_n_ruls;
    nhood->_n_rv.u-=nhood->_n_rvls;
    for (regpos=24, regpos1=19; regpos>4; regpos--, regpos1--) {
      nhood->matrix[regpos].m=nhood->matrix[regpos1].m;
      if (!nhood->matrix[regpos1].m) continue;
      nhood->matrix[regpos].a=nhood->matrix[regpos1].a;
      nhood->matrix[regpos].y=nhood->matrix[regpos1].y;
      nhood->matrix[regpos].u=nhood->matrix[regpos1].u;
      nhood->matrix[regpos].v=nhood->matrix[regpos1].v;
      nhood->matrix[regpos].ax=nhood->matrix[regpos1].ax;
    }
    for (regpos=0; regpos<_m5x5[1]; regpos++) {
/*      nhood->matrix[regpos].a=0;
      nhood->matrix[regpos].y=0;
      nhood->matrix[regpos].u=0;
      nhood->matrix[regpos].v=0;*/
      nhood->matrix[regpos].m=0;
/*      nhood->matrix[regpos].ax=0;*/
    }
  }
  if  (((dir==5) || (dir==6) || (dir==7)) && 
       (nhood->_n_cnty<(nhood->height-1))) {
    nhood->_n_cnty++;
    nhood->_n_ia.u+=nhood->_n_ials;
    nhood->_n_iy.u+=nhood->_n_iyls;
    nhood->_n_iu.u+=nhood->_n_iuls;
    nhood->_n_iv.u+=nhood->_n_ivls;
    nhood->_n_m.u+=nhood->_n_mls;
    nhood->_n_ax.u+=nhood->_n_axls;
    nhood->_n_ra.u+=nhood->_n_rals;
    nhood->_n_ry.u+=nhood->_n_ryls;
    nhood->_n_ru.u+=nhood->_n_ruls;
    nhood->_n_rv.u+=nhood->_n_rvls;
    for (regpos=0, regpos1=_m5x5[1]; regpos<20; regpos++, regpos1++) {
      nhood->matrix[regpos].m=nhood->matrix[regpos1].m;
      if (!nhood->matrix[regpos1].m) continue;
      nhood->matrix[regpos].a=nhood->matrix[regpos1].a;
      nhood->matrix[regpos].y=nhood->matrix[regpos1].y;
      nhood->matrix[regpos].u=nhood->matrix[regpos1].u;
      nhood->matrix[regpos].v=nhood->matrix[regpos1].v;
      nhood->matrix[regpos].ax=nhood->matrix[regpos1].ax;
    }
    for (regpos=20; regpos<25; regpos++) {
/*      nhood->matrix[regpos].a=0;
      nhood->matrix[regpos].y=0;
      nhood->matrix[regpos].u=0;
      nhood->matrix[regpos].v=0;*/
      nhood->matrix[regpos].m=0;
/*      nhood->matrix[regpos].ax=0;*/
    }
  }
  if (nhood->matrix[12].m) return;
  if (nhood->_n_iaes) nhood->matrix[12].a.i=nhood->_n_iaget(nhood->_n_ia);
  if (nhood->_n_iyes) nhood->matrix[12].y.i=nhood->_n_iyget(nhood->_n_iy);
  if (nhood->_n_iues) nhood->matrix[12].u.i=nhood->_n_iuget(nhood->_n_iu);
  if (nhood->_n_ives) nhood->matrix[12].v.i=nhood->_n_ivget(nhood->_n_iv);
  if (nhood->_n_axs) nhood->matrix[12].ax.i=nhood->_n_axget(nhood->_n_ax);
  nhood->matrix[12].m=nhood->_n_mget(nhood->_n_m);
  if (!nhood->matrix[12].m) nhood->matrix[12].m=1;
}

/******* LOAD PIXEL IF NOT LOADED ********************************************/
void LoadIfUnload3X3(TNhood *nhood,short regpos)

{
  MomImageData ia,iy,iu,iv,m,ax;
  

  /*check if already loaded*/
  if (nhood->matrix[regpos].m) return;

  /* set initial pointers */
  ia=nhood->_n_ia;
  iy=nhood->_n_iy;
  iu=nhood->_n_iu;
  iv=nhood->_n_iv;
  m=nhood->_n_m;
  ax=nhood->_n_ax;
  if  ((regpos==6) || (regpos==11) || (regpos==16)) {
    if (nhood->_n_cntx>0) {
      ia.u-=nhood->_n_ias;
      iy.u-=nhood->_n_iys;
      iu.u-=nhood->_n_ius;
      iv.u-=nhood->_n_ivs;
      m.u-=nhood->_n_ms;
      ax.u-=nhood->_n_axs;
    }
    else goto _n_LOAD_INVALID;
  }
  if  ((regpos==8) || (regpos==13) || (regpos==18)) {
    if (nhood->_n_cntx<(nhood->width-1)) {
      ia.u+=nhood->_n_ias;
      iy.u+=nhood->_n_iys;
      iu.u+=nhood->_n_ius;
      iv.u+=nhood->_n_ivs;
      m.u+=nhood->_n_ms;
      ax.u+=nhood->_n_axs;
    }
    else goto _n_LOAD_INVALID;
  }
  if  ((regpos==6) || (regpos==7) || (regpos==8)) {
    if (nhood->_n_cnty>0) {
      ia.u-=nhood->_n_ials;
      iy.u-=nhood->_n_iyls;
      iu.u-=nhood->_n_iuls;
      iv.u-=nhood->_n_ivls;
      m.u-=nhood->_n_mls;
      ax.u-=nhood->_n_axls;
    }
    else goto _n_LOAD_INVALID;
  }
  if ((regpos==16) || (regpos==17) || (regpos==18)) {
    if (nhood->_n_cnty<(nhood->height-1)) {
      ia.u+=nhood->_n_ials;
      iy.u+=nhood->_n_iyls;
      iu.u+=nhood->_n_iuls;
      iv.u+=nhood->_n_ivls;
      m.u+=nhood->_n_mls;
      ax.u+=nhood->_n_axls;
    }
    else goto _n_LOAD_INVALID;
  }

  /*load valid pixel*/
  if (nhood->_n_iaes) nhood->matrix[regpos].a.i=nhood->_n_iaget(ia);
  if (nhood->_n_iyes) nhood->matrix[regpos].y.i=nhood->_n_iyget(iy);
  if (nhood->_n_iues) nhood->matrix[regpos].u.i=nhood->_n_iuget(iu);
  if (nhood->_n_ives) nhood->matrix[regpos].v.i=nhood->_n_ivget(iv);
  if (nhood->_n_axs) nhood->matrix[regpos].ax.i=nhood->_n_axget(ax);
  nhood->matrix[regpos].m=nhood->_n_mget(m);
  if (!nhood->matrix[regpos].m) nhood->matrix[regpos].m=1;

  return;


  /*load invalid pixel*/
_n_LOAD_INVALID:

  nhood->matrix[regpos].a.i=(nhood->matrix[12].a.i & nhood->_n_amaska) |
    nhood->_n_amasko;
  nhood->matrix[regpos].y.i=(nhood->matrix[12].y.i & nhood->_n_ymaska) |
    nhood->_n_ymasko;
  nhood->matrix[regpos].u.i=(nhood->matrix[12].u.i & nhood->_n_umaska) |
    nhood->_n_umasko;
  nhood->matrix[regpos].v.i=(nhood->matrix[12].v.i & nhood->_n_vmaska) |
    nhood->_n_vmasko;
  nhood->matrix[regpos].m=0;
  nhood->matrix[regpos].ax.i=(nhood->matrix[12].ax.i & nhood->_n_axmaska) |
    nhood->_n_axmasko;
  return;
}

/*****************************************************************************/
/******* INTRA ***************************************************************/
/*****************************************************************************/

/******* Preload Matrix ******************************************************/
void Preload(TNhood *nhood)

{
  int regpos;
  short blkcnty, blkcntx;
  short blkcntxstart,blkcntxstop;
  short blkcntystart,blkcntystop;
  int yoffset,xoffset;
  long offset;
  int repy,repx,strepx;
  MomImageData ia,iy,iu,iv,m,ax;
  MomImageData stia,stiy,stiu,stiv,stm,stax;  /* store linestart position*/

  TCoor ypos,xpos,stxpos;

  /* set initial pointers */
  stxpos=nhood->_n_icntx;
  ypos=nhood->_n_icnty;
  
  /* init subcnts, offsets and repeat values*/
  yoffset=nhood->_n_yblksize;
  ypos=nhood->_n_icnty-yoffset;
  if (ypos < 0) {
    repy=-ypos;
    yoffset+=ypos;  /*  += : ypos is negativ*/
  }
  else {
    repy=0;
  }

  xoffset=nhood->_n_xblksize;
  stxpos=nhood->_n_icntx-xoffset;
  if (stxpos < 0) {
    strepx=-stxpos;
    xoffset+=stxpos;
  }
  else {
    strepx=0;
  }

  /*local copy of imagedata pointer*/
  offset=yoffset*nhood->_n_inwidth+xoffset;
  stia.u=nhood->_n_ia.u-(offset<<_typesize[nhood->_n_iaes]);
  stiy.u=nhood->_n_iy.u-(offset<<_typesize[nhood->_n_iyes]);
  stiu.u=nhood->_n_iu.u-(offset<<_typesize[nhood->_n_iues]);
  stiv.u=nhood->_n_iv.u-(offset<<_typesize[nhood->_n_ives]);
  stax.u=nhood->_n_ax.u-(offset<<_typesize[nhood->_n_axs]);
  stm.u=nhood->_n_m.u-(offset<<_typesize[nhood->_n_ms]);

  /* get matrix values */
  blkcntxstart=nhood->_n_xblkstart;
  blkcntxstop=nhood->_n_xblkstop;
  blkcntystart=nhood->_n_yblkstart;
  blkcntystop=nhood->_n_yblkstop;
  for (blkcnty=blkcntystart; blkcnty<= blkcntystop; blkcnty++) {
    

    xpos=stxpos;
    repx=strepx;

    ia.u=stia.u;
    iy.u=stiy.u;
    iu.u=stiu.u;
    iv.u=stiv.u;
    ax.u=stax.u;
    m.u=stm.u;
    xpos=stxpos;
    repx=strepx;

    for (blkcntx=blkcntxstart; blkcntx<= blkcntxstop; blkcntx++) {
      regpos=_my[blkcnty]+blkcntx;

      /* pixel in invalid range*/
      if (repx || (xpos >= nhood->_n_inwidth) || 
	  repy || (ypos >= nhood->_n_inheight)) {
	if (nhood->_n_iaes) {
	  nhood->matrix[regpos].a.i=(nhood->_n_iaget(ia) & nhood->_n_amaska) |
	    nhood->_n_amasko;
	}
	if (nhood->_n_iyes) {
	nhood->matrix[regpos].y.i=(nhood->_n_iyget(iy) & nhood->_n_ymaska) |
	  nhood->_n_ymasko;
	}
	if (nhood->_n_iues) {
	nhood->matrix[regpos].u.i=(nhood->_n_iuget(iu) & nhood->_n_umaska) |
	  nhood->_n_umasko;
	}
	if (nhood->_n_ives) {
	nhood->matrix[regpos].v.i=(nhood->_n_ivget(iv) & nhood->_n_vmaska) |
	  nhood->_n_vmasko;
	}
	if (nhood->_n_axs) {
	nhood->matrix[regpos].ax.i=(nhood->_n_axget(ax) & nhood->_n_axmaska) |
	  nhood->_n_axmasko;
	}
	nhood->matrix[regpos].m=0;
      }

      /*pixel in valid range*/
      else {
	if (nhood->_n_iaes) nhood->matrix[regpos].a.i=nhood->_n_iaget(ia);
	if (nhood->_n_iyes) nhood->matrix[regpos].y.i=nhood->_n_iyget(iy);
	if (nhood->_n_iues) nhood->matrix[regpos].u.i=nhood->_n_iuget(iu);
	if (nhood->_n_ives) nhood->matrix[regpos].v.i=nhood->_n_ivget(iv);
	if (nhood->_n_axs) nhood->matrix[regpos].ax.i=nhood->_n_axget(ax);
	if (nhood->_n_ms) nhood->matrix[regpos].m=nhood->_n_mget(m);
	else nhood->matrix[regpos].m=1;
	if (!nhood->matrix[regpos].m) nhood->matrix[regpos].m=1;
      }

      /* inc position*/
      if (!repx) {
	if (xpos < (nhood->_n_inwidth-1)) {
	  ia.u+=nhood->_n_ias;
	  iy.u+=nhood->_n_iys;
	  iu.u+=nhood->_n_ius;
	  iv.u+=nhood->_n_ivs;
	  ax.u+=nhood->_n_axs;
	  m.u+=nhood->_n_ms;
	}
      }
      else {
	repx--;
      }
      xpos++;
    }

    /* next line*/
    if (!repy) {
      if (ypos < (nhood->_n_inheight-1)) {
	stia.u+=nhood->_n_ials;
	stiy.u+=nhood->_n_iyls;
	stiu.u+=nhood->_n_iuls;
	stiv.u+=nhood->_n_ivls;
	stax.u+=nhood->_n_axls;
	stm.u+=nhood->_n_mls;
      }
    }
    else {
      repy--;
    }
    ypos++;
  }
}

/******* Preload Matrix with Subsampling *************************************/
void PreloadSub(TNhood *nhood)

{
  int irepcnty,irepcntx,stirepcntx;
  char irepeatx,irepeaty,istepx,istepy;
  int yoffset,xoffset;
  long offset;
  MomImageData ia,iy,iu,iv;
  MomImageData stia,stiy,stiu,stiv;  /* store linestart position*/
  TCoor ypos,xpos,stxpos;
  short regpos,regpos1;
  short invalidx,invalidy;
  short blkcnty, blkcntx;
  short blkcntxstart,blkcntxstop;
  short blkcntystart,blkcntystop;
  
  /* local copies of conversion parameters*/
  irepeatx=nhood->_n_irepeatx;
  irepeaty=nhood->_n_irepeaty;
  istepx=nhood->_n_istepx;
  istepy=nhood->_n_istepy;


  /* init subcnts, offsets and repeat values*/
  yoffset=(nhood->_n_yblksize/irepeaty)*istepy;
  irepcnty=nhood->_n_irepcnty-(nhood->_n_yblksize%irepeaty);

  if (irepcnty < 0) {
    yoffset+=istepy;
    irepcnty+=irepeaty;
  }

  ypos=nhood->_n_icnty-yoffset;
  if (ypos < nhood->_n_firsty) {
    yoffset+=ypos-nhood->_n_firsty;
  }

  xoffset=(nhood->_n_xblksize/irepeatx)*istepx;
  stirepcntx=nhood->_n_irepcntx-(nhood->_n_xblksize%irepeatx);
  if (stirepcntx < 0) {
    xoffset+=istepx;
    stirepcntx+=irepeatx;
  }


  stxpos=nhood->_n_icntx-xoffset;
  if (stxpos < nhood->_n_firstx) {
    xoffset+=stxpos-nhood->_n_firstx;
  }

  /*local copy of imagedata pointer*/
  offset=yoffset*nhood->_n_inwidth+xoffset;
  stia.u=nhood->_n_ia.u-(offset<<_typesize[nhood->_n_iaes]);
  stiy.u=nhood->_n_iy.u-(offset<<_typesize[nhood->_n_iyes]);
  stiu.u=nhood->_n_iu.u-(offset<<_typesize[nhood->_n_iues]);
  stiv.u=nhood->_n_iv.u-(offset<<_typesize[nhood->_n_ives]);

  /* get matrix values */
  blkcntxstart=nhood->_n_xblkstart;
  blkcntxstop=nhood->_n_xblkstop;
  blkcntystart=nhood->_n_yblkstart;
  blkcntystop=nhood->_n_yblkstop;
  for (blkcnty=blkcntystart; blkcnty<= blkcntystop; blkcnty++) {
    

    irepcntx=stirepcntx;
    xpos=stxpos;

    ia.u=stia.u;
    iy.u=stiy.u;
    iu.u=stiu.u;
    iv.u=stiv.u;


    invalidy=irepcnty || (ypos<0);
    regpos1=_my[blkcnty]+blkcntxstart;
    if ((invalidy || (ypos >= nhood->_n_inheight)) &&
	(blkcnty!=blkcntystart)) {

      /* invalid row*/
      regpos=regpos1-_my[1];
      for (blkcntx=blkcntxstart; blkcntx<=blkcntxstop; blkcntx++) {
	nhood->matrix[regpos1].a.i=(nhood->matrix[regpos].a.i
				    & nhood->_n_amaska) | nhood->_n_amasko;
	nhood->matrix[regpos1].y.i=(nhood->matrix[regpos].y.i 
				    & nhood->_n_ymaska) | nhood->_n_ymasko;
	nhood->matrix[regpos1].u.i=(nhood->matrix[regpos].u.i &
				    nhood->_n_umaska) | nhood->_n_umasko;
	nhood->matrix[regpos1].v.i=(nhood->matrix[regpos].v.i &
				    nhood->_n_vmaska) | nhood->_n_vmasko;
	/*nhood->matrix[regpos1].ax.i=nhood->matrix[regpos].ax.i & 
	  nhood->_n_axmaska) | nhood->_n_axmasko;*/
	/* not availibale with up-down-conversion*/
	regpos++;
	regpos1++;
      }
    }
    else {

      /*valid row*/
       for (blkcntx=blkcntxstart; blkcntx<=blkcntxstop; blkcntx++) {

	/* pixel in invalid range*/
	invalidx=irepcntx || (xpos<0) || (xpos >= nhood->_n_inwidth);
	if (invalidx || invalidy) {
	  if ((blkcntx!=blkcntxstart) && invalidx) {
	    regpos=regpos1-1;
	    nhood->matrix[regpos1].a.i=(nhood->matrix[regpos].a.i &
				      nhood->_n_amaska) | nhood->_n_amasko;
	    nhood->matrix[regpos1].y.i=(nhood->matrix[regpos].y.i &
				      nhood->_n_ymaska) | nhood->_n_ymasko;
	    nhood->matrix[regpos1].u.i=(nhood->matrix[regpos].u.i &
				      nhood->_n_umaska) | nhood->_n_umasko;
	    nhood->matrix[regpos1].v.i=(nhood->matrix[regpos].v.i &
				      nhood->_n_vmaska) | nhood->_n_vmasko;
	  }
	  else {
	    if (nhood->_n_iaes) {
	      nhood->matrix[regpos1].a.i=(nhood->_n_iaget(ia) & 
					nhood->_n_amaska) | nhood->_n_amasko;
	    }
	    if (nhood->_n_iyes) {
	      nhood->matrix[regpos1].y.i=(nhood->_n_iyget(iy) &
					nhood->_n_ymaska) | nhood->_n_ymasko;
	    }
	    if (nhood->_n_iues) {
	      nhood->matrix[regpos1].u.i=(nhood->_n_iuget(iu) &
					nhood->_n_umaska) | nhood->_n_umasko;
	    }
	    if (nhood->_n_ives) {
	      nhood->matrix[regpos1].v.i=(nhood->_n_ivget(iv) &
					nhood->_n_vmaska) | nhood->_n_vmasko;
	    }
	  }
	}
	
	/*pixel in valid range*/
	else {
	  if (nhood->_n_iaes) nhood->matrix[regpos1].a.i=nhood->_n_iaget(ia);
	  if (nhood->_n_iyes) nhood->matrix[regpos1].y.i=nhood->_n_iyget(iy);
	  if (nhood->_n_iues) nhood->matrix[regpos1].u.i=nhood->_n_iuget(iu);
	  if (nhood->_n_ives) nhood->matrix[regpos1].v.i=nhood->_n_ivget(iv);
	}
	regpos1++;

	/* inc position*/
	irepcntx++;
	if (irepcntx == irepeatx) {
	  if (xpos>= nhood->_n_firstx) {
	    if (xpos < (nhood->_n_inwidth-1)) {
	      ia.u+=nhood->_n_ias;
	      iy.u+=nhood->_n_iys;
	      iu.u+=nhood->_n_ius;
	      iv.u+=nhood->_n_ivs;
	    }
	  }
	  xpos+=istepx;
	  irepcntx=0;
	}
      }
    }
  
    /* next line*/
    irepcnty++;
    if (irepcnty == irepeaty) {
      if (ypos>= nhood->_n_firsty) {
	if (ypos < (nhood->_n_inheight-1)) {
	  stia.u+=nhood->_n_ials;
	  stiy.u+=nhood->_n_iyls;
	  stiu.u+=nhood->_n_iuls;
	  stiv.u+=nhood->_n_ivls;
	}
      }
      ypos+=istepy;
      irepcnty=0;
    }
  }
}

/******* SHIFT UP AND LOAD NEW LINE ******************************************/
void ShiftUpLineLoad(TNhood *nhood)

{
  int regpos,regpos1;
  TCoor xpos,ypos;
  int xoffset,yoffset;
  long offset;
  MomImageData ia,iy,iu,iv,m,ax;
  int repx,repy;
  short blkcnty, blkcntx;
  short blkcntxstart,blkcntxstop;
  short blkcntystart,blkcntystop;

  nhood->_n_icnty--;
  nhood->_n_ia.u-=nhood->_n_ials;
  nhood->_n_iy.u-=nhood->_n_iyls;
  nhood->_n_iu.u-=nhood->_n_iuls;
  nhood->_n_iv.u-=nhood->_n_ivls;
  nhood->_n_ax.u-=nhood->_n_axls;
  nhood->_n_m.u-=nhood->_n_mls;
/*  if (matrixshift) {*/
    nhood->_n_cnty--;
    nhood->_n_ra.u-=nhood->_n_rals;
    nhood->_n_ry.u-=nhood->_n_ryls;
    nhood->_n_ru.u-=nhood->_n_ruls;
    nhood->_n_rv.u-=nhood->_n_rvls;
/*  }*/

  /* execute shift*/
  blkcntxstart=nhood->_n_xblkstart;
  blkcntxstop=nhood->_n_xblkstop;
  blkcntystart=nhood->_n_yblkstart;
  blkcntystop=nhood->_n_yblkstop;
  for (blkcntx=blkcntxstart; blkcntx<= blkcntxstop; blkcntx++) {
    for (blkcnty=blkcntystop; blkcnty> blkcntystart; blkcnty--) {

      regpos=_my[blkcnty]+blkcntx;
      regpos1=regpos-_my[1];
      nhood->matrix[regpos].a=nhood->matrix[regpos1].a;
      nhood->matrix[regpos].y=nhood->matrix[regpos1].y;
      nhood->matrix[regpos].u=nhood->matrix[regpos1].u;
      nhood->matrix[regpos].v=nhood->matrix[regpos1].v;
      nhood->matrix[regpos].ax=nhood->matrix[regpos1].ax;
      nhood->matrix[regpos].m=nhood->matrix[regpos1].m;
    }
  }

  /* reload top row */
  yoffset=nhood->_n_yblksize;

  ypos=nhood->_n_icnty-yoffset;
  if (ypos < 0) {
    repy=-ypos;
    yoffset+=ypos;
  }
  else {
    repy=0;
  }

  /* invalid row*/
  if (repy) {
    regpos=_my[blkcntystart]+blkcntxstart;
    for (blkcntx=blkcntxstart; blkcntx<=blkcntxstop; blkcntx++) {
      nhood->matrix[regpos].a.i=(nhood->matrix[regpos].a.i
				 & nhood->_n_amaska) | nhood->_n_amasko;
      nhood->matrix[regpos].y.i=(nhood->matrix[regpos].y.i &
				 nhood->_n_ymaska) | nhood->_n_ymasko;
      nhood->matrix[regpos].u.i=(nhood->matrix[regpos].u.i &
				 nhood->_n_umaska) | nhood->_n_umasko;
      nhood->matrix[regpos].v.i=(nhood->matrix[regpos].v.i &
				 nhood->_n_vmaska) | nhood->_n_vmasko;
      nhood->matrix[regpos].ax.i=(nhood->matrix[regpos].ax.i &
				  nhood->_n_axmaska) | nhood->_n_axmasko;
      nhood->matrix[regpos].m=0;
      regpos++;
    }
    return;
  }
  
  /* init xoffset and xrepeat */
  xoffset=nhood->_n_xblksize;

  xpos=nhood->_n_icntx-xoffset;
  if (xpos < 0) {
    repx=-xpos;
    xoffset+=xpos;
  }
  else {
    repx=0;
  }

  /*local copy of imagedata pointer*/
  offset=yoffset*nhood->_n_inwidth+xoffset;
  ia.u=nhood->_n_ia.u-(offset<<_typesize[nhood->_n_iaes]);
  iy.u=nhood->_n_iy.u-(offset<<_typesize[nhood->_n_iyes]);
  iu.u=nhood->_n_iu.u-(offset<<_typesize[nhood->_n_iues]);
  iv.u=nhood->_n_iv.u-(offset<<_typesize[nhood->_n_ives]);
  ax.u=nhood->_n_ax.u-(offset<<_typesize[nhood->_n_axs]);
  m.u=nhood->_n_m.u-(offset<<_typesize[nhood->_n_ms]);

  /* valid row*/
  regpos=_my[blkcntystart]+blkcntxstart;
  for (blkcntx=blkcntxstart; blkcntx<=blkcntxstop; blkcntx++) {
    
    /* pixel in invalid range*/
    if (repx || (xpos >=nhood->_n_inwidth)) {
      if (nhood->_n_iaes) {
	nhood->matrix[regpos].a.i=(nhood->_n_iaget(ia) & nhood->_n_amaska) |
	  nhood->_n_amasko;
      }
      if (nhood->_n_iyes) {
	nhood->matrix[regpos].y.i=(nhood->_n_iyget(iy) & nhood->_n_ymaska) |
	  nhood->_n_ymasko;
      }
      if (nhood->_n_iues) {
	nhood->matrix[regpos].u.i=(nhood->_n_iuget(iu) & nhood->_n_umaska) |
	  nhood->_n_umasko;
      }
      if (nhood->_n_ives) {
	nhood->matrix[regpos].v.i=(nhood->_n_ivget(iv) & nhood->_n_vmaska) |
	  nhood->_n_vmasko;
      }
      if (nhood->_n_axs) {
	nhood->matrix[regpos].ax.i=(nhood->_n_axget(ax) & nhood->_n_axmaska) |
	  nhood->_n_axmasko;
      }
      nhood->matrix[regpos].m=0;  
    }

    /*pixel in valid range*/
    else {
      if (nhood->_n_iaes) nhood->matrix[regpos].a.i=nhood->_n_iaget(ia);
      if (nhood->_n_iyes) nhood->matrix[regpos].y.i=nhood->_n_iyget(iy);
      if (nhood->_n_iues) nhood->matrix[regpos].u.i=nhood->_n_iuget(iu);
      if (nhood->_n_ives) nhood->matrix[regpos].v.i=nhood->_n_ivget(iv);
      if (nhood->_n_axs) nhood->matrix[regpos].ax.i=nhood->_n_axget(ax);
      if (nhood->_n_ms) nhood->matrix[regpos].m=nhood->_n_mget(m);
      else nhood->matrix[regpos].m=1;
      if (!nhood->matrix[regpos].m) nhood->matrix[regpos].m=1;
    }
    regpos++;
    
    
    /* next line*/
    if (!repx) {
      if (xpos < (nhood->_n_inwidth-1)) {
	ia.u+=nhood->_n_ias;
	iy.u+=nhood->_n_iys;
	iu.u+=nhood->_n_ius;
	iv.u+=nhood->_n_ivs;
	ax.u+=nhood->_n_axs;
	m.u+=nhood->_n_ms;
      }
    }
    else {
      repx--;
    }
    xpos++;
  }
}

/******* SHIFT DOWN AND LOAD NEW LINE ****************************************/
void ShiftDownLineLoad(TNhood *nhood)

{
  int regpos,regpos1;
  TCoor xpos;
  int xoffset,yoffset;
  long offset;
  MomImageData ia,iy,iu,iv,m,ax;
  int repx;
  short blkcnty, blkcntx;
  short blkcntxstart,blkcntxstop;
  short blkcntystart,blkcntystop;

  nhood->_n_icnty++;
  nhood->_n_ia.u+=nhood->_n_ials;
  nhood->_n_iy.u+=nhood->_n_iyls;
  nhood->_n_iu.u+=nhood->_n_iuls;
  nhood->_n_iv.u+=nhood->_n_ivls;
  nhood->_n_ax.u+=nhood->_n_axls;
  nhood->_n_m.u+=nhood->_n_mls;
/*  if (matrixshift) {*/
    nhood->_n_cnty++;
    nhood->_n_ra.u+=nhood->_n_rals;
    nhood->_n_ry.u+=nhood->_n_ryls;
    nhood->_n_ru.u+=nhood->_n_ruls;
    nhood->_n_rv.u+=nhood->_n_rvls;
/*  }*/

  /* execute shift*/
  blkcntxstart=nhood->_n_xblkstart;
  blkcntxstop=nhood->_n_xblkstop;
  blkcntystart=nhood->_n_yblkstart;
  blkcntystop=nhood->_n_yblkstop;
  for (blkcntx=blkcntxstart; blkcntx<= blkcntxstop; blkcntx++) {
    for (blkcnty=blkcntystart; blkcnty< blkcntystop; blkcnty++) {

      regpos=_my[blkcnty]+blkcntx;
      regpos1=regpos+_my[1];
      nhood->matrix[regpos].a=nhood->matrix[regpos1].a;
      nhood->matrix[regpos].y=nhood->matrix[regpos1].y;
      nhood->matrix[regpos].u=nhood->matrix[regpos1].u;
      nhood->matrix[regpos].v=nhood->matrix[regpos1].v;
      nhood->matrix[regpos].ax=nhood->matrix[regpos1].ax;
      nhood->matrix[regpos].m=nhood->matrix[regpos1].m;
    }
  }

  /* reload top row */
  yoffset=nhood->_n_yblksize;

  /* invalid row*/
  if (nhood->_n_icnty+yoffset >= nhood->_n_inheight) {
    regpos=_my[blkcntystop]+blkcntxstart;
    for (blkcntx=blkcntxstart; blkcntx<=blkcntxstop; blkcntx++) {
      nhood->matrix[regpos].a.i=(nhood->matrix[regpos].a.i &
				 nhood->_n_amaska) | nhood->_n_amasko;
      nhood->matrix[regpos].y.i=(nhood->matrix[regpos].y.i &
				 nhood->_n_ymaska) | nhood->_n_ymasko;
      nhood->matrix[regpos].u.i=(nhood->matrix[regpos].u.i &
				 nhood->_n_umaska) | nhood->_n_umasko;
      nhood->matrix[regpos].v.i=(nhood->matrix[regpos].v.i &
				 nhood->_n_vmaska) | nhood->_n_vmasko;
      nhood->matrix[regpos].ax.i=(nhood->matrix[regpos].ax.i &
				  nhood->_n_axmaska) | nhood->_n_axmasko;
      nhood->matrix[regpos].m=0;
      regpos++;
    }
    return;
  }
  
  /* init xoffset and xrepeat */
  xoffset=nhood->_n_xblksize;

  xpos=nhood->_n_icntx-xoffset;
  if (xpos < 0) {
    repx=-xpos;
    xoffset+=xpos;
  }
  else {
    repx=0;
  }

  offset=yoffset*nhood->_n_inwidth-xoffset;
  ia.u=nhood->_n_ia.u+(offset<<_typesize[nhood->_n_iaes]);;
  iy.u=nhood->_n_iy.u+(offset<<_typesize[nhood->_n_iyes]);;
  iu.u=nhood->_n_iu.u+(offset<<_typesize[nhood->_n_iues]);;
  iv.u=nhood->_n_iv.u+(offset<<_typesize[nhood->_n_ives]);;
  ax.u=nhood->_n_ax.u+(offset<<_typesize[nhood->_n_axs]);;
  m.u=nhood->_n_m.u+(offset<<_typesize[nhood->_n_ms]);
  
  /* valid row*/
  regpos=_my[blkcntystop]+blkcntxstart;
  for (blkcntx=blkcntxstart; blkcntx<=blkcntxstop; blkcntx++) {
    
    /* pixel in invalid range*/
    if (repx || (xpos >=nhood->_n_inwidth)) {
      if (nhood->_n_iaes) {
	nhood->matrix[regpos].a.i=(nhood->_n_iaget(ia) & nhood->_n_amaska) |
	  nhood->_n_amasko;
      }
      if (nhood->_n_iyes) {
	nhood->matrix[regpos].y.i=(nhood->_n_iyget(iy) & nhood->_n_ymaska) |
	  nhood->_n_ymasko;
      }
      if (nhood->_n_iues) {
	nhood->matrix[regpos].u.i=(nhood->_n_iuget(iu) & nhood->_n_umaska) |
	  nhood->_n_umasko;
      }
      if (nhood->_n_ives) {
	nhood->matrix[regpos].v.i=(nhood->_n_ivget(iv) & nhood->_n_vmaska) |
	  nhood->_n_vmasko;
      }
      if (nhood->_n_axs) {
	nhood->matrix[regpos].ax.i=(nhood->_n_axget(ax) & nhood->_n_axmaska) |
	  nhood->_n_axmasko;
      }
      nhood->matrix[regpos].m=0;
    }
    
    /*pixel in valid range*/
    else {
      if (nhood->_n_iaes) nhood->matrix[regpos].a.i=nhood->_n_iaget(ia);
      if (nhood->_n_iyes) nhood->matrix[regpos].y.i=nhood->_n_iyget(iy);
      if (nhood->_n_iues) nhood->matrix[regpos].u.i=nhood->_n_iuget(iu);
      if (nhood->_n_ives) nhood->matrix[regpos].v.i=nhood->_n_ivget(iv);
      if (nhood->_n_axs) nhood->matrix[regpos].ax.i=nhood->_n_axget(ax);
      if (nhood->_n_ms) nhood->matrix[regpos].m=nhood->_n_mget(m);
      else nhood->matrix[regpos].m=1;
      if (!nhood->matrix[regpos].m) nhood->matrix[regpos].m=1;
    }
    regpos++;
    
    /* next line*/
    if (!repx) {
      if (xpos < (nhood->_n_inwidth-1)) {
	ia.u+=nhood->_n_ias;
	iy.u+=nhood->_n_iys;
	iu.u+=nhood->_n_ius;
	iv.u+=nhood->_n_ivs;
	ax.u+=nhood->_n_axs;
	m.u+=nhood->_n_ms;
      }
    }
    else {
      repx--;
    }
    xpos++;
  }
}

/******* SHIFT LEFT AND LOAD NEW LINE ****************************************/
void ShiftLeftLineLoad(TNhood *nhood)

{
  int regpos,regpos1;
  TCoor xpos,ypos;
  int xoffset,yoffset;
  long offset;
  MomImageData ia,iy,iu,iv,m,ax;
  int repx,repy;
  short blkcnty, blkcntx;
  short blkcntxstart,blkcntxstop;
  short blkcntystart,blkcntystop;

  nhood->_n_icntx--;
  nhood->_n_ia.u-=nhood->_n_ias;
  nhood->_n_iy.u-=nhood->_n_iys;
  nhood->_n_iu.u-=nhood->_n_ius;
  nhood->_n_iv.u-=nhood->_n_ivs;
  nhood->_n_ax.u-=nhood->_n_axs;
  nhood->_n_m.u-=nhood->_n_ms;
/*  if (matrixshift) {*/
    nhood->_n_cntx--;
    nhood->_n_ra.u-=nhood->_n_ras;
    nhood->_n_ry.u-=nhood->_n_rys;
    nhood->_n_ru.u-=nhood->_n_rus;
    nhood->_n_rv.u-=nhood->_n_rvs;
/*  }*/

  /* execute shift*/
  blkcntxstart=nhood->_n_xblkstart;
  blkcntxstop=nhood->_n_xblkstop;
  blkcntystart=nhood->_n_yblkstart;
  blkcntystop=nhood->_n_yblkstop;
  for (blkcnty=blkcntystart; blkcnty<= blkcntystop; blkcnty++) {
    for (blkcntx=blkcntxstop; blkcntx> blkcntxstart; blkcntx--) {
      regpos=_my[blkcnty]+blkcntx;

      regpos1=regpos-1;
      nhood->matrix[regpos].a=nhood->matrix[regpos1].a;
      nhood->matrix[regpos].y=nhood->matrix[regpos1].y;
      nhood->matrix[regpos].u=nhood->matrix[regpos1].u;
      nhood->matrix[regpos].v=nhood->matrix[regpos1].v;
      nhood->matrix[regpos].ax=nhood->matrix[regpos1].ax;
      nhood->matrix[regpos].m=nhood->matrix[regpos1].m;
    }
  }

  /* reload left culomn */
  xoffset=nhood->_n_xblksize;

  xpos=nhood->_n_icntx-xoffset;
  if (xpos < 0) {
    repx=-xpos;
    xoffset+=xpos;
  }
  else {
    repx=0;
  }

  /* invalid culomn*/
  if (repx > 0) {
    regpos=_my[blkcntystart]+blkcntxstart;
    for (blkcnty=blkcntystart; blkcnty<=blkcntystop; blkcnty++) {

      nhood->matrix[regpos].a.i=(nhood->matrix[regpos].a.i &
				 nhood->_n_amaska) | nhood->_n_amasko;
      nhood->matrix[regpos].y.i=(nhood->matrix[regpos].y.i &
				 nhood->_n_ymaska) | nhood->_n_ymasko;
      nhood->matrix[regpos].u.i=(nhood->matrix[regpos].u.i &
				 nhood->_n_umaska) | nhood->_n_umasko;
      nhood->matrix[regpos].v.i=(nhood->matrix[regpos].v.i &
				 nhood->_n_vmaska) | nhood->_n_vmasko;
      nhood->matrix[regpos].ax.i=(nhood->matrix[regpos].ax.i &
				  nhood->_n_axmaska) | nhood->_n_axmasko;
      nhood->matrix[regpos].m=0;
      regpos+=_my[1];
    }
    return;
  }
  
  /* init yoffset and yrepeat */
  yoffset=nhood->_n_yblksize;

  ypos=nhood->_n_icnty-yoffset;
  if (ypos < 0) {
    repy=-ypos;
    yoffset+=ypos;
  }
  else {
    repy=0;
  }


  /*local copy of imagedata pointer*/
  offset=yoffset*nhood->_n_inwidth+xoffset;
  ia.u=nhood->_n_ia.u-(offset<<_typesize[nhood->_n_iaes]);
  iy.u=nhood->_n_iy.u-(offset<<_typesize[nhood->_n_iyes]);
  iu.u=nhood->_n_iu.u-(offset<<_typesize[nhood->_n_iues]);
  iv.u=nhood->_n_iv.u-(offset<<_typesize[nhood->_n_ives]);
  ax.u=nhood->_n_ax.u-(offset<<_typesize[nhood->_n_axs]);
  m.u=nhood->_n_m.u-(offset<<_typesize[nhood->_n_ms]);

  /* valid column*/
  regpos=_my[blkcntystart]+blkcntxstart;
  for (blkcnty=blkcntystart; blkcnty<=blkcntystop; blkcnty++) {
    
    /* pixel in invalid range*/
    if (repy || (ypos >=nhood->_n_inheight)) { 
      if (nhood->_n_iaes) {
	nhood->matrix[regpos].a.i=(nhood->_n_iaget(ia) & nhood->_n_amaska) |
	  nhood->_n_amasko;
      }
      if (nhood->_n_iyes) {
	nhood->matrix[regpos].y.i=(nhood->_n_iyget(iy) & nhood->_n_ymaska) |
	  nhood->_n_ymasko;
      }
      if (nhood->_n_iues) {
	nhood->matrix[regpos].u.i=(nhood->_n_iuget(iu) & nhood->_n_umaska) |
	  nhood->_n_umasko;
      }
      if (nhood->_n_ives) {
	nhood->matrix[regpos].v.i=(nhood->_n_ivget(iv) & nhood->_n_vmaska) |
	  nhood->_n_vmasko;
      }
      if (nhood->_n_axs) {
	nhood->matrix[regpos].ax.i=(nhood->_n_axget(ax) & nhood->_n_axmaska) |
	  nhood->_n_axmasko;
      }
      nhood->matrix[regpos].m=0;
    }
    
    /*pixel in valid range*/
    else {
      if (nhood->_n_iaes) nhood->matrix[regpos].a.i=nhood->_n_iaget(ia);
      if (nhood->_n_iyes) nhood->matrix[regpos].y.i=nhood->_n_iyget(iy);
      if (nhood->_n_iues) nhood->matrix[regpos].u.i=nhood->_n_iuget(iu);
      if (nhood->_n_ives) nhood->matrix[regpos].v.i=nhood->_n_ivget(iv);
      if (nhood->_n_axs) nhood->matrix[regpos].ax.i=nhood->_n_axget(ax);
      if (nhood->_n_ms) nhood->matrix[regpos].m=nhood->_n_mget(m);
      else nhood->matrix[regpos].m=1;
      if (!nhood->matrix[regpos].m) nhood->matrix[regpos].m=1;
    }
    regpos+=_my[1];    
    
    
    /* next line*/
    if (!repy) {
      if (ypos < (nhood->_n_inheight-1)) {
	ia.u+=nhood->_n_ials;
	iy.u+=nhood->_n_iyls;
	iu.u+=nhood->_n_iuls;
	iv.u+=nhood->_n_ivls;
	ax.u+=nhood->_n_axls;
	m.u+=nhood->_n_mls;
      }
    }
    else {
      repy--;
    }
    ypos++;
  }
}

/******* SHIFT RIGHT AND LOAD NEW LINE ***************************************/
void ShiftRightLineLoad(TNhood *nhood)

{
  int regpos,regpos1;
  TCoor ypos;
  int xoffset,yoffset;
  long offset;
  MomImageData ia,iy,iu,iv,m,ax;
  int repy;
  short blkcnty, blkcntx;
  short blkcntxstart,blkcntxstop;
  short blkcntystart,blkcntystop;

  nhood->_n_icntx++;
  nhood->_n_ia.u+=nhood->_n_ias;
  nhood->_n_iy.u+=nhood->_n_iys;
  nhood->_n_iu.u+=nhood->_n_ius;
  nhood->_n_iv.u+=nhood->_n_ivs;
  nhood->_n_ax.u+=nhood->_n_axs;
  nhood->_n_m.u+=nhood->_n_ms;
/*  if (matrixshift) {*/
    nhood->_n_cntx++;
    nhood->_n_ra.u+=nhood->_n_ras;
    nhood->_n_ry.u+=nhood->_n_rys;
    nhood->_n_ru.u+=nhood->_n_rus;
    nhood->_n_rv.u+=nhood->_n_rvs;
/*  }*/


  /* execute shift*/
  blkcntxstart=nhood->_n_xblkstart;
  blkcntxstop=nhood->_n_xblkstop;
  blkcntystart=nhood->_n_yblkstart;
  blkcntystop=nhood->_n_yblkstop;
  for (blkcnty=blkcntystart; blkcnty<= blkcntystop; blkcnty++) {
    for (blkcntx=blkcntxstart; blkcntx< blkcntxstop; blkcntx++) {
      regpos=_my[blkcnty]+blkcntx;

      regpos1=regpos+1;
      nhood->matrix[regpos].a=nhood->matrix[regpos1].a;
      nhood->matrix[regpos].y=nhood->matrix[regpos1].y;
      nhood->matrix[regpos].u=nhood->matrix[regpos1].u;
      nhood->matrix[regpos].v=nhood->matrix[regpos1].v;
      nhood->matrix[regpos].ax=nhood->matrix[regpos1].ax;
      nhood->matrix[regpos].m=nhood->matrix[regpos1].m;
    }
  }

  /* reload right column */
  xoffset=nhood->_n_xblksize;

  /* invalid culomn*/
  if (nhood->_n_icntx+xoffset >= nhood->_n_inwidth) {
    regpos=_my[blkcntystart]+blkcntxstop;
    for (blkcnty=blkcntystart; blkcnty<=blkcntystop; blkcnty++) {

      nhood->matrix[regpos].a.i=(nhood->matrix[regpos].a.i &
				 nhood->_n_amaska) | nhood->_n_amasko;
      nhood->matrix[regpos].y.i=(nhood->matrix[regpos].y.i &
				 nhood->_n_ymaska) | nhood->_n_ymasko;
      nhood->matrix[regpos].u.i=(nhood->matrix[regpos].u.i &
				 nhood->_n_umaska) | nhood->_n_umasko;
      nhood->matrix[regpos].v.i=(nhood->matrix[regpos].v.i &
				 nhood->_n_vmaska) | nhood->_n_vmasko;
      nhood->matrix[regpos].ax.i=(nhood->matrix[regpos].ax.i &
				  nhood->_n_axmaska) | nhood->_n_axmasko;
      nhood->matrix[regpos].m=0;
      regpos+=_my[1];
    }
    return;
  }

  /* init yoffset and yrepeat */
  yoffset=nhood->_n_yblksize;

  ypos=nhood->_n_icnty-yoffset;
  if (ypos < 0) {
    repy=-ypos;
    yoffset+=ypos;
  }
  else {
    repy=0;
  }


  /*local copy of imagedata pointer*/
  offset=yoffset*nhood->_n_inwidth-xoffset;
  ia.u=nhood->_n_ia.u-(offset<<_typesize[nhood->_n_iaes]);
  iy.u=nhood->_n_iy.u-(offset<<_typesize[nhood->_n_iyes]);
  iu.u=nhood->_n_iu.u-(offset<<_typesize[nhood->_n_iues]);
  iv.u=nhood->_n_iv.u-(offset<<_typesize[nhood->_n_ives]);
  ax.u=nhood->_n_ax.u-(offset<<_typesize[nhood->_n_axs]);
  m.u=nhood->_n_m.u-(offset<<_typesize[nhood->_n_ms]);

  /* valid column*/
  regpos=_my[blkcntystart]+blkcntxstop;
  for (blkcnty=blkcntystart; blkcnty<=blkcntystop; blkcnty++) {

      /* pixel in invalid range*/
    if (repy || (ypos >=nhood->_n_inheight)) {
      if (nhood->_n_iaes) {
	nhood->matrix[regpos].a.i=(nhood->_n_iaget(ia) & nhood->_n_amaska) |
	  nhood->_n_amasko;
      }
      if (nhood->_n_iyes) {
	nhood->matrix[regpos].y.i=(nhood->_n_iyget(iy) & nhood->_n_ymaska) |
	  nhood->_n_ymasko;
      }
      if (nhood->_n_iues) {
	nhood->matrix[regpos].u.i=(nhood->_n_iuget(iu) & nhood->_n_umaska) |
	  nhood->_n_umasko;
      }
      if (nhood->_n_ives) {
	nhood->matrix[regpos].v.i=(nhood->_n_ivget(iv) & nhood->_n_vmaska) |
	  nhood->_n_vmasko;
      }
      if (nhood->_n_axs) {
	nhood->matrix[regpos].ax.i=(nhood->_n_axget(ax) & nhood->_n_axmaska) |
	  nhood->_n_axmasko;
      }
      nhood->matrix[regpos].m=0;
    }
    
    /*pixel in valid range*/
    else {
      if (nhood->_n_iaes) nhood->matrix[regpos].a.i=nhood->_n_iaget(ia);
      if (nhood->_n_iyes) nhood->matrix[regpos].y.i=nhood->_n_iyget(iy);
      if (nhood->_n_iues) nhood->matrix[regpos].u.i=nhood->_n_iuget(iu);
      if (nhood->_n_ives) nhood->matrix[regpos].v.i=nhood->_n_ivget(iv);
      if (nhood->_n_axs) nhood->matrix[regpos].ax.i=nhood->_n_axget(ax);
      if (nhood->_n_ms) nhood->matrix[regpos].m=nhood->_n_mget(m);
      else nhood->matrix[regpos].m=1;
      if (!nhood->matrix[regpos].m) nhood->matrix[regpos].m=1;
    }

    regpos+=_my[1];

    /* next line*/
    if (!repy) {
      if (ypos < (nhood->_n_inheight-1)) {
	ia.u+=nhood->_n_ials;
	iy.u+=nhood->_n_iyls;
	iu.u+=nhood->_n_iuls;
	iv.u+=nhood->_n_ivls;
	ax.u+=nhood->_n_axls;
	m.u+=nhood->_n_mls;
      }
    }
    else {
      repy--;
    }
    ypos++;
  }
}

/******* SHIFT UP AND LOAD NEW LINE WITH INPUT SUBSAMPLING *******************/
void ShiftUpSubLineLoad(TNhood *nhood)

{
  int irepcnty,irepcntx;
  char irepeatx,irepeaty,istepx,istepy;
  int yoffset,xoffset;
  long offset;
  MomImageData ia,iy,iu,iv;
  TCoor ypos,xpos;
  short regpos,regpos1;
  short invalidx,invalidy;
  short blkcnty, blkcntx;
  short blkcntxstart,blkcntxstop;
  short blkcntystart,blkcntystop;
  

  /* local copies of conversion parameters*/
  irepeatx=nhood->_n_irepeatx;
  irepeaty=nhood->_n_irepeaty;
  istepx=nhood->_n_istepx;
  istepy=nhood->_n_istepy;

  /* update pointer and counter*/
  nhood->_n_irepcnty--;
  if (nhood->_n_irepcnty<0) {
    nhood->_n_irepcnty=irepeaty-1;
    nhood->_n_icnty-=istepy;
    nhood->_n_ia.u-=nhood->_n_ials;
    nhood->_n_iy.u-=nhood->_n_iyls;
    nhood->_n_iu.u-=nhood->_n_iuls;
    nhood->_n_iv.u-=nhood->_n_ivls;
  }
  nhood->_n_cnty--;
  nhood->_n_ra.u-=nhood->_n_rals;
  nhood->_n_ry.u-=nhood->_n_ryls;
  nhood->_n_ru.u-=nhood->_n_ruls;
  nhood->_n_rv.u-=nhood->_n_rvls;

  /* shift data in array*/
  blkcntxstart=nhood->_n_xblkstart;
  blkcntxstop=nhood->_n_xblkstop;
  blkcntystart=nhood->_n_yblkstart;
  blkcntystop=nhood->_n_yblkstop;

  for (blkcnty=blkcntystop; blkcnty>blkcntystart; blkcnty--) {
    regpos1=_my[blkcnty]+blkcntxstart;
    regpos=regpos1-_my[1];
    for (blkcntx=blkcntxstart; blkcntx<=blkcntxstop; blkcntx++) {
      nhood->matrix[regpos1].a=nhood->matrix[regpos].a;
      nhood->matrix[regpos1].y=nhood->matrix[regpos].y;
      nhood->matrix[regpos1].u=nhood->matrix[regpos].u;
      nhood->matrix[regpos1].v=nhood->matrix[regpos].v;
      /*nhood->matrix[regpos1].ax=nhood->matrix[regpos].ax;*/
      /* not availibale with up-down-conversion*/
      regpos++;
      regpos1++;
    }
  }


  /* init subcnts, offsets and repeat values*/
  yoffset=(nhood->_n_yblksize/irepeaty)*istepy;
  irepcnty=nhood->_n_irepcnty-(nhood->_n_yblksize%irepeaty);

  if (irepcnty < 0) {
    yoffset+=istepy;
    irepcnty+=irepeaty;
  }

  ypos=nhood->_n_icnty-yoffset;
  if (ypos < nhood->_n_firsty) {
    yoffset+=ypos-nhood->_n_firsty;
  }

  xoffset=(nhood->_n_xblksize/irepeatx)*istepx;
  irepcntx=nhood->_n_irepcntx-(nhood->_n_xblksize%irepeatx);
  if (irepcntx < 0) {
    xoffset+=istepx;
    irepcntx+=irepeatx;
  }


  xpos=nhood->_n_icntx-xoffset;
  if (xpos < nhood->_n_firstx) {
    xoffset+=xpos-nhood->_n_firstx;
  }

  /*local copy of imagedata pointer*/
  offset=yoffset*nhood->_n_inwidth+xoffset;
  ia.u=nhood->_n_ia.u-(offset<<_typesize[nhood->_n_iaes]);
  iy.u=nhood->_n_iy.u-(offset<<_typesize[nhood->_n_iyes]);
  iu.u=nhood->_n_iu.u-(offset<<_typesize[nhood->_n_iues]);
  iv.u=nhood->_n_iv.u-(offset<<_typesize[nhood->_n_ives]);

  /* get matrix values */

  invalidy=irepcnty || (ypos<0);
  regpos1=_my[blkcntystart]+blkcntxstart;

  for (blkcntx=blkcntxstart; blkcntx<=blkcntxstop; blkcntx++) {

    /* pixel in invalid range*/
    invalidx=irepcntx || (xpos<0) || (xpos >= nhood->_n_inwidth);
    if (invalidx || invalidy) {
      /* pixel in invalid range*/

      if ((blkcntx!=blkcntxstart) && invalidx) {
	/* copy if not first pixel in row*/
	regpos=regpos1-1;
	nhood->matrix[regpos1].a.i=(nhood->matrix[regpos].a.i &
				  nhood->_n_amaska) | nhood->_n_amasko;
	nhood->matrix[regpos1].y.i=(nhood->matrix[regpos].y.i &
				  nhood->_n_ymaska) | nhood->_n_ymasko;
	nhood->matrix[regpos1].u.i=(nhood->matrix[regpos].u.i &
				  nhood->_n_umaska) | nhood->_n_umasko;
	nhood->matrix[regpos1].v.i=(nhood->matrix[regpos].v.i &
				  nhood->_n_vmaska) | nhood->_n_vmasko;
      }
      else {
	/* preload invalid first pixel of row*/
	if (nhood->_n_iaes) {
	  nhood->matrix[regpos1].a.i=(nhood->_n_iaget(ia) & 
				    nhood->_n_amaska) | nhood->_n_amasko;
	}
	if (nhood->_n_iyes) {
	  nhood->matrix[regpos1].y.i=(nhood->_n_iyget(iy) &
				    nhood->_n_ymaska) | nhood->_n_ymasko;
	}
	if (nhood->_n_iues) {
	  nhood->matrix[regpos1].u.i=(nhood->_n_iuget(iu) &
				    nhood->_n_umaska) | nhood->_n_umasko;
	}
	if (nhood->_n_ives) {
	  nhood->matrix[regpos1].v.i=(nhood->_n_ivget(iv) &
				    nhood->_n_vmaska) | nhood->_n_vmasko;
	}
      }
    }
	
	
    /*pixel in valid range*/
    else {
      if (nhood->_n_iaes) nhood->matrix[regpos1].a.i=nhood->_n_iaget(ia);
      if (nhood->_n_iyes) nhood->matrix[regpos1].y.i=nhood->_n_iyget(iy);
      if (nhood->_n_iues) nhood->matrix[regpos1].u.i=nhood->_n_iuget(iu);
      if (nhood->_n_ives) nhood->matrix[regpos1].v.i=nhood->_n_ivget(iv);
    }
    regpos1++;

    /* inc position*/
    irepcntx++;
    if (irepcntx == irepeatx) {
      if (xpos>= nhood->_n_firstx) {
	if (xpos < (nhood->_n_inwidth-1)) {
	  ia.u+=nhood->_n_ias;
	  iy.u+=nhood->_n_iys;
	  iu.u+=nhood->_n_ius;
	  iv.u+=nhood->_n_ivs;
	}
      }
      xpos+=istepx;
      irepcntx=0;
    }
  }
}
  
/******* SHIFT DOWN AND LOAD NEW LINE WITH INPUT SUBSAMPLING *****************/
void ShiftDownSubLineLoad(TNhood *nhood)

{
  int irepcnty,irepcntx;
  char irepeatx,irepeaty,istepx,istepy;
  int yoffset,xoffset;
  long offset;
  MomImageData ia,iy,iu,iv;
  TCoor ypos,xpos;
  short regpos,regpos1;
  short invalidx,invalidy;
  short blkcnty, blkcntx;
  short blkcntxstart,blkcntxstop;
  short blkcntystart,blkcntystop;
  
  /* local copies of conversion parameters*/
  irepeatx=nhood->_n_irepeatx;
  irepeaty=nhood->_n_irepeaty;
  istepx=nhood->_n_istepx;
  istepy=nhood->_n_istepy;

  /* update pointer and counter*/
  nhood->_n_irepcnty++;
  if (nhood->_n_irepcnty==irepeaty) {
    nhood->_n_irepcnty=0;
    nhood->_n_icnty+=istepy;
    nhood->_n_ia.u+=nhood->_n_ials;
    nhood->_n_iy.u+=nhood->_n_iyls;
    nhood->_n_iu.u+=nhood->_n_iuls;
    nhood->_n_iv.u+=nhood->_n_ivls;
  }
  nhood->_n_cnty++;
  nhood->_n_ra.u+=nhood->_n_rals;
  nhood->_n_ry.u+=nhood->_n_ryls;
  nhood->_n_ru.u+=nhood->_n_ruls;
  nhood->_n_rv.u+=nhood->_n_rvls;

  /* shift data in array*/
  blkcntxstart=nhood->_n_xblkstart;
  blkcntxstop=nhood->_n_xblkstop;
  blkcntystart=nhood->_n_yblkstart;
  blkcntystop=nhood->_n_yblkstop;
    
  for (blkcnty=blkcntystart; blkcnty< blkcntystop; blkcnty++) {
    regpos1=_my[blkcnty]+blkcntxstart;
    regpos=regpos1+_my[1];
    for (blkcntx=blkcntxstart; blkcntx<=blkcntxstop; blkcntx++) {
      nhood->matrix[regpos1].a=nhood->matrix[regpos].a;
      nhood->matrix[regpos1].y=nhood->matrix[regpos].y;
      nhood->matrix[regpos1].u=nhood->matrix[regpos].u;
      nhood->matrix[regpos1].v=nhood->matrix[regpos].v;
      /*nhood->matrix[regpos1].ax=nhood->matrix[regpos].ax;*/
      /* not availibale with up-down-conversion*/
      regpos++;
      regpos1++;
    }
  }


  /* init subcnts, offsets and repeat values*/
  yoffset=(nhood->_n_yblksize/irepeaty)*istepy;
  irepcnty=nhood->_n_irepcnty+(nhood->_n_yblksize%irepeaty);

  if (irepcnty >= irepeaty) {
    yoffset+=istepy;
    irepcnty-=irepeaty;
  }

  ypos=nhood->_n_icnty+yoffset;

  xoffset=(nhood->_n_xblksize/irepeatx)*istepx;
  irepcntx=nhood->_n_irepcntx-(nhood->_n_xblksize%irepeatx);
  if (irepcntx < 0) {
    xoffset+=istepx;
    irepcntx+=irepeatx;
  }


  xpos=nhood->_n_icntx-xoffset;
  if (xpos < nhood->_n_firstx) {
    xoffset+=xpos-nhood->_n_firstx;
  }

  /*local copy of imagedata pointer*/
  offset=yoffset*nhood->_n_inwidth-xoffset;
  ia.u=nhood->_n_ia.u+(offset<<_typesize[nhood->_n_iaes]);
  iy.u=nhood->_n_iy.u+(offset<<_typesize[nhood->_n_iyes]);
  iu.u=nhood->_n_iu.u+(offset<<_typesize[nhood->_n_iues]);
  iv.u=nhood->_n_iv.u+(offset<<_typesize[nhood->_n_ives]);

  /* get matrix values */
  invalidy=irepcnty || (ypos >= nhood->_n_inheight);
  regpos1=_my[blkcntystop]+blkcntxstart;

  /* invalid row*/
  if (invalidy && (blkcntystop!=2)) {
    
    /* copy if not first row*/
      regpos=regpos1-_my[1];
      for (blkcntx=blkcntxstart; blkcntx<=blkcntystop; blkcntx++) {
	nhood->matrix[regpos1].a.i=(nhood->matrix[regpos].a.i &
				    nhood->_n_amaska) | nhood->_n_amasko;
	nhood->matrix[regpos1].y.i=(nhood->matrix[regpos].y.i &
				    nhood->_n_ymaska) | nhood->_n_ymasko;
	nhood->matrix[regpos1].u.i=(nhood->matrix[regpos].u.i &
				    nhood->_n_umaska) | nhood->_n_umasko;
	nhood->matrix[regpos1].v.i=(nhood->matrix[regpos].v.i &
				    nhood->_n_vmaska) | nhood->_n_vmasko;
	/*nhood->matrix[regpos1].ax.i=nhood->matrix[regpos].ax.i & 
	  nhood->_n_axmaska | nhood->_n_axmasko;*/
	/* not availibale with up-down-conversion*/
	regpos++;
	regpos1++;
      }
    }

  else {

    /*valid row*/
    for (blkcntx=blkcntxstart; blkcntx<=blkcntxstop; blkcntx++) {
      
      /* pixel in invalid range*/
      invalidx=irepcntx || (xpos<0) || (xpos >= nhood->_n_inwidth);
      if (invalidx || invalidy) {
	if ((blkcntx!=blkcntxstart) && invalidx) {
          /* copy if not firts pixel in row*/
	  regpos=regpos1-1;
	  nhood->matrix[regpos1].a.i=(nhood->matrix[regpos].a.i &
				    nhood->_n_amaska) | nhood->_n_amasko;
	  nhood->matrix[regpos1].y.i=(nhood->matrix[regpos].y.i &
				    nhood->_n_ymaska) | nhood->_n_ymasko;
	  nhood->matrix[regpos1].u.i=(nhood->matrix[regpos].u.i &
				    nhood->_n_umaska) | nhood->_n_umasko;
	  nhood->matrix[regpos1].v.i=(nhood->matrix[regpos].v.i &
				    nhood->_n_vmaska) | nhood->_n_vmasko;
	}
	else {
          /* preload invalid first pixel of row*/
	  if (nhood->_n_iaes) {
	    nhood->matrix[regpos1].a.i=(nhood->_n_iaget(ia) & 
				      nhood->_n_amaska) | nhood->_n_amasko;
	  }
	  if (nhood->_n_iyes) {
	    nhood->matrix[regpos1].y.i=(nhood->_n_iyget(iy) &
				      nhood->_n_ymaska) | nhood->_n_ymasko;
	  }
	  if (nhood->_n_iues) {
	    nhood->matrix[regpos1].u.i=(nhood->_n_iuget(iu) &
				      nhood->_n_umaska) | nhood->_n_umasko;
	  }
	  if (nhood->_n_ives) {
	    nhood->matrix[regpos1].v.i=(nhood->_n_ivget(iv) &
				      nhood->_n_vmaska) | nhood->_n_vmasko;
	  }
	}
      }
	
      /*pixel in valid range*/
      else {
	if (nhood->_n_iaes) nhood->matrix[regpos1].a.i=nhood->_n_iaget(ia);
	if (nhood->_n_iyes) nhood->matrix[regpos1].y.i=nhood->_n_iyget(iy);
	if (nhood->_n_iues) nhood->matrix[regpos1].u.i=nhood->_n_iuget(iu);
	if (nhood->_n_ives) nhood->matrix[regpos1].v.i=nhood->_n_ivget(iv);
      }
      regpos1++;

      /* inc position*/
      irepcntx++;
      if (irepcntx == irepeatx) {
	if (xpos>= nhood->_n_firstx) {
	  if (xpos < (nhood->_n_inwidth-1)) {
	    ia.u+=nhood->_n_ias;
	    iy.u+=nhood->_n_iys;
	    iu.u+=nhood->_n_ius;
	    iv.u+=nhood->_n_ivs;
	  }
	}
	xpos+=istepx;
	irepcntx=0;
      }
    }
  }
}

/******* SHIFT LEFT AND LOAD NEW LINE WITH INPUT SUBSAMPLING *****************/
void ShiftLeftSubLineLoad(TNhood *nhood)

{
  int irepcnty,irepcntx;
  char irepeatx,irepeaty,istepx,istepy;
  int yoffset,xoffset;
  long offset;
  MomImageData ia,iy,iu,iv;
  TCoor ypos,xpos;
  short regpos,regpos1;
  short invalidx,invalidy;
  short blkcnty, blkcntx;
  short blkcntxstart,blkcntxstop;
  short blkcntystart,blkcntystop;
  
  /* local copies of conversion parameters*/
  irepeatx=nhood->_n_irepeatx;
  irepeaty=nhood->_n_irepeaty;
  istepx=nhood->_n_istepx;
  istepy=nhood->_n_istepy;

  /* update pointer and counter*/
  nhood->_n_irepcntx--;
  if (nhood->_n_irepcntx<0) {
    nhood->_n_irepcntx=irepeatx-1;
    nhood->_n_icntx-=istepx;
    nhood->_n_ia.u-=nhood->_n_ias;
    nhood->_n_iy.u-=nhood->_n_iys;
    nhood->_n_iu.u-=nhood->_n_ius;
    nhood->_n_iv.u-=nhood->_n_ivs;
  }
  nhood->_n_cntx--;
  nhood->_n_ra.u-=nhood->_n_ras;
  nhood->_n_ry.u-=nhood->_n_rys;
  nhood->_n_ru.u-=nhood->_n_rus;
  nhood->_n_rv.u-=nhood->_n_rvs;

  /* shift data in array*/
  blkcntxstart=nhood->_n_xblkstart;
  blkcntxstop=nhood->_n_xblkstop;
  blkcntystart=nhood->_n_yblkstart;
  blkcntystop=nhood->_n_yblkstop;
    
  for (blkcntx=blkcntxstop; blkcntx>blkcntxstart; blkcntx--) {
    regpos1=blkcntx+_my[blkcntystart];
    regpos=regpos1-1;
    for (blkcnty=blkcntystart; blkcnty<=blkcntystop; blkcnty++) {
      nhood->matrix[regpos1].a=nhood->matrix[regpos].a;
      nhood->matrix[regpos1].y=nhood->matrix[regpos].y;
      nhood->matrix[regpos1].u=nhood->matrix[regpos].u;
      nhood->matrix[regpos1].v=nhood->matrix[regpos].v;
      /*nhood->matrix[regpos1].ax=nhood->matrix[regpos].ax;*/
      /* not availibale with up-down-conversion*/
      regpos+=_my[1];
      regpos1+=_my[1];
    }
  }


  /* init subcnts, offsets and repeat values*/
  xoffset=(nhood->_n_xblksize/irepeatx)*istepx;
  irepcntx=nhood->_n_irepcntx-(nhood->_n_xblksize%irepeatx);

  if (irepcntx < 0) {
    xoffset+=istepx;
    irepcntx+=irepeatx;
  }

  xpos=nhood->_n_icntx-xoffset;
  if (xpos <  nhood->_n_firstx) {
    xoffset+=xpos-nhood->_n_firstx;
  }

  yoffset=(nhood->_n_yblksize/irepeaty)*istepy;
  irepcnty=nhood->_n_irepcnty-(nhood->_n_yblksize%irepeaty);
  if (irepcnty < 0) {
    yoffset+=istepy;
    irepcnty+=irepeaty;
  }


  ypos=nhood->_n_icnty-yoffset;
  if (ypos <  nhood->_n_firsty) {
    yoffset+=ypos-nhood->_n_firsty;
  }

  /*local copy of imagedata pointer*/
  offset=xoffset+yoffset*nhood->_n_inwidth;
  ia.u=nhood->_n_ia.u-(offset<<_typesize[nhood->_n_iaes]);
  iy.u=nhood->_n_iy.u-(offset<<_typesize[nhood->_n_iyes]);
  iu.u=nhood->_n_iu.u-(offset<<_typesize[nhood->_n_iues]);
  iv.u=nhood->_n_iv.u-(offset<<_typesize[nhood->_n_ives]);

  /* get matrix values */

  invalidx=irepcntx || (xpos<0);
  regpos1=_my[blkcntystart]+blkcntxstart;

  for (blkcnty=blkcntystart; blkcnty<=blkcntystop; blkcnty++) {

    invalidy=irepcnty || (ypos<0) || (ypos >= nhood->_n_inheight);
    if (invalidx || invalidy) {
      /* pixel in invalid range*/

      if ((blkcnty!=blkcntystart) && invalidy) {
	/* copy if not top pixel in culomn*/
	regpos=regpos1-_my[1];
	nhood->matrix[regpos1].a.i=(nhood->matrix[regpos].a.i &
				  nhood->_n_amaska) | nhood->_n_amasko;
	nhood->matrix[regpos1].y.i=(nhood->matrix[regpos].y.i &
				  nhood->_n_ymaska) | nhood->_n_ymasko;
	nhood->matrix[regpos1].u.i=(nhood->matrix[regpos].u.i &
				  nhood->_n_umaska) | nhood->_n_umasko;
	nhood->matrix[regpos1].v.i=(nhood->matrix[regpos].v.i &
				  nhood->_n_vmaska) | nhood->_n_vmasko;
      }
      else {
	/* preload invalid top pixel of culomn*/
	if (nhood->_n_iaes) {
	  nhood->matrix[regpos1].a.i=(nhood->_n_iaget(ia) & 
				    nhood->_n_amaska) | nhood->_n_amasko;
	}
	if (nhood->_n_iyes) {
	  nhood->matrix[regpos1].y.i=(nhood->_n_iyget(iy) &
				    nhood->_n_ymaska) | nhood->_n_ymasko;
	}
	if (nhood->_n_iues) {
	  nhood->matrix[regpos1].u.i=(nhood->_n_iuget(iu) &
				    nhood->_n_umaska) | nhood->_n_umasko;
	}
	if (nhood->_n_ives) {
	  nhood->matrix[regpos1].v.i=(nhood->_n_ivget(iv) &
				    nhood->_n_vmaska) | nhood->_n_vmasko;
	}
      }
    }
	
    /*pixel in valid range*/
    else {
      if (nhood->_n_iaes) nhood->matrix[regpos1].a.i=nhood->_n_iaget(ia);
      if (nhood->_n_iyes) nhood->matrix[regpos1].y.i=nhood->_n_iyget(iy);
      if (nhood->_n_iues) nhood->matrix[regpos1].u.i=nhood->_n_iuget(iu);
      if (nhood->_n_ives) nhood->matrix[regpos1].v.i=nhood->_n_ivget(iv);
    }
    regpos1+=_my[1];

    /* inc position*/
    irepcnty++;
    if (irepcnty == irepeaty) {
      if (ypos>= nhood->_n_firsty) {
	if (ypos < (nhood->_n_inheight-1)) {
	  ia.u+=nhood->_n_ials;
	  iy.u+=nhood->_n_iyls;
	  iu.u+=nhood->_n_iuls;
	  iv.u+=nhood->_n_ivls;
	}
      }
      ypos+=istepy;
      irepcnty=0;
    }
  }
}


/******* SHIFT RIGHT AND LOAD NEW LINE WITH INPUT SUBSAMPLING ****************/
void ShiftRightSubLineLoad(TNhood *nhood)

{
  int irepcntx,irepcnty;
  char irepeaty,irepeatx,istepy,istepx;
  int xoffset,yoffset;
  long offset;
  MomImageData ia,iy,iu,iv;
  TCoor xpos,ypos;
  short regpos,regpos1;
  short invalidx,invalidy;
  short blkcntx, blkcnty;
  short blkcntystart,blkcntystop;
  short blkcntxstart,blkcntxstop;
  
  /* local copies of conversion parameters*/
  irepeaty=nhood->_n_irepeaty;
  irepeatx=nhood->_n_irepeatx;
  istepy=nhood->_n_istepy;
  istepx=nhood->_n_istepx;

  /* update pointer and counter*/
  nhood->_n_irepcntx++;
  if (nhood->_n_irepcntx==irepeatx) {
    nhood->_n_irepcntx=0;
    nhood->_n_icntx+=istepx;
    nhood->_n_ia.u+=nhood->_n_ias;
    nhood->_n_iy.u+=nhood->_n_iys;
    nhood->_n_iu.u+=nhood->_n_ius;
    nhood->_n_iv.u+=nhood->_n_ivs;
  }
  nhood->_n_cntx++;
  nhood->_n_ra.u+=nhood->_n_ras;
  nhood->_n_ry.u+=nhood->_n_rys;
  nhood->_n_ru.u+=nhood->_n_rus;
  nhood->_n_rv.u+=nhood->_n_rvs;

  /* shift data in array*/
  blkcntxstart=nhood->_n_xblkstart;
  blkcntxstop=nhood->_n_xblkstop;
  blkcntystart=nhood->_n_yblkstart;
  blkcntystop=nhood->_n_yblkstop;
    
  for (blkcntx=blkcntxstart; blkcntx< blkcntxstop; blkcntx++) {
    regpos1=blkcntx+_my[blkcntystart];
    regpos=regpos1+1;
    for (blkcnty=blkcntystart; blkcnty<=blkcntxstop; blkcnty++) {
      nhood->matrix[regpos1].a=nhood->matrix[regpos].a;
      nhood->matrix[regpos1].y=nhood->matrix[regpos].y;
      nhood->matrix[regpos1].u=nhood->matrix[regpos].u;
      nhood->matrix[regpos1].v=nhood->matrix[regpos].v;
      /*nhood->matrix[regpos1].ax=nhood->matrix[regpos].ax;*/
      /* not availibale with up-down-conversion*/
      regpos+=_my[1];
      regpos1+=_my[1];
    }
  }


  /* init subcnts, offsets and repeat values*/
  xoffset=(nhood->_n_xblksize/irepeatx)*istepx;
  irepcntx=nhood->_n_irepcntx+(nhood->_n_xblksize%irepeatx);

  if (irepcntx >= irepeatx) {
    xoffset+=istepx;
    irepcntx-=irepeatx;
  }

  xpos=nhood->_n_icntx+xoffset;

  yoffset=(nhood->_n_yblksize/irepeaty)*istepy;
  irepcnty=nhood->_n_irepcnty-(nhood->_n_yblksize%irepeaty);
  if (irepcnty < 0) {
    yoffset+=istepy;
    irepcnty+=irepeaty;
  }

  ypos=nhood->_n_icnty-yoffset;
  if (ypos <  nhood->_n_firsty) {
    yoffset+=ypos-nhood->_n_firsty;
  }

  /*local copy of imagedata pointer*/
  offset=xoffset-yoffset*nhood->_n_inwidth;
  ia.u=nhood->_n_ia.u+(offset<<_typesize[nhood->_n_iaes]);
  iy.u=nhood->_n_iy.u+(offset<<_typesize[nhood->_n_iyes]);
  iu.u=nhood->_n_iu.u+(offset<<_typesize[nhood->_n_iues]);
  iv.u=nhood->_n_iv.u+(offset<<_typesize[nhood->_n_ives]);

  /* get matrix values */

  invalidx=irepcntx || (xpos >= nhood->_n_inwidth);
  regpos1=_my[blkcntystart]+blkcntxstop;

  /* invalid culonm*/
  if (invalidx && (blkcntxstop!=2)) {

    /* copy if not first culomn*/
    regpos=regpos1-1;
    for (blkcnty=blkcntystart; blkcnty<=blkcntystop; blkcnty++) {
      nhood->matrix[regpos1].a.i=(nhood->matrix[regpos].a.i &
				  nhood->_n_amaska) | nhood->_n_amasko;
      nhood->matrix[regpos1].y.i=(nhood->matrix[regpos].y.i &
				  nhood->_n_ymaska) | nhood->_n_ymasko;
      nhood->matrix[regpos1].u.i=(nhood->matrix[regpos].u.i &
				  nhood->_n_umaska) | nhood->_n_umasko;
      nhood->matrix[regpos1].v.i=(nhood->matrix[regpos].v.i &
				  nhood->_n_vmaska) | nhood->_n_vmasko;
      /*nhood->matrix[regpos1].ax.i=nhood->matrix[regpos].ax & 
	nhood->_n_aymaska) | nhood->_n_aymasko;*/
      /* not availibale with up-down-conversion*/
      regpos+=_my[1];
      regpos1+=_my[1];
    }
  }
  else {

    /*valid culomn*/
    for (blkcnty=blkcntystart; blkcnty<=blkcntystop; blkcnty++) {

      /* pixel in invalid range*/
      invalidy=irepcnty || (ypos<0) || (ypos >= nhood->_n_inheight);
      if (invalidx || invalidy) {
	if ((blkcnty!=blkcntystart) && invalidy) {
          /* copy if not top pixel in culomn*/
	  regpos=regpos1-_my[1];
	  nhood->matrix[regpos1].a.i=(nhood->matrix[regpos].a.i &
				    nhood->_n_amaska) | nhood->_n_amasko;
	  nhood->matrix[regpos1].y.i=(nhood->matrix[regpos].y.i &
				    nhood->_n_ymaska) | nhood->_n_ymasko;
	  nhood->matrix[regpos1].u.i=(nhood->matrix[regpos].u.i &
				    nhood->_n_umaska) | nhood->_n_umasko;
	  nhood->matrix[regpos1].v.i=(nhood->matrix[regpos].v.i &
				    nhood->_n_vmaska) | nhood->_n_vmasko;
	}
	else {
          /* preload invalid top pixel of culomn*/
	  if (nhood->_n_iaes) {
	    nhood->matrix[regpos1].a.i=(nhood->_n_iaget(ia) & 
				      nhood->_n_amaska) | nhood->_n_amasko;
	  }
	  if (nhood->_n_iyes) {
	    nhood->matrix[regpos1].y.i=(nhood->_n_iyget(iy) &
				      nhood->_n_ymaska) | nhood->_n_ymasko;
	  }
	  if (nhood->_n_iues) {
	    nhood->matrix[regpos1].u.i=(nhood->_n_iuget(iu) &
				      nhood->_n_umaska) | nhood->_n_umasko;
	  }
	  if (nhood->_n_ives) {
	    nhood->matrix[regpos1].v.i=(nhood->_n_ivget(iv) &
				      nhood->_n_vmaska) | nhood->_n_vmasko;
	  }
	}
      }
	
      /*pixel in valid range*/
      else {
	if (nhood->_n_iaes) nhood->matrix[regpos1].a.i=nhood->_n_iaget(ia);
	if (nhood->_n_iyes) nhood->matrix[regpos1].y.i=nhood->_n_iyget(iy);
	if (nhood->_n_iues) nhood->matrix[regpos1].u.i=nhood->_n_iuget(iu);
	if (nhood->_n_ives) nhood->matrix[regpos1].v.i=nhood->_n_ivget(iv);
      }
      regpos1+=_my[1];

      /* inc position*/
      irepcnty++;
      if (irepcnty == irepeaty) {
	if (ypos>= nhood->_n_firsty) {
	  if (ypos < (nhood->_n_inheight-1)) {
	    ia.u+=nhood->_n_ials;
	    iy.u+=nhood->_n_iyls;
	    iu.u+=nhood->_n_iuls;
	    iv.u+=nhood->_n_ivls;
	  }
	}
	ypos+=istepy;
	irepcnty=0;
      }
    }
  }
}

/******* PRINT A WARNING AND EXIT IF DESIRED ********************************/
void addlib_stop_here(char *message,int doexit, int exitcode)

{
  fprintf(stderr,message);

  if (doexit) exit(exitcode);
}
