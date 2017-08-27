/*! \file cg_filters.cpp
 *  \brief Enter description here.
 *  \author Georgi Gerganov
 */

#include "cg_filters.h"

#include "cg_io.h"

#include <cmath>

namespace CG {
  namespace Filters {

    Gaussian::Gaussian() {
      _nW = 0;
      _sig = 0.0;

      _kernel = NULL;

      _lastN = -1;
      _tmp = NULL;

      _lastNRGB = -1;
      _tmpRGB = NULL;
    }

    Gaussian::~Gaussian() {
      if (_kernel) delete [] _kernel;
      if (_tmp) delete [] _tmp;
      if (_tmpRGB) delete [] _tmpRGB;
    }

    void Gaussian::initialize(int nW, CG_Float sig) {
      _nW = nW;
      _sig = sig;

      if (_kernel) delete [] _kernel;
      _kernel = new CG_Float[2*_nW+1];

      CG_Float wsum = 0.0;
      CG_Float isig = (_sig == 0.0) ? 1.0 : 1.0/_sig;
      for (int i = -_nW; i <= _nW; ++i) {
        _kernel[i + _nW] = isig*exp(-0.5*(i*i)*isig*isig);
        wsum += _kernel[i + _nW];
      }

      wsum = (wsum == 0.0) ? 1.0 : 1.0/wsum;
      for (int i = -_nW; i <= _nW; ++i) { _kernel[i + _nW] *= wsum; }
    }

    void Gaussian::filterRGB(float *img, int nx, int ny, CG_Float blend) {
      int n = nx*ny;
      if (n != _lastNRGB) {
        if (_tmpRGB) delete [] _tmpRGB;
        _tmpRGB = new float[3*n];
        _lastNRGB = n;
      }

      // 1D Gaussian filter in the X direction
      int y;
#pragma omp parallel private(y)
      {
#pragma omp for schedule(dynamic)
        for (y = 0; y < ny; ++y) {
          for (int x = 0; x < nx; ++x) {
            float sumr = 0;
            float sumg = 0;
            float sumb = 0;
            for (int rr = - _nW; rr <= _nW; ++rr) {
              int rx = x + rr;
              if (rx >= nx) rx -= nx;
              else if (rx < 0) rx += nx;
              //if (rx < 0 || rx >= nx) continue;
              float vr = img[3*(y*nx + rx) + 0];
              float vg = img[3*(y*nx + rx) + 1];
              float vb = img[3*(y*nx + rx) + 2];
              CG_Float cw = _kernel[rr + _nW];
              sumr += vr*cw;
              sumg += vg*cw;
              sumb += vb*cw;
            }
            _tmpRGB[3*(y*nx+x) + 0] = sumr;
            _tmpRGB[3*(y*nx+x) + 1] = sumg;
            _tmpRGB[3*(y*nx+x) + 2] = sumb;
          }
        }
      }

      CG_Float tw = 1.0;
      if (blend > 1.0) tw = 1.0/(1.0 + blend);

      // 1D Gaussian filter in the Y direction
#pragma omp parallel private(y)
      {
#pragma omp for schedule(dynamic)
        for (y = 0; y < ny; ++y) {
          for (int x = 0; x < nx; ++x) {
            float sumr = 0;
            float sumg = 0;
            float sumb = 0;
            for (int rr = - _nW; rr <= _nW; ++rr) {
              int ry = y + rr;
              if (ry < 0 || ry >= ny) continue;
              float vr = _tmpRGB[3*(ry*nx + x) + 0];
              float vg = _tmpRGB[3*(ry*nx + x) + 1];
              float vb = _tmpRGB[3*(ry*nx + x) + 2];
              CG_Float cw = _kernel[rr + _nW];
              sumr += vr*cw;
              sumg += vg*cw;
              sumb += vb*cw;
            }
            img[3*(y*nx+x) + 0] = (img[3*(y*nx+x) + 0] + blend*sumr)*tw;
            img[3*(y*nx+x) + 1] = (img[3*(y*nx+x) + 1] + blend*sumg)*tw;
            img[3*(y*nx+x) + 2] = (img[3*(y*nx+x) + 2] + blend*sumb)*tw;
          }
        }
      }
    }

    void Gaussian::filterGray(CG_Float *img, int nx, int ny, CG_Float blend, bool cycleY) {
      int n = nx*ny;
      if (n != _lastN) {
        if (_tmp) delete [] _tmp;
        _tmp = new CG_Float[n];
        _lastN = n;
      }

      // 1D Gaussian filter in the X direction
      int y;
#pragma omp parallel private(y)
      {
#pragma omp for schedule(dynamic)
        for (y = 0; y < ny; ++y) {
          for (int x = 0; x < nx; ++x) {
            CG_Float sumr = 0;
            for (int rr = - _nW; rr <= _nW; ++rr) {
              int rx = x + rr;
              if (rx >= nx) rx -= nx;
              else if (rx < 0) rx += nx;
              //if (rx < 0 || rx >= nx) continue;
              CG_Float vr = img[y*nx + rx];
              CG_Float cw = _kernel[rr + _nW];
              sumr += vr*cw;
            }
            _tmp[y*nx+x] = sumr;
          }
        }
      }

      CG_Float tw = 1.0;
      if (blend > 1.0) tw = 1.0/(1.0 + blend);

      // 1D Gaussian filter in the Y direction
#pragma omp parallel private(y)
      {
#pragma omp for schedule(dynamic)
        for (y = 0; y < ny; ++y) {
          for (int x = 0; x < nx; ++x) {
            CG_Float sumr = 0;
            CG_Float sumw = 0;
            for (int rr = - _nW; rr <= _nW; ++rr) {
              int ry = y + rr;
              if (cycleY) {
                if (ry >= ny) ry -= ny;
                else if (ry < 0) ry += ny;
              } else if (ry < 0 || ry >= ny) continue;
              CG_Float vr = _tmp[ry*nx + x];
              CG_Float cw = _kernel[rr + _nW];
              sumr += vr*cw;
              sumw += cw;
            }
            img[y*nx+x] = (img[y*nx+x] + blend*sumr/sumw)*tw;
          }
        }
      }
    }

  }
}
