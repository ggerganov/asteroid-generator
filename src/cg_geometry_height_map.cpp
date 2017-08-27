/*! \file cg_geometry_height_map.cpp
 *  \brief Enter description here.
 *  \author Georgi Gerganov
 */

#include "cg_geometry_height_map.h"

#include "cg_io.h"

#include <algorithm>

namespace CG {

  GeometryHeightMap::GeometryHeightMap() {
    _pos._x = 0.0;
    _pos._y = 0.0;
    _pos._z = 0.0;

    _rot._x = 0.0;
    _rot._y = 0.0;
    _rot._z = 0.0;

    setRotation(_rot._x, _rot._y, _rot._z);
    _rotationSpeed = 0.5;

    _radius = 1*_KM_; // 1 km
    _iradius = 1.0 / _radius;
    _radius2 = _radius*_radius;

    _gConeAlpha = 0.5*M_PI2;
    _cosCone = cos(_gConeAlpha);
    _sinCone = sin(_gConeAlpha);
    _tanCone = tan(_gConeAlpha);
    _ctgCone = 1.0/_tanCone;

    _maxConeSteps = 32;
    _maxBinarySteps = 16;

    _shadowType = 1;
  }

  GeometryHeightMap::~GeometryHeightMap() {}

  bool GeometryHeightMap::traceRay(Vec3f &p, const Vec3f &d) {
#ifndef CG_FAST
    p -= _pos;
    CG_Float pl2 = p.l2();

    CG_Float dp = p.dot(d);
    CG_Float dl2 = d.l2();
    CG_Float D = (dp*dp - dl2*(p.l2() - _radius2));

    if (D < 0.0) return false;

    D = (-dp - sqrt(D))/dl2;
    p += _pos + (d*D);

    CG_Float h = getHeight(p);
    CG_Float curh = p.l();
    if (curh - 0.1 < h) return true;

    CG_Float cosD = -(d.dot(p)/curh);
    CG_Float sinD = sqrt(1.0 - cosD*cosD);
    CG_Float tanD = (cosD == 0.0) ? 1.0 : sinD/cosD;
    CG_Float fact = 1.0/(cosD*(1.0+tanD*_ctgCone));

    Vec3f po, pn;
    int coneSteps = 0;
    int binarySteps = 0;
    while (true) {
      CG_Float x = (curh - h)*fact + 100.0;

      if (x < 1000.0) x = 1000.0;

      po = p;
      p += d*(x*(1.0 - (_radius - h)*_iradius));
      h = getHeight(p);
      curh = p.l();

      if (curh > 1.01*_radius) return false;

      if (curh < h) {
        while (binarySteps < _maxBinarySteps) {
          x *= 0.5;
          pn = po + d*x;
          h = getHeight(pn);
          curh = pn.l();
          if (curh > h) po = pn;
          ++binarySteps;
        }
        p = po;
        return true;
      }

      if (++coneSteps > _maxConeSteps) break;
    }
    return true;
#else
    p -= _pos;
    CG_Float pl2 = p.l2();

    // point is outside sphere
    CG_Float dp = p.dot(d);
    CG_Float dl2 = d.l2();
    CG_Float D = (dp*dp - dl2*(p.l2() - _radius2));

    if (D < 0.0) return false;

    D = (-dp - sqrt(D))/dl2;
    p += _pos + (d*D);

    CG_Float h = getHeight(p);
    CG_Float curh = p.l();
    if (curh - 0.1 < h) return true;

    CG_Float cosD = -(d.dot(p)/curh);
    CG_Float sinD = sqrt(1.0 - cosD*cosD);
    CG_Float tanD = (cosD == 0.0) ? 1.0 : sinD/cosD;
    CG_Float fact = 1.0/(cosD*(1.0+tanD*_ctgCone));

    Vec3f po, pn;
    int coneSteps = 0;
    int binarySteps = 0;
    while (true) {
      CG_Float x = (curh - h)*fact+5000.0;

      po._x = p._x;
      po._y = p._y;
      po._z = p._z;

      p += (d*x)*(1.0 - (_radius - h)*_iradius);
      h = getHeight(p);
      curh = p.l();

      if (curh > 1.01*_radius) return false;

      if (curh < h) {
        while (binarySteps < _maxBinarySteps) {
          x *= 0.5;
          pn = po + d*x;
          h = getHeight(pn);
          curh = pn.l();
          if (curh > h) po = pn;
          ++binarySteps;
        }
        p = po;
        return true;
      }

      if (++coneSteps > _maxConeSteps) break;
    }
    return true;
#endif
  }

  bool GeometryHeightMap::traceLight(Vec3f &p, const Vec3f &d, Vec3f pd, CG_Float &slen) {
    if (pd.l() > _radius - 0.1) { slen = 1.0; return true; }

    p -= _pos;
    CG_Float pl2 = p.l2();

    CG_Float dp = p.dot(d);
    CG_Float dl2 = d.l2();
    CG_Float D = (dp*dp - dl2*(p.l2() - _radius2));

    if (D < 0.0) return false;

    D = (-dp - sqrt(D))/dl2;
    p += _pos + (d*D);

    slen = 1e6;
    pd -= p;
    Vec3f pn;
    CG_Float curh, h;
    const int nSteps = 16;
    const CG_Float dx = 1.0/nSteps;
    for (int i = 0; i < nSteps; ++i) {
      pn = p + i*dx*pd;
      h = getHeight(pn);
      curh = pn.l();
      slen = std::min(slen, curh - h);
      if (curh < h) slen += dx;
    }
    slen += 4*_KM_;

    slen = 1.0 + slen*0.0005;
    if (slen < 0.0) slen = 0.0;
    else if (slen > 1.0) slen = 1.0;
    return true;
  }

  bool GeometryHeightMap::isLightVisible(Vec3f p, Vec3f l, CG_Float &s) {
    if (_shadowType == 0) {
      // hard shadows
      s = 0.5;
      Vec3f d = p - l; d.norm();
      if (!traceRay(l, d)) return false;
      p -= l; if (p.l() < 1.0) { s = 1.0; return true; }
      return false;
    } else {
      // soft shadows
      s = 0.0;
      Vec3f d = p - l; d.norm();
      traceLight(l, d, p, s);
      return true;
    }
  }

  void GeometryHeightMap::getNormal(Vec3f &p) {
#ifndef CG_FAST
    Vec3f p0 = p; p0.norm(1.0);
    Vec3f p1 = p0; p1._z = -p1._x;
    Vec3f p2 = (p0-p1).cross(p1);
    p1 = p2.cross(p0);

    p1.norm(10.0);
    p2.norm(10.0);

    p1 += p; getSurfacePoint(p1); p1 -= p;
    p2 += p; getSurfacePoint(p2); p2 -= p;

    p = p1.cross(p2); p.norm();
#else
    Vec3f p0 = p; p0.norm(1.0);
    Vec3f p1 = p0; p1._z = -p1._x;
    Vec3f p2 = (p0-p1).cross(p1);
    p1 = p2.cross(p0);

    p1.norm(10.0);
    p2.norm(10.0);

    p1 += p; getSurfacePoint(p1); p1 -= p;
    p2 += p; getSurfacePoint(p2); p2 -= p;

    p = p1.cross(p2); p.norm();
#endif
  }

  void GeometryHeightMap::setShadows(int type) {
    _shadowType = type;
  }

#ifndef CG_FAST
  void GeometryHeightMap::getSurfacePoint(Vec3f &p) {
    p.norm(getHeight(p));
  }
#else
#endif

  void GeometryHeightMap::getAngles(Vec3f p, CG_Float &cosPhi, CG_Float &sinTheta) {
#ifndef CG_FAST
    p.norm();
    sinTheta = asin(p._y)*M_iPI2;
    cosPhi = atan2(p._z, p._x)*M_iPI;
#else
    p.norm();
    sinTheta = asin(p._y)*M_iPI2;
    cosPhi = atan2(p._z, p._x)*M_iPI;
#endif
  }

  void GeometryHeightMap::rotatePoint(Vec3f &p) {
    CG_Float tx = p._x*_cosry - p._z*_sinry;
    CG_Float ty = p._x*_sinrxsinry + p._y*_cosrx + p._z*_sinrxcosry;
    p._z = p._x*_cosrxsinry - p._y*_sinrx + p._z*_cosrxcosry;
    p._x = tx;
    p._y = ty;
  }

  void GeometryHeightMap::setConeStepParameters(CG_Float gConeAlpha, int maxConeSteps, int maxBinarySteps) {
    if (gConeAlpha > 0) {
      _gConeAlpha = 0.5*M_PI2;
      _cosCone = cos(_gConeAlpha);
      _sinCone = sin(_gConeAlpha);
      _tanCone = tan(_gConeAlpha);
      _ctgCone = 1.0/_tanCone;
    }

    if (maxConeSteps >= 0) { _maxConeSteps = maxConeSteps; }
    if (maxBinarySteps >= 0) { _maxBinarySteps = maxBinarySteps; }
  }

  void GeometryHeightMap::setRotation(CG_Float rx, CG_Float ry, CG_Float rz) {
    _rot._x = rx;
    _rot._y = ry;
    _rot._z = rz;

    _sinrx = sin(_rot._x);
    _cosrx = cos(_rot._x);
    _sinry = sin(_rot._y);
    _cosry = cos(_rot._y);
    _sinrxsinry = _sinrx*_sinry;
    _sinrxcosry = _sinrx*_cosry;
    _cosrxsinry = _cosrx*_sinry;
    _cosrxcosry = _cosrx*_cosry;
  }

}
