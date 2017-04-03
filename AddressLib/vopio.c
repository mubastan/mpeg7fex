/* vopio.c
*
* vopio  includes funcition to read and write pictures
*        and store them in internal y-, u- and v-buffers
*/

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
/* Filename :    $RCSfile: vopio.c,v $
   Version  :    Revision: 1.11
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
// Applicable File Name:  vopio.c
//
*/

/******* INCLUDES ************************************************************/
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include "momusys.h"
#include "vopio.h"
//#include "address.h"

/******* DEFINES *************************************************************/
#define ushort unsigned short
#define ulong  unsigned long
#define uchar  unsigned char

/******* MAKROS  *************************************************************/

/******* TYPES ***************************************************************/

/******* VARIABLES ***********************************************************/
#ifdef ADDPROF
framebuffercounter *readcnt=0,*writecnt=0;
#endif

/******* FUNCTIONS ***********************************************************/

/******* Code ****************************************************************/

/**********************************************************************/
/*  initvop  initialises vop structures and allocates memory            */
/**********************************************************************/
MomVop *initvop(TCoor x, TCoor y, int colorformat, MomImageType atype,
	     MomImageType ytype, MomImageType utype, MomImageType vtype)

{
  MomVop* newvop;
/*  int i;*/
  ulong size;
  ulong csize;
  uchar *a, *b;

  newvop=(MomVop *)malloc(sizeof(MomVop));
  if (!newvop) {
    fprintf(stderr,"ERROR : Out of memory");
    exit(errno);
  }

  newvop->a_chan=(MomImage *)malloc(sizeof(MomImage));
  newvop->y_chan=(MomImage *)malloc(sizeof(MomImage));
  if (!(newvop->a_chan && newvop->y_chan)) {
    fprintf(stderr,"ERROR : Out of memory");
    exit(errno);
  }

  size=x*y;
/*  newvop->a_chan->data=(MomImageData *)malloc(sizeof(MomImageData));
  newvop->y_chan->data=(MomImageData *)malloc(sizeof(MomImageData));
  if (!(newvop->a_chan->data && newvop->y_chan->data)) {
    fprintf(stderr,"ERROR : Out of memory");
    exit(errno);
  }*/
  newvop->a_chan->data=(MomImageData*)&newvop->a_chan->f;	// alpha channel
  newvop->y_chan->data=(MomImageData*)&newvop->y_chan->f;	// Y channel

  a=newvop->a_chan->data->u=(uchar *)malloc(size*typesize(atype));
  b=newvop->y_chan->data->u=(uchar *)malloc(size*typesize(ytype));
  if (!(newvop->a_chan->data->u && newvop->y_chan->data->u)) {
    fprintf(stderr,"ERROR : Out of memory");
    exit(errno);
  }

  newvop->pextend=(MomVopExtend *)malloc(sizeof(MomVopExtend));
  if (!newvop->pextend) {
    fprintf(stderr,"ERROR : Out of memory");
    exit(errno);
  }


  newvop->ident=0;
  newvop->width=x;
  newvop->height=y;
  newvop->hor_spat_ref=0;
  newvop->ver_spat_ref=0;
  newvop->scaling=0;
  newvop->arbitrary_shape=1;
  newvop->binary_shape=0;
  newvop->visibility=1;
  newvop->comp_order=0;

  newvop->pnext_spat=0;
  newvop->pnext_temp=0;
/*  newvop->pprev_temp=0;*/


  newvop->a_chan->version=newvop->y_chan->version = 0L;
  newvop->a_chan->x=newvop->y_chan->x = x;
  newvop->a_chan->y=newvop->y_chan->y = y;
  newvop->a_chan->upperodd=newvop->y_chan->upperodd = 0;
  newvop->a_chan->grid=newvop->y_chan->grid = 's';
  newvop->a_chan->type = atype;
  newvop->y_chan->type = ytype;

  memset(newvop->a_chan->data->u,0,size*typesize(atype));
  memset(newvop->y_chan->data->u,0,size*typesize(ytype));


/*  newvop->reseg_levels=0;
  newvop->preseg_list=0;*/

  if (colorformat > 1) x>>=1;
  if (colorformat > 2) y>>=1;
  if (!colorformat) x=y=0;

  newvop->u_chan=(MomImage *)malloc(sizeof(MomImage));
  newvop->v_chan=(MomImage *)malloc(sizeof(MomImage));
  if (!(newvop->u_chan && newvop->v_chan)) {
    fprintf(stderr,"ERROR : Out of memory");
    exit(errno);
  }

  csize=x*y;
  newvop->u_chan->version=newvop->v_chan->version=0L;
  newvop->u_chan->x=newvop->v_chan->x=x;
  newvop->u_chan->y=newvop->v_chan->y=y;
  newvop->u_chan->upperodd=newvop->v_chan->upperodd=0;
  newvop->u_chan->grid=newvop->v_chan->grid='s';
  newvop->u_chan->type=utype;
  newvop->v_chan->type=vtype;

/*  newvop->u_chan->data=(MomImageData *)malloc(sizeof(MomImageData));
  newvop->v_chan->data=(MomImageData *)malloc(sizeof(MomImageData));
  if (!(newvop->u_chan->data && newvop->v_chan->data)) {
    fprintf(stderr,"ERROR : Out of memory");
    exit(errno);
  }*/
  newvop->u_chan->data=(MomImageData*)&newvop->u_chan->f;
  newvop->v_chan->data=(MomImageData*)&newvop->v_chan->f;

  // not a colored image -- single band
  if (!colorformat) {
    newvop->u_chan->data->u=0;
    newvop->v_chan->data->u=0;
    return(newvop);
  }

  newvop->u_chan->data->u=(uchar *)malloc(csize*typesize(utype));
  newvop->v_chan->data->u=(uchar *)malloc(csize*typesize(vtype));
  if (!(newvop->u_chan->data->u && newvop->v_chan->data->u)) {
    fprintf(stderr,"ERROR : Out of memory");
    exit(errno);
  }

  memset(newvop->u_chan->data->u,0,csize*typesize(utype));
  memset(newvop->v_chan->data->u,0,csize*typesize(vtype));

  return(newvop);
}

/**
 * allocate memory for a MomVop image
 * If a type is NONE_TYPE that channel will not be allocated, and will be set to NULL
 * added by mb - 28.06.2008
 **/
MomVop *initVop(int width, int height, MomImageType atype, MomImageType ytype, MomImageType utype, MomImageType vtype)
{
  MomVop* newvop = 0;
  ulong size;

  newvop = (MomVop *)malloc(sizeof(MomVop));
  if (!newvop) {
    fprintf(stderr,"ERROR : Out of memory");
    //exit(errno);
    return NULL;
  }

  //initialize all channels to NULL
  newvop->a_chan = 0;
  newvop->y_chan = 0;
  newvop->u_chan = 0;
  newvop->v_chan = 0;

  // size of image
  size = width*height;


  if(atype != NONE_TYPE){
		newvop->a_chan=(MomImage *)malloc(sizeof(MomImage));
		if (!newvop->a_chan) {
			fprintf(stderr,"ERROR : initVop - Memory allocation failed");
			exit(errno);
		}

		newvop->a_chan->data=(MomImageData*)&newvop->a_chan->f;

		newvop->a_chan->data->u = (uchar *)malloc( size*typesize(atype) );
		if (!newvop->a_chan->data->u) {
			fprintf(stderr,"ERROR : initVop - Memory allocation failed");
			exit(errno);
		}
		newvop->binary_shape = 1;		//Key for alpha plane (1-binary,0-blending)
	    newvop->a_chan->x = width;
	    newvop->a_chan->y = height;
	    newvop->a_chan->upperodd= 0;
	    newvop->a_chan->grid = 's';		//square
	    newvop->a_chan->type = atype;

		// set initial value to 0
		memset( newvop->a_chan->data->u, 0, size*typesize(atype));
  }

  if(ytype != NONE_TYPE){
		newvop->y_chan=(MomImage *)malloc(sizeof(MomImage));
		if (!newvop->y_chan) {
			fprintf(stderr,"ERROR : initVop - Memory allocation failed");
			exit(errno);
		}

		newvop->y_chan->data=(MomImageData*)&newvop->y_chan->f;

		newvop->y_chan->data->u = (uchar *)malloc( size*typesize(ytype) );
		if (!newvop->y_chan->data->u) {
			fprintf(stderr,"ERROR : initVop - Memory allocation failed");
			exit(errno);
		}
		newvop->y_chan->x = width;
		newvop->y_chan->y = height;
		newvop->y_chan->upperodd = 0;
		newvop->y_chan->grid = 's';
		newvop->y_chan->type = ytype;

		// set initial value to 0
		memset(newvop->y_chan->data->u, 0, size*typesize(ytype));
  }

  if(utype != NONE_TYPE){
		newvop->u_chan=(MomImage *)malloc(sizeof(MomImage));
		if (!newvop->u_chan) {
			fprintf(stderr,"ERROR : initVop - Memory allocation failed");
			exit(errno);
		}

		newvop->u_chan->data=(MomImageData*)&newvop->u_chan->f;

		newvop->u_chan->data->u = (uchar *)malloc( size*typesize(utype) );
		if (!newvop->u_chan->data->u) {
			fprintf(stderr,"ERROR : initVop - Memory allocation failed");
			exit(errno);
		}
		newvop->u_chan->x = width;
		newvop->u_chan->y = height;
		newvop->u_chan->upperodd = 0;
		newvop->u_chan->grid = 's';
		newvop->u_chan->type = utype;

		// set initial value to 0
		memset(newvop->u_chan->data->u, 0, size*typesize(utype));
  }

  if(vtype != NONE_TYPE){
		newvop->v_chan=(MomImage *)malloc(sizeof(MomImage));
		if (!newvop->v_chan) {
			fprintf(stderr,"ERROR : initVop - Memory allocation failed");
			exit(errno);
		}

		newvop->v_chan->data=(MomImageData*)&newvop->v_chan->f;

		newvop->v_chan->data->u = (uchar *)malloc( size*typesize(vtype) );
		if (!newvop->v_chan->data->u) {
			fprintf(stderr,"ERROR : initVop - Memory allocation failed");
			exit(errno);
		}
		newvop->v_chan->x = width;
		newvop->v_chan->y = height;
		newvop->v_chan->upperodd = 0;
		newvop->v_chan->grid = 's';
		newvop->v_chan->type = vtype;

		// set initial value to 0
		memset(newvop->v_chan->data->u, 0, size*typesize(vtype));
  }


  newvop->width = width;
  newvop->height = height;
  newvop->arbitrary_shape = 1;
  newvop->pextend = 0;


  return newvop;
}

/**********************************************************************/
/*  what is the use of this function ??? (mb)						  */
/**********************************************************************/
void setvop(MomVop *vop, TCoor x, TCoor y)

{
  vop->ident=0;
  vop->width=x;
  vop->height=y;
  vop->hor_spat_ref=0;
  vop->ver_spat_ref=0;
  vop->scaling=0;
  vop->arbitrary_shape=1;
  vop->binary_shape=0;
  vop->visibility=1;
  vop->comp_order=0;

  vop->pnext_spat=0;
  vop->pnext_temp=0;
/*  vop->pprev_temp=0;*/


  vop->a_chan=0;
  vop->y_chan=0;
  vop->u_chan=0;
  vop->v_chan=0;
}


/**********************************************************************/
/*  freevop  frees allocated memory of vop                            */
/**********************************************************************/
MomVop *freevop(MomVop *vop)

{
	if (vop) {

		// free a_chan
		if (vop->a_chan) {
			if (vop->a_chan->data->u)
				free(vop->a_chan->data->u);

			free(vop->a_chan);
	    }

		// free y_chan
		if (vop->y_chan) {
			if (vop->y_chan->data->u) {
			  free(vop->y_chan->data->u);
			}
			free(vop->y_chan);
		}

		// free u_chan
		if (vop->u_chan) {

			if (vop->u_chan->data->u)
			  free(vop->u_chan->data->u);

			free(vop->u_chan);
		}

		// free v_chan
		if (vop->v_chan) {

			if (vop->v_chan->data->u)
			  free(vop->v_chan->data->u);

			free(vop->v_chan);
		}

		//if (vop->pextend)
		//	free(vop->pextend);


		free(vop);
	}

  return 0;
}

/**********************************************************************/
/*  initimg  initializes memory for an image, single band, sets data to 0
/**********************************************************************/
MomImage *initimg(TCoor x, TCoor y, MomImageType type)

{
  MomImage *newimg;
  ulong size;

  newimg=(MomImage *)malloc(sizeof(MomImage));
  if (!newimg) {
    fprintf(stderr,"ERROR : Out of memory");
    exit(errno);
  }

  size=x*y;
  newimg->version=0L;
  newimg->x=x;
  newimg->y=y;
  newimg->upperodd=0;
  newimg->grid='s';
  newimg->type=type;


/*  newimg->data=(MomImageData *)malloc(sizeof(MomImageData));
  if (!newimg->data) {
    fprintf(stderr,"ERROR : Out of memory");
    exit(errno);
  }*/
  newimg->data=(MomImageData*)&newimg->f;

  newimg->data->u=(unsigned char *)malloc(size*typesize(type));
  if (!newimg->data->u) {
    fprintf(stderr,"ERROR : Out of memory");
    exit(errno);
  }

  memset(newimg->data->u,0,size*typesize(type));

  return(newimg);
}

/**********************************************************************/
/*  freevop  frees allocated memory of vop                            */
/**********************************************************************/
MomImage *freeimg(MomImage *img)

{
  if (img) {
/*    if (img->data) {*/
      if (img->data->u) free(img->data->u);
/*      free(img->data);
    }*/
    free(img);
  }
  return 0;
}


/**********************************************************************/
/*  datasize  returns datasize of selected dataformat                 */
/**********************************************************************/

int datasize(MomImage *image)

{
  if (image) {
    switch (image->type) {
    case UCHAR_TYPE: return sizeof(char);
    case SHORT_TYPE: return sizeof(short);
    case FLOAT_TYPE: return sizeof(float);
    case NONE_TYPE: return 0;
    }
  }
  return 0;
}

/**********************************************************************/
/*  typesize  returns datasize of selected dataformat                 */
/**********************************************************************/

int typesize(MomImageType type)

{
  switch (type) {
  case UCHAR_TYPE: return sizeof(char);
  case SHORT_TYPE: return sizeof(short);
  case FLOAT_TYPE: return sizeof(float);
  case NONE_TYPE: return 0;
  }
  return 0;
}

/**********************************************************************/
/*  typmax  returns maximumvalue of selected dataformat               */
/**********************************************************************/

ulong typemax(MomImageType type)

{
  switch (type) {
  case UCHAR_TYPE: return 255;
  case SHORT_TYPE: return 32767;
  case FLOAT_TYPE: return 0XFFFFFFFF;
  case NONE_TYPE: return 0;
  }
  return 0;
}

/**********************************************************************/
/*  iorefs  returns read-write function for selected buffer           */
/**********************************************************************/

void iorefs(MomImageType type,TChan (**readfunc)(MomImageData),
	     void (**writefunc)(MomImageData,TChan))

{
  switch (type) {
  case UCHAR_TYPE:
    if (readfunc) *readfunc=&getuchardata;
    if (writefunc) *writefunc=&setuchardata;
    return;
  case SHORT_TYPE:
    if (readfunc) *readfunc=&getshortdata;
    if (writefunc) *writefunc=&setshortdata;
    return;
  case FLOAT_TYPE:
    if (readfunc) *readfunc=&getunused;
    if (writefunc) *writefunc=&setunused;
    return;
  case NONE_TYPE:
    return;
  }
  return;
}

/**********************************************************************/
/*  irefs   returns read function for selected buffer                 */
/**********************************************************************/

void iref(MomImageType type,TChan(**readfunc)(MomImageData))

{
  switch (type) {
  case UCHAR_TYPE:
    if (readfunc) *readfunc=&getuchardata;
    return;
  case SHORT_TYPE:
    if (readfunc) *readfunc=&getshortdata;
    return;
  case FLOAT_TYPE:
    if (readfunc) *readfunc=&getfloatdata;
    return;
  case NONE_TYPE:
    return;
  }
  return;
}

/**********************************************************************/
/*  orefs   returns write function for selected buffer                */
/**********************************************************************/

void oref(MomImageType type,void (**writefunc)(MomImageData,TChan))

{
  switch (type) {
  case UCHAR_TYPE:
    if (writefunc) *writefunc=&setuchardata;
    return;
  case SHORT_TYPE:
    if (writefunc) *writefunc=&setshortdata;
    return;
  case FLOAT_TYPE:
    if (writefunc) *writefunc=&setfloatdata;
    return;
  case NONE_TYPE:
    return;
  }
  return;
}

/**********************************************************************/
/*  setdata  sets data in an imagememory                              */
/**********************************************************************/

void setdata(MomImageData data,MomImageType type,TChan value)

{
  if (!data.u) return;
#ifdef ADDPROF
  if (writecnt) (*writecnt)++;
#endif

  switch (type) {
  case UCHAR_TYPE:
    if (value<0) value=0;
    if (value>255) value=255;
    *data.u=(uchar)value;
    return;
  case SHORT_TYPE:
    *data.s=(short)value;
    return;
  case FLOAT_TYPE:
    *data.f=TCHAN_TO_FLOAT(value);
    return;
  case NONE_TYPE:
    return;
  }
}

/**********************************************************************/
/*  setuchardata  sets data in an imagememory                         */
/**********************************************************************/

void setuchardata(MomImageData data,TChan value)

{
#ifdef ADDPROF
  if (writecnt) (*writecnt)++;
#endif
  if (value<0) value=0;
  if (value>255) value=255;
  *data.u=(uchar)value;
  return;
}

/*#define setuchardata(datapointer,value) \
  (long)(uchar)*datapointer.u=value*/

/**********************************************************************/
/*  setshortdata  sets data in an imagememory                         */
/**********************************************************************/

void setshortdata(MomImageData data,TChan value)

{
#ifdef ADDPROF
  if (writecnt) (*writecnt)++;
#endif
  *data.s=(short)value;
  return;
}

/**********************************************************************/
/*  setfloatdata  sets data in an imagememory                         */
/**********************************************************************/

void setfloatdata(MomImageData data,TChan value)

{
#ifdef ADDPROF
  if (writecnt) (*writecnt)++;
#endif
  *data.f=(*((float *)(&(value))));
  return;
}

/**********************************************************************/
/*  setunused  dummy write function for unused imagebuffer            */
/**********************************************************************/

void setunused(MomImageData notused1,TChan notused2)

{
  return;
}

/**********************************************************************/
/*  getdata  returns data from an imagememory                         */
/**********************************************************************/

TChan getdata(MomImageData data,MomImageType type)

{
  if (data.u) {
#ifdef ADDPROF
    if (readcnt) (*readcnt)++;
#endif
    switch (type) {
    case UCHAR_TYPE: return (TChan)(uchar)*data.u;
    case SHORT_TYPE: return (TChan)*data.s;
    case FLOAT_TYPE: return FLOAT_TO_TCHAN(*data.f);
    }
  }
  return 0;
}

/**********************************************************************/
/*  getuchardata  returns data from an imagememory                    */
/**********************************************************************/

TChan getuchardata(MomImageData data)

{
#ifdef ADDPROF
  if (readcnt) (*readcnt)++;
#endif
  return (TChan)(uchar)*data.u;
}

/*#define getuchardata(datapointer) \
  data.u ? (long)(uchar)*datapointer.u : 0*/

/**********************************************************************/
/*  getshortdata  returns data from an imagememory                    */
/**********************************************************************/

TChan getshortdata(MomImageData data)

{
#ifdef ADDPROF
  if (readcnt) (*readcnt)++;
#endif
  return (TChan)*data.s;
}

/*#define getshortdata(datapointer) \
  data.s ? (long)*datapointer.s : 0*/

/**********************************************************************/
/*  getfloatdata  returns data from an imagememory                    */
/**********************************************************************/

TChan getfloatdata(MomImageData data)

{
#ifdef ADDPROF
  if (readcnt) (*readcnt)++;
#endif
  return (*((TChan *)(data.f)));
}

/*#define getshortdata(datapointer) \
  data.s ? (long)*datapointer.s : 0*/

/**********************************************************************/
/*  getunused     returns dummy data from an unsued imagememory       */
/**********************************************************************/

TChan getunused(MomImageData notused)

{
  return 0;
}

/**********************************************************************/
/*  incdata  increments data from an imagememory                      */
/**********************************************************************/

void incdata(MomImageData data,MomImageType type)

{
  if (!data.u) return;
#ifdef ADDPROF
  if (writecnt) (*writecnt)++;
  if (readcnt) (*readcnt)++;
#endif
  switch (type) {
  case UCHAR_TYPE:
    (*data.u)++;
    return;
  case SHORT_TYPE:
    (*data.s)++;
    return;
  case FLOAT_TYPE:
    (*data.f)+=1.0;
    return;
  }
}

/**********************************************************************/
/*  xydata  get adress of data at positionx,y                         */
/**********************************************************************/

uchar *xydata(MomImage *image,TCoor x,TCoor y)

{
  if (!image) return 0;
  if (!(image->data)) return 0;
  if (!(image->data->u)) return 0;
  if (x >= image->x) return 0;
  if (y >= image->y) return 0;

  return (image->data->u +  datasize(image)* (x + (y *image->x)));

}
