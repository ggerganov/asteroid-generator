/*! \file cg_sky.cpp
 *  \brief Enter description here.
 *  \author Georgi Gerganov
 */

#include "cg_sky.h"

#include "cg_io.h"
#include "cg_perlin.h"
#include "cg_utils.h"

namespace CG {
  Sky::Sky() {
    //_cosALim = 0.75;
    //_sinTLim = 0.50;
    _cosALim = 1.0;
    _sinTLim = 1.0;
    _icosALim = 1.0/_cosALim;
    _isinTLim = 1.0/_sinTLim;
  }

  Sky::~Sky() {}

  void Sky::initialize(int nx, int ny, int levels) {
    if (levels >= _maxLevels) levels = _maxLevels-1;

    _nx = nx;
    _ny = ny;
    _texture = new float[3*_nx*_ny];

    PerlinNoise perlin(_maxLevels);
    perlin.genTextureRGBSpaceBackground(_texture, _nx, _ny, levels);
  }

  void Sky::getColor(Vec3f p, float &r, float &g, float &b) {
#ifndef CG_FAST_SKY
    p.norm();
    CG_Float sinT = asin(p._y)*M_iPI2;
    CG_Float cosA = atan2(p._z, p._x)*M_iPI;
#else
    CG_Float sinT = p._y;
    CG_Float cosA = p._x;
#endif

    getColor(cosA, sinT, r, g, b);
  }

  void Sky::getColor(CG_Float cosA, CG_Float sinT, float &r, float &g, float &b) {
#ifndef CG_FAST_SKY
    if (cosA < -_cosALim || cosA > _cosALim ||
        sinT < -_sinTLim || sinT > _sinTLim) {
      r = 0.0; g = 0.0; b = 0.0;
      return;
    }

    cosA *= _icosALim;
    sinT *= _isinTLim;

    cosA += 0.25; if (cosA >= 1.0) cosA -= 2.0;
    //sinT += 0.25; if (sinT >= 1.0) sinT -= 2.0;
    cosA = 2*(fmod(1.0+cosA, 1.00) - 0.50);
    sinT = 2*(fmod(1.0+sinT, 1.00) - 0.50);
    cosA = fabs(cosA);
    sinT = fabs(sinT);

    CG_Float L = 1.0;

    int ix = (0.5*L*(1.0+cosA))*_nx;
    int iy = (0.5*L*(1.0+sinT))*_ny;

    ix -= _nx/2; if (ix < 0) ix += _nx;
    iy -= _ny/4; if (iy < 0) iy += _ny;
#else
    int ix = (0.5*(1.0+cosA))*_nx;
    int iy = (0.5*(1.0+sinT))*_ny;
#endif

    int idx = iy*_nx + ix;
    r = _texture[3*idx + 0];
    g = _texture[3*idx + 1];
    b = _texture[3*idx + 2];
  }

}
