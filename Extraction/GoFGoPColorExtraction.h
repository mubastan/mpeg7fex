///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Stephan Herrmann
// Munich University of technology
// based on the code from
// A. Mufit Ferman - University of Rochester
// Vincent Hsu, June 11, 2001
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
// purpose, assign or donate the code to a third party and inhibit third
// parties from using the code for non MPEG-7 conforming products.
//
// Copyright (c) 1998-2002.
//
// This notice must be included in all copies or derivative works.
//
// Applicable File Name:  GoFGoPColorExtraction.h
//
// modified by mb - remove unnecessary parts,interface
// date: 2009-11-12

#ifndef __GOFGOPCOLOREXTRACTION_H__
#define __GOFGOPCOLOREXTRACTION_H__

#include "Descriptors/ScalableColor.h"
#include "ScalableColorExtraction.h"

#include "AddressLib/vopio.h"

struct gofhistlist_struct{
  unsigned long* histogram;
  struct gofhistlist_struct* next;
};

typedef struct gofhistlist_struct TGOFHISTLIST;

namespace XM
{

class GoFGoPColorExtractionTool
{
public:
  GoFGoPColorExtractionTool(int numCoeff = 256, int bitPlanesDiscarded = 0 );
  ~GoFGoPColorExtractionTool();

  // Also connects the Descriptor (result memnory) to the extraction
  // If set to "0" it automatically creates the descriptor
  //GoFGoPColorExtractionTool(GoFGoPColorDescriptorInterfaceABC* GoFGoPColor);
  GoFGoPColorExtractionTool( ScalableColorDescriptor* scDescriptorInterface );


  // This informs the extractor where the source data comes from
  int SetSourceMedia( MomVop* img );

  // set type of aggregation, default: 0 (average)
  void setAggregation( int aggregationTye = 0 );

  ScalableColorDescriptor* GetDescriptor(void);
  int SetDescriptor( ScalableColorDescriptor* scalableColorDescriptor);

  // reset descriptor, set aggregation
  unsigned long initExtraction( int aggregationTye = 0 );

  // current frame
  unsigned long extract( MomVop* img );

  // This is executed after the extraction: does the aggregation + quantization + Haar Transform
  unsigned long postExtraction(void);

  // reset the GoF
  void reset();

private:

  int m_Aggregation;
  int m_NoOfHistograms;
  TGOFHISTLIST* m_HistogramList;
  void releaseHistograms();

  ScalableColorExtractionTool* m_ScalableColorExtraction;
  ScalableColorDescriptor* m_Descriptor;

};

};


#endif //__GOFGOPCOLOREXTRACTION_H__
