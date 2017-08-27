/*! \file cg_raytracer.h
 *  \brief Raytracer Class
 *  \author Georgi Gerganov
*/

#ifndef __CG_RAYTRACER_H__
#define __CG_RAYTRACER_H__

#include "cg_config.h"

#include "cg_timer.h"
#include "cg_frame_buffer.h"

namespace CG {
  class Sky;
  class Camera;
  class Geometry;
  class OpenGLInitializator;

  class Raytracer {
    public:
      Raytracer();
      virtual ~Raytracer();

      void initialize(int argc, char **argv);
      void setOpenGL(OpenGLInitializator *opengl);

      void start();

    protected:
      virtual void run();

    private:
      void updateCamera(CG_Float time);
      CG_Float renderPixel(int px, int py, float &rr, float &gg, float &bb);

      Timer _timer;
      int _nRenderedScenes;
      int _nTotalRenderedScenes;

      int _sx;
      int _sy;

      int _nRaysPerPixelX;
      int _nRaysPerPixelY;
      CG_Float _rayPixelDx;
      CG_Float _rayPixelDy;
      CG_Float _inRaysPerPixel;

      CG_Float _timeStep;

      float *_screenRGB;
      float *_screenRGBBuffer;
      FrameBufferChar *_frameBuffer;

      Sky *_sky;
      Camera *_camera;
      Geometry *_geometry;
      OpenGLInitializator *_opengl;
  };

}

#endif
