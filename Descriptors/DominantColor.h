///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// University of California Santa Barbara, Kapil Chhabra, Yining Deng.
// Mitsubishi Electric ITE-VIL, Leszek Cieplinski.
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
// Applicable File Name:  DominantColor.h
//
// modified by mb - remove unnecessary parts,interface
// date: 2009-11-13

#ifndef __DOMINANTCOLOR_H__
#define __DOMINANTCOLOR_H__

#include "ColorSpace.h"
#include "ColorQuant.h"

namespace XM
{

typedef struct DominantColor
{
  int m_Percentage;			// MPEG-7 values: 0-32 (5 bit)
  int m_ColorValue[3];		// MPEG-7 values: 12 bit
  int m_ColorVariance[3];	// MPEG-7 values: 0,1 (1 bit)
} DOMCOL;

class DominantColorDescriptor
{
public:

  DominantColorDescriptor();
  virtual ~DominantColorDescriptor();

  // accessor methods
  virtual ColorQuantizerDescriptor* GetColorQuantizerDescriptor();
  virtual unsigned long SetColorQuantizerDescriptor(ColorQuantizerDescriptor *);
  virtual ColorSpaceDescriptor* GetColorSpaceDescriptor();
  virtual unsigned long SetColorSpaceDescriptor(ColorSpaceDescriptor *);

  virtual int GetDominantColorsNumber();
  virtual bool GetColorSpacePresent() {return m_ColorSpacePresent;};
  virtual bool GetColorQuantizationPresent() {return m_ColorQuantizationPresent;};
  virtual bool GetVariancePresent() {return m_VariancePresent;};
  virtual int GetSpatialCoherency();
  virtual DOMCOL* GetDominantColors();

  virtual void SetDominantColorsNumber(int dc_number);
  virtual void SetColorSpacePresent(bool colspcpres){ m_ColorSpacePresent = colspcpres; }
  virtual void SetColorQuantizationPresent(bool colquantpres){ m_ColorQuantizationPresent = colquantpres; }
  virtual void SetVariancePresent(bool varpres) { m_VariancePresent = varpres; }
  virtual void SetSpatialCoherency(int sc);
  virtual void SetDominantColors(int *percents, int **colors, int **variances);

  virtual void resetDescriptor();

  virtual void Print();

  //virtual unsigned long ExportDDL(GenericDSInterfaceABC *aParentDescription);
  //virtual unsigned long ImportDDL(GenericDSInterfaceABC *aDescription);

private:

    // descriptor data
    int m_DominantColorsNumber;			// 1-8
    bool m_ColorSpacePresent;
    bool m_ColorQuantizationPresent;
    bool m_VariancePresent;
    int m_SpatialCoherency;
    DOMCOL* m_dc;							// defined above, keeps the dominant color statistics
    ColorSpaceDescriptor* m_ColorSpace;
    ColorQuantizerDescriptor* m_Quantizer;

};

};

#endif
