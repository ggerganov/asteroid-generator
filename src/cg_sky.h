/*! \file cg_sky.h
 *  \brief Generate Cosmic Sky.
 *  \author Georgi Gerganov
 */

#ifndef __CG_SKY_H__
#define __CG_SKY_H__

#include "cg_config.h"

#include "cg_vec3.h"

namespace CG {
  class Sky {
    public:
      Sky();
      ~Sky();

      void initialize(int nx, int ny, int levels);

      void getColor(Vec3f p, float &r, float &g, float &b);
      void getColor(CG_Float cosA, CG_Float sinT, float &r, float &g, float &b);

    private:
      int _nx;
      int _ny;

      float *_texture;

      CG_Float _cosALim;
      CG_Float _sinTLim;
      CG_Float _icosALim;
      CG_Float _isinTLim;

      static const int _maxLevels = 12;
  };
}

#endif
