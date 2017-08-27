/*! \file cg_go_asteroid.h
 *  \brief The Asteroid Geometry
 *  \author Georgi Gerganov
*/

#ifndef __CG_GO_ASTEROID_H__
#define __CG_GO_ASTEROID_H__

#include "cg_geometry_height_map.h"

#include <vector>

namespace CG {
  namespace GeometryObjects {
    class Asteroid : public GeometryHeightMap {
      public:
        Asteroid();
        ~Asteroid();

        virtual void initialize();

        virtual CG_Float getColor(Vec3f p);

        virtual void updateParameters(CG_Float time);

        virtual void switchMipmapping(int val);

      private:
        virtual CG_Float getHeight(Vec3f p);

        void generateTerrain();
        void generateTextures();

        CG_Float _maxDepth;

        bool _regenerate;

        bool _isMipmapping;
        CG_Float _mipmapDistance;
        CG_Float _mipmapTerrainFactor;
        CG_Float _mipmapTextureFactor;

        struct _st_terrain {
          int nx;
          int ny;
          int rnx;
          int rny;
          int n;
          CG_Float *hmap;
          CG_Float *hmap2;
          CG_Float *craters;
        };
        _st_terrain _terrain;

        int _nTextures;
        struct _st_texture {
          int _texX;
          int _texY;
          int _texN;
          float *_texData;
        };
        std::vector<_st_texture> _textures;
    };

  }
}

#endif
