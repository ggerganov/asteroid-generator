/*! \file cg_perlin.cpp
 *  \brief Enter description here.
 *  \author Georgi Gerganov
 */

#include "cg_perlin.h"

#include "cg_io.h"
#include "cg_utils.h"
#include "cg_filters.h"

inline int myFloor(CG_Float x) {
  return (int)x - (x < 0 && (int)x != x);
}

namespace CG {

  PerlinNoise::PerlinNoise(int power) {
    _n = 1; for (int i = 0; i < power; ++i) _n *= 2;
    _nMask = _n - 1;

    _r = new CG_Float[_n*_n];
    for (int i = 0; i < _n*_n; ++i) {
      _r[i] = Utils::frand();
    }
  }

  PerlinNoise::~PerlinNoise() {
    if (_r) delete [] _r;
  }

  CG_Float PerlinNoise::sample(CG_Float x, CG_Float y) const {
    int xi = myFloor(x);
    int yi = myFloor(y);

    CG_Float tx = x - xi;
    CG_Float ty = y - yi;

    int rx0 = xi & _nMask;
    int rx1 = (rx0 + 1) & _nMask;
    int ry0 = yi & _nMask;
    int ry1 = (ry0 + 1) & _nMask;

    float c00 = _r[ry0*_nMask + rx0];
    float c10 = _r[ry0*_nMask + rx1];
    float c01 = _r[ry1*_nMask + rx0];
    float c11 = _r[ry1*_nMask + rx1];

    CG_Float sx = smooth(tx);
    CG_Float sy = smooth(ty);

    CG_Float nx0 = interp(c00, c10, sx);
    CG_Float nx1 = interp(c01, c11, sx);

    return interp(nx0, nx1, sy);
  }

  void PerlinNoise::genTextureRGBSpaceBackground(float *texture, int nx, int ny, int levels) {
    int _y_;
#pragma omp parallel private(_y_)
    {
#pragma omp for schedule(dynamic)
      for (_y_ = 0; _y_ < ny; ++_y_) {
        for (int x = 0; x < nx; ++x) {
          int ax = x;
          int ay = _y_;

          CG_Float scale = 1.0, p = 0;
          for (int ip = 0; ip < levels; ++ip) {
            CG_Float px = (scale*ax)/nx;
            CG_Float py = (scale*ay)/ny;
            CG_Float pp = sample(px, py);

            p += pp/(ip+1);
            scale *= 2;
          }
          CG_Float rr = 0.00*p*p + 0.8*(1-p)*p + 0.0*(1-p)*(1-p);
          CG_Float gg = 0.03*p*p + 0.0*(1-p)*p + 0.0*(1-p)*(1-p);
          CG_Float bb = 0.00*p*p + 0.0*(1-p)*p + 0.3*(1-p)*(1-p)*cos((M_PI*_y_)/ny);

          if (rr < 0.0) rr = 0.0; if (rr > 1.0) rr = 1.0;
          if (gg < 0.0) gg = 0.0; if (gg > 1.0) gg = 1.0;
          if (bb < 0.0) bb = 0.0; if (bb > 1.0) bb = 1.0;

          texture[3*(ay*nx + ax) + 0] = rr*2.0;
          texture[3*(ay*nx + ax) + 1] = gg*2.0;
          texture[3*(ay*nx + ax) + 2] = bb*2.0;
        }
      }
    }
    Filters::Gaussian filter1; filter1.initialize(10, 5.0);
    filter1.filterRGB(texture, nx, ny, 100.0);

#pragma omp parallel private(_y_)
    {
#pragma omp for schedule(dynamic)
      for (_y_ = 0; _y_ < ny; ++_y_) {
        for (int x = 0; x < nx; ++x) {
          int ax = x;
          int ay = _y_;

          CG_Float px = (512.0*ax)/nx;
          CG_Float py = (512.0*ay)/ny;
          CG_Float p = sample(px, py);
          if (p > 0.990) {
            p -= 0.990; p = 50.0*p + 0.5;
            CG_Float b = Utils::frand();
            texture[3*(ay*nx + ax) + 0] = 1.0*p*(1-0.5*b) + 1.0;
            texture[3*(ay*nx + ax) + 1] = 1.0*p*(1-b) + 1.0;
            texture[3*(ay*nx + ax) + 2] = 1.0*p*b + 1.0;
          }
        }
      }
    }
    Filters::Gaussian filter2; filter2.initialize(6, 2.00);
    filter2.filterRGB(texture, nx, ny, 4.00);
  }

  void PerlinNoise::genTextureGrayMetal(float *texture, int nx, int ny, int levels) {
    int _y_;
#pragma omp parallel private(_y_)
    {
#pragma omp for schedule(dynamic)
      for (_y_ = 0; _y_ < nx; ++_y_) {
        int ay = _y_;

        CG_Float scale = 1.0, p = 0;
        for (int ip = 0; ip < levels; ++ip) {
          CG_Float px = 0.0;
          CG_Float py = (scale*ay)/nx;
          CG_Float pp = sample(px, py);

          p += pp/(ip+1);
          scale *= 2;
        }
        CG_Float rr = p*(1.0 + 0.1*Utils::frand());

        for (int x = 0; x < ny; ++x) {
          int ax = x;
          texture[ax*nx + ay] = rr;
        }
      }
    }

    float min = texture[0];
    float max = texture[0];
    for (int i = 0; i < nx*ny; ++i) {
      if (texture[i] < min) min = texture[i];
      if (texture[i] > max) max = texture[i];
    }

    float inv = 0.2/(max - min);
    for (int i = 0; i < nx*ny; ++i) {
      texture[i] = 0.7+(texture[i] - min)*inv;
    }

  }

  void PerlinNoise::genTextureGrayRock(float *texture, int nx, int ny, int levels) {
    int _y_;
#pragma omp parallel private(_y_)
    {
#pragma omp for schedule(dynamic)
      for (_y_ = 0; _y_ < nx; ++_y_) {
        int ay = _y_;
        for (int x = 0; x < ny; ++x) {
          int ax = x;

          CG_Float scale = 1.0, p = 0;
          for (int ip = 0; ip < levels; ++ip) {
            CG_Float px = (scale*ax)/nx;
            CG_Float py = (scale*ay)/nx;
            CG_Float pp = sample(px, py);

            p += pp/(ip+1);
            scale *= 2;
          }
          CG_Float rr = p;

          texture[ax*nx + ay] = rr;
        }
      }
    }

    float min = texture[0];
    float max = texture[0];
    for (int i = 0; i < nx*ny; ++i) {
      if (texture[i] < min) min = texture[i];
      if (texture[i] > max) max = texture[i];
    }

    float inv = 0.8/(max - min);
    for (int i = 0; i < nx*ny; ++i) {
      texture[i] = 0.2+(texture[i] - min)*inv + Utils::frand()*0.01;
    }

    float *tmp = new float[nx*ny];
    //levels -= 2;
#pragma omp parallel private(_y_)
    {
#pragma omp for schedule(dynamic)
      for (_y_ = 0; _y_ < nx; ++_y_) {
        int ay = _y_;
        for (int x = 0; x < ny; ++x) {
          int ax = x;

          CG_Float scale = 1.0, p = 0;
          for (int ip = 0; ip < levels; ++ip) {
            CG_Float px = (scale*ax)/nx;
            CG_Float py = (scale*ay)/nx;
            CG_Float pp = sample(px, py);

            p += pp/(ip+1);
            scale *= 2;
          }
          CG_Float rr = p;

          tmp[ax*nx + ay] = rr;
        }
      }
    }

    min = tmp[0];
    max = tmp[0];
    for (int i = 0; i < nx*ny; ++i) {
      if (tmp[i] < min) min = tmp[i];
      if (tmp[i] > max) max = tmp[i];
    }

    inv = 0.99/(max - min);
    for (int i = 0; i < nx*ny; ++i) {
      tmp[i] = 0.0+(tmp[i] - min)*inv + Utils::frand()*0.05;
    }

    for (int i = 0; i < nx*ny; ++i) {
      texture[i] *= 1.0 - tmp[i]*tmp[i]*(3.0 - 2*tmp[i]);
    }

    min = texture[0];
    max = texture[0];
    for (int i = 0; i < nx*ny; ++i) {
      if (texture[i] < min) min = texture[i];
      if (texture[i] > max) max = texture[i];
    }

    inv = 0.4/(max - min);
    for (int i = 0; i < nx*ny; ++i) {
      texture[i] = 0.5+(texture[i] - min)*inv;
    }
    delete [] tmp;
  }

  void PerlinNoise::genTerrainAsteroid1(CG_Float *hmap, int nx, int ny, int levels, bool valleys) {
    int _y_;
#pragma omp parallel private(_y_)
    {
#pragma omp for schedule(dynamic)
      for (_y_ = 0; _y_ < ny; ++_y_) {
        int ay = _y_;
        for (int x = 0; x < nx; ++x) {
          int ax = x;

          CG_Float scale = 1.0, p = 0;
          for (int ip = 0; ip < levels; ++ip) {
            CG_Float px = (scale*ax)/nx;
            CG_Float py = (scale*ay)/ny;
            CG_Float pp = sample(px, py);

            p += pp/(ip+1);
            scale *= 2;
          }

          hmap[ax*nx + ay] = p;
        }
      }
    }

    float min = hmap[0];
    float max = hmap[0];
    for (int i = 0; i < nx*ny; ++i) {
      if (hmap[i] < min) min = hmap[i];
      if (hmap[i] > max) max = hmap[i];
    }

    float inv = 0.9/(max - min);
    for (int y = 0; y < ny; ++y) {
      for (int x = 0; x < nx; ++x) {
        int i = y*nx + x;
        CG_Float hmin = 0.0;
        CG_Float range = 1.0;
        CG_Float p = hmin + (hmap[i] - min)*inv*range;

        hmap[i] = p;
        if (valleys) {
          //if (hmap[i] > 0.5) hmap[i] *= 0.4;
          if (hmap[i] > 0.5) hmap[i] *= 0.01;
          if (std::fabs(y-ny/2) > 0.4*ny) hmap[i] *= 0.01;
        }
      }
    }
  }

  void PerlinNoise::genTerrainCraters1(CG_Float *hmap, int nx, int ny, int levels) {
    int _y_;
#pragma omp parallel private(_y_)
    {
#pragma omp for schedule(dynamic)
      for (_y_ = 0; _y_ < ny; ++_y_) {
        int ay = _y_;
        for (int x = 0; x < nx; ++x) {
          int ax = x;

          CG_Float scale = 1.0, p = 0;
          for (int ip = 0; ip < levels; ++ip) {
            CG_Float px = (scale*ax)/nx;
            CG_Float py = (scale*ay)/ny;
            CG_Float pp = sample(px, py);

            p += pp/(ip+1);
            scale *= 2;
          }

          hmap[ax*nx + ay] = p;
        }
      }
    }

    float min = hmap[0];
    float max = hmap[0];
    for (int i = 0; i < nx*ny; ++i) {
      if (hmap[i] < min) min = hmap[i];
      if (hmap[i] > max) max = hmap[i];
    }

    float inv = 1.0/(max - min);
    for (int i = 0; i < nx*ny; ++i) {
      CG_Float p = 0.0 + (hmap[i] - min)*inv;
      int ip = p*100000.0;
      if ((ip % 10000) > 20*Utils::frand()) p = 0.0;
      hmap[i] = p;
    }
  }
}
