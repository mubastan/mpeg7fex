//////////////////////////////////////////////////////////////////////////////
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
// Applicable File Name:  GoFGoPColorExtraction.cpp
//

#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "GoFGoPColorExtraction.h"

//static int initextractionwascalled=0;
extern "C" int GOF_ulongcmp(const void *arg1, const void *arg2);

//----------------------------------------------------------------------------
int GOF_ulongcmp(const void *arg1, const void *arg2)
{
  if (*((unsigned long*)arg1) < *((unsigned long*)arg2)) return -1;
  if (*((unsigned long*)arg1) > *((unsigned long*)arg2)) return 1;
  return 0;
}
//=============================================================================


using namespace XM;


//=============================================================================
GoFGoPColorExtractionTool::GoFGoPColorExtractionTool( int numCoeff, int bitPlanesDiscarded ):
m_Aggregation(0),
m_NoOfHistograms(0),
m_HistogramList(0)

{
    this->m_ScalableColorExtraction = new ScalableColorExtractionTool( numCoeff, bitPlanesDiscarded );	// default values: (coeff, bitPlanesDisc)=(256, 0)
    this->m_Descriptor = this->m_ScalableColorExtraction->GetDescriptor();
}

//----------------------------------------------------------------------------
GoFGoPColorExtractionTool::~GoFGoPColorExtractionTool()
{
  // release descriptor
  // will be done by the clients
  //if (m_Descriptor)
  //  m_Descriptor->release();

  if (m_ScalableColorExtraction)
	  delete m_ScalableColorExtraction;

  // release the histograms
  this->releaseHistograms();
}


//----------------------------------------------------------------------------
// This informs the extractor where the source data is coming from. This
// could be an audio file, image stream etc. Here it's just a filename.
int GoFGoPColorExtractionTool::SetSourceMedia( MomVop* img)
{
  if( !img ) return -1;

  if ( m_ScalableColorExtraction )
    m_ScalableColorExtraction->SetSourceMedia( img );

  return 0;
}


//----------------------------------------------------------------------------
ScalableColorDescriptor* GoFGoPColorExtractionTool::GetDescriptor(void)

{
  return m_Descriptor;
}

//----------------------------------------------------------------------------
int GoFGoPColorExtractionTool::SetDescriptor( ScalableColorDescriptor* scalableColorDescriptor )

{
  if ( m_Descriptor == scalableColorDescriptor ) return 0;

  if ( m_Descriptor )
	  delete m_Descriptor;

  m_Descriptor = scalableColorDescriptor;

  if ( m_Descriptor && m_ScalableColorExtraction )
      m_ScalableColorExtraction->SetDescriptor( m_Descriptor );

  return 0;
}

// set type of aggregation, default: 0 (average)
void GoFGoPColorExtractionTool::setAggregation( int aggregationTye )
{
	if(aggregationTye == 0 || aggregationTye == 1 || aggregationTye == 2)
		m_Aggregation = aggregationTye;
	else
		m_Aggregation = 0;		// default: average

}
//----------------------------------------------------------------------------
// reset the extraction process
void GoFGoPColorExtractionTool::reset()
{
	this->m_Descriptor->resetDescriptor();
	this->releaseHistograms();
}

//----------------------------------------------------------------------------

// This initialises the extraction process.
unsigned long GoFGoPColorExtractionTool::initExtraction( int aggregationTye )

{
  /* example for initializing extraction paramters*/
  //m_Aggregation=atoi(getpara("AggregationType","0"/*default value*/));
  setAggregation( aggregationTye );

  this->releaseHistograms();

  return(0UL);
}

//----------------------------------------------------------------------------
// This uses the img pointer to extract the Scalable Color histogram and add to the list of hitograms
unsigned long GoFGoPColorExtractionTool::extract( MomVop* img )

{
  // select media type you require and remove comment
  unsigned long no_of_bins;
  unsigned long *SC_histogram, *histogram;

  if (!img)
	  return (unsigned long)-1;

  SetSourceMedia( img );		// set the media for Scalable Color extraction

  // set descriptor values to zero
  m_Descriptor->resetDescriptor();

  // extraction of ScalableColor description
  SC_histogram = m_ScalableColorExtraction->ComputeNormalizedHistogram();
  if (!SC_histogram) {
    fprintf(stderr,"Error: GoFGoPColorExtractionTool::extract did not get the histogram from ScalableColor\n");
    return (unsigned long) -1;
  }

  no_of_bins = m_ScalableColorExtraction->GetDescriptor()->GetNumberOfCoefficients();


  if (!m_HistogramList) {

    m_HistogramList=(TGOFHISTLIST*)malloc(sizeof(TGOFHISTLIST));
    if (!m_HistogramList) {
      fprintf(stderr,"Error: Out of memory\n");
      return (unsigned long) -1;
    }
    /* create new copy of histogram and add histogram to linked list*/
    histogram = (unsigned long*)malloc(no_of_bins*sizeof(unsigned long));
    if (!histogram) {
      fprintf(stderr,"Error: Out of memory\n");
      return (unsigned long) -1;
    }
    memcpy( histogram, SC_histogram, no_of_bins*sizeof(unsigned long) );
    m_HistogramList->histogram = histogram;
    m_HistogramList->next=0;
    m_NoOfHistograms = 1;
  }
  else {
    unsigned long i;
    switch ( m_Aggregation ) {
		case 0:			//average
		  histogram = m_HistogramList->histogram;
		  // add to the current histogram (mb)
		  for ( i = 0; i < no_of_bins; i++ ){
			*histogram += *SC_histogram;
			histogram++;
			SC_histogram++;
		  }
		  break;
		case 1:			//median
		  {
			/* create new histogram copy and put it as first element in the list*/
			TGOFHISTLIST *newhist;

			newhist=(TGOFHISTLIST*)malloc(sizeof(TGOFHISTLIST));
			if (!newhist) {
			  fprintf(stderr,"Error: Out of memory\n");
			  return (unsigned long) -1;
			}
			/* create new copy of histogram and add histogram to linked list*/
			histogram=(unsigned long*)malloc(no_of_bins*sizeof(unsigned long));
			if (!histogram) {
			  fprintf(stderr,"Error: Out of memory\n");
			  return (unsigned long) -1;
			}
			memcpy(histogram,SC_histogram,no_of_bins*sizeof(unsigned long));
			newhist->histogram=histogram;
			newhist->next=m_HistogramList;
			newhist=m_HistogramList;
			m_NoOfHistograms++;
			  }
		  break;
		case 2://intersection
			histogram = m_HistogramList->histogram;
			for ( i = 0; i < no_of_bins; i++ ){
				if ( *histogram > *SC_histogram )
					*histogram = *SC_histogram;
				histogram++;
				SC_histogram++;
		  }
		  break;
		default:
		  fprintf(stderr,"Warning: Unkown Aggregation type\n");
		}

  }
  // All done
  return 0;
}

//----------------------------------------------------------------------------

// aggregation + quantization + Haar Transform after extraction
unsigned long GoFGoPColorExtractionTool::postExtraction(void)

{
  unsigned long no_of_bins,i;
  unsigned long *onebin=0;
  unsigned long *histogram=0;

  no_of_bins = m_ScalableColorExtraction->GetDescriptor()->GetNumberOfCoefficients();

  // median
  if ( m_Aggregation == 1 ) {
    onebin = (unsigned long*)malloc(m_NoOfHistograms*sizeof(unsigned long));
    if (!onebin) {
      fprintf(stderr,"Error: Out of memory\n");
      return (unsigned long) -1;
    }
  }

  histogram = m_HistogramList->histogram;

  switch (m_Aggregation) {
  case 0:					//average
    for ( i = 0; i < no_of_bins; i++ )
	{
      *histogram = (unsigned long)(((double)*histogram/(double)m_NoOfHistograms)+0.5);
      histogram++;
    }
    break;

  case 1:				//median
    for ( i = 0; i < no_of_bins; i++ )
	{
      int j;
      TGOFHISTLIST *currhist;

      currhist=m_HistogramList;
      for (j=0; j<m_NoOfHistograms; j++)
	  {
		onebin[j]=currhist->histogram[j];
		currhist=currhist->next;
      }
      qsort( onebin, m_NoOfHistograms, sizeof(unsigned long), &GOF_ulongcmp);
      *histogram=onebin[m_NoOfHistograms>>1];
      histogram++;
    }
    break;
  case 2://intersection
    break;
  default:
    fprintf(stderr,"Warning: Unkown Aggregation type\n");
  }

  m_ScalableColorExtraction->QuantizeHistogram(m_HistogramList->histogram);		// modifies the histogram in the descriptor
  m_ScalableColorExtraction->HaarTransform(0);	// if 0, gets the histogram from the descriptor

  if (onebin) free(onebin);

  return(0UL);
}

void GoFGoPColorExtractionTool::releaseHistograms()
{

	// destroy list
	while (m_HistogramList) {

		TGOFHISTLIST *next;

		next = m_HistogramList->next;
		if ( m_HistogramList->histogram )
			free( m_HistogramList->histogram );

		free( m_HistogramList );
		m_HistogramList = next;
	}

    m_NoOfHistograms = 0;

}
