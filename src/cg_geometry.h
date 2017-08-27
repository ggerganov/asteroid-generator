/*! \file cg_geometry.h
 *  \brief Geometry Class
 *  \author Georgi Gerganov
*/

#ifndef __CG_GEOMETRY_H__
#define __CG_GEOMETRY_H__

#include "cg_vec3.h"

namespace CG {
  class Geometry {
    public:
      Geometry();
      virtual ~Geometry();

      virtual void initialize() = 0;
      virtual bool traceRay(Vec3f &p, const Vec3f &d) = 0;
      virtual bool isLightVisible(Vec3f p, Vec3f l, CG_Float &s) = 0;

      virtual void getNormal(Vec3f &p) = 0;
      virtual CG_Float getColor(Vec3f p) = 0;

      virtual void updateParameters(CG_Float time) = 0;

      void setCameraDistance(CG_Float d) { _cameraDistance = d; }
      void setCameraPos(const Vec3f v) {
        _cameraPos._x = v._x;
        _cameraPos._y = v._y;
        _cameraPos._z = v._z;
      }

      virtual void switchMipmapping(int val = -1) = 0;
      virtual void setShadows(int type = 8) = 0;

    protected:
      CG_Float _cameraDistance;
      Vec3f _cameraPos;
  };

}

#endif
