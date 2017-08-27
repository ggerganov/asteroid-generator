/*! \file cg_go_death_star.h
 *  \brief The Death Start Geometry
 *  \author Georgi Gerganov
*/

#ifndef __CG_GO_DEATH_STAR_H__
#define __CG_GO_DEATH_STAR_H__

#include "cg_geometry_height_map.h"

#include <vector>

namespace CG {
  namespace GeometryObjects {
    class DeathStar : public GeometryHeightMap {
      public:
        DeathStar();
        ~DeathStar();

        virtual void initialize();

        virtual CG_Float getColor(Vec3f p);

        virtual void updateParameters(CG_Float time);

      private:
        virtual CG_Float getHeight(Vec3f p);

        bool isInsideWeapon(Vec3f p);
        bool isInsideWeapon(Vec3f p, CG_Float &cosP);
        bool isNearWeapon(Vec3f p, const CG_Float fact = 0.995);

        bool isInsideTunnel(Vec3f p);
        bool isNearTunnel(Vec3f p, const CG_Float fact = 4.0);

        void generateTextures();
        void generatePatches();

        int getPatchID(Vec3f p);
        int getPatchID(CG_Float cosA, CG_Float sinT);

        // the death ray weapon parameters
        struct {
          Vec3f _dir;
          CG_Float _aperture;
          CG_Float _halfAperture;
          CG_Float _cosHA;
          CG_Float _cosHAInner;
          CG_Float _focus;
          CG_Float _depthFactor;
          CG_Float _depthFactorInner;

          Vec3f _norm1;
          Vec3f _center;
        } _weapon;

        // the tunnel parameters
        struct {
          CG_Float _width;
          CG_Float _halfWidth;
          CG_Float _depth;
        } _tunnel;

        int _nTextures;
        struct _st_texture {
          int _texX;
          int _texY;
          int _texN;
          float *_texData;
        };
        std::vector<_st_texture> _textures;

        int _nPatches;
        int _nPatchTSectors;
        int _nPatchASectors;
        struct _st_patch {
          CG_Float _minA;
          CG_Float _maxA;
          CG_Float _idA;
          CG_Float _minT;
          CG_Float _maxT;
          CG_Float _idT;
          CG_Float _height;

          int _texID;
        };
        std::vector<_st_patch> _patches;
    };

  }
}

#endif
