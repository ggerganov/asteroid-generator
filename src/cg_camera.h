/*! \file cg_camera.h
 *  \brief Camera Class
 *  \author Georgi Gerganov
*/

#ifndef __CG_CAMERA_H__
#define __CG_CAMERA_H__

#include "cg_vec3.h"

namespace CG {

  class Camera {
    public:
      Camera();

      void move();
      void rotate(CG_Float dx, CG_Float dy);
      void updateSpeed(bool dirF, bool dirB, bool dirL, bool dirR);

      void setPos(CG_Float x, CG_Float y, CG_Float z);
      void getPos(CG_Float &x, CG_Float &y, CG_Float &z) const;

      void setRot(CG_Float x, CG_Float y, CG_Float z);

      inline void setPos(Vec3f p) { _pos = p; }
      inline Vec3f getPos() const { return _pos; }

      Vec3f getPixelDirection(CG_Float px, CG_Float py) const;

      void setNumberOfPixels(int nWidth, int nHeight);

      inline CG_Float getScreenWidth()  const { return _screenWidth; }
      inline CG_Float getScreenHeight() const { return _screenHeight; }

      inline CG_Float getPixelSizeX() const { return _pixelSizeX; }
      inline CG_Float getPixelSizeY() const { return _pixelSizeY; }

      void print();

    private:
      CG_Float _focalLength;
      CG_Float _FOVX;
      CG_Float _FOVY;

      CG_Float _near;
      CG_Float _far;

      Vec3f _pos;
      Vec3f _rot;
      Vec3f _dir;
      Vec3f _dirx;
      Vec3f _diry;
      Vec3f _speed;

      CG_Float _screenWidth;
      CG_Float _screenHeight;

      int _nPixelsWidth;
      int _nPixelsHeight;

      CG_Float _pixelSizeX;
      CG_Float _pixelSizeY;
      CG_Float _rotFactor;
      CG_Float _speedFactor;

      CG_Float _cosrx;
      CG_Float _sinrx;
      CG_Float _cosry;
      CG_Float _sinry;
  };

}

#endif
