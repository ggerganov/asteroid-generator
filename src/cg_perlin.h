/*! \file cg_perlin.h
 *  \brief Generate Perlin noise.
 *  \author Georgi Gerganov
 */

#ifndef __CG_PERLIN_H__
#define __CG_PERLIN_H__

#include "cg_config.h"

#include <cmath>

namespace CG {
  class PerlinNoise {
    public:
      PerlinNoise(int power = 12);
      ~PerlinNoise();

      CG_Float sample(CG_Float x, CG_Float y) const;

      void genTextureRGBSpaceBackground(float *texture, int nx, int ny, int levels);
      void genTextureGrayMetal(float *texture, int nx, int ny, int levels);
      void genTextureGrayRock(float *texture, int nx, int ny, int levels);

      void genTerrainAsteroid1(CG_Float *hmap, int nx, int ny, int levels, bool valleys = true);
      void genTerrainCraters1(CG_Float *hmap, int nx, int ny, int levels);

      int getN() const { return _n; }

    private:
      inline CG_Float smooth(CG_Float t) const { return t*t*(3.0 - 2.0*t); }
      inline CG_Float interp(CG_Float v1, CG_Float v2, CG_Float t) const {
        return v1 + (v2-v1)*t;
      }

      int _n;
      int _nMask;

      CG_Float *_r;
  };
}

#endif
