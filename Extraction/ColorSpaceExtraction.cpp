//////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// TU-Munich, S.Herrmann
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
// Applicable File Name:  ColorSpaceExtraction.cpp
//

#include <stdio.h>

#include "AddressLib/vopio.h"
#include "Descriptors/ColorSpace.h"                     /* Naming Convention */
#include "ColorSpaceExtraction.h"

//----------------------------------------------------------------------------
void  YUV_To_HSV(TPixel *res, TPixel *in,TCoor x, TCoor y)
{
  res->a=in[12].a; /*position 12 is central position of 5x5 2D input matrix*/
  res->y=in[12].y;
  res->u=in[12].u;
  res->v=in[12].v;
  res->ax=in[12].ax;
}

//----------------------------------------------------------------------------
void  YUV_To_RGB(TPixel *res, TPixel *in,TCoor x, TCoor y)
{
  /* g => in[12].y
     b => in[12].u
     r => in[12].v*/

  res->a=in[12].a; /*position 12 is central position of 5x5 2D input matrix*/
  res->y.i=in[12].y.i - (TChan)( 0.3437*((float)in[12].u.i-128)+
			 0.7142*((float)in[12].v.i-128)+0.5);
  res->u.i=in[12].y.i + (TChan) (1.772*((float)in[12].u.i-128)+0.5);
  res->v.i=in[12].y.i + (TChan) (1.4017*((float)in[12].v.i-128)+0.5);
  res->ax=in[12].ax;

  /* limit value range from 0 -255*/
  if(res->y.i < 0) res->y.i=0;
  if(res->y.i >255) res->y.i=255;
  if(res->u.i < 0) res->u.i=0;
  if(res->u.i >255) res->u.i=255;
  if(res->v.i < 0) res->v.i=0;
  if(res->v.i >255) res->v.i=255;
}
//----------------------------------------------------------------------------
void  RGB_To_YUV(TPixel *res, TPixel *in,TCoor x, TCoor y)
{
  /* g => in[12].y
     b => in[12].u
     r => in[12].v*/

#ifdef _DL_FLOAT_TEST
  float ftesta=1.0;
  float ftestb=2.0;
  ftesta=ftesta+ftestb;
  if (ftesta != 3.0) {
    fprintf(stderr,"Warning: dynamic link problem with float operations"
	    " (ColorSpace)\n");
    ftesta=1.0;
    ftestb=ftesta;
    ftestb=2.0;
    ftesta=ftesta+ftestb;
    if ((ftesta - 2.9) <= 0.2) {
      fprintf(stderr,"         Hope Initalization was OK!\n");
    }
  }
#endif

  res->a=in[12].a; /*position 12 is central position of 5x5 2D input matrix*/
  res->y.i=(TChan)( 0.587*((float)in[12].y.i) +
		    0.114*((float)in[12].u.i) +
		    0.299*((float)in[12].v.i) + 0.5);
  res->u.i=128+(TChan)(-0.331*((float)in[12].y.i) +
		       0.500*((float)in[12].u.i) -
		       0.169*((float)in[12].v.i) + 0.5);
  res->v.i=128+(TChan)(-0.419*((float)in[12].y.i) -
		       0.081*((float)in[12].u.i) +
		       0.500*((float)in[12].v.i) + 0.5);

  /* limit value range from 0 -255*/
  if(res->y.i < 0) res->y.i=0;
  if(res->y.i >255) res->y.i=255;
  if(res->u.i < 0) res->u.i=0;
  if(res->u.i >255) res->u.i=255;
  if(res->v.i < 0) res->v.i=0;
  if(res->v.i >255) res->v.i=255;

  res->ax=in[12].ax;
}


//----------------------------------------------------------------------------
void  RGB_To_HSV(TPixel *res, TPixel *in,TCoor x, TCoor y)
{
  /* g => in[12].y
     b => in[12].u
     r => in[12].v

     v => res.y;
     h => res.u;
     s => res.v;
*/

  static TChan maxrgb,minrgb;
  static char order;
  static double floath;

  /* 0 = rgb, 1=gbr, 2=grb, 3=rbg, 4=bgr, 5=brg*/

  res->a=in[12].a; /*position 12 is central position of 5x5 2D input matrix*/
  res->ax=in[12].ax;

  /*sort r g and b and get max and min value*/
  if (in[12].y.i > in[12].u.i) {
    if (in[12].v.i > in[12].y.i) {
      maxrgb=in[12].v.i;
      minrgb=in[12].u.i;
      order=0;
    }
    else {
      if (in[12].u.i > in[12].v.i) {
	maxrgb=in[12].y.i;
	minrgb=in[12].v.i;
	order=1;
      }
      else {
	maxrgb=in[12].y.i;
	minrgb=in[12].u.i;
	order=2;
      }
    }
  }
  else {
    if (in[12].v.i > in[12].u.i) {
      maxrgb=in[12].v.i;
      minrgb=in[12].y.i;
      order=3;
    }
    else {
      if (in[12].y.i > in[12].v.i) {
	maxrgb=in[12].u.i;
	minrgb=in[12].v.i;
	order=4;
      }
      else {
	maxrgb=in[12].u.i;
	minrgb=in[12].y.i;
	order=5;
      }
    }
  }

  if (!maxrgb) {
    res->y.i=0;
    res->u.i=0;
    res->v.i=0;
    return;
  }

  res->y.i=maxrgb;
  res->v.i=((maxrgb-minrgb)*255)/maxrgb;

  if (maxrgb==minrgb) {
    res->u.i=0;
    return;
  }

  switch (order) {
  case 0:
    floath=1.0-(double)(in[12].v.i-in[12].y.i)/(double)(in[12].v.i-in[12].u.i);
    break;
  case 1:
    floath=3.0-(double)(in[12].y.i-in[12].u.i)/(double)(in[12].y.i-in[12].v.i);
    break;
  case 2:
    floath=1.0+(double)(in[12].y.i-in[12].v.i)/(double)(in[12].y.i-in[12].u.i);
    break;
  case 3:
    floath=5.0+(double)(in[12].v.i-in[12].u.i)/(double)(in[12].v.i-in[12].y.i);
    break;
  case 4:
    floath=3.0+(double)(in[12].u.i-in[12].y.i)/(double)(in[12].u.i-in[12].v.i);
    break;
  case 5:
    floath=5.0-(double)(in[12].u.i-in[12].v.i)/(double)(in[12].u.i-in[12].y.i);
    break;
  default:
    fprintf(stderr,"Error: Internal error in RGB to HSV transformation\n");
    floath=0.0;
  }

  res->u.i=(TChan)(floath/6*255);
}

//----------------------------------------------------------------------------
void  HSV_To_RGB(TPixel *res, TPixel *in, TCoor x, TCoor y) //added by LG
{
	long R,G,B,max,min,i;
	R = G = B = 0;
	double H, S, f, q, t;

	H = (double)in[12].y.i;
	S = (double)in[12].u.i;
	max = in[12].v.i;
	min = (long)(max*(1.0-S/255.0)+0.5);

	H = H/255.0*360.0;
	if (H == -1)
	{
		R=max;
		G=max;
		B=max;
	}
	else
	{
	   if (H == 360.0) H=0.0;
	   H = H/60.0;
	   i = (long)(H);
	   f = H - i;
	   q=max*(1-f)+min*f;
	   t=max*f+min*(1-f);
	   switch (i)
	   {
		   case 0: R = max; G =  (long)(t+0.5);   B =  min; break;
		   case 1: R = (long)(q+0.5);   G =  max; B =  min; break;
		   case 2: R = min; G = max;  B =  (long)(t+0.5);   break;
		   case 3: R = min; G = (long)(q+0.5);    B =  max; break;
		   case 4: R = (long)(t+0.5);   G = min;  B =  max; break;
		   case 5: R = max; G = min;  B =  (long)(q+0.5);   break;
	   }
	}
	res->v.i = R;
	res->y.i = G;
	res->u.i = B;
}


//----------------------------------------------------------------------------
void  LinearTransform(TPixel *res, TPixel *in,TCoor x, TCoor y,
		      float *conversionmatrix)
{
  res->a=in[12].a; /*position 12 is central position of 5x5 2D input matrix*/
  res->y.i=(TChan)(conversionmatrix[0] * (float)in[12].y.i +
    conversionmatrix[1] * (float)in[12].u.i+
		 conversionmatrix[2] * (float)in[12].v.i + 0.5);
  res->u.i=(TChan)(conversionmatrix[3] * (float)in[12].y.i +
    conversionmatrix[4] * (float)in[12].u.i+
		 conversionmatrix[5] * (float)in[12].v.i + 0.5);
  res->v.i=(TChan)(conversionmatrix[6] * (float)in[12].y.i +
    conversionmatrix[7] * (float)in[12].u.i+
		 conversionmatrix[8] * (float)in[12].v.i + 0.5);
  res->ax=in[12].ax;

  /* limit value range from 0-255*/
  if(res->y.i < 0) res->y.i=0;
  if(res->y.i >255) res->y.i=255;
  if(res->u.i < 0) res->u.i=0;
  if(res->u.i >255) res->u.i=255;
  if(res->v.i < 0) res->v.i=0;
  if(res->v.i >255) res->v.i=255;
}

//----------------------------------------------------------------------------
void  InverseLinearTransform(TPixel *res, TPixel *in,TCoor x, TCoor y,
		float *conversionmatrix)
{
  float inv[9];

  matrix_inversion(conversionmatrix, inv);
  res->a=in[12].a; /*position 12 is central position of 5x5 2D input matrix*/
  res->y.i=(TChan)(inv[0] * (float)in[12].y.i +
    inv[1] * (float)in[12].u.i+
		 inv[2] * (float)in[12].v.i + 0.5);
  res->u.i=(TChan)(inv[3] * (float)in[12].y.i +
    inv[4] * (float)in[12].u.i+
		 inv[5] * (float)in[12].v.i + 0.5);
  res->v.i=(TChan)(inv[6] * (float)in[12].y.i +
    inv[7] * (float)in[12].u.i+
		 inv[8] * (float)in[12].v.i + 0.5);
  res->ax=in[12].ax;

  /* limit value range from 0-255*/
  if(res->y.i < 0) res->y.i=0;
  if(res->y.i >255) res->y.i=255;
  if(res->u.i < 0) res->u.i=0;
  if(res->u.i >255) res->u.i=255;
  if(res->v.i < 0) res->v.i=0;
  if(res->v.i >255) res->v.i=255;
}

//----------------------------------------------------------------------------
void  RGB_To_HMMD(TPixel *res, TPixel *in, TCoor x, TCoor y)
{
     // input
	 // g => in[12].y
	 // b => in[12].u
	 // r => in[12].v

     // output
	 // hue => res.y;
	 // sum => res.u;
	 // diff => res.v;

	static TChan max,min,R,G,B;
	float hue;

	//GBR order
	R = in[12].v.i;
	G = in[12].y.i;
	B = in[12].u.i;

	max=R;
	if(max<G) {max=G;}
	if(max<B) {max=B;}

	min=R;
	if(min>G) {min=G;}
	if(min>B) {min=B;}

	if (max == min) // R==G==B : exact gray
	{
		hue = 0; //hue is undefined
	}
	else
	{   //solve Hue
		if(R==max)
		{
			hue=((G-B)/(float)(max-min));
		}
		else if(G==max)
		{
			hue=(2.0+(B-R)/(float)(max-min));
		}
		else /*if(B==max)*/
		{
			hue=(4.0+(R-G)/(float)(max-min));
		}

		hue*=60;
		if(hue<0.0) hue+=360;
	}
	res->a=in[12].a; /*position 12 is central position of 5x5 2D input matrix*/
	res->ax=in[12].ax;
	res->y.i = (long)(hue+.5);//range 0 ~ 360 //sth might crash if buffer is 8 bit wide
	res->u.i = (long)((max+min)/2.0+0.5); //range 0 ~ 255
	res->v.i = (long)(max-min+0.5); //range 0 ~ 255
	return;
}

//----------------------------------------------------------------------------
void  HMMD_To_RGB(TPixel *res, TPixel *in, TCoor x, TCoor y) //added by LGCIT
{
	long R,G,B,max,min,sum,diff,i;
	double H, f, q, t;

	H = (double)in[12].y.i;
	sum = in[12].u.i;
	diff = in[12].v.i;
	max = (long)(sum+diff/2.0+0.5);
	min = (long)(sum-diff/2.0+0.5);

	if (max == min)
	{
		R=max;
		G=max;
		B=max;
	}
	else
	{
	   if (H == 360.0) H=0.0;
	   H = H/60.0;
	   i = (long)(H);
	   f = H - i;
	   q=max*(1-f)+min*f;
	   t=max*f+min*(1-f);
	   switch (i)
	   {
	   case 0: R = max; G =  (long)(t+0.5);   B =  min; break;
	   case 1: R = (long)(q+0.5);   G =  max; B =  min; break;
	   case 2: R = min; G = max;  B =  (long)(t+0.5);   break;
	   case 3: R = min; G = (long)(q+0.5);    B =  max; break;
	   case 4: R = (long)(t+0.5);   G = min;  B =  max; break;
	   case 5:
	   default: R = max; G = min;  B =  (long)(q+0.5);   break;
	   }
	}
	res->v.i = R;
	res->y.i = G;
	res->u.i = B;
}

//----------------------------------------------------------------------------
void  RGB_To_Monochrome(TPixel *res, TPixel *in,TCoor x, TCoor y)
{
  /* g => in[12].y
     b => in[12].u
     r => in[12].v*/

  res->a=in[12].a; /*position 12 is central position of 5x5 2D input matrix*/
  res->y.i=(TChan)( 0.587*in[12].y.i) + (TChan)(0.114*in[12].u.i) + (TChan)(0.299*in[12].v.i);
  res->u.i=0;
  res->v.i=0;

  /* limit value range from 0 -255*/
  if(res->y.i < 0) res->y.i=0;
  if(res->y.i >255) res->y.i=255;

  res->ax=in[12].ax;
}
//----------------------------------------------------------------------------
void  NoConvert(TPixel *res, TPixel *in,TCoor x, TCoor y)
{
  res->a=in[12].a; /*position 12 is central position of 5x5 2D input matrix*/
  res->y=in[12].y;
  res->u=in[12].u;
  res->v=in[12].v;
  res->ax=in[12].ax;
}

//----------------------------------------------------------------------------
void matrix_inversion(float a[9],float m[9]) //sth bad name
{
	float c[9];
	float det;

	det = a[0]*(a[4]*a[8]-a[5]*a[7])+a[1]*(a[5]*a[6]-a[3]*a[8])+a[2]*(a[3]*a[7]-a[4]*a[6]);
	c[0] = a[4]*a[8]-a[5]*a[7]; c[1] = a[5]*a[6]-a[3]*a[8]; c[2] = a[3]*a[7]-a[4]*a[6];
	c[3] = a[2]*a[7]-a[1]*a[8]; c[4] = a[8]*a[0]-a[2]*a[6]; c[5] = a[1]*a[6]-a[0]*a[7];
	c[6] = a[1]*a[5]-a[2]*a[4]; c[7] = a[2]*a[3]-a[0]*a[5]; c[8] = a[0]*a[4]-a[1]*a[3];
	m[0] = c[0]/det; m[1] = c[3]/det; m[2] = c[6]/det;
	m[3] = c[1]/det; m[4] = c[4]/det; m[5] = c[7]/det;
	m[6] = c[2]/det; m[7] = c[5]/det; m[8] = c[8]/det;
}
