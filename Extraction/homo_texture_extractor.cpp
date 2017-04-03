// This software module was originally developed by
//
// ICU(Information Communication University), Yong-Ju Jung,Ki Won Yoo,Yong Man Ro.
// ETRI, Mun Churl Kim
// Samsung, Yun Ju Yu, Yang Lim Choi.
// in cooperation with UCSB, Hyundai, HHI.
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
// Copyright (c) 1998-2000.
//
// This notice must be included in all copies or derivative works.
//
// Applicable File Name:  homo_texture_extractor.h.cpp
//

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "homo_texture_extractor.h"

#define SWAP(a,b) tempr=(a);(a)=(b);(b)=tempr

#define Quant_level 255
#define	size 128  // image size
#define Nray 128
#define Nview 180



double Num_pixel;

double hdata[5][128];
double vdata[6][180];
double dc;
double stdev;
double vec[5][6];
double dvec[5][6];

double dcmin=0.0, dcmax=255.0;	// 2001.01.31 - yjyu@samsung.com
double stdmin=1.309462,stdmax=109.476530;

double mmax[5][6]={{18.392888,18.014313,18.002143,18.083845,18.046575,17.962099},
		   {19.368960,18.628248,18.682786,19.785603,18.714615,18.879544},
		   {20.816939,19.093605,20.837982,20.488190,20.763511,19.262577},
		   {22.298871,20.316787,20.659550,21.463502,20.159304,20.280403},
		   {21.516125,19.954733,20.381041,22.129800,20.184864,19.999331}};

double mmin[5][6]={{6.549734,8.886816,8.885367,6.155831,8.810013,8.888925},
		   {6.999376,7.859269,7.592031,6.754764,7.807377,7.635503},
		   {8.299334,8.067422,7.955684,7.939576,8.518458,8.672599},
		   {9.933642,9.732479,9.725933,9.802238,10.076958,10.428015},
		   {11.704927,11.690975,11.896972,11.996963,11.977944,11.944282}};

double dmax[5][6]={{21.099482,20.749788,20.786944,20.847705,20.772294,20.747129},
		   {22.658359,21.334119,21.283285,22.621111,21.773690,21.702166},
		   {24.317046,21.618960,24.396872,23.797967,24.329333,21.688523},
		   {25.638742,24.102725,22.687910,25.216958,22.334769,22.234942},
		   {24.692990,22.978804,23.891302,25.244315,24.281915,22.699811}};

double dmin[5][6]={{9.052970,11.754891,11.781252,8.649997,11.674788,11.738701},
		   {9.275178,10.386329,10.066189,8.914539,10.292868,10.152977},
		   {10.368594,10.196313,10.211122,10.112823,10.648101,10.801070},
		   {11.737487,11.560674,11.551509,11.608201,11.897524,12.246614},
		   {13.303207,13.314553,13.450340,13.605001,13.547492,13.435994}};


////////////////////////////////////////////////////////
//double	dcmean,	dcdev, stdmean, stddev;



typedef struct {
	double r,i;
} COMPLEX;
typedef struct {
	double r;  //radius
	double a;  //angle
} CYLINDER;
typedef struct {
	double x;
	double y;
} CARTESIAN;



COMPLEX *timage[1024];
COMPLEX *inimage[512];
COMPLEX *image[512];

COMPLEX operator *(COMPLEX a,double b)
{
	COMPLEX ret;

	ret.r=a.r*b;
	ret.i=a.i*b;
	return ret;
}
//-----------------------------------------------------------------------------------
COMPLEX operator /(COMPLEX a,double b)
{
	COMPLEX ret;

	ret.r=a.r/b;
	ret.i=a.i/b;
	return ret;
}

//-----------------------------------------------------------------------------------
COMPLEX operator *(COMPLEX a,COMPLEX b)
{
	COMPLEX ret;

	ret.r=a.r*b.r-a.i*b.i;
	ret.i=a.r*b.i+a.i*b.r;
	return ret;
}

//-----------------------------------------------------------------------------------
COMPLEX operator +(COMPLEX a,COMPLEX b)
{
	COMPLEX ret;
	ret.r=a.r+b.r;
	ret.i=a.i+b.i;
	return ret;
}

//-----------------------------------------------------------------------------------
COMPLEX operator -(COMPLEX a,COMPLEX b)
{
	COMPLEX ret;
	ret.r=a.r-b.r;
	ret.i=a.i-b.i;
	return ret;
}

//-----------------------------------------------------------------------------------
double sqrt(COMPLEX a)
{
	return /*std::*/sqrt(a.r*a.r+a.i*a.i);
}

//-----------------------------------------------------------------------------------
void swap(double *data,int size2)
{
	int i,center=size2/2;
	double tempr;

	for(i=0;i<center;i++){
		SWAP(data[i],data[i+center]);
	}
}

//-----------------------------------------------------------------------------------
void swap(COMPLEX  *data,int size2)
{
	int i,center=size2/2;
	COMPLEX tempr;

	for(i=0;i<center;i++){
		SWAP(data[i],data[i+center]);
	}
}

//-----------------------------------------------------------------------------------
void four1(COMPLEX *data1,int nn,int isign)
{
	int n,mmax,m,j,istep,i;
	double wtemp,wr,wpr,wpi,wi,theta;
	double tempr,tempi;
	double *data;

	swap(data1,nn);
	data=(double *) data1;

	n=nn << 1;
	j=1;
	for (i=1;i<n;i+=2) {
		if (j > i) {
			SWAP(data[j-1],data[i-1]);
			SWAP(data[j],data[i]);
		}
		m=n >> 1;
		while (m >= 2 && j > m) {
			j -= m;
			m >>= 1;
		}
		j += m;
	}
	mmax=2;

	while (n > mmax) {
		istep=2*mmax;
		theta=6.28318530717959/(isign*mmax);
		wtemp=sin(0.5*theta);
		wpr = -2.0*wtemp*wtemp;
		wpi=sin(theta);
		wr=1.0;
		wi=0.0;
		for (m=1;m<mmax;m+=2) {
			for (i=m;i<=n;i+=istep) {
				j=i+mmax;
				tempr=wr*data[j-1]-wi*data[j];
				tempi=wr*data[j]+wi*data[j-1];
				data[j-1]=data[i-1]-tempr;
				data[j]=data[i]-tempi;
				data[i-1] += tempr;
				data[i] += tempi;
			}
			wr=(wtemp=wr)*wpr-wi*wpi+wr;
			wi=wi*wpr+wtemp*wpi+wi;
		}
		mmax=istep;
	}
	swap(data1,nn);
}

//-----------------------------------------------------------------------------------
void fft2d(COMPLEX **inimage,COMPLEX **timage,int size2,int x,int y,int inc,double dx,double dy)
{
	COMPLEX *ptr;
	COMPLEX  buf[2*size];
	double pix,piy,theta;
	int cx,cy;
	int i,j,k;
	cx=-size/2;
	cy=-size/2;
	pix=2*M_PI*dx/size2;
	piy=2*M_PI*dy/size2;

	for(i=0;i<size2;i++){
		for(j=0;j<size2;j++){
			theta=(i+cy)*piy+(j+cx)*pix;
			image[i][j].r= inimage[i][j].r*cos(theta);
			image[i][j].i= inimage[i][j].r*sin(theta);
		}
		four1(image[i],size2,-1);
	}
	for(i=size/2-1;i<size2;i++){
		for(j=0;j<size2;j++){
			buf[j]=image[j][i];
		}
		four1(buf,size2,-1);
		ptr=buf;
		k=i*inc+y;
		for(j=x;j<size2*inc;j+=inc){
			timage[k][j]=*ptr;
			ptr++;
		}
	}
}

//-----------------------------------------------------------------------------------
COMPLEX getprojfromfft(CARTESIAN cart,COMPLEX **inimage,int size2)
{
	int x,y;
	int x2,y2;
	double rx,ry;

	COMPLEX ret;
	COMPLEX buf1,buf2;

	ret.r=ret.i=0;

	x=(int ) cart.x;
	y=(int ) cart.y;

	rx=cart.x-x;
	ry=cart.y-y;
	if(x<0 || y<0 || x>size2-1 || y>size2-1){
		return ret;
	}

	x2=x+1;		// append
	y2=y+1;		// append

	if(x2==size2) x2=0;	// append
	if(y2==size2) y2=0;	// append


	buf1.r=inimage[y][x].r+(inimage[y][x2].r-inimage[y][x].r)*rx;
	buf1.i=inimage[y][x].i+(inimage[y][x2].i-inimage[y][x].i)*rx;

	buf2.r=inimage[y2][x].r+(inimage[y2][x2].r-inimage[y2][x].r)*rx;
	buf2.i=inimage[y2][x].i+(inimage[y2][x2].i-inimage[y2][x].i)*rx;

	ret.r=buf1.r+(buf2.r-buf1.r)*ry;
	ret.i=buf1.i+(buf2.i-buf1.i)*ry;

	return ret;
}

//-----------------------------------------------------------------------------------
void radon(unsigned char (*cin)[size],double (*fin)[Nray],int nr,int nv)
{
	int i,j;
	int size2,size3,nr2;
	COMPLEX out[512];
	double stepray,stepview;
	double view,ray,dt;
	double cosv,sinv;
	CARTESIAN cartesian;

	size3=Nray/2;

	dc=stdev=0;
	for(i=0;i<size;i++){
		for(j=0;j<size;j++){
			inimage[i][j].r=(double)cin[i][j];
			inimage[i][j].i=0;
			dc+=inimage[i][j].r;
			stdev+=(inimage[i][j].r*inimage[i][j].r);
		}
	}
	dc=(dc)/(size*size);
	stdev=stdev/(size*size);
	stdev=sqrt(stdev-(dc)*(dc));

	fft2d(inimage,timage,size,2,2,3,-2.0/3.0,-2.0/3.0);
	fft2d(inimage,timage,size,1,2,3,-2.0/3.0,-1.0/3.0);
	fft2d(inimage,timage,size,0,2,3,-2.0/3.0,-0.0/3.0);
	fft2d(inimage,timage,size,2,1,3,-1.0/3.0,-2.0/3.0);
	fft2d(inimage,timage,size,2,0,3,-0.0/3.0,-2.0/3.0);
	fft2d(inimage,timage,size,1,1,3,-1.0/3.0,-1.0/3.0);
	fft2d(inimage,timage,size,0,1,3,-1.0/3.0,-0.0/3.0);
	fft2d(inimage,timage,size,1,0,3,-0.0/3.0,-1.0/3.0);
	fft2d(inimage,timage,size,0,0,3,-0.0/3.0,-0.0/3.0);

	nr2=nr/2;
	size2=size*3;
	stepray=((double) 1.0)/nr;
	stepview=((double) M_PI)/nv;

	for(i=0,view=0;i<nv;i++,view=view+stepview){
		cosv=cos(view)*nr;
		sinv=sin(view)*nr;
		for(j=0,ray=0;j<nr2;j++,ray=ray+stepray*3){			// there are errors at this module.
			cartesian.x=(ray*cosv+size2/2);//nr*size;
			cartesian.y=(ray*sinv+size2/2);//nr*size;
			out[j+nr2]=getprojfromfft(cartesian,timage,size2);
		}
		for(j=0;j<nr2;j++){
			out[-j+nr2].r=out[nr2+j].r;
			out[-j+nr2].i=-out[nr2+j].i;
		}
		out[0].r=out[0].i=0;
		for(j=0;j<nr;j++)
			fin[i][j]=out[j].r*out[j].r+out[j].i*out[j].i;	// power spectrum
	}

	size3=Nray/2;

	for(j=0;j<size3;j++)		// lam-rac filtering
	{
		dt=(size3-j)*(size3-j)/16.;
		for(i=0;i<Nview;i++)
			fin[i][j]=dt*fin[i][j];
	}
}

//----------------------------------------------------------------------------
void vatomdesign()
{
	int	i,k;
	int vshft[6]={90,60,30,0,-30,-60};
	double (*buf)=new double [3*180];
	double a,x,cnt;

	a=15./sqrt(2.*log(2));
	cnt=89.5;

	for(i=0;i<180;i++)
	{
		x=(i-cnt)/a;
		buf[i]=exp(-0.5*x*x);
		buf[i+180]=exp(-0.5*x*x);
		buf[i+2*180]=exp(-0.5*x*x);
	}

	for(k=0;k<6;k++)
		for(i=0;i<180;i++)
			vdata[k][i]=buf[i+180+vshft[k]]*buf[i+180+vshft[k]];

	delete [] buf;
}

//--------------------------------------------------------------------------------
void hatomdesign()
{
	int i,k,size2;
	int	shift2[5]={2,5,11,23,47};
	double (*data)[3*128]=new double [5][3*128];
	double BW[5]={2, 4, 8, 16, 32};
	double par[5];
	double a,x,cnt;

	size2=Nray/2;


	for(k=0;k<5;k++)
		par[k]=(BW[k]/2.)/(sqrt(2.*log(2)));

	for(k=0;k<5;k++)
	{
		cnt=(double)size2+0.5;
		for(i=0;i<128;i++)
		{
			x=(i-cnt);
			a=par[k]*par[k];
			data[k][i]=exp(-0.5*(x-128)*(x-128)/a);
			data[k][i+128]=exp(-0.5*x*x/a);
			data[k][i+2*128]=exp(-0.5*(x+128)*(x+128)/a);
		}
	}

	for(k=0;k<5;k++)
		for(i=0;i<128;i++)
			hdata[k][i]=data[k][i+Nray+shift2[k]]*data[k][i+Nray+shift2[k]];


	delete [] data;

}

//-----------------------------------------------------------------------------
void feature(double (*fin)[128],double (*vec)[6],double (*dvec)[6])
{
	int i,j,n,m;
	double t;
	double deviation[5][6];

	for(n=0;n<5;n++)
		for(m=0;m<6;m++){
			vec[n][m]=0;
			deviation[n][m]=0;
		}


	for(m=0;m<6;m++)
	{
		for(i=0;i<180;i++)		// # of angular feature channel = 6
		{
			for(j=0;j<64;j++)
			{
				t=fin[i][j]*vdata[m][i]*hdata[0][j];
				vec[0][m]+=t;
				deviation[0][m]+=(t*t);
			}
			for(j=0;j<64;j++)
			{
				t=fin[i][j]*vdata[m][i]*hdata[1][j];
				vec[1][m]+=t;
				deviation[1][m]+=(t*t);
			}
			for(j=0;j<64;j++)
			{
				t=fin[i][j]*vdata[m][i]*hdata[2][j];
				vec[2][m]+=t;
				deviation[2][m]+=(t*t);
			}
			for(j=0;j<64;j++)
			{
				t=fin[i][j]*vdata[m][i]*hdata[3][j];
				vec[3][m]+=t;
				deviation[3][m]+=(t*t);
			}
			for(j=0;j<64;j++)
			{
				t=fin[i][j]*vdata[m][i]*hdata[4][j];
				vec[4][m]+=t;
				deviation[4][m]+=(t*t);
			}
		}
	}

	for(n=0;n<5;n++)
		for(m=0;m<6;m++)
		{
			vec[n][m]/=Num_pixel;
			deviation[n][m]/=Num_pixel;
			deviation[n][m]=deviation[n][m]-vec[n][m]*vec[n][m];
			deviation[n][m]=sqrt(deviation[n][m]);
			vec[n][m]=log(1+vec[n][m]);
			dvec[n][m]=log(1+deviation[n][m]);
		}
}

//-----------------------------------------------------------------------------
void quantization(void)
{
	int dc1,std1,m1,d1,n,m;
	double dcstep,stdstep,mstep,dstep;

	dcstep=(dcmax-dcmin)/Quant_level;
		dc1=(int)((dc-dcmin)/(dcmax-dcmin)*Quant_level);
		if(dc1>255) dc1=255;
		else if (dc1<0) dc1=0;
		else ;

		dc=dc1;
	stdstep=(stdmax-stdmin)/Quant_level;
		std1=(int)((stdev-stdmin)/(stdmax-stdmin)*Quant_level);
		if(std1>255) std1=255;
		else if (std1<0) std1=0;
		else ;

		stdev=std1;

	for(n=0;n<5;n++)
		for(m=0;m<6;m++)
		{
			mstep=(mmax[n][m]-mmin[n][m])/Quant_level;
				m1=(int)((vec[n][m]-mmin[n][m])/(mmax[n][m]-mmin[n][m])*Quant_level);
				if(m1>255) m1=255;
				else if (m1<0) m1=0;
				else ;
				vec[n][m]=m1;
		}
	for(n=0;n<5;n++)
		for(m=0;m<6;m++)
		{
			dstep=(dmax[n][m]-dmin[n][m])/Quant_level;
				d1=(int)((dvec[n][m]-dmin[n][m])/(dmax[n][m]-dmin[n][m])*Quant_level);
				if(d1>255) d1=255;
				else if (d1<0) d1=0;
				else ;
				dvec[n][m]=d1;
		}
}

//-----------------------------------------------------------------------------
void module(unsigned char *imagedata, int image_height, int image_width)
{
	int	i;
	unsigned char	(*cin)[size]=new unsigned char [size][size];
	double	(*fin)[Nray]=new double [Nview][Nray];
	//double	count=0,count2=0,count3=0,count4=0,count5=0;
	//double	sum=0,min=1.e10;
	for(i=0;i<1024;i++){
		timage[i]=(COMPLEX *) calloc(1024,sizeof(COMPLEX));
	}
	for(i=0;i<512;i++){
		inimage[i]=(COMPLEX *) calloc(512,sizeof(COMPLEX));
		image[i]=(COMPLEX *) calloc(512,sizeof(COMPLEX));
	}


	// 2000.10.11 - yjyu@samsung.com
	for(i=0;i<size;i++)
		for(int j=0;j<size;j++)
			cin[i][j] = imagedata[i*image_width+j];


    radon(cin,fin,Nray,Nview);
	//dc=(dc)*(dc);	// 2001.01.31 - yjyu@samsung.com

    feature(fin,vec,dvec);	// feature extraction

	for(i=0;i<1024;i++)
		free(timage[i]);
	for(i=0;i<512;i++) {
		free(inimage[i]);
		free(image[i]);
	}


	delete	[]	fin;
	delete	[]	cin;

	//////////////////////////////////////////////
	// quantization of features min max extract.//
	//////////////////////////////////////////////

	quantization();


}

//======================================================================================

CHomogeneousTexture_Extractor::CHomogeneousTexture_Extractor()
{
}

//-----------------------------------------------------------------------------------
CHomogeneousTexture_Extractor::~CHomogeneousTexture_Extractor()
{
}


//------------------------------------------------------------------------------------
void CHomogeneousTexture_Extractor::FeatureExtraction(unsigned char* image)
{
	int	n, m;
	Num_pixel = 180*64;


	vatomdesign();
	hatomdesign();
	module(image, image_height, image_width);

	m_dc = (int)dc;
	m_std = (int)stdev;

	for(n=0;n<5;n++)
		for(m=0;m<6;m++)
			mean2[n][m]=(int)(vec[n][m]);		// mean : 30 energy features
	for(n=0;n<5;n++)
		for(m=0;m<6;m++)
			dev2[n][m]=(int)(dvec[n][m]);		// dev	: 30 deviation features

}


//------------------------------------------------------------------------------------
//Arbitrary Shape routine starts here

void mintest(int a, int& min) {
	if (a<min) min = a;
}

//-----------------------------------------------------------------------------
bool maxtest(int a, int& max) {
	if (a>max) {
		max = a;
		return true;
	}
	else
		return false;
}

//-----------------------------------------------------------------------------
void CHomogeneousTexture_Extractor::ArbitraryShape(unsigned char* a_image, unsigned char* y_image, int image_height, int image_width) {

	int i, j, x, y;
	int flag, a_min, a_max;
	int center_x = 0, center_y = 0;
	int a_size, pad_height_count, pad_width_count;
	int **m_arbitraryShape, **m_arbitraryShape_temp;
	unsigned char **m_arbitraryShape_patch;

	m_arbitraryShape = (int **)calloc(image_height+2, sizeof(int *));
	m_arbitraryShape_temp = (int **)calloc(image_height+2, sizeof(int *));
	m_arbitraryShape_patch = (unsigned char **)calloc(image_height, sizeof(unsigned char *));
	for(i=0;i<image_height+2;i++) {
	  m_arbitraryShape[i] = (int *)calloc(image_width+2, sizeof(int));
	  m_arbitraryShape_temp[i] = (int *)calloc(image_width+2, sizeof(int));
	}
	for(i=0;i<image_height;i++)
		m_arbitraryShape_patch[i] = (unsigned char *)calloc(image_width, sizeof(unsigned char));

	flag = 0;
	for(i=1;i<image_height+1;i++) {
		for(j=1;j<image_width+1;j++) {
			if(a_image[(i-1)*image_width+(j-1)] != (unsigned char)0) { // the pixel is white
				flag = 1;
				m_arbitraryShape[i][j]= 1;
				m_arbitraryShape_temp[i][j]= 1;
			}
		}
	}
	if(flag ==0) {
		printf("The mask image is all black!");
		exit(0);
	}

	flag=1;
	a_max=0;
	while(flag) {
		flag=0;
		for(i=0;i<image_height+2;i++) {
			for(j=0;j<image_width+2;j++) {
				m_arbitraryShape[i][j] = m_arbitraryShape_temp[i][j];
				if (maxtest(m_arbitraryShape[i][j],a_max)) {			// maxtest(a, &max): if(a>max) max = a
					center_y = i-1;
					center_x = j-1;
				}
			}
		}

		for(i=1;i<image_height+1;i++) {
			for(j=1;j<image_width+1;j++) {
				if (m_arbitraryShape[i][j] != 0) {
					a_min = m_arbitraryShape[i][j];
					mintest(m_arbitraryShape[i-1][j],a_min);		//mintest(a,&min) if (a<min) min = a;
					mintest(m_arbitraryShape[i+1][j],a_min);
					mintest(m_arbitraryShape[i][j-1],a_min);
					mintest(m_arbitraryShape[i+1][j-1],a_min);
					mintest(m_arbitraryShape[i-1][j-1],a_min);
					mintest(m_arbitraryShape[i][j+1],a_min);
					mintest(m_arbitraryShape[i+1][j+1],a_min);
					mintest(m_arbitraryShape[i-1][j+1],a_min);

					if(m_arbitraryShape[i][j]==a_min) {
						m_arbitraryShape_temp[i][j] = m_arbitraryShape[i][j]+1;
						flag = 1;
					}
					else
						m_arbitraryShape_temp[i][j]=m_arbitraryShape[i][j];
				}
			}
		}

	}	//while(flag ==1)

	//printf("p2\n");

	a_size= a_max-1;
    pad_height_count = (int)( ((float)image_height) / (2.0*((float)a_size)) ) + 1;
	pad_width_count = (int)( ((float)image_width) / (2.0*((float)a_size)) ) + 1;

	for(y=0;y<pad_height_count;y++) {
		for(x=0;x<pad_width_count;x++) {
			for(i=0;i<2*a_size+1;i++) {
				for(j=0;j<2*a_size+1;j++) {
					if( ((i+y*(2*a_size+1))<image_height) && ((j+x*(2*a_size+1))<image_width) )
						m_arbitraryShape_patch[i+y*(2*a_size+1)][j+x*(2*a_size+1)] = y_image[(center_y-a_size+i)*image_width + center_x-a_size+j];
				}
			}
		}
	}

	for(i=0;i<image_height;i++)
		for(j=0;j<image_width;j++)
			y_image[i*image_width+j]=m_arbitraryShape_patch[i][j];

	for(i=0;i<image_height+2;i++) {
	  free(m_arbitraryShape[i]);
	  free(m_arbitraryShape_temp[i]);
	}
	free(m_arbitraryShape);
	free(m_arbitraryShape_temp);
	for(i=0;i<image_height;i++) {
	  free(m_arbitraryShape_patch[i]);
	}
	free(m_arbitraryShape_patch);

}
