/*
 * nhood.h
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
/* based on   :  MPEG-4 Momusys VM Data structure                            */
/*****************************************************************************/

/**** LISSCCSInfo ************************************************************/
/* Filename :    $RCSfile: nhood.h,v $
   Version  :    Revision: 1.12
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
// Applicable File Name:  nhood.h
//
*/

#ifndef _NHOOD_
#define _NHOOD_

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
struct size_conv_struct
{
  char	repeatx;
  char	strepx;
  char	stepx;
  char	repeaty;
  char	strepy;
  char	stepy;
};

typedef struct size_conv_struct TSizeConv;

struct nhood_struct
{
  MomImage *_n_marker;             /* pointer to markerfield*/
  TCoor _n_inwidth,_n_inheight; /* input image size*/

  /* matrix size*/
  char _n_xblksize;             /* matrix x radius without centrer*/
  char _n_yblksize;             /* matrix y radius without centrer*/
  char _n_xblkstart;            /* top line of matrix*/
  char _n_yblkstart;            /* left line of matrix*/
  char _n_xblkstop;             /* buttom line of matrix*/
  char _n_yblkstop;             /* right line of matrix*/
  char _n_xblkdelta;            /* preload line x delta*/
  char _n_yblkdelta;            /* preload line y delta*/


  /*replacment values*/
  TChan _n_amaska;
  TChan _n_ymaska;
  TChan _n_umaska;
  TChan _n_vmaska;
  TChan _n_axmaska;
  TChan _n_amasko;
  TChan _n_ymasko;
  TChan _n_umasko;
  TChan _n_vmasko;
  TChan _n_axmasko;


/*  MomImageType _n_iatype;
  MomImageType _n_iytype;
  MomImageType _n_iutype;
  MomImageType _n_ivtype;*/

  char _n_inchanels;     /* corrected flags of input channels*/
  char _n_reschanels;    /* corrected flags of result channels*/
  char _n_scanmode;      /* corrected scanmode for conectivity*/
  char _n_mintern;       /* flag for internally controlled  marker field*/
  char error;            /* error indicator*/

  /* Interface from addressing methodes to neighborhood*/
  TPixel matrix[25];    /* input matrix register*/
  TPixel result;        /* output result vector*/


  /* position counters*/
  TCoor width,height;      /* reuslt image size*/
  TCoor _n_cntx,_n_cnty;    /* curent pixel position in result frame*/
  TCoor _n_stx,_n_sty;      /* curent pixel position of startmark*/
  TCoor _n_maxx,_n_maxy;    /* maximumpositions of processingwindow*/
  TCoor _n_icntx,_n_icnty;  /* curent pixel position in input frame*/
  char	_n_irepeatx,_n_irepeaty;/* subsampling of inputvalues*/
  char	_n_istepx,_n_istepy;/* subsampling of resultvalues*/
  char	_n_irepcntx,_n_irepcnty;
       			   /* repeated times of imput position*/
  char	_n_stirepcntx,_n_stirepcnty;
  			   /* initial values of repeat counters*/
  char	_n_firstx,_n_firsty;  /* first pixel in step raster*/

  /*incrementer values for pointer shifts*/
  short _n_iaes,_n_iyes,_n_iues,_n_ives; /*element sizes*/
  short _n_raes,_n_ryes,_n_rues,_n_rves;
  short _n_ias,_n_iys,_n_ius,_n_ivs,_n_ms,_n_axs; /*horizontal step size*/
  short _n_ras,_n_rys,_n_rus,_n_rvs;
  TCoor _n_ials,_n_iyls,_n_iuls,_n_ivls,_n_mls,_n_axls; /*vertoical step size*/
  TCoor _n_rals,_n_ryls,_n_ruls,_n_rvls;

  /* references to read-write functions*/
  TChan(*_n_iaget)(MomImageData data);
  TChan(*_n_iyget)(MomImageData data);
  TChan(*_n_iuget)(MomImageData data);
  TChan(*_n_ivget)(MomImageData data);
  TChan(*_n_axget)(MomImageData data);
  TChan(*_n_mget) (MomImageData data);
  TChan(*_n_raget)(MomImageData data);
  TChan(*_n_ryget)(MomImageData data);
  TChan(*_n_ruget)(MomImageData data);
  TChan(*_n_rvget)(MomImageData data);

  void (*_n_iaset)(MomImageData data,TChan value);
  void (*_n_iyset)(MomImageData data,TChan value);
  void (*_n_iuset)(MomImageData data,TChan value);
  void (*_n_ivset)(MomImageData data,TChan value);
  void (*_n_axset)(MomImageData data,TChan value);
  void (*_n_mset) (MomImageData data,TChan value);
  void (*_n_raset)(MomImageData data,TChan value);
  void (*_n_ryset)(MomImageData data,TChan value);
  void (*_n_ruset)(MomImageData data,TChan value);
  void (*_n_rvset)(MomImageData data,TChan value);
  MomImageType _n_ratype,_n_rytype,_n_rutype,_n_rvtype,_n_mtype,_n_axtype;


  /* pointer to pixel positions*/
  MomImageData _n_ia,_n_iy,_n_iu,_n_iv,_n_m,_n_,_n_ax;
                           /* input pixel positions in memory*/
  MomImageData _n_ra,_n_ry,_n_ru,_n_rv;
                           /* outout pixel position in memory*/
  MomImageData stia,stiy,stiu,stiv,stm,stax; /* line start positions */
  MomImageData stra,stry,stru,strv;	     /*	in memory*/

  MomImageData fstia,fstiy,fstiu,fstiv,fstm,fstax; /* frame start positions*/
  MomImageData fstra,fstry,fstru,fstrv;		   /* in memory*/
  };
typedef struct nhood_struct TNhood;

/******* VARIABLES ***********************************************************/
/*extern TNhood regs;*/
extern TSizeConv size1to1;
extern TSizeConv uphv1TO2;
extern TSizeConv uph1TO2;
extern TSizeConv upv1TO2;
extern TSizeConv downhv2TO1;
extern TSizeConv downv2TO1;
extern TSizeConv downh2TO1;
extern short _typesize[5];
extern char dirtopos[8];

/******* FUNCTIONS ***********************************************************/
void ConstructNhood(TNhood *nhood,MomVop *result, MomVop *in,
			      MomImage *aux,MomImage *marker,int usemarker,
			      char reschanels,char inchanels,char connect,
			      char scanmode,
			      TChan ahold,TChan yhold,TChan uhold,TChan vhold,
			      TChan axhold,TCoor winx,TCoor winy,TCoor resposx,
			      TCoor resposy,TCoor inposx,TCoor inposy,
			      TSizeConv inconv);

void DestructNhood(TNhood *nhood);

int CheckError(TNhood *nhood);

/* for fifo_area*/
void Goto(TNhood *nhood,TCoor dx,TCoor dy);  /* ref not moved*/
void Preload4(TNhood *nhood,TCoor dx,TCoor dy); /* ref not moved*/
void Preload8(TNhood *nhood,TCoor dx,TCoor dy); /* ref not moved*/
void MUpdate(TNhood *nhood,char dir);        /* write marker for
							  process on queue
							  out*/

void UpdatePos(TNhood *nhood,char dir);      /* for process on queue in*/

/*for rec_area*/
void Shift(TNhood *nhood,char dir);
void LoadIfUnload3X3(TNhood *nhood,short regpos);

/* for intra*/
void Preload(TNhood *nhood);
void PreloadSub(TNhood *nhood);
void ShiftUpLineLoad(TNhood *nhood);
void ShiftDownLineLoad(TNhood *nhood);
void ShiftLeftLineLoad(TNhood *nhood);
void ShiftRightLineLoad(TNhood *nhood);
void ShiftUpSubLineLoad(TNhood *nhood);
void ShiftDownSubLineLoad(TNhood *nhood);
void ShiftLeftSubLineLoad(TNhood *nhood);
void ShiftRightSubLineLoad(TNhood *nhood);

void addlib_stop_here(char *message,int doexit, int exitcode);

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif
