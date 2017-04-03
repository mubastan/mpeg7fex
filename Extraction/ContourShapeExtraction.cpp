///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Mitsubishi Electric
// ITE-VIL, W.Price
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
// purpose, assign or donate the code to a third party and inhibit third parties// from using the code for non MPEG-7 conforming products.
//
// Copyright (c) 1998-2000.
//
// This notice must be included in all copies or derivative works.
//
// Applicable File Name:  ContourShapeExtraction.cpp
//
///////////////////////////////////////////////////////////////////////////////

// Set this if the images being processed are white on a black background
#define WHITE_ON_BLACK

#include <cmath>
#include <cstring>
#include <cstdlib>

#include "Descriptors/ContourShape.h"
#include "ContourShapeExtraction.h"

#ifndef M_PI
#define M_PI (3.14159265358979323846)
#endif

using namespace XM;

//=============================================================================
ContourShapeExtractionTool::ContourShapeExtractionTool():
 m_Descriptor(0),
 m_ImageMedia(0)
{
    m_Descriptor = new ContourShapeDescriptor();
}

//----------------------------------------------------------------------------
ContourShapeExtractionTool::~ContourShapeExtractionTool()
{
  // commented out - mb - 07.07.2008
  //if (m_Descriptor)
  //  m_Descriptor->release();
}


//----------------------------------------------------------------------------
ContourShapeDescriptor* ContourShapeExtractionTool::GetDescriptor()
{
  return m_Descriptor;
}


//----------------------------------------------------------------------------
int ContourShapeExtractionTool::SetDescriptor(ContourShapeDescriptor* aContourShapeDescriptor)

{
    if (m_Descriptor == aContourShapeDescriptor) return 0;

    if (m_Descriptor)
    delete m_Descriptor;

    m_Descriptor = aContourShapeDescriptor;

    return 0;
}

//----------------------------------------------------------------------------
// alpha channel (a_chan) or y_chan of MomVop image is the object mask
unsigned long ContourShapeExtractionTool::extract(MomVop *img)
{
  const int CONTOUR_SIZE = 500;

  if (!m_Descriptor)
    return (unsigned long)-1;


  m_ImageMedia = img;
  if (m_ImageMedia==0) return (unsigned long)-1;

  Point2 *coords = new Point2[CONTOUR_SIZE];
  int nContour = ExtractContour(CONTOUR_SIZE, coords);

  if (nContour > 0)
  {
    ExtractPeaks(nContour, coords);
  }

  delete[] coords;

  return 0;
}

//----------------------------------------------------------------------------
// alpha channel (a_chan) or y_chan of MomVop image is the object mask
unsigned long ContourShapeExtractionTool::ExtractContour(int n, Point2* const &ishp)
{
  int dr[] = {  0, -1, -1, -1,  0,  1,  1,  1 };
  int dc[] = {  1,  1,  0, -1, -1, -1,  0,  1 };

  int size = 0;
  Point2* xy = 0;
  MomImage* mask_chan;

  if (m_ImageMedia->a_chan) {
    mask_chan=m_ImageMedia->a_chan;
  }
  else {
    mask_chan=m_ImageMedia->y_chan;
  }

  for (unsigned int r = 0; (r < mask_chan->y) && (size == 0); r++)
  {

#ifdef WHITE_ON_BLACK
    unsigned char lastPel = 0;
#else // !WHITE_ON_BLACK
    unsigned char lastPel = 255;
#endif // !WHITE_ON_BLACK

    for (unsigned int c = 0; (c < mask_chan->x) && (size == 0); c++)
    {
      unsigned char thisPel = *xydata(mask_chan, c, r);

      if (thisPel != lastPel)
      {
        unsigned char        dir = 0, dir0 = 0;
        unsigned int cr = r;
        unsigned int cc = c;
        unsigned char *p[8];

        do
        {
          p[0] = xydata(mask_chan, cc + 1, cr    );
          p[1] = xydata(mask_chan, cc + 1, cr - 1);
          p[2] = xydata(mask_chan, cc    , cr - 1);
          p[3] = xydata(mask_chan, cc - 1, cr - 1);
          p[4] = xydata(mask_chan, cc - 1, cr    );
          p[5] = xydata(mask_chan, cc - 1, cr + 1);
          p[6] = xydata(mask_chan, cc    , cr + 1);
          p[7] = xydata(mask_chan, cc + 1, cr + 1);

          int i;
          for (i = 0; i < 8; i++)
          {
#ifdef WHITE_ON_BLACK
            if (p[(dir+3-i)&7] && (*p[(dir+3-i)&7] != 0))
            {
              if (!p[(dir+4-i)&7])
              {
                dir = (dir+3-i)&7;
                break;
              }
              else if (*p[(dir+4-i)&7] == 0)
              {
                dir = (dir+3-i)&7;
                break;
              }
            }
#else // !WHITE_ON_BLACK
            if (p[(dir+3-i)&7] && (*p[(dir+3-i)&7] == 0))
            {
              if (!p[(dir+4-i)&7])
              {
                dir = (dir+3-i)&7;
                break;
              }
              else if (*p[(dir+4-i)&7] != 0)
              {
                dir = (dir+3-i)&7;
                break;
              }
            }
#endif // !WHITE_ON_BLACK
          }

          if (i == 8)
            break;
          else if (size == 0)
            dir0 = dir;
          else if ((cr == r) && (cc == c) && (dir == dir0))
            break;

          if ((size % 32) == 0)
          {
            Point2 *nxy = new Point2[size+32];
            memset(nxy, 0, (size+32)*sizeof(Point2));
            if (size > 0)
            {
              memcpy(nxy, xy, size*sizeof(Point2));
              delete[] xy;
            }
            xy = nxy;
          }

          xy[size].x = cc;
          xy[size].y = cr;
          size++;

          cr += dr[dir]; cc += dc[dir];

        } while (1);
      }
    }
  }

  if (size == 0)
    return 0;

  double per = 0.0;
  double *dst = new double[size];
  for (int i1 = 0; i1 < size; i1++)
  {
    int i2 = (i1 == 0) ? (size-1) : (i1-1);
    double dx = xy[i1].x - xy[i2].x;
    double dy = xy[i1].y - xy[i2].y;
    dst[i1] = sqrt(dx*dx + dy*dy);
    per += dst[i1];
  }

  double del = per / (double)n;

  int cur = 0;
  double oldd = dst[cur];
  ishp[0] = xy[0];
  for (int j = 1; j < n; j++)
  {
    if (oldd > del)
    {
      double f = del / oldd;
      oldd -= del;
      int i1 = (cur < size-1) ? (cur + 1) : 0;
      double xs = f*(xy[i1].x - ishp[j-1].x);
      double ys = f*(xy[i1].y - ishp[j-1].y);
      ishp[j].x = ishp[j-1].x + xs;
      ishp[j].y = ishp[j-1].y + ys;
    }
    else
    {
      double newd = oldd + dst[++cur];
      while (newd < del)
        newd += dst[++cur];
      oldd = newd - del;
      double f = (dst[cur] - oldd) / dst[cur];
      int i1 = (cur < size-1) ? (cur + 1) : 0;
      double xs = f*(xy[i1].x - xy[cur].x);
      double ys = f*(xy[i1].y - xy[cur].y);
      ishp[j].x = xy[cur].x + xs;
      ishp[j].y = xy[cur].y + ys;
    }
  }

  delete[] dst;
  delete[] xy;

  return n;
}

//----------------------------------------------------------------------------
unsigned long ContourShapeExtractionTool::ExtractPeaks(int n, const Point2 * const &ishp)
{
  Point2 *fshp = new Point2[n];

  Point2 *peaks = new Point2[CONTOURSHAPE_MAXCSS];
  int nPeaks = 0;

  Point2 *dxdy = new Point2[n];
  Point2 *d2xd2y = new Point2[n];
  Point2 *ang  = new Point2[n];
  Point2 *fxfy = new Point2[n];

  double *nMinima = new double[n];
  double *nMaxima = new double[n];
  int nNmin = 0, nNmax = 0;

  double *oMinima = new double[n];
  double *oMaxima = new double[n];
  int oNmin = 0, oNmax = 0;

  for (int n1 = 0; n1 < n; n1++)
  {
    int n2 = (n1 > 0) ? (n1 - 1) : (n - 1);
    dxdy[n1].x = ishp[n1].x - ishp[n2].x;
    dxdy[n1].y = ishp[n1].y - ishp[n2].y;
  }

  int rec = 0, maxrec = (int)(0.262144 * n * n);
  do
  {
    if (nNmin > 0)
      memcpy(oMinima, nMinima, nNmin*sizeof(double));
    oNmin = nNmin;

    if (nNmax > 0)
      memcpy(oMaxima, nMaxima, nNmax*sizeof(double));
    oNmax = nNmax;

    ang[0].x = 0.0;
    d2xd2y[0].x = dxdy[0].x - dxdy[n-1].x;
    d2xd2y[0].y = dxdy[0].y - dxdy[n-1].y;
    double len = sqrt(dxdy[0].x*dxdy[0].x + dxdy[0].y*dxdy[0].y);
    for (int i1 = 1; i1 < n; i1++)
    {
      ang[i1].x = len;
      d2xd2y[i1].x = dxdy[i1].x - dxdy[i1-1].x;
      d2xd2y[i1].y = dxdy[i1].y - dxdy[i1-1].y;
      len += sqrt(dxdy[i1].x*dxdy[i1].x + dxdy[i1].y*dxdy[i1].y);
    }

    double ilen = 1.0/len;
    for (int i2 = 0; i2 < n; i2++)
    {
      ang[i2].x *= ilen;
      ang[i2].y = dxdy[i2].x*d2xd2y[i2].y - dxdy[i2].y*d2xd2y[i2].x;
    }

    nNmin = nNmax = 0;
    double y0, x0;
    double x1 = ang[0].x;
    double y1 = ang[0].y;
    for (int j1 = 0; j1 < n; j1++)
    {
      int j1w = j1 + 1;
      while (j1w >= n) j1w -= n;
      x0 = x1;
      y0 = y1;
      x1 = ang[j1w].x;
      y1 = ang[j1w].y;
      if ((y0 < -CONTOURSHAPE_T) && (y1 >= -CONTOURSHAPE_T))
      {
        for (int j2 = j1+1; j2 < j1+n; j2++)
        {
          int j2w = j2;
          while (j2w >= n) j2w -= n;
          double y2 = ang[j2w].y;

          if (y2 < -CONTOURSHAPE_T)
            break;

          if (y2 >= CONTOURSHAPE_T)
          {
            double x2 = ang[j2w].x;
            double dx = x2 - x0;
            while (dx < 0.0) dx += 1.0;
            double x = -y0 * dx / (y2 - y0) + x0;
            while (x > 1.0) x -= 1.0;
            nMinima[nNmin] = x;
            nNmin++;
            break;
          }
        }
      }
      if ((y0 >= CONTOURSHAPE_T) && (y1 < CONTOURSHAPE_T))
      {
        for (int j2 = j1+1; j2 < j1+n; j2++)
        {
          int j2w = j2;
          while (j2w >= n) j2w -= n;
          double y2 = ang[j2w].y;

          if (y2 >= CONTOURSHAPE_T)
            break;

          if (y2 < -CONTOURSHAPE_T)
          {
            double x2 = ang[j2w].x;
            double dx = x2 - x0;
            while (dx < 0.0) dx += 1.0;
            double x = -y0 * dx / (y2 - y0) + x0;
            while (x > 1.0) x -= 1.0;
            nMaxima[nNmax] = x;
            nNmax++;
            break;
          }
        }
      }
    }

    for (int f1 = 0; f1 < n; f1++)
    {
      int f0 = (f1 > 0) ? (f1 - 1) : (n - 1);
      int f2 = (f1 < n - 1) ? (f1 + 1) : 0;
      fxfy[f1].x = 0.25 * (dxdy[f0].x + 2.0*dxdy[f1].x + dxdy[f2].x);
      fxfy[f1].y = 0.25 * (dxdy[f0].y + 2.0*dxdy[f1].y + dxdy[f2].y);
    }
    memcpy(dxdy, fxfy, n*sizeof(Point2));

    if ((nNmin < oNmin) && (nNmax < oNmax) &&
        (oNmin <= (CONTOURSHAPE_MAXCSS)) &&
        (oNmax <= (CONTOURSHAPE_MAXCSS)))
    {
      for (int m1 = 0; m1 < nNmin; m1++)
      {
        int idx = 0;
        double diff = 9999.9;
        for (int k1 = 0; k1 < oNmin; k1++)
        {
          double d = fabs(nMinima[m1] - oMinima[k1]);
          if (d > 0.5) d = 1.0 - d;
          if (d < diff)
          {
            idx = k1;
            diff = d;
          }
        }
        oNmin--;
        if (idx < oNmin)
          memmove(&oMinima[idx], &oMinima[idx+1], (oNmin-idx)*sizeof(double));
      }

      for (int m2 = 0; m2 < nNmax; m2++)
      {
        int idx = 0;
        double diff = 9999.9;
        for (int k1 = 0; k1 < oNmax; k1++)
        {
          double d = fabs(nMaxima[m2] - oMaxima[k1]);
          if (d > 0.5) d = 1.0 - d;
          if (d < diff)
          {
            idx = k1;
            diff = d;
          }
        }
        oNmax--;
        if (idx < oNmax)
          memmove(&oMaxima[idx], &oMaxima[idx+1], (oNmax-idx)*sizeof(double));
      }

      while (oNmin)
      {
        int idx = 0;
        double diff = 9999.9;
        for (int m3 = 0; m3 < oNmax; m3++)
        {
          double d = fabs(oMaxima[m3] - oMinima[0]);
          if (d > 0.5) d = 1.0 - d;
          if (d < diff)
          {
            idx = m3;
            diff = d;
          }
        }

        double x = 0.5 * (oMinima[0] + oMaxima[idx]);
        if (fabs(oMinima[0] - oMaxima[idx]) > 0.5)
        {
          if (x > 0.5) x -= 0.5;
          else         x += 0.5;
        }

        int xidx = 0;
        diff = fabs(ang[0].x - x);
        if (diff > 0.5) diff = 1.0 - diff;
        for (int l1 = 1; l1 < n; l1++)
        {
          double d = fabs(ang[l1].x - x);
          if (d > 0.5) d = 1.0 - d;
          if (d < diff)
          {
            diff = d;
            xidx = l1;
          }
        }

        memmove(&peaks[1], &peaks[0], (CONTOURSHAPE_MAXCSS-1)*sizeof(peaks[0]));
        peaks[0].x = (double)xidx;
        peaks[0].y = rec;
        if (nPeaks < CONTOURSHAPE_MAXCSS)
          nPeaks++;

        if (--oNmin)
          memmove(&oMinima[0], &oMinima[1], oNmin*sizeof(oMinima[0]));
        if (--oNmax > idx)
          memmove(&oMaxima[idx], &oMaxima[idx+1], (oNmax-idx)*sizeof(oMaxima[0]));
      }
    }

    rec++;

  } while ((rec < maxrec) && (nNmin > 0) && (nNmax > 0));

  delete[] oMaxima;
  delete[] oMinima;
  delete[] nMaxima;
  delete[] nMinima;
  delete[] fxfy;
  delete[] ang;

  double xc = 0.0, yc = 0.0;
  double len = 0;
  for (int s1 = 0; s1 < n; s1++)
  {
    len += sqrt(dxdy[s1].x*dxdy[s1].x + dxdy[s1].y*dxdy[s1].y);
    xc += dxdy[s1].x;
    yc += dxdy[s1].y;
    fshp[s1].x = xc;
    fshp[s1].y = yc;
  }

  double nsmap = 1.0 / ((double)n*(double)n);
  for (int p1 = 0; p1 < nPeaks; p1++)
  {
    double pl = 0.0;
    for (int p2 = 0; p2 < peaks[p1].x; p2++)
      pl += sqrt(dxdy[p2].x*dxdy[p2].x + dxdy[p2].y*dxdy[p2].y);
    peaks[p1].x = pl / len;
    peaks[p1].y = CONTOURSHAPE_TXA0 * pow(peaks[p1].y*nsmap, CONTOURSHAPE_TXA1);
  }

  delete[] d2xd2y;
  delete[] dxdy;

  double offset = peaks[0].x;
  for (int p2 = 0; p2 < nPeaks; p2++)
  {
    peaks[p2].x -= offset;
    if (peaks[p2].x < 0.0)
      peaks[p2].x += 1.0;
  }

  if (peaks[0].y < CONTOURSHAPE_AP)
    nPeaks = 0;

  for (int p3 = 0; p3 < nPeaks; p3++)
  {
    if (peaks[p3].y < CONTOURSHAPE_YP*peaks[0].y)
      nPeaks = p3;
  }

  m_Descriptor->SetNoOfPeaks(nPeaks);
  if (nPeaks == 0)
    m_Descriptor->SetHighestPeakY(0);

  double py = 0.0;
  for (int i = 0; i < nPeaks; i++)
  {
    unsigned short qx, qy;
    qx = (unsigned short)((int)((CONTOURSHAPE_XMASK * peaks[i].x / CONTOURSHAPE_XMAX) + 0.5) & CONTOURSHAPE_XMASK);
    if (i == 0)
    {
      unsigned long qyl = (unsigned long)((CONTOURSHAPE_YMASK * peaks[i].y / CONTOURSHAPE_YMAX) + 0.5);
      if (qyl > CONTOURSHAPE_YMASK)
        qy = CONTOURSHAPE_YMASK;
      else
        qy = (unsigned short)(qyl & CONTOURSHAPE_YMASK);
      py = (qy * CONTOURSHAPE_YMAX / (double)CONTOURSHAPE_YMASK);
    }
    else
    {
      unsigned long qyl = (unsigned long)((CONTOURSHAPE_YnMASK * peaks[i].y / py) + 0.5);
      if (qyl > CONTOURSHAPE_YnMASK)
        qy = CONTOURSHAPE_YnMASK;
      else
        qy = (unsigned short)(qyl & CONTOURSHAPE_YnMASK);
      py = (qy * py / (double)CONTOURSHAPE_YnMASK);
    }
    m_Descriptor->SetPeak(i, qx, qy);
    if (i == 0)
      m_Descriptor->SetHighestPeakY(qy);
  }

  unsigned long qe, qc;
  ExtractCurvature(n, ishp, qc, qe);
  m_Descriptor->SetGlobalCurvature(qc, qe);

  if (nPeaks > 0)
  {
    ExtractCurvature(n, fshp, qc, qe);
    m_Descriptor->SetPrototypeCurvature(qc, qe);
  }

  delete[] peaks;
  delete[] fshp;

  return nPeaks;
}

//----------------------------------------------------------------------------
typedef struct
{
  int i;
  double x, dx;
} Edge;

typedef struct
{
  int   i;
  double x, y;
} IndexCoords;

static int compare_ind(const void *v1, const void *v2)
{
  return (((const IndexCoords *)v1)->y <= ((const IndexCoords *)v2)->y) ? -1 : 1;
}

static int compare_edges(const void *v1, const void *v2)
{
  return (((const Edge *)v1)->x <= ((const Edge *)v2)->x ? -1 : 1);
}
//----------------------------------------------------------------------------
void ContourShapeExtractionTool::ExtractCurvature(int n, const Point2 * const &shp, unsigned long &qc, unsigned long &qe)
{
  double ecc = 0.0, cir = 0.0;

  IndexCoords *ind = new IndexCoords[n];
  double x1 = shp[0].x, x2 = shp[0].x;
  for (int k0 = 0; k0 < n; k0++)
  {
    if (shp[k0].x < x1) x1 = shp[k0].x;
    if (shp[k0].x > x2) x2 = shp[k0].x;
    ind[k0].i = k0;
    ind[k0].x = shp[k0].x;
    ind[k0].y = shp[k0].y;
  }
  qsort(ind, n, sizeof(ind[0]), compare_ind);
  double y1 = ind[0].y, y2 = ind[n-1].y;

  int iw = (int)(x2 - x1 + 1);
  int ih = (int)(y2 - y1 + 1);
  unsigned char *xy = new unsigned char[iw*ih];
  memset(xy, 0, iw*ih*sizeof(unsigned char));

  int nedge = 0;
  Edge *edgelist = new Edge[n];

  int ybot = (int)ceil(y1 - 0.5);
  int ytop = (int)floor(y2 - 0.5);
  if (ybot - (int)y1 < 0) ybot = (int)y1;
  if (ytop - (int)y1 >= ih) ytop = ih - 1 + (int)y1;

  int k1 = 0;
  for (int y = ybot; y <= ytop; y++)
  {
    for (; (k1 < n) && (ind[k1].y < (y + 0.5)); k1++)
    {
      int i1 = ind[k1].i;
      int i0 = (i1 < n-1) ? i1+1 : 0;
      int i2 = (i1 > 0) ? i1-1 : n-1;

      if (shp[i0].y <= y - 0.5)
      {
        int e;
        for (e = 0; (e < nedge) && (edgelist[e].i != i1); e++);
        if (e < nedge)
        {
          memmove(&edgelist[e], &edgelist[e+1], (nedge-e)*sizeof(edgelist[0]));
          nedge--;
        }
      }
      else if (shp[i0].y >= y + 0.5)
      {
        edgelist[nedge].i = i1;
        edgelist[nedge].dx = (shp[i1].x - shp[i0].x) / (shp[i1].y - shp[i0].y);
        edgelist[nedge].x = edgelist[nedge].dx * (y - shp[i0].y) + shp[i0].x;
        nedge++;
      }

      if (shp[i2].y <= y - 0.5)
      {
        int e;
        for (e = 0; (e < nedge) && (edgelist[e].i != i2); e++);
        if (e < nedge)
        {
          memmove(&edgelist[e], &edgelist[e+1], (nedge-e)*sizeof(edgelist[0]));
          nedge--;
        }
      }
      else if (shp[i2].y >= y + 0.5)
      {
        edgelist[nedge].i = i2;
        edgelist[nedge].dx = (shp[i1].x - shp[i2].x) / (shp[i1].y - shp[i2].y);
        edgelist[nedge].x = edgelist[nedge].dx * (y - shp[i2].y) + shp[i2].x;
        nedge++;
      }
    }

    qsort(edgelist, nedge, sizeof(edgelist[0]), compare_edges);

    for (int s = 0; s < nedge-1; s+=2)
    {
      int xl = (int)ceil(edgelist[s].x) - (int)x1;
      if (xl < 0) xl = 0;
      int xr = (int)floor(edgelist[s+1].x) - (int)x1;
      if (xr >= iw) xr = iw - 1;
      int yl = (int)(y - (int)y1);
      for (int f = xl; f <= xr; f++)
        xy[f + yl*iw] = 255;

      edgelist[s].x += edgelist[s].dx;
      edgelist[s+1].x += edgelist[s+1].dx;
    }
  }

  double perim = sqrt((shp[0].x-shp[n-1].x)*(shp[0].x-shp[n-1].x) +
                      (shp[0].y-shp[n-1].y)*(shp[0].y-shp[n-1].y));
  for (int p = 1; p < n; p++)
  {
    perim += sqrt((shp[p].x-shp[p-1].x)*(shp[p].x-shp[p-1].x) +
                  (shp[p].y-shp[p-1].y)*(shp[p].y-shp[p-1].y));
  }

  double vol = 0;
  double meanx = 0.0;
  double meany = 0.0;

  for (int vy = 0; vy < ih; vy++)
  {
    for (int vx = 0; vx < iw; vx++)
    {
      if (xy[vx + vy*iw])
      {
        meanx+=vx;
        meany+=vy;
        vol++;
      }
    }
  }
  meanx/=vol;
  meany/=vol;

  double i11 = 0.0, i20 = 0.0, i02 = 0.0;
//  double rad = 0.0, co = 0.0;
  for (int ey = 0; ey < ih; ey++)
  {
    for (int ex = 0; ex < iw; ex++)
    {
      if (xy[ex + ey*iw])
      {
        i11 += (ex-meanx)*(ey-meany);
        i20 += (ex-meanx)*(ex-meanx);
        i02 += (ey-meany)*(ey-meany);
      }
    }
  }

  double temp1 = (i20 + i02);
  double temp2 = sqrt(i20*i20 + i02*i02 - 2.0*i02*i20 + 4.0*i11*i11);

  cir = perim * perim / vol;
  ecc = sqrt((temp1 + temp2) / (temp1 - temp2));

  delete[] ind;
  delete[] edgelist;
  delete[] xy;

  if (ecc >= CONTOURSHAPE_EMAX)
    qe = CONTOURSHAPE_EMASK;
  else if (ecc < CONTOURSHAPE_EMIN)
    qe = 0;
  else
    qe = (unsigned long)(((unsigned long)((CONTOURSHAPE_EMASK+1) * (ecc - CONTOURSHAPE_EMIN) / (CONTOURSHAPE_EMAX - CONTOURSHAPE_EMIN))) & CONTOURSHAPE_EMASK);

  if (cir >= CONTOURSHAPE_CMAX)
    qc = CONTOURSHAPE_CMASK;
  else if (cir < CONTOURSHAPE_CMIN)
    qc = 0;
  else
    qc = (unsigned long)(((unsigned long)((CONTOURSHAPE_CMASK+1) * (cir - CONTOURSHAPE_CMIN) / (CONTOURSHAPE_CMAX - CONTOURSHAPE_CMIN))) & CONTOURSHAPE_CMASK);
}


