///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Kapil Chhabra, Yining Deng, Xinding Sun, Dr. Hyundoo Shin, Prof. B.S. Manjunath
// University of California, Santa Barbara.
// (contributing organizations names)
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
// Applicable File Name:  DominantColorAppl.cpp
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "imgutil.h"
//#include "domcol.h"

void rgb2luv2(unsigned char *RGB,float *LUV,int size)
{
  int i;
  double x,y,X,Y,Z,den,u2,v2,X0,Z0,Y0,u20,v20,r,g,b;

  X0 = (0.607+0.174+0.201);
  Y0 = (0.299+0.587+0.114);
  Z0 = (      0.066+1.117);

/* Y0 = 1.0 */
  u20 = 4*X0/(X0+15*Y0+3*Z0);
  v20 = 9*Y0/(X0+15*Y0+3*Z0);

  for (i=0;i<size;i+=3)
  {
    if (RGB[i]<=20)  r=(double) (8.715e-4*RGB[i]);
    else r=(double) pow((RGB[i]+25.245)/280.245, 2.22);

    if (RGB[i+1]<=20)  g=(double) (8.715e-4*RGB[i+1]);
    else g=(double) pow((RGB[i+1]+25.245)/280.245, 2.22);

    if (RGB[i+2]<=20)  b=(double) (8.715e-4*RGB[i+2]);
    else b=(double) pow((RGB[i+2]+25.245)/280.245, 2.22);

    X = 0.412453*r + 0.357580*g + 0.180423*b;
    Y = 0.212671*r + 0.715160*g + 0.072169*b;
    Z = 0.019334*r + 0.119193*g + 0.950227*b;

    if (X==0.0 && Y==0.0 && Z==0.0)
    {
      x=1.0/3.0; y=1.0/3.0;
    }
    else
    {
      den=X+Y+Z;
      x=X/den; y=Y/den;
    }

    den=-2*x+12*y+3;
    u2=4*x/den;
    v2=9*y/den;

    if (Y>0.008856) LUV[i] = (float) (116*pow(Y,1.0/3.0)-16);
    else LUV[i] = (float) (903.3*Y);
    LUV[i+1] = (float) (13*LUV[i]*(u2-u20));
    LUV[i+2] = (float) (13*LUV[i]*(v2-v20));
  }
}

void rgb2luv(int *RGB,float *LUV,int size)
{
  int i;
  double x,y,X,Y,Z,den,u2,v2,X0,Z0,Y0,u20,v20,r,g,b;

  X0 = (0.607+0.174+0.201);
  Y0 = (0.299+0.587+0.114);
  Z0 = (      0.066+1.117);

/* Y0 = 1.0 */
  u20 = 4*X0/(X0+15*Y0+3*Z0);
  v20 = 9*Y0/(X0+15*Y0+3*Z0);

  for (i=0;i<size;i+=3)
  {
    if (RGB[i]<=20)  r=(double) (8.715e-4*RGB[i]);
    else r=(double) pow((RGB[i]+25.245)/280.245, 2.22);

    if (RGB[i+1]<=20)  g=(double) (8.715e-4*RGB[i+1]);
    else g=(double) pow((RGB[i+1]+25.245)/280.245, 2.22);

    if (RGB[i+2]<=20)  b=(double) (8.715e-4*RGB[i+2]);
    else b=(double) pow((RGB[i+2]+25.245)/280.245, 2.22);

    X = 0.412453*r + 0.357580*g + 0.180423*b;
    Y = 0.212671*r + 0.715160*g + 0.072169*b;
    Z = 0.019334*r + 0.119193*g + 0.950227*b;

    if (X==0.0 && Y==0.0 && Z==0.0)
    {
      x=1.0/3.0; y=1.0/3.0;
    }
    else
    {
      den=X+Y+Z;
      x=X/den; y=Y/den;
    }

    den=-2*x+12*y+3;
    u2=4*x/den;
    v2=9*y/den;

    if (Y>0.008856) LUV[i] = (float) (116*pow(Y,1.0/3.0)-16);
    else LUV[i] = (float) (903.3*Y);
    LUV[i+1] = (float) (13*LUV[i]*(u2-u20));
    LUV[i+2] = (float) (13*LUV[i]*(v2-v20));
  }
}

void luv2rgb(int *RGB,float *LUV,int size)
{
  int i,k;
  double x,y,X,Y,Z,den,u2,v2,X0,Z0,Y0,u20,v20,vec[3];

  X0 = (0.607+0.174+0.201);
  Y0 = (0.299+0.587+0.114);
  Z0 = (      0.066+1.117);

/* Y0 = 1.0 */
  u20 = 4*X0/(X0+15*Y0+3*Z0);
  v20 = 9*Y0/(X0+15*Y0+3*Z0);

  for (i=0;i<size;i+=3)
  {
    if (LUV[i]>0)
    {
      if (LUV[i]<8.0) Y=((double) LUV[i])/903.3;
      else Y=pow(( ((double) LUV[i]) +16)/116.0,3.0);
      u2=((double) LUV[i+1])/13.0/((double) LUV[i])+u20;
      v2=((double) LUV[i+2])/13.0/((double) LUV[i])+v20;

      den = 6+3*u2-8*v2;
      if (den<0) printf("den<0\n");
      if (den==0) printf("den==0\n");
      x = 4.5*u2/den;
      y = 2.0*v2/den;

      X=x/y*Y;
      Z=(1-x-y)/y*Y;
    }
    else { X=0.0; Y=0.0; Z=0.0; }

    vec[0] = ( 3.240479*X-1.537150*Y-0.498536*Z);
    vec[1] = (-0.969256*X+1.875992*Y+0.041556*Z);
    vec[2] = ( 0.055648*X-0.204043*Y+1.057311*Z);
    for (k=0;k<3;k++)
    {
      if  (vec[k]<=0.018) vec[k] = 255*4.5*vec[k];
      else vec[k] = 255*(1.099*pow(vec[k],0.45)-0.099);
      if (vec[k]>255) vec[k] = 255;
      else if (vec[k]<0) vec[k] = 0;
      RGB[i+k] = lroundf((float)vec[k]);
    }
  }
}

void inputimgraw(char *fname,unsigned char *img,int size,/*int ny,int nx,*/int dim)
{
  FILE *fimg;

  fimg=fopen(fname,"rb");
  if (!fimg)
  {
    printf("unable to read %s\n",fname);
    exit(-1);
  }
  fread(img, sizeof(unsigned char), size*dim, fimg);
  fclose (fimg);
}

void outputimgraw(char *fname,unsigned char *img,int ny,int nx,int dim)
{
  FILE *fimg;

  fimg=fopen(fname,"wb");
  fwrite(img, sizeof(unsigned char), ny*nx*dim, fimg);
  fclose(fimg);
}

float **fmatrix(int nr, int nc)
{
  int i,j;
  float **m;

  m=(float**)malloc(sizeof(float*)*nr);
  if (!m) return NULL;
  for(i=0;i<nr;i++)
  {
    m[i]=(float*)malloc(sizeof(float) * nc);
    if (!m[i]) return NULL;
    for (j=0;j<nc;j++) m[i][j]=0.0;
  }
  return m;
}

void free_fmatrix(float **m, int nr)
{
  int i;

  for (i=0;i<nr;i++) free(m[i]);
  free (m);
}

//#ifdef _VISUALC_
/*
int lroundf(float x)
{
  int i;
  float y;

  y=(float)floor(x);
  if ((x-y)>=0.5) x = (float)ceil(x);
  else x = (float)floor(x);
  i = (int) x;
  return i;
}
*/
//#endif

/* sort float from large to small */
void piksrt(int n, float *num, int *index)
{
  int i,j;
  int indextmp;
  float numtmp;

  for (i=0;i<n;i++) index[i]= i;
  for (j=1;j<n;j++)
  {
    numtmp=num[j];
    indextmp=index[j];
    i=j-1;
    while (i>=0 && num[i]<numtmp)
    {
      num[i+1]=num[i];
      index[i+1]=index[i];
      i--;
    }
    num[i+1]=numtmp;
    index[i+1]=indextmp;
  }
}

float distance(float *a,float *b,int dim)
{
  int i;
  float dist=0.0;

  for (i=0;i<dim;i++) dist+=sqr(a[i]-b[i]);
  dist = (float)sqrt(dist);
  return dist;
}

float distance2(float *a,float *b,int dim)
{
  int i;
  float dist=0.0;

  for (i=0;i<dim;i++) dist+=sqr(a[i]-b[i]);
  return dist;
}

//-------------------------------------------------------------------------------------------
void getcmap(float *B,unsigned char *cmap,float **cb,int npt,int dim,int N)
{
  int i,j;
  float mindif,dif;

  for (i=0;i<npt;i++)
  {
    mindif = distance2(B,cb[0],dim);
    cmap[i]=0;
    for (j=1;j<N;j++)
    {
      dif = distance2(B,cb[j],dim);
      if (dif<mindif) { cmap[i]=j; mindif=dif; }
    }
    B += dim;
  }
}

int mergecb(float *B,float **cb,unsigned char *P,int npt,int N,float thresh,int dim)
{
  int i,j,newN,*count,l,ei,debug=0;
  float **dist;

  if (N==1) return N;

  count=(int *)malloc(sizeof(int)*N);
  for (l=0;l<npt;l++) count[P[l]]++;

  dist=(float **)fmatrix(N,N);
  ei = N-1;
  for (i=0;i<ei;i++)
  {
    for (j=i+1;j<N;j++)
    {
      dist[i][j] = distance2(cb[i],cb[j],dim);
      dist[j][i] = dist[i][j];
    }
  }

  if (debug) printf("thresh %f ",thresh);
  newN=mergecb1(dist,B,cb,N,thresh,dim,count);

  free_fmatrix(dist,N);
  free(count);
  return newN;
}

int mergecb1(float **dist,float *B,float **cb,int newN,float thresh,int dim,int *count)
{
  int i,j,mini,minj,total,k,ei;
  float mindist;
  //int debug=0;

  ei = newN-1;
  mindist = 100000;
  mini = minj = 0;
  for (i=0;i<ei;i++)
  {
    for (j=i+1;j<newN;j++)
    {
      if (dist[i][j]<mindist)
      {
        mindist=dist[i][j];
        mini=i;
        minj=j;
      }
    }
  }

  while (newN>1 && mindist<thresh)
  {
    total=count[mini]+count[minj];
    for (k=0;k<dim;k++)
      cb[mini][k] = (count[mini]*cb[mini][k]+count[minj]*cb[minj][k])/total;
    count[mini]=total;
    ei = newN-1;
    for (i=minj;i<ei;i++)
    {
      count[i]=count[i+1];
      for (k=0;k<dim;k++) cb[i][k]=cb[i+1][k];
    }
    for (i=minj;i<ei;i++)
    {
      for (j=0;j<minj;j++) dist[i][j]=dist[i+1][j];
    }
    for (j=minj;j<ei;j++)
    {
      for (i=0;i<minj;i++) dist[i][j]=dist[i][j+1];
    }
    for (i=minj;i<ei;i++)
    {
      for (j=minj;j<ei;j++) dist[i][j]=dist[i+1][j+1];
    }
    newN--;

    for (j=0;j<newN;j++)
    {
      dist[mini][j] = distance2(cb[mini],cb[j],dim);
      dist[j][mini] = dist[mini][j];
    }
    mindist=100000;
    ei = newN-1;
    for (i=0;i<ei;i++)
    {
      for (j=i+1;j<newN;j++)
      {
        if (dist[i][j]<mindist)
        {
          mindist=dist[i][j];
          mini=i;
          minj=j;
        }
      }
    }
  }
  return newN;
}

int greedy(float *A,int nvec,int ndim,int N,float **codebook,float t,unsigned char *P)
{
  int iv,in,jn,imax,nsplit,*index2,k,i,retgla,kn;
  float *totalw,*d,**buf, *variance;

  buf=fmatrix(N,ndim);
  d=(float*)malloc(sizeof(float)*ndim);
  variance=(float*)malloc(sizeof(float)*N);
  totalw=(float*)malloc(sizeof(float)*N);
  index2=(int*)malloc(sizeof(int)*N);

/* Calculate the initial centroid */
  for (k=0;k<ndim;k++) codebook[0][k]=0.0;
  totalw[0]=0; i=0;
  for (iv=0;iv<nvec;iv++)
  {
    P[iv]= 0;
    totalw[0]+=1.0;
    for (k=0;k<ndim;k++) codebook[0][k] += A[i++];
  }
  for (k=0;k<ndim;k++) codebook[0][k]/=totalw[0];

  in=1;
  while (in<N)
  {
/*  find the maximum variance */
    for (jn=0;jn<in;jn++)
    {
      variance[jn]=0;
      totalw[jn]=0;
      for (k=0;k<ndim;k++) buf[jn][k]=0.0;
    }
    i = 0;
    for (iv=0;iv<nvec;iv++)
    {
      for (k=0;k<ndim;k++) d[k] = codebook[P[iv]][k] - A[i++];
      for (k=0;k<ndim;k++)
      {
        buf[P[iv]][k] += sqr(d[k]);
        variance[P[iv]]+= buf[P[iv]][k];
      }
      totalw[P[iv]] += 1.0;
    }
    for (jn=0;jn<in;jn++)
    {
      for (k=0;k<ndim;k++) buf[jn][k] = (float)sqrt(buf[jn][k]/totalw[jn]);
    }
    piksrt(in,variance,index2);

/*  split */
    nsplit=in/2+1;
    if ((nsplit+in)>N) nsplit=N-in;
    for (jn=0;jn<nsplit;jn++)
    {
      imax=index2[jn];
      for (k=0;k<ndim;k++) codebook[in+jn][k] = codebook[imax][k] - buf[imax][k];
      for (k=0;k<ndim;k++) codebook[imax] [k] = codebook[imax][k] + buf[imax][k];
    }

/*  run gla on the codebook */
    in=in+nsplit;
    retgla=gla(A,nvec,ndim,in,codebook,t,P);

/*  find the code vectors same, remove them and stop */
    if (retgla)
    {
      for (jn=0;jn<in-1;jn++)
      {
        for (kn=jn+1;kn<in;kn++)
        {
          if (distance2(codebook[jn],codebook[kn],ndim)==0)
          {
            for (i=kn;i<in-1;i++)
              for (k=0;k<ndim;k++) codebook[i][k]=codebook[i+1][k];
            in--;
          }
        }
      }
      break;
    }
  }
  free_fmatrix(buf,N);
  free(d);
  free(index2);
  free(variance);
  free(totalw);
  return in;
}

int gla(float *A,int nvec,int ndim,int N,float **codebook,float t,unsigned char *P)
{
  int iv,in,i,j,jn,codeword_exist=0,k,l;
  float *totalw,d1,d2,rate,lastmse=0.0f,mse,*d;

  totalw=(float*)malloc(sizeof(float)*N);
  d=(float*)malloc(sizeof(float)*ndim);

  for (i=0;i<5;i++)
  {
/*  get the new partition and total distortion using NN */
    mse=0.0; j=0;
    for (iv=0;iv<nvec;iv++)
    {
      for (k=0;k<ndim;k++) d[k] = A[j++]-codebook[0][k];
      d1=0;
      for (k=0;k<ndim;k++) d1 += sqr(d[k]);
      P[iv]=0;
      for (in=1;in<N;in++)
      {
        d2=0; l=j-ndim;
        for (k=0;k<ndim;k++)
        {
          d2+=sqr(A[l]-codebook[in][k]); l++;
          if (d2>=d1) break;
        }
        if (d2<d1)  { d1=d2; P[iv]= (unsigned char) in; }
      }
      mse+=d1;
    }

/*  get the new codebook using centroid */
    for (in=0;in<N;in++)
    {
      totalw[in]=0.0;
      for (k=0;k<ndim;k++) codebook[in][k]=0;
    }
    j = 0;
    for (iv=0;iv<nvec;iv++)
    {
      for (k=0;k<ndim;k++) codebook[P[iv]][k] += A[j++];
      totalw[P[iv]]+=1.0;
    }
    for (in=0;in<N;in++)
    {
      if (totalw[in]>0.0)
      {
        for (k=0;k<ndim;k++) codebook[in][k] /= totalw[in];
      }
      else
      {
/*      assign a training vector not in the codebook as code vector */
        codeword_exist=1;
        iv= lroundf ( ((float) rand()) *(nvec-1)/RAND_MAX);
        while (codeword_exist<=2 && codeword_exist>0)
        {
          j = iv*ndim;
          for (k=0;k<ndim;k++) codebook[in][k] = A[j++];
          for (jn=0;jn<N;jn++)
          {
            if (jn!=in)
            {
              for (k=0;k<ndim;k++) d[k]=codebook[jn][k]-codebook[in][k];
              d1=0;
              for (k=0;k<ndim;k++) d1 += sqr(d[k]);
              if (d1==0) break;
            }
          }
          if (jn==N) codeword_exist=0;
          else
          {
            iv = lroundf( ((float) rand()) *(nvec-1)/RAND_MAX);
            codeword_exist++;
          }
        }
      }
    }
    if (i>0)
    {
      rate=(lastmse-mse)/lastmse;
      if (rate<t) break;
    }
    lastmse=mse;
  }

  for (in=0;in<N;in++)
  {
    totalw[in]=0.0;
    for (k=0;k<ndim;k++) codebook[in][k]=0;
  }
  j = 0;
  for (iv=0;iv<nvec;iv++)
  {
    for (k=0;k<ndim;k++) codebook[P[iv]][k] += A[j++];
    totalw[P[iv]]+=1.0;
  }
  for (in=0;in<N;in++)
  {
    if (totalw[in]>0)
    {
      for (k=0;k<ndim;k++) codebook[in][k] /= totalw[in];
    }
  }

  free(d);
  free(totalw);
  return codeword_exist;
}

//----------------------------------------------------------------------------------------
int getsize()
{
	int choice,NY,NX,size;
	printf("select the source of input:\n");
    printf("\n1) Enter 1 for input from an image in RGB format \n");
    printf("\n2) Enter 2 for input from an arbitary set of pixels \n");
    scanf ("%d",&choice);
    if (choice==1){
		printf("\n enter the height of the image\n");
        scanf("%d",&NY);
		printf("\n enter the width of the image\n");
		scanf("%d",&NX);
		size=NY*NX;
		return (size);
	}
	if (choice==2){
		printf("\n enter the number of pixels \n");
		scanf("%d",&size);
		return(size);
	}
	if((choice!=1)&&(choice!=2)){
		printf("error");
		exit(1);
	}
	return 0;
}

unsigned char* getpixels(int size)
{
	unsigned char *RGB;
	int imgsize,dim;
	dim=3;
	imgsize=size*3;
	RGB =(unsigned char*)malloc(sizeof(unsigned char)*imgsize);
    inputimgraw("input1.raw",RGB,size,dim);
	printf("done");
	return (RGB);
}
//-------------------------------------------------------------------------------------
