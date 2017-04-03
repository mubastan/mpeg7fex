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
/* Filename :    $RCSfile: intra.c,v $
   Version  :    Revision: 1.10
   Last Edit:    Date: 2002/01/15 17:37:07
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
// Applicable File Name:  intra.c
//
*/

/******* INCLUDES ************************************************************/
#include <stdio.h>

#include "vopio.h"
#include "intra.h"
#ifdef EVAL
  #include "evaladdr.h"
  #include "eval.h"
#endif
#ifdef EVAL
  #include "pptrace.h"
#endif

/******* DEFINES *************************************************************/

/******* TYPES ***************************************************************/

/******* VARIABLES ***********************************************************/
TChan _intra_regin1=0,_intra_regin2=0,_intra_regin3=0,_intra_regin4=0;
TChan _intra_regin5=0;
TChan _intra_regout1=0,_intra_regout2=0,_intra_regout3=0,_intra_regout4=0;
void *_intra_infopt=0;


/******* FUNCTIONS ***********************************************************/

/******* Code ****************************************************************/

/* central function for intra related functions*/
void intra_proc(MomVop *resvop, MomVop *invop,MomImage *aux,
	       int reschanels,int inchanels,
	       int connect,int scanmode,
	       TChan ahold,TChan yhold,TChan uhold,TChan vhold,TChan axhold,
	       TCoor winx,TCoor winy,TCoor resposx,TCoor resposy,
	       TCoor inposx,TCoor inposy,TSizeConv inconv,
	       void (*operation)(TPixel *,TPixel *, TCoor, TCoor))

{
  TCoor lineend;
  void (*pre)(TNhood *);
  void (*right)(TNhood *);
  void (*down)(TNhood *);
  void (*left)(TNhood *);
  void (*up)(TNhood *);
  void (*defaultdir)(TNhood *);
  TNhood regs;

#ifdef EVAL
  eval_BeginMeasureDur();
  eval_BeginAddrMode(SECTION_INTRA);
  eval_BeginRecMode(invop,resvop,inchanels,reschanels);
  eval_PictSize(invop,NULL,resvop,aux);
  eval_BeginScan(scanmode);
  eval_ScanWindow(winx,winy,inposx,inposy,0,0,resposx,resposy);
  eval_BeginConSC(connect);
  eval_BeginOpr((void*)operation);
  eval_StartAddr();
#endif

#ifdef PPTRACE
  PPTrace_StartOfTool();
  PPTrace_LogType((void*)operation,reschanels,connect);
  PPTrace_LogIntraReg(1);
#endif

  ConstructNhood (&regs,resvop,invop,aux,0,0,
	      reschanels,inchanels,connect,scanmode,
	      ahold,yhold,uhold,vhold,axhold,
	      winx,winy,resposx,resposy,inposx,inposy,inconv);
  if (regs.error) {
    DestructNhood(&regs);
    return;
  }

  /* select shift functions*/
  if ((regs._n_irepeatx | regs._n_irepeaty |
       regs._n_istepx | regs._n_istepy) > 1) {
    pre=PreloadSub;
    right=ShiftRightSubLineLoad;
    down=ShiftDownSubLineLoad;
    left=ShiftLeftSubLineLoad;
    up=ShiftUpSubLineLoad;
  }
  else {
    pre=Preload;
    right=ShiftRightLineLoad;
    down=ShiftDownLineLoad;
    left=ShiftLeftLineLoad;
    up=ShiftUpLineLoad;
  }



  /* loops for imagescan*/
  switch (scanmode) {
  case SCAN_HO:

    regs._n_cnty=resposy;
    regs._n_icnty=inposy;
    regs._n_irepcnty=regs._n_stirepcnty;

    regs._n_cntx=resposx;
    regs._n_icntx=inposx;
    regs._n_irepcntx=regs._n_stirepcntx; /*must be set before preload*/

    regs._n_ia.u=regs.stia.u;
    regs._n_iy.u=regs.stiy.u;
    regs._n_iu.u=regs.stiu.u;
    regs._n_iv.u=regs.stiv.u;
    regs._n_ax.u=regs.stax.u;
    regs._n_ra.u=regs.stra.u;
    regs._n_ry.u=regs.stry.u;
    regs._n_ru.u=regs.stru.u;
    regs._n_rv.u=regs.strv.u;

    /*preload linestart*/
#ifdef EVAL
    eval_LogPreload();
#endif
    pre(&regs);

    lineend= regs._n_maxx;
    while (1) {

#ifdef EVAL
      eval_LogOpr(regs._n_cntx,regs._n_cnty);
#endif
      /* execute embedded function*/
#ifdef PPTRACE
      PPTrace_LogIntraIn(regs.matrix, regs._n_cntx,regs._n_cnty);
#endif
      operation(&regs.result,regs.matrix,regs._n_cntx,regs._n_cnty);
#ifdef PPTRACE
      PPTrace_LogIntraOut(regs.result);
#endif

      if (regs._n_raes) regs._n_raset(regs._n_ra,regs.result.a.i);
      if (regs._n_ryes) regs._n_ryset(regs._n_ry,regs.result.y.i);
      if (regs._n_rues) regs._n_ruset(regs._n_ru,regs.result.u.i);
      if (regs._n_rves) regs._n_rvset(regs._n_rv,regs.result.v.i);
      if (regs._n_axs) regs._n_axset(regs._n_ax,regs.result.ax.i);

      if (regs._n_cntx==lineend) {

	/*check for frame end*/
	if (regs._n_cnty == regs._n_maxy) break;

	/* go to next result line*/
	regs._n_cnty++;

	regs.stra.u+=regs._n_rals;
	regs.stry.u+=regs._n_ryls;
	regs.stru.u+=regs._n_ruls;
	regs.strv.u+=regs._n_rvls;
	regs.stax.u+=regs._n_axls;

	/* go to input line start*/
	regs._n_irepcnty++;
	if (regs._n_irepcnty==regs._n_irepeaty) {
	  regs._n_irepcnty=0;
	  regs._n_icnty+=regs._n_istepy;
	  regs.stia.u+=regs._n_ials;
	  regs.stiy.u+=regs._n_iyls;
	  regs.stiu.u+=regs._n_iuls;
	  regs.stiv.u+=regs._n_ivls;
	}

	/* reset x counters and set pointer to linestart*/
	regs._n_cntx=resposx;
	regs._n_icntx=inposx;
	regs._n_irepcntx=regs._n_stirepcntx; /*must be set before preload*/

	regs._n_ia.u=regs.stia.u;
	regs._n_iy.u=regs.stiy.u;
	regs._n_iu.u=regs.stiu.u;
	regs._n_iv.u=regs.stiv.u;
	regs._n_ax.u=regs.stax.u;
	regs._n_ra.u=regs.stra.u;
	regs._n_ry.u=regs.stry.u;
	regs._n_ru.u=regs.stru.u;
	regs._n_rv.u=regs.strv.u;

	/*preload linestart*/
#ifdef EVAL
  eval_LogPreload();
#endif
	pre(&regs);

      }
      else {
	right(&regs);
      }

    }
    break;
  case SCAN_VE:

    regs._n_cntx=resposx;
    regs._n_icntx=inposx;
    regs._n_irepcntx=regs._n_stirepcntx;

    regs._n_cnty=resposy;
    regs._n_icnty=inposy;
    regs._n_irepcnty=regs._n_stirepcnty; /*must be set before preload*/

    regs._n_ia.u=regs.stia.u;
    regs._n_iy.u=regs.stiy.u;
    regs._n_iu.u=regs.stiu.u;
    regs._n_iv.u=regs.stiv.u;
    regs._n_ax.u=regs.stax.u;
    regs._n_ra.u=regs.stra.u;
    regs._n_ry.u=regs.stry.u;
    regs._n_ru.u=regs.stru.u;
    regs._n_rv.u=regs.strv.u;


    /*preload linestart*/
#ifdef EVAL
    eval_LogPreload();
#endif
    pre(&regs);

    lineend= regs._n_maxy;
    while (1) {


#ifdef EVAL
      eval_LogOpr(regs._n_cntx,regs._n_cnty);
#endif
      /* execute embedded function*/
#ifdef PPTRACE
      PPTrace_LogIntraIn(regs.matrix, regs._n_cntx,regs._n_cnty);
#endif
      operation(&regs.result,regs.matrix,regs._n_cntx,regs._n_cnty);
#ifdef PPTRACE
      PPTrace_LogIntraOut(regs.result);
#endif

      if (regs._n_raes) regs._n_raset(regs._n_ra,regs.result.a.i);
      if (regs._n_ryes) regs._n_ryset(regs._n_ry,regs.result.y.i);
      if (regs._n_rues) regs._n_ruset(regs._n_ru,regs.result.u.i);
      if (regs._n_rves) regs._n_rvset(regs._n_rv,regs.result.v.i);
      if (regs._n_axs) regs._n_axset(regs._n_ax,regs.result.ax.i);

      if (regs._n_cnty==lineend) {

	/*check for frame end*/
	if (regs._n_cntx == regs._n_maxx) break;

	/* go to next result line*/
	regs._n_cntx++;

	regs.stra.u+=regs._n_ras;
	regs.stry.u+=regs._n_rys;
	regs.stru.u+=regs._n_rus;
	regs.strv.u+=regs._n_rvs;
	regs.stax.u+=regs._n_axs;

	/* go to input line start*/
	regs._n_irepcntx++;
	if (regs._n_irepcntx==regs._n_irepeatx) {
	  regs._n_irepcntx=0;
	  regs._n_icntx+=regs._n_istepx;
	  regs.stia.u+=regs._n_ias;
	  regs.stiy.u+=regs._n_iys;
	  regs.stiu.u+=regs._n_ius;
	  regs.stiv.u+=regs._n_ivs;
	}

	/* reset x counters and set pointer to linestart*/
	regs._n_cnty=resposy;
	regs._n_icnty=inposy;
	regs._n_irepcnty=regs._n_stirepcnty; /*must be set before preload*/

	regs._n_ia.u=regs.stia.u;
	regs._n_iy.u=regs.stiy.u;
	regs._n_iu.u=regs.stiu.u;
	regs._n_iv.u=regs.stiv.u;
	regs._n_ax.u=regs.stax.u;
	regs._n_ra.u=regs.stra.u;
	regs._n_ry.u=regs.stry.u;
	regs._n_ru.u=regs.stru.u;
	regs._n_rv.u=regs.strv.u;

	/*preload linestart*/
#ifdef EVAL
    eval_LogPreload();
#endif
	pre(&regs);

      }
      else {
	down(&regs);
      }

    }
    break;
  case SCAN_HM:
    regs._n_cnty=resposy;
    regs._n_icnty=inposy;
    regs._n_irepcnty=regs._n_stirepcnty;

    regs._n_cntx=resposx;
    regs._n_icntx=inposx;
    regs._n_irepcntx=regs._n_stirepcntx;

    regs._n_ia.u=regs.stia.u;
    regs._n_iy.u=regs.stiy.u;
    regs._n_iu.u=regs.stiu.u;
    regs._n_iv.u=regs.stiv.u;
    regs._n_ax.u=regs.stax.u;
    regs._n_ra.u=regs.stra.u;
    regs._n_ry.u=regs.stry.u;
    regs._n_ru.u=regs.stru.u;
    regs._n_rv.u=regs.strv.u;

    /*preload linestart*/
#ifdef EVAL
    eval_LogPreload();
#endif
    pre(&regs);
    defaultdir=right;
    lineend=(regs._n_maxx);

    while (1) {

#ifdef EVAL
      eval_LogOpr(regs._n_cntx,regs._n_cnty);
#endif
      /* execute embedded function*/
#ifdef PPTRACE
      PPTrace_LogIntraIn(regs.matrix, regs._n_cntx,regs._n_cnty);
#endif
      operation(&regs.result,regs.matrix,regs._n_cntx,regs._n_cnty);
#ifdef PPTRACE
      PPTrace_LogIntraOut(regs.result);
#endif

      if (regs._n_raes) regs._n_raset(regs._n_ra,regs.result.a.i);
      if (regs._n_ryes) regs._n_ryset(regs._n_ry,regs.result.y.i);
      if (regs._n_rues) regs._n_ruset(regs._n_ru,regs.result.u.i);
      if (regs._n_rves) regs._n_rvset(regs._n_rv,regs.result.v.i);
      if (regs._n_axs) regs._n_axset(regs._n_ax,regs.result.ax.i);

      if (regs._n_cntx==lineend) {
	if (defaultdir==right) {
	  lineend=resposx;
	  defaultdir=left;
	}
	else {
	  lineend=regs._n_maxx;
	  defaultdir=right;
	}

	/*check for frame end*/
	if (regs._n_cnty == (regs._n_maxy)) break;
/*	for (i=1; i<regs._n_rsuby; i++) {*/
	  /* next row*/
	  /* shift down*/
/*	  down(&regs,0);
	}*/
	down(&regs);
      }
      else {
/*	for (i=1; i<regs._n_rsubx; i++) {*/
	  /* shift left or right*/
/*	  defaultdir(&regs,0);
	}*/
	defaultdir(&regs);
      }
    }
    break;
  case SCAN_VM:
    regs._n_cnty=resposy;
    regs._n_icnty=inposy;
    regs._n_irepcnty=regs._n_stirepcnty;

    regs._n_cntx=resposx;
    regs._n_icntx=inposx;
    regs._n_irepcntx=regs._n_stirepcntx;

    regs._n_ia.u=regs.stia.u;
    regs._n_iy.u=regs.stiy.u;
    regs._n_iu.u=regs.stiu.u;
    regs._n_iv.u=regs.stiv.u;
    regs._n_ax.u=regs.stax.u;
    regs._n_ra.u=regs.stra.u;
    regs._n_ry.u=regs.stry.u;
    regs._n_ru.u=regs.stru.u;
    regs._n_rv.u=regs.strv.u;

    /*preload linestart*/
#ifdef EVAL
    eval_LogPreload();
#endif
    pre(&regs);
    defaultdir=down;
    lineend=regs._n_maxy;

    while (1) {

#ifdef EVAL
      eval_LogOpr(regs._n_cntx,regs._n_cnty);
#endif
      /* execute embedded function*/
#ifdef PPTRACE
      PPTrace_LogIntraIn(regs.matrix, regs._n_cntx,regs._n_cnty);
#endif
      operation(&regs.result,regs.matrix,regs._n_cntx,regs._n_cnty);
#ifdef PPTRACE
      PPTrace_LogIntraOut(regs.result);
#endif

      if (regs._n_raes) regs._n_raset(regs._n_ra,regs.result.a.i);
      if (regs._n_ryes) regs._n_ryset(regs._n_ry,regs.result.y.i);
      if (regs._n_rues) regs._n_ruset(regs._n_ru,regs.result.u.i);
      if (regs._n_rves) regs._n_rvset(regs._n_rv,regs.result.v.i);
      if (regs._n_axs) regs._n_axset(regs._n_ax,regs.result.ax.i);

      if (regs._n_cnty==lineend) {
	if (defaultdir==up) {
	  lineend=regs._n_maxy;
	  defaultdir=down;
	}
	else {
	  lineend=resposy;
	  defaultdir=up;
	}

	if (regs._n_cntx == regs._n_maxx) break;
/*	for (i=1; i<regs._n_rsuby; i++) {*/
	  /* next culomn*/
	  /* shift right*/
/*	  right(&regs,0);
	}*/
	right(&regs);
      }
      else {
/*	for (i=1; i<regs._n_rsubx; i++) {*/
	  /* shift up or down*/
/*	  defaultdir(&regs,0);
	}*/
	defaultdir(&regs);
      }
    }
    break;
  case SCAN_LOOP:
  default :
    addlib_stop_here("Warning : scanmode not implemented\n",0,0);
    DestructNhood(&regs);
    return;
  }
  DestructNhood(&regs);

#ifdef PPTRACE
  PPTrace_LogIntraReg(1);
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

  return;
}


