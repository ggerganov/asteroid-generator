/*! \file cg_go_death_star.cpp
 *  \brief Enter description here.
 *  \author Georgi Gerganov
 */

#include "cg_go_death_star.h"

#include "cg_io.h"
#include "cg_utils.h"
#include "cg_perlin.h"

namespace CG {
  namespace GeometryObjects {

    DeathStar::DeathStar() {}
    DeathStar::~DeathStar() {}

    void DeathStar::initialize() {
      _timer.start();

      _pos._x = 0.0;
      _pos._y = 0.0;
      _pos._z = 0.0;

      _rot._x = 0.0;
      _rot._y = 0.0;
      _rot._z = 0.0;

      _sinry = sin(_rot._y);
      _cosry = cos(_rot._y);
      _rotationSpeed = 0.5;

      _radius = 60*_KM_; // 60 km
      _radius2 = _radius*_radius;

      _weapon._dir._x = 0.4;
      _weapon._dir._y = 0.4;
      _weapon._dir._z = -1.0;
      _weapon._dir.norm();
      _weapon._aperture = 0.15*M_PI;
      _weapon._halfAperture = 0.5*_weapon._aperture;
      _weapon._cosHA = cos(_weapon._halfAperture);
      _weapon._cosHAInner = (0.985 + _weapon._cosHA)*0.5;
      _weapon._focus = 1.1*_radius;
      _weapon._depthFactor = 5.0;
      _weapon._depthFactorInner = 3.5;

      Vec3f p1 = _weapon._dir; p1._x += 1.0;
      _weapon._norm1 = (_weapon._dir-p1).cross(p1);
      _weapon._norm1.norm();
      _weapon._center = _weapon._dir*_radius;

      _tunnel._width = 2.0*_KM_;
      _tunnel._halfWidth = 0.5*_tunnel._width;
      _tunnel._depth = 500*_M_;

      _nTextures = 10;
      generateTextures();

      _nPatches = 100;
      _nPatchTSectors = 6;
      _nPatchASectors = 8;
      generatePatches();
    }

    CG_Float DeathStar::getColor(Vec3f p) {
      rotatePoint(p);

      bool bNearTunnel = isNearTunnel(p);
      bool bInsideTunnel = isInsideTunnel(p);
      bool bNearWeapon = isNearWeapon(p);

      if (bInsideTunnel) return 0.5;
      if (bNearWeapon) return 1.0;
      if (bNearTunnel) return 1.0;

      CG_Float cosA, sinT, col = 1.0;
      getAngles(p, cosA, sinT);

      int pl = getPatchID(cosA, sinT);

      if (cosA > _patches[pl]._minA &&
          cosA < _patches[pl]._maxA &&
          sinT > _patches[pl]._minT &&
          sinT < _patches[pl]._maxT) {
        int tx = (_textures[_patches[pl]._texID]._texX*
                 (cosA - _patches[pl]._minA))*(_patches[pl]._idA);
        int ty = (_textures[_patches[pl]._texID]._texY*
                 (sinT - _patches[pl]._minT))*(_patches[pl]._idT);
        col = _textures[_patches[pl]._texID]._texData[ty*_textures[_patches[pl]._texID]._texX + tx];
      }

      return col;
    }

    void DeathStar::updateParameters(CG_Float time) {
      if (time < 0.0) time = _timer.time();

      if (false) {
        CG_Float a = 2.51+2.5*sin(M_PI*time/5.0);
        _weapon._depthFactor = a;
        _weapon._depthFactorInner = a*3.5/5.0;
      }

      if (true) {
        _rot._y = _rotationSpeed*_timer.time()*0.025;
        _sinry = sin(_rot._y);
        _cosry = cos(_rot._y);
      }

      if (false) {
        for (int i = 0; i < _nPatches; ++i) {
          _patches[i]._height = (0.45*sin(time*((i*857289)%124)*0.01) + 0.5)*_KM_;
        }
      }
    }

    CG_Float DeathStar::getHeight(Vec3f p) {
      CG_Float glift = 0.0;
      //CG_Float glift = -0.1*_KM_;
      //if (getColor(p) == 1.0) glift = -0.2*_KM_;

      rotatePoint(p);
      int pid = getPatchID(p);
      if (pid >= 0 && !isNearTunnel(p) && !isNearWeapon(p)) {
        //glift += _patches[pid]._height * (0.5 + 0.5*sin(p._x*0.001));
        //glift += _patches[pid]._height;
      }
      //glift += 0.999*_KM_* (0.5 + 0.5*sin(p._x*0.0002 + p._x*0.00002 +p._y*0.00005));

      CG_Float cosP = 0.0;
      if (!isInsideWeapon(p, cosP)) {
        if (isInsideTunnel(p)) {
          CG_Float h = _radius + (fabs(p._y)-_tunnel._halfWidth)*100.0;
          if (h < _radius - _tunnel._depth) h = _radius - _tunnel._depth;
          return h;
        }
        return _radius + glift;
      }
      if (cosP > _weapon._cosHAInner) {
        CG_Float lift = 0.0;
        p -= _weapon._center; p.norm();
        if (fabs(p.dot(_weapon._norm1)) < 0.03) lift = 200.0;

        CG_Float f = _weapon._depthFactor - (cosP - _weapon._cosHAInner)*1000.0;
        if (f < _weapon._depthFactorInner) f = _weapon._depthFactorInner;
        lift += _radius*(1.0 - f*(cosP-_weapon._cosHA));
        if (lift > _radius) lift = _radius; return lift;
      }
      return _radius*(1.0 - _weapon._depthFactor*(cosP-_weapon._cosHA));
    }

    bool DeathStar::isInsideWeapon(Vec3f p) {
      p -= _pos; p.norm();
      if (p.dot(_weapon._dir) > _weapon._cosHA) return true;
      return false;
    }

    bool DeathStar::isInsideWeapon(Vec3f p, CG_Float &cosP) {
      p -= _pos; p.norm();
      cosP = p.dot(_weapon._dir);
      if (cosP > _weapon._cosHA) return true;
      return false;
    }

    bool DeathStar::isNearWeapon(Vec3f p, const CG_Float fact) {
      p -= _pos; p.norm();
      if (p.dot(_weapon._dir) > fact*_weapon._cosHA) return true;
      return false;
    }

    bool DeathStar::isInsideTunnel(Vec3f p) {
     return (fabs(p._y) < _tunnel._halfWidth);
    }

    bool DeathStar::isNearTunnel(Vec3f p, const CG_Float fact) {
     return (fabs(p._y) < fact*_tunnel._halfWidth);
    }

    void DeathStar::generateTextures() {
      for (int i = 0; (int) i < _textures.size(); ++i) {
        if (_textures[i]._texData) delete [] _textures[i]._texData;
      }

      _textures.clear();
      _textures.resize(_nTextures);

      for (int i = 0; i < _nTextures; ++i) {
        PerlinNoise perlin;
        _textures[i]._texX = 1024;
        _textures[i]._texY = 512;
        _textures[i]._texN = _textures[i]._texX*_textures[i]._texY;

        _textures[i]._texData = new float[_textures[i]._texN];
        perlin.genTextureGrayMetal(_textures[i]._texData, _textures[i]._texX, _textures[i]._texY, 8);
      }
    }

    void DeathStar::generatePatches() {
      _patches.clear();
      _patches.resize(10000);

      int i = 0;
      CG_Float curT = -1.0, dT;
      for (int y = 0; ; ++y) {
        CG_Float curA = -1.0;
        dT = 0.05 + 2.0/_nPatchTSectors*Utils::frand();
        for (int x = 0; ; ++x) {
          CG_Float ra;
          ra = Utils::frand();
          _patches[i]._minT = curT + 0.05*ra*ra*dT;
          ra = Utils::frand();
          _patches[i]._maxT = curT + dT - 0.05*ra*ra*dT;
          _patches[i]._idT = 1.0/(_patches[i]._maxT - _patches[i]._minT);
          _patches[i]._minA = curA; curA += 0.01 + 0.8/_nPatchASectors*Utils::frand();
          _patches[i]._maxA = curA;
          _patches[i]._idA = 1.0/(_patches[i]._maxA - _patches[i]._minA);
          _patches[i]._height = 0.1*_KM_;
          _patches[i]._texID = rand()%_nTextures;
          ra = Utils::frand();
          curA += ra*ra*0.05 + 0.001;
          ++i;
          if (curA >= 1.0) break;
        }
        curT += dT + 0.0001;
        if (curT >= 1.0) break;
      }
      _nPatches = i;
      _patches.resize(_nPatches);

    }

    int DeathStar::getPatchID(Vec3f p) {
      CG_Float cosA, sinT;
      getAngles(p, cosA, sinT);
      return getPatchID(cosA, sinT);
    }

    int DeathStar::getPatchID(CG_Float cosA, CG_Float sinT) {
      int pl = 0, ph = _nPatches-1, curp;
      while (true) {
        curp = (pl + ph)/2;
        if ((sinT > _patches[curp]._maxT) ||
            ( (sinT > _patches[curp]._minT) &&
              (sinT < _patches[curp]._maxT) &&
              (cosA > _patches[curp]._maxA))) {
          pl = curp + 1;
        } else {
          ph = curp;
        }
        if (pl == ph) break;
      }

      if (cosA > _patches[pl]._minA &&
          cosA < _patches[pl]._maxA &&
          sinT > _patches[pl]._minT &&
          sinT < _patches[pl]._maxT) {
        return pl;
      }
      return -1;
    }

  }
}
