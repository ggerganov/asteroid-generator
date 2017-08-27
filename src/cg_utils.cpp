/*! \file cg_utils.cpp
 *  \brief Enter description here.
 *  \author Georgi Gerganov
 */

#include "cg_utils.h"

namespace CG {

  void Utils::bloomFilter(float *img, int nx, int ny, int down, CG_Float weight) {
    int nnx = nx / down;
    int nny = ny / down;
    CG_Float w = 1.0/(down*down);

    float *lowres = new float[3*nnx*nny];

    for (int y = 0; y < nny; ++y) {
      for (int x = 0; x < nnx; ++x) {
        int rx = down*x;
        int ry = down*y;
        CG_Float sumr = 0.0;
        CG_Float sumg = 0.0;
        CG_Float sumb = 0.0;
        for (int cx = 0; cx < down; ++cx) {
          for (int cy = 0; cy < down; ++cy) {
            int idx = (ry+cy)*nx+(rx+cx);
            sumr += img[3*idx + 0];
            sumg += img[3*idx + 1];
            sumb += img[3*idx + 2];
          }
        }
        int idx = y*nnx + x;
        lowres[3*idx + 0] = sumr*w;
        lowres[3*idx + 1] = sumg*w;
        lowres[3*idx + 2] = sumb*w;
      }
    }

    CG_Float tw = 1.0/(1.0 + weight);
    for (int y = 0; y < ny; ++y) {
      for (int x = 0; x < nx; ++x) {
        int idx = y*nx + x;

        int rx = x/down;
        int ry = y/down;
        int idxr = ry*nnx + rx;

        img[3*idx + 0] = (img[3*idx + 0] + weight*lowres[3*idxr + 0])*tw;
        img[3*idx + 1] = (img[3*idx + 1] + weight*lowres[3*idxr + 1])*tw;
        img[3*idx + 2] = (img[3*idx + 2] + weight*lowres[3*idxr + 2])*tw;
      }
    }

    delete [] lowres;
  }

  void Utils::gaussianFilter(float *img, int nx, int ny, CG_Float window, CG_Float sigma, CG_Float weight) {
    float *tmp = new float[3*nx*ny];

    int _nW = window;

    // 1D Gaussian filter in the X direction
    for (int y = 0; y < ny; ++y) {
      for (int x = 0; x < nx; ++x) {
        CG_Float sumw = 0;
        CG_Float sumr = 0;
        CG_Float sumg = 0;
        CG_Float sumb = 0;
        //for (int rx = x - _nW; rx <= x + _nW; ++rx) {
        for (int rr = - _nW; rr <= _nW; ++rr) {
          int rx = x + rr;
          if (rx < 0 || rx >= nx) continue;
          CG_Float vr = img[3*(y*nx + rx)+0];
          CG_Float vg = img[3*(y*nx + rx)+1];
          CG_Float vb = img[3*(y*nx + rx)+2];
          //CG_Float cw = _kern[rx-x+_nW];
          //CG_Float cw = 1.0;
          CG_Float cw = 1.0/(std::fabs(rr)+1.0);
          sumr += vr*cw;
          sumg += vg*cw;
          sumb += vb*cw;
          sumw += cw;
        }
        tmp[3*(y*nx+x)+0] = sumr / sumw;
        tmp[3*(y*nx+x)+1] = sumg / sumw;
        tmp[3*(y*nx+x)+2] = sumb / sumw;
      }
    }

    CG_Float tw = 1.0/(1.0 + weight);

    // 1D Gaussian filter in the Y direction
    for (int y = 0; y < ny; ++y) {
      for (int x = 0; x < nx; ++x) {
        CG_Float sumw = 0;
        CG_Float sumr = 0;
        CG_Float sumg = 0;
        CG_Float sumb = 0;
        //for (int ry = y - _nW; ry <= y + _nW; ++ry) {
        for (int rr = - _nW; rr <= _nW; ++rr) {
          int ry = y + rr;
          if (ry < 0 || ry >= ny) continue;
          CG_Float vr = tmp[3*(ry*nx + x)+0];
          CG_Float vg = tmp[3*(ry*nx + x)+1];
          CG_Float vb = tmp[3*(ry*nx + x)+2];
          //CG_Float cw = _kern[ry-y+_nW];
          //CG_Float cw = 1.0;
          CG_Float cw = 1.0/(std::fabs(rr)+1.0);
          sumr += vr*cw;
          sumg += vg*cw;
          sumb += vb*cw;
          sumw += cw;
        }
        img[3*(y*nx+x)+0] = (img[3*(y*nx+x)+0] + weight*sumr/sumw)*tw;
        img[3*(y*nx+x)+1] = (img[3*(y*nx+x)+1] + weight*sumg/sumw)*tw;
        img[3*(y*nx+x)+2] = (img[3*(y*nx+x)+2] + weight*sumb/sumw)*tw;
      }
    }

    delete [] tmp;
  }

  void Utils::gaussianFilterGray(CG_Float *img, int nx, int ny, CG_Float window, CG_Float sigma, CG_Float weight) {
    CG_Float *tmp = new CG_Float[nx*ny];

    int _nW = window;

    // 1D Gaussian filter in the X direction
    for (int y = 0; y < ny; ++y) {
      for (int x = 0; x < nx; ++x) {
        CG_Float sumw = 0;
        CG_Float sumr = 0;
        for (int rr = - _nW; rr <= _nW; ++rr) {
          int rx = x + rr;
          if (rx >= nx) rx -= nx;
          else if (rx < 0) rx += nx;
          //if (rx < 0 || rx >= nx) continue;
          CG_Float vr = img[y*nx + rx];
          //CG_Float cw = _kern[rx-x+_nW];
          //CG_Float cw = 1.0;
          CG_Float cw = 1.0/(std::fabs(rr)+1.0);
          sumr += vr*cw;
          sumw += cw;
        }
        tmp[y*nx+x] = sumr / sumw;
      }
    }

    CG_Float tw = 1.0/(1.0 + weight);

    // 1D Gaussian filter in the Y direction
    for (int y = 0; y < ny; ++y) {
      for (int x = 0; x < nx; ++x) {
        CG_Float sumw = 0;
        CG_Float sumr = 0;
        for (int rr = - _nW; rr <= _nW; ++rr) {
          int ry = y + rr;
          if (ry < 0 || ry >= ny) continue;
          CG_Float vr = tmp[ry*nx + x];
          //CG_Float cw = _kern[ry-y+_nW];
          //CG_Float cw = 1.0;
          CG_Float cw = 1.0/(std::fabs(rr)+1.0);
          sumr += vr*cw;
          sumw += cw;
        }
        img[y*nx+x] = (img[y*nx+x] + weight*sumr/sumw)*tw;
      }
    }

    delete [] tmp;
  }
}
