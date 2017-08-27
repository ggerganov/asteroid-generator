/*! \file cg_go_asteroid.cpp
 *  \brief Enter description here.
 *  \author Georgi Gerganov
 */

#include "cg_go_asteroid.h"

#include "cg_io.h"
#include "cg_utils.h"
#include "cg_perlin.h"
#include "cg_filters.h"

namespace CG {
  namespace GeometryObjects {

    Asteroid::Asteroid() {
      _terrain.hmap = NULL;
      _terrain.hmap2 = NULL;
      _terrain.craters = NULL;
      _textures.clear();
    }
    Asteroid::~Asteroid() {}

    void Asteroid::initialize() {
      _timer.start();

      _pos._x = 0.0;
      _pos._y = 0.0;
      _pos._z = 0.0;

      _rot._x = 0.0;
      _rot._y = 0.0;
      _rot._z = 0.0;

      _sinry = sin(_rot._y);
      _cosry = cos(_rot._y);
      _rotationSpeed = 16.5;

      _radius = 40*_KM_;
      _iradius = 1.0/_radius;
      _radius2 = _radius*_radius;

#ifndef CG_FAST
      setConeStepParameters(M_PI2, 48, 24);
#else
      setConeStepParameters(M_PI2, 32, 16);
#endif

      //_maxDepth = 20.0*_KM_;
      _maxDepth = 0.0*_KM_;

      _regenerate = false;

      _isMipmapping = true;
      _mipmapDistance = 300*_KM_;
      _mipmapTerrainFactor = 0.05;
      _mipmapTextureFactor = 1.5;

      generateTerrain();

      _nTextures = 1;
      generateTextures();
    }

    CG_Float Asteroid::getColor(Vec3f p) {
      rotatePoint(p);

      CG_Float cosA, sinT, col = 1.0;
      getAngles(p, cosA, sinT);

      int tx = (_textures[0]._texX*0.5*(1.0 + cosA));
      int ty = (_textures[0]._texY*0.5*(1.0 + sinT));
      col = _textures[0]._texData[ty*_textures[0]._texX + tx];

      if (_isMipmapping) {
        if (_cameraDistance < _mipmapDistance) {
          CG_Float fact = (_mipmapDistance/_cameraDistance - 1.0)*_mipmapTextureFactor;

          cosA = 8*(fmod(1.0+cosA, 0.25) - 0.1250);
          sinT = 4*(fmod(1.0+sinT, 0.50) - 0.250);
          sinT = fabs(sinT);

          int tx = (_textures[0]._texX*0.5*(1.0 + cosA));
          int ty = (_textures[0]._texY*0.5*(1.0 + sinT));
          CG_Float col2 = _textures[0]._texData[ty*_textures[0]._texX + tx];

          col = (col+fact*col2)/(1.0+fact);
        }
      }

      return col;
    }

    void Asteroid::updateParameters(CG_Float time) {
      if (time < 0.0) time = _timer.time();

#ifndef CG_FAST
      if (true) {
        _rot._y = _rotationSpeed*time*0.025;
        setRotation(_rot._x, _rot._y, _rot._z);

        if (_cameraDistance >= 325*_KM_) {
          if (_regenerate) {
            if (_maxDepth > 0.1*_KM_) {
              _maxDepth = 345.0*_KM_ - _cameraDistance;
            }
            else {
              _maxDepth = 0.0;
              INFO(" [+] Generating new asteroid ...\n");
              generateTerrain();
              _regenerate = false;
            }
          } else {
           if (!_regenerate && _maxDepth < 20*_KM_) { _maxDepth = 345.0*_KM_ - _cameraDistance; }
          }
        } else {
          if ( _cameraDistance < 275*_KM_) {
            _regenerate = true;
          }
        }
        if (_maxDepth < 0.0) _maxDepth = 0.0;
        //FATAL("%g %g\n", _maxDepth, _cameraDistance);
      }
#else
      if (true) {
        _maxDepth = 20*_KM_;
        _rot._y = _rotationSpeed*time*0.025;
        setRotation(_rot._x, _rot._y, _rot._z);

        if (_cameraDistance >= 505*_KM_) {
          if (_regenerate) {
            INFO(" [+] Generating new asteroid ...\n");
            generateTerrain();
            _regenerate = false;
          }
        } else {
          if ( _cameraDistance < 390*_KM_) {
            _regenerate = true;
          }
        }
      }
#endif
    }

    void Asteroid::switchMipmapping(int val) {
      if (val < 0) _isMipmapping = !_isMipmapping;
      else if (val == 0) _isMipmapping = false;
      else _isMipmapping = true;

      if (_isMipmapping) {
        INFO(" [+] Mipmapping is ON\n");
      } else {
        INFO(" [+] Mipmapping is OFF\n");
      }
    }

    CG_Float Asteroid::getHeight(Vec3f p) {
      CG_Float glift = 0.0*_KM_;

      CG_Float sinT = 0.0;
      CG_Float cosA = 0.0;
      rotatePoint(p);
      getAngles(p, cosA, sinT);

      CG_Float ftx0 = 0.5*(1.0 + cosA)*_terrain.rnx;
      CG_Float fty0 = 0.5*(1.0 + sinT)*_terrain.rny;

      int tx0 = ftx0;
      int ty0 = fty0;
      int tx1 = tx0 + 1; if (tx1 >= _terrain.rnx) tx1 = 0;
      int ty1 = ty0 + 1; if (ty1 >= _terrain.rny) ty1 = ty0;

      CG_Float v0 = _terrain.hmap[ty0*_terrain.nx + tx0];
      CG_Float v1 = _terrain.hmap[ty0*_terrain.nx + tx1];
      CG_Float v2 = _terrain.hmap[ty1*_terrain.nx + tx0];
      CG_Float v3 = _terrain.hmap[ty1*_terrain.nx + tx1];

      v0 = v0 + (v1 - v0)*(ftx0 - tx0);
      v1 = v2 + (v3 - v2)*(ftx0 - tx0);
      v0 = v0 + (v1 - v0)*(fty0 - ty0);

      glift += _maxDepth*v0;

      if (_isMipmapping) {
        if (_cameraDistance < _mipmapDistance) {
          CG_Float fact = (_mipmapDistance/_cameraDistance - 1.0)*_mipmapTerrainFactor;

          //cosA = 2.0*(fmod(1.0+cosA, 1.0) - 0.50);
          //sinT = 2.0*(fmod(1.0+sinT, 1.0) - 0.50);

          //CG_Float fact2 = 1.0;
          //if (sinT > 0.8 || sinT < -0.8) fact2 = (1.0 - 10.0*(fabs(sinT) - 0.8));
          //if (fact2 < 0.0) fact2 = 0.0;
          //fact *= fact2;

          cosA = 4*(fmod(1.0+cosA+0.25, 0.5) - 0.25);
          sinT = 2*(fmod(1.0+sinT+0.50, 1.0) - 0.50);

          cosA = fabs(cosA);
          sinT = fabs(sinT);

          ftx0 = 0.5*(1.0 + cosA)*(_terrain.rnx-1);
          fty0 = 0.5*(1.0 + sinT)*(_terrain.rny-1);

          tx0 = ftx0;
          ty0 = fty0;
          tx1 = tx0 + 1; if (tx0 >= _terrain.rnx) tx1 = tx0;
          ty1 = ty0 + 1; if (ty0 >= _terrain.rny) ty1 = ty0;

          //v0 = _terrain.craters[ty0*_terrain.nx + tx0];
          //v1 = _terrain.craters[ty0*_terrain.nx + tx1];
          //v2 = _terrain.craters[ty1*_terrain.nx + tx0];
          //v3 = _terrain.craters[ty1*_terrain.nx + tx1];

          v0 = _terrain.hmap2[ty0*_terrain.nx + tx0];
          v1 = _terrain.hmap2[ty0*_terrain.nx + tx1];
          v2 = _terrain.hmap2[ty1*_terrain.nx + tx0];
          v3 = _terrain.hmap2[ty1*_terrain.nx + tx1];

          v0 = v0 + (v1 - v0)*(ftx0 - tx0);
          v1 = v2 + (v3 - v2)*(ftx0 - tx0);
          v0 = v0 + (v1 - v0)*(fty0 - ty0);

          glift += _maxDepth*v0*fact;
        }
      }

      return _radius - glift;
    }

    void Asteroid::generateTerrain() {
      Filters::Gaussian filter;
      Filters::Gaussian filter2;
      Filters::Gaussian filter3;
      filter.initialize(10, 3.0);
      filter2.initialize(10, 2.0);
      filter3.initialize(50, 8.0);

      if (_terrain.hmap) delete [] _terrain.hmap;
      if (_terrain.hmap2) delete [] _terrain.hmap2;
      if (_terrain.craters) delete [] _terrain.craters;

#ifndef CG_FAST
      _terrain.nx = 1024;
      _terrain.ny = 1024;
#else
      _terrain.nx = 256;
      _terrain.ny = 256;
#endif
      _terrain.rnx = _terrain.nx;
      _terrain.rny = _terrain.ny;
      _terrain.n = _terrain.nx*_terrain.ny;

      PerlinNoise perlin(12);
      PerlinNoise perlin2(12);

      _terrain.craters = new CG_Float[_terrain.n];
      perlin2.genTerrainCraters1(_terrain.craters, _terrain.nx, _terrain.ny, 6);
      for (int y = 0; y < _terrain.ny; ++y) {
        for (int x = 0; x < _terrain.nx; ++x) {
          _terrain.craters[y*_terrain.nx + x] *= 0.1*(0.5+Utils::frand());
        }
      }
      filter2.filterGray(_terrain.craters, _terrain.nx, _terrain.ny, 0.99);

      _terrain.hmap = new CG_Float[_terrain.n];
      _terrain.hmap2 = new CG_Float[_terrain.n];

      perlin.genTerrainAsteroid1(_terrain.hmap, _terrain.nx, _terrain.ny, 5);
      perlin2.genTerrainAsteroid1(_terrain.hmap2, _terrain.nx, _terrain.ny, 4, false);

      CG_Float hmax = _terrain.hmap[0];
      CG_Float hmin2 = _terrain.hmap2[0];
      CG_Float hmax2 = _terrain.hmap2[0];
      for (int y = 0; y < _terrain.rny; ++y) {
        CG_Float T = ((CG_Float)(y-_terrain.rny/2))/_terrain.rny*M_PI;
        CG_Float L = 1.0;
        if (fabs(T) > 0.9*M_PI2) L = cos(T);
        L += 0.05;
        if (L > 1.0) L = 1.0;
        for (int x = _terrain.rnx-1; x >= 0; --x) {
          CG_Float fi0 = L*x;
          int i0 = fi0;

          CG_Float v0 = _terrain.hmap[y*_terrain.nx + i0];
          CG_Float v1 = _terrain.hmap[y*_terrain.nx + i0];
          if (x < _terrain.rnx-1) v1 = _terrain.hmap[y*_terrain.nx + i0 + 1];

          _terrain.hmap[y*_terrain.nx + x] = v0 + (v1 - v0)*(fi0-i0);

          v0 = _terrain.craters[y*_terrain.nx + i0];
          v1 = _terrain.craters[y*_terrain.nx + i0];
          if (x < _terrain.rnx-1) v1 = _terrain.craters[y*_terrain.nx + i0 + 1];

          _terrain.craters[y*_terrain.nx + x] = v0 + (v1 - v0)*(fi0-i0);
          if (L < 0.10) _terrain.craters[y*_terrain.nx + x] = 0.0;

          if (_terrain.hmap[y*_terrain.nx + x] > hmax) hmax = _terrain.hmap[y*_terrain.nx + x];
          if (_terrain.hmap2[y*_terrain.nx + x] < hmin2) hmin2 = _terrain.hmap2[y*_terrain.nx + x];
          if (_terrain.hmap2[y*_terrain.nx + x] > hmax2) hmax2 = _terrain.hmap2[y*_terrain.nx + x];
        }
      }

      if (hmax > 0.0) hmax = 0.5/hmax;
      for (int y = 0; y < _terrain.ny; ++y) {
        for (int x = 0; x < _terrain.nx; ++x) {
          _terrain.hmap[y*_terrain.nx + x] *= hmax;
          _terrain.hmap2[y*_terrain.nx + x] = 0.0 + (_terrain.hmap2[y*_terrain.nx + x] - hmin2)/(hmax2 - hmin2);
        }
      }

      for (int y = 0; y < _terrain.ny/2; ++y) {
        for (int x = 0; x < _terrain.nx/2; ++x) {
          _terrain.hmap2[(_terrain.ny-y-1)*_terrain.nx + x] = _terrain.hmap2[y*_terrain.nx + x];
          _terrain.hmap2[(_terrain.ny-y-1)*_terrain.nx + _terrain.nx - x - 1] = _terrain.hmap2[y*_terrain.nx + x];
          _terrain.hmap2[y*_terrain.nx + _terrain.nx - x - 1] = _terrain.hmap2[y*_terrain.nx + x];
        }
      }

      filter3.filterGray(_terrain.hmap, _terrain.nx, _terrain.ny);
      filter3.filterGray(_terrain.hmap, _terrain.nx, _terrain.ny);
      filter3.filterGray(_terrain.hmap2, _terrain.nx, _terrain.ny, 1000.0, true);
      filter3.filterGray(_terrain.hmap2, _terrain.nx, _terrain.ny, 1000.0, true);

      int f = 1;
      for (int l = 0; l < 7; ++l) {
        for (int y = 0; y < _terrain.ny-1; ++y) {
          for (int x = 0; x < _terrain.nx-1; ++x) {
            CG_Float fx = ((CG_Float)x)/f;
            CG_Float fy = ((CG_Float)y)/f;
            int ix = fx;
            int iy = fy;

            CG_Float v0 = _terrain.craters[iy*_terrain.nx + ix];
            CG_Float v1 = _terrain.craters[iy*_terrain.nx + ix + 1];
            CG_Float v2 = _terrain.craters[(iy+1)*_terrain.nx + ix];
            CG_Float v3 = _terrain.craters[(iy+1)*_terrain.nx + ix + 1];

            v0 = v0 + (v1 - v0)*(fx - ix);
            v1 = v2 + (v3 - v2)*(fx - ix);
            v0 = v0 + (v1 - v0)*(fy - iy);

            _terrain.hmap[y*_terrain.nx + x] += v0;
            _terrain.hmap2[y*_terrain.nx + x] += 5*v0;
          }
        }
        f *= 2;
      }

      filter.filterGray(_terrain.craters, _terrain.nx, _terrain.ny);
      filter.filterGray(_terrain.hmap, _terrain.nx, _terrain.ny);
      filter.filterGray(_terrain.hmap2, _terrain.nx, _terrain.ny, 1000.0, true);
    }

    void Asteroid::generateTextures() {
      for (int i = 0; (int) i < _textures.size(); ++i) {
        if (_textures[i]._texData) delete [] _textures[i]._texData;
      }

      _textures.clear();
      _textures.resize(_nTextures);

      for (int i = 0; i < _nTextures; ++i) {
        PerlinNoise perlin;
#ifndef CG_FAST
        _textures[i]._texX = 1024;
        _textures[i]._texY = 1024;
#else
        _textures[i]._texX = 256;
        _textures[i]._texY = 256;
#endif
        _textures[i]._texN = _textures[i]._texX*_textures[i]._texY;

        _textures[i]._texData = new float[_textures[i]._texN];
        perlin.genTextureGrayRock(_textures[i]._texData, _textures[i]._texX, _textures[i]._texY, 10);
      }

      for (int y = 0; y < _textures[0]._texY; ++y) {
        for (int x = 0; x < _textures[0]._texX/2; ++x) {
          _textures[0]._texData[y*_textures[0]._texX + x] =
            _textures[0]._texData[(y+1)*_textures[0]._texX - x - 1];
        }
      }
    }

  }
}
