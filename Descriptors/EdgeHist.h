// File Name:  EdgeHist.h
//
// modified by mb - remove unnecessary parts, interface
// date: 2009-11-13

#ifndef __EDGEHIST_H__
#define __EDGEHIST_H__


namespace XM
{

typedef	struct Edge_Histogram_Descriptor
{
  double Local_Edge[80];
} EHD;


class EdgeHistogramDescriptor
{
public:
  EdgeHistogramDescriptor();
  virtual ~EdgeHistogramDescriptor();

  // Accesssor methods
  unsigned long GetSize();
  EHD* GetEdgeHistogram();
  double* GetEdgeHistogramD();

  // EHD Output values as char array of size GetSize() (fixed at 80)
  char*	GetEdgeHistogramElement();

  void	SetEdgeHistogram(EHD* pEdge_Histogram);
  void	SetEdgeHistogramElement(char* pEdge_Histogram);

  // added by mb
  void resetDescriptor();

  // distance between 2 EHDs
  float distance(EdgeHistogramDescriptor* ehd);
  float distanceNorm(EdgeHistogramDescriptor* ehd);

  //virtual unsigned long ExportDDL(GenericDSInterfaceABC *aParentDescription);
  //virtual unsigned long ImportDDL(GenericDSInterfaceABC *aDescription);

public:
  // actual values:
  EHD* m_pEdge_Histogram;
  char* m_pEdge_HistogramElement;

  static double QuantTable[5][8];

};

};

#endif
