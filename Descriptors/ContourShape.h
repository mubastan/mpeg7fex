///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Mitsubishi Electric ITE-VIL
// W.Price
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
// Applicable File Name:  ContourShape.h
//
// modified by mb - remove unnecessary parts,interface
// date: 2009-11-14

#ifndef __CONTOUR_SHAPE__
#define __CONTOUR_SHAPE__


#define BITS_TO_MASK(a)             ((2 << ((a) - 1)) - 1)

#define CONTOURSHAPE_YP            0.05

#define CONTOURSHAPE_AP            0.09
#define CONTOURSHAPE_MAXCSS        10
#define CONTOURSHAPE_T             0.000001
#define CONTOURSHAPE_TXA0          3.8
#define CONTOURSHAPE_TXA1          0.6

#define CONTOURSHAPE_CSSPEAKBITS   6
#define CONTOURSHAPE_XBITS         6
#define CONTOURSHAPE_YBITS         7
#define CONTOURSHAPE_YnBITS        3
#define CONTOURSHAPE_CBITS         6
#define CONTOURSHAPE_EBITS         6

#define CONTOURSHAPE_ETHR          0.6
#define CONTOURSHAPE_CTHR          1.0
#define CONTOURSHAPE_ECOST         0.4
#define CONTOURSHAPE_CCOST         0.3

#define CONTOURSHAPE_NMATCHPEAKS   2
#define CONTOURSHAPE_TMATCHPEAKS   0.9

#define CONTOURSHAPE_XMAX          1.0
#define CONTOURSHAPE_YMAX          1.7
#define CONTOURSHAPE_CMIN          12.0
#define CONTOURSHAPE_CMAX          110.0
#define CONTOURSHAPE_EMIN          1.0
#define CONTOURSHAPE_EMAX          10.0

#define CONTOURSHAPE_CSSPEAKMASK   BITS_TO_MASK(CONTOURSHAPE_CSSPEAKBITS)
#define CONTOURSHAPE_XMASK         BITS_TO_MASK(CONTOURSHAPE_XBITS)
#define CONTOURSHAPE_YMASK         BITS_TO_MASK(CONTOURSHAPE_YBITS)
#define CONTOURSHAPE_YnMASK        BITS_TO_MASK(CONTOURSHAPE_YnBITS)
#define CONTOURSHAPE_CMASK         BITS_TO_MASK(CONTOURSHAPE_CBITS)
#define CONTOURSHAPE_EMASK         BITS_TO_MASK(CONTOURSHAPE_EBITS)


namespace XM
{

class Point2
{
public:
  double x, y;
};

class ContourShapeDescriptor
{
public:
	ContourShapeDescriptor();
	virtual ~ContourShapeDescriptor();

	// accessor methods
	virtual void SetNoOfPeaks(unsigned char cPeaks);
	virtual unsigned char GetNoOfPeaks() const;
	virtual void SetGlobalCurvature(unsigned long lC, unsigned long lE);
	virtual void GetGlobalCurvature(unsigned long &lC, unsigned long &lE) const;
	virtual void SetPrototypeCurvature(unsigned long lC, unsigned long lE);
	virtual void GetPrototypeCurvature(unsigned long &lC, unsigned long &lE) const;
	virtual void SetHighestPeakY(unsigned short iHigh);
	virtual unsigned short GetHighestPeakY() const;
	virtual void SetPeak(unsigned char cIndex, unsigned short iX, unsigned short iY);
	virtual void GetPeak(unsigned char cIndex, unsigned short &iX, unsigned short &iY);

	//print the values
	virtual void Print();

	//virtual unsigned long ExportDDL(GenericDSInterfaceABC *aParentDescription);
	//virtual unsigned long ImportDDL(GenericDSInterfaceABC *aDescription);

private:

	// descriptor data
	unsigned char m_cPeaksCount;
	unsigned long m_piGlobalCurvatureVector[2];
	unsigned long m_piPrototypeCurvatureVector[2];
	unsigned short m_iHighestPeakY;
	unsigned short *m_pPeak;

};

};

#endif
