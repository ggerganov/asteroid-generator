/*! \file cg_geometry_height_map.h
 *  \brief General purpose height map geometry on spherical object
 *  \author Georgi Gerganov
*/

#ifndef __CG_GEOMETRY_HEIGHT_MAP_H__
#define __CG_GEOMETRY_HEIGHT_MAP_H__

#include "cg_geometry.h"

#include "cg_timer.h"

namespace CG {
  class GeometryHeightMap : public Geometry {
    public:
      GeometryHeightMap();
      ~GeometryHeightMap();

      virtual bool traceRay(Vec3f &p, const Vec3f &d);

    private:
      virtual bool traceLight(Vec3f &p, const Vec3f &d, Vec3f pd, CG_Float &slen);

    public:
      virtual bool isLightVisible(Vec3f p, Vec3f l, CG_Float &s);

      virtual void getNormal(Vec3f &p);

      virtual void setShadows(int type);

    protected:
      virtual CG_Float getHeight(Vec3f p) = 0;

#ifndef CG_FAST
      virtual void getSurfacePoint(Vec3f &p);
#else
      inline void getSurfacePoint(Vec3f &p) {
        p.norm(getHeight(p));
      }
#endif

      virtual void getAngles(Vec3f p, CG_Float &cosPhi, CG_Float &sinTheta);
      virtual void rotatePoint(Vec3f &p);

      void setConeStepParameters(CG_Float gConeAlpha = -1.0,
                                 int maxConeSteps = -1.0,
                                 int maxBinarySteps = -1.0);

      void setRotation(CG_Float rx, CG_Float ry, CG_Float rz);

      Timer _timer;

      Vec3f _pos;
      Vec3f _rot;

      CG_Float _sinrx;
      CG_Float _cosrx;
      CG_Float _sinry;
      CG_Float _cosry;
      CG_Float _sinrxsinry;
      CG_Float _sinrxcosry;
      CG_Float _cosrxsinry;
      CG_Float _cosrxcosry;
      CG_Float _rotationSpeed;

      CG_Float _radius;
      CG_Float _iradius;
      CG_Float _radius2;

    private:
      CG_Float _gConeAlpha;
      CG_Float _cosCone;
      CG_Float _sinCone;
      CG_Float _tanCone;
      CG_Float _ctgCone;

      int _maxConeSteps;
      int _maxBinarySteps;

      int _shadowType;
  };
}

#endif
