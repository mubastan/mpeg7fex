//////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Yong-Sung Kim - Hanyang University, Korea
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
// Applicable File Name:  RegionShapeExtraction.cpp
//

#include <cmath>
#include <iostream>

#include "Extraction/RegionShapeExtraction.h"

//=============================================================================
using namespace XM;

// Static variables

// real-value of RegionShape basis function
double	RegionShapeExtractionTool::m_pBasisR[ART_ANGULAR][ART_RADIAL][ART_LUT_SIZE][ART_LUT_SIZE];

// imaginary-value of RegionShape basis function
double	RegionShapeExtractionTool::m_pBasisI[ART_ANGULAR][ART_RADIAL][ART_LUT_SIZE][ART_LUT_SIZE];
bool	RegionShapeExtractionTool::m_bLUTInit = false;

//=============================================================================
RegionShapeExtractionTool::RegionShapeExtractionTool()
{
    m_Media = 0;
    m_Descriptor = 0;

    m_Descriptor = new RegionShapeDescriptor();
}

//----------------------------------------------------------------------------
RegionShapeExtractionTool::~RegionShapeExtractionTool()
{ // will be released by the clients! (mb - 27.12.2008)
  //if (m_Descriptor)
    //delete m_Descriptor;
}

//----------------------------------------------------------------------------
int RegionShapeExtractionTool:: SetDescriptor(RegionShapeDescriptor
                                 *aRegionShapeDescriptor)
{
  if (m_Descriptor == aRegionShapeDescriptor)
    return 0;

	if (m_Descriptor) delete m_Descriptor;
	m_Descriptor = aRegionShapeDescriptor;

	return 0;
}


//----------------------------------------------------------------------------
RegionShapeDescriptor *RegionShapeExtractionTool::GetDescriptor()
{
  return m_Descriptor;
}

//----------------------------------------------------------------------------
// uses a_chan of input image, foreground pixel values > 0
unsigned long RegionShapeExtractionTool::extract(MomVop* img)
{
	// check for descriptor
	if (!m_Descriptor) return (unsigned long)-1;

	if (!img)
		return (unsigned long)-1;

	m_Media = img;

	GenerateBasisLUT();
	Reset();
	FindCenterOfMass();
	FindRadius();
	ExtractCoefficients();
	Save();

	return 0;
}


//----------------------------------------------------------------------------
inline void RegionShapeExtractionTool::GenerateBasisLUT()
{
	if(m_bLUTInit) return;

	double angle, temp, radius;
	int p, r, x, y;

	int maxradius = ART_LUT_SIZE/2;		// 50/2 = 25

	for(y=0 ; y<ART_LUT_SIZE ; y++)
	for(x=0 ; x<ART_LUT_SIZE ; x++)
	{
		radius = HYPOT(x-maxradius, y-maxradius);
		if(radius < maxradius)
		{
			angle = atan2(y-maxradius, x-maxradius);

			for(p=0 ; p<ART_ANGULAR ; p++)
			for(r=0 ; r<ART_RADIAL ; r++)
			{
				temp = cos(radius*M_PI*r/maxradius);
				m_pBasisR[p][r][x][y] = temp*cos(angle*p);
				m_pBasisI[p][r][x][y]  = temp*sin(angle*p);
			}
		}
		else
		{
			for(p=0 ; p<ART_ANGULAR ; p++)
			for(r=0 ; r<ART_RADIAL ; r++)
			{
				m_pBasisR[p][r][x][y] = 0;
				m_pBasisI[p][r][x][y]  = 0;
			}
		}
	}

	m_bLUTInit = true;
}

//----------------------------------------------------------------------------
// uses a_chan of input image, foreground pixel values > 0
inline void RegionShapeExtractionTool::FindCenterOfMass()
{
	int x, y;
	unsigned int m10=0, m01=0;

	//ImageMedia->a_chan->data->u[j*ImageMedia->width+i]
	unsigned char *pImage = m_Media->a_chan->data->u;
	unsigned char size = datasize(m_Media->a_chan);

	//std::cout<<"Data size : " << (int)size << std::endl;	// 1

	int width = m_Media->width;
	int height = m_Media->height;

	m_mass = 0;

	for ( y = 0; y < height; y++ )
		for ( x = 0; x < width; x++ )
		{
			if( *pImage > 0 )
			{
				m_mass++;
				m10 += x;
				m01 += y;
			}
			pImage += size;
		}

	m_centerX = (double)m10/(double)m_mass;
	m_centerY = (double)m01/(double)m_mass;

	//std::cout<<"Image size : " << m_Media->y_chan->x << "x" << m_Media->y_chan->y << std::endl;
	//std::cout<<"Area: " << m_Media->y_chan->x * m_Media->y_chan->y << std::endl;

	//std::cout<<"m_mass: " << m_mass << std::endl;
	//std::cout<<"Center of mass: (x,y): " << m_centerX << ", " << m_centerY << std::endl;
}

//----------------------------------------------------------------------------
inline void RegionShapeExtractionTool::FindRadius()
{
	int x, y;
	double temp_radius;

	unsigned char *pImage = m_Media->a_chan->data->u;
	unsigned char size = datasize(m_Media->a_chan);

	int width = m_Media->width;
	int height = m_Media->height;

	m_radius = 0;

	for ( y = 0 ; y < height; y++ )
		for ( x = 0 ; x < width; x++)
		{
			// foreground pixel
			if( *pImage > 0 )
			{
				temp_radius = HYPOT(x-m_centerX, y-m_centerY);
				if(temp_radius > m_radius)
					m_radius = temp_radius;
			}
			pImage += size;
		}

	//std::cout<<"Radius: " << m_radius << std::endl;
}

//----------------------------------------------------------------------------
inline void RegionShapeExtractionTool::ExtractCoefficients()
{
	int x, y;
	int p, r;
	double dx, dy, tx, ty;

	unsigned char *pImage = m_Media->a_chan->data->u;
	unsigned char size = datasize(m_Media->a_chan);

	int width = m_Media->width;
	int height = m_Media->height;

	for ( y = 0; y < height; y++ )
		for ( x = 0; x < width; x++ )
		{
			// foreground pixel
			if( *pImage > 0 )
			{
				// map image coordinate (x,y) to basis function coordinate (tx,ty)
				dx = x - m_centerX;
				dy = y - m_centerY;
				tx = ((dx*ART_LUT_RADIUS) / m_radius) + ART_LUT_RADIUS;
				ty = ((dy*ART_LUT_RADIUS) / m_radius) + ART_LUT_RADIUS;

				// summation of basis function
				if(tx >= 0 && tx < ART_LUT_SIZE && ty >= 0 && ty < ART_LUT_SIZE)

				for(p=0 ; p<ART_ANGULAR ; p++)
				for(r=0 ; r<ART_RADIAL ; r++)
				{
					m_pCoeffR[p][r] += GetReal(p, r, tx, ty);
					m_pCoeffI[p][r] -= GetImg(p, r, tx, ty);
				}
			}
			pImage += size;
		}
}

//----------------------------------------------------------------------------
inline double RegionShapeExtractionTool::GetReal(int p, int r, double dx, double dy)
{
	int x = (int)dx;
	int y = (int)dy;

	double ix = dx - x;
	double iy = dy - y;

	double x1 = m_pBasisR[p][r][x][y] + (m_pBasisR[p][r][x+1][y]-m_pBasisR[p][r][x][y]) * ix;
	double x2 = m_pBasisR[p][r][x][y+1] + (m_pBasisR[p][r][x+1][y+1]-m_pBasisR[p][r][x][y+1]) * ix;

	return (x1 + (x2-x1) * iy);
}

//----------------------------------------------------------------------------
inline double RegionShapeExtractionTool::GetImg(int p, int r, double dx, double dy)
{
	int x = (int)dx;
	int y = (int)dy;

	double ix = dx - x;
	double iy = dy - y;

	double x1 = m_pBasisI[p][r][x][y] + (m_pBasisI[p][r][x+1][y]-m_pBasisI[p][r][x][y]) * ix;
	double x2 = m_pBasisI[p][r][x][y+1] + (m_pBasisI[p][r][x+1][y+1]-m_pBasisI[p][r][x][y+1]) * ix;

	return (x1 + (x2-x1) * iy);
}

//----------------------------------------------------------------------------
inline void RegionShapeExtractionTool::Reset()
{
	for(int p=0 ; p<ART_ANGULAR ; p++)
	for(int r=0 ; r<ART_RADIAL ; r++)
	{
		m_pCoeffR[p][r] = 0;
		m_pCoeffI[p][r] = 0;
	}

	m_radius = 0;
}

//----------------------------------------------------------------------------
inline void RegionShapeExtractionTool::Save()
{
	//std::cout <<"Descriptor : " << std::endl;
	for(int r=0 ; r<ART_RADIAL ; r++)
	for(int p=0 ; p<ART_ANGULAR ; p++)
	{
		m_Descriptor->SetElement(p, r, HYPOT(m_pCoeffR[p][r]/m_mass, m_pCoeffI[p][r]/m_mass));
		//std::cout << HYPOT(m_pCoeffR[p][r]/m_mass, m_pCoeffI[p][r]/m_mass) << " ";
	}
	//std::cout << std::endl;
}
