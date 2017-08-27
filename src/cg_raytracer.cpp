/*! \file cg_raytracer.cpp
 *  \brief Enter description here.
 *  \author Georgi Gerganov
 */

#include "cg_raytracer.h"

#include "cg_io.h"
#include "cg_sky.h"
#include "cg_utils.h"
#include "cg_camera.h"
#include "cg_opengl.h"
#include "cg_perlin.h"
#include "cg_trajectory.h"

#include "cg_go_asteroid.h"
#include "cg_go_death_star.h"

#include <thread>

namespace CG {

  Raytracer::Raytracer() {
    _timer.start();

    _nRenderedScenes = 0;
    _nTotalRenderedScenes = 0;

    _sx = 0;
    _sy = 0;

    _nRaysPerPixelX = 1;
    _nRaysPerPixelY = 1;
    _rayPixelDx = 1.0;
    _rayPixelDy = 1.0;
    _inRaysPerPixel = 1.0;

    _timeStep = 0.060; // 60 ms

    _screenRGB = NULL;
    _screenRGBBuffer = NULL;
    _camera = NULL;
    _opengl = NULL;
  }

  Raytracer::~Raytracer() {
    if (_screenRGB) delete [] _screenRGB;
    if (_screenRGBBuffer) delete [] _screenRGBBuffer;

    if (_camera) delete [] _camera;
    if (_geometry) delete [] _geometry;
  }

  void Raytracer::initialize(int argc, char **argv) {
    INFO(" [+] Initializing Raytracer components ...\n");

    if (argc > 6) { _nRaysPerPixelX = atoi(argv[6]); }
    if (argc > 7) { _nRaysPerPixelX = atoi(argv[7]); }

    _rayPixelDx = 1.0/_nRaysPerPixelX;
    _rayPixelDy = 1.0/_nRaysPerPixelY;
    _inRaysPerPixel = 1.0/(_nRaysPerPixelX*_nRaysPerPixelY);

    INFO("     - Initializing Sky ...\n");
    _sky = new Sky();
    _sky->initialize(2048, 2048, 8);

    INFO("     - Initializing Camera ...\n");
    _camera = new Camera();
    _camera->setPos(15.0*_KM_, 20.0*_KM_, -260*_KM_);

    INFO("     - Initializing Geometry ...\n");
    //_geometry = new GeometryObjects::DeathStar();
    _geometry = new GeometryObjects::Asteroid();
    _geometry->initialize();

    if (argc > 11) { int m = atoi(argv[11]); _geometry->switchMipmapping(m); }
    if (argc > 12) { _timeStep = atof(argv[12]); }
    if (argc > 14) { int st = atoi(argv[14]); _geometry->setShadows(st); }

    INFO("     - Time step set to %g s\n", _timeStep);
  }

  void Raytracer::setOpenGL(OpenGLInitializator *opengl) {
    _opengl = opengl;

    _sx = _opengl->getWindowSizeX();
    _sy = _opengl->getWindowSizeY();
    _camera->setNumberOfPixels(_sx, _sy);

    _screenRGB = new float[3*_sx*_sy];
    _screenRGBBuffer = new float[3*_sx*_sy];
    for (int i = 0; i < 3*_sx*_sy; ++i) {
      _screenRGB[i] = 0.0;
      _screenRGBBuffer[i] = 0.0;
    }
    _opengl->setDisplayBuffer(_screenRGBBuffer);
    _frameBuffer = _opengl->getFrameBufferPtr();
  }

  void Raytracer::start() {
      static std::thread worker(&Raytracer::run, this);
  }

  void Raytracer::run() {
    INFO(" [+] Raytracer Main Thread started ...\n");

    while (true) {
      CG_Float curTime = _nTotalRenderedScenes*_timeStep;

      updateCamera(curTime);
      _geometry->updateParameters(curTime);

      if (!_screenRGB) continue;
      if (_frameBuffer->isFull()) {
        CGSleep(50.0);
        continue;
      }

      int _y_;
#pragma omp parallel private(_y_)
      {
        float rr, gg, bb;
        #pragma omp for schedule(dynamic)
        for (_y_ = -_sy/2; _y_ < _sy/2; ++_y_) {
          for (int x = -_sx/2; x < _sx/2; ++x) {
            CG_Float p = 0.0;
            int ax = x + _sx/2;
            int ay = _y_ + _sy/2;
            p = renderPixel(x, _y_, rr, gg, bb);
            if (p == 0.0) {
              _screenRGB[3*(ay*_sx + ax) + 0] = rr;
              _screenRGB[3*(ay*_sx + ax) + 1] = gg;
              _screenRGB[3*(ay*_sx + ax) + 2] = bb;
            } else {
              _screenRGB[3*(ay*_sx + ax) + 0] = p;
              _screenRGB[3*(ay*_sx + ax) + 1] = p;
              _screenRGB[3*(ay*_sx + ax) + 2] = p;
            }
          }
        }
      }

      _frameBuffer->pushFrame(_screenRGB);
      ++_nRenderedScenes;
      ++_nTotalRenderedScenes;

      if (_nRenderedScenes % 10 == 0) {
        INFO("     - Render time per scene = %g, (frames in buffer = %d)\n", _timer.time()/_nRenderedScenes, _frameBuffer->getAvailableFrames());
        _nRenderedScenes = 0;
        _timer.start();
      }
    }
  }

  Vec3f lp0(300*_KM_, 0*_KM_, -350*_KM_);
  CG_Float Raytracer::renderPixel(int px, int py, float &rr, float &gg, float &bb) {
    CG_Float finalRes = 0.0;
    CG_Float fpx = (CG_Float) px;
    CG_Float fpy = (CG_Float) py;

    rr = 0.0;
    gg = 0.0;
    bb = 0.0;

    float crr = 0.0;
    float cgg = 0.0;
    float cbb = 0.0;

    for (int pry = 0; pry < _nRaysPerPixelY; ++pry) {
      for (int prx = 0; prx < _nRaysPerPixelX; ++prx) {
        CG_Float res = 0.0;

        Vec3f p = _camera->getPos();
        Vec3f d = _camera->getPixelDirection(fpx + prx*_rayPixelDx, fpy + pry*_rayPixelDy);

        Vec3f lp = lp0;

        if (!_geometry->traceRay(p, d)) {
          _sky->getColor(d, crr, cgg, cbb);
          rr += crr;
          gg += cgg;
          bb += cbb;
          continue;
        }

        Vec3f p0 = p;
        Vec3f lp0 = lp;
        CG_Float shadowFactor;
        //bool isLightVisible = true;
        res = _geometry->getColor(p);

        lp = lp - p; lp.norm();
        _geometry->getNormal(p);

        CG_Float dot = lp.dot(p);
        //if (dot < 0.05) {
        //  res *= 0.05;
        //} else {
        //  res *= dot;
        //  isLightVisible = _geometry->isLightVisible(p0, lp0, shadowFactor);
        //}
        //if (!isLightVisible && dot > 0.0) res *= 0.5;
        if (dot < 0.0) dot = 0.0;
        _geometry->isLightVisible(p0, lp0, shadowFactor);
        res *= dot*shadowFactor*shadowFactor;

        finalRes += res;
      }
    }

    rr = (rr + finalRes)*_inRaysPerPixel;
    gg = (gg + finalRes)*_inRaysPerPixel;
    bb = (bb + finalRes)*_inRaysPerPixel;

    return finalRes*_inRaysPerPixel;
  }

  void Raytracer::updateCamera(CG_Float time) {
    CG_Float rx = 0.0, ry = 0.0, rz = 0.0;

#ifndef CG_FAST
    Trajectory::P1(time, -300.0*_KM_, -200.0*_KM_, 30.0, rx, ry, rz);
    Trajectory::P2(time, 0.20*M_PI, 0.25*M_PI, 41.0, rx, ry, rz);
    _camera->setPos(rx, ry, rz);

    rx = 0.0; ry = 0.0; rz = 0.0;
    Trajectory::R2(time, 0.20*M_PI, 0.25*M_PI, 41.0, rx, ry, rz);
    Trajectory::R1(time, 0.0005*M_PI, 0.0006*M_PI, 3.0, 2.5, rx, ry, rz);
    _camera->setRot(rx-0.011, ry, rz);
#else
    Trajectory::P1(time, -400.0*_KM_, +125.0*_KM_, 30.0, rx, ry, rz);
    Trajectory::P2(time, 0.20*M_PI, 0.25*M_PI, 41.0, rx, ry, rz);
    _camera->setPos(rx, ry, rz);

    rx = 0.0; ry = 0.0; rz = 0.0;
    Trajectory::R2(time, 0.20*M_PI, 0.25*M_PI, 41.0, rx, ry, rz);
    Trajectory::R1(time, 0.0005*M_PI, 0.0006*M_PI, 3.0, 2.5, rx, ry, rz);
    _camera->setRot(rx-0.011, ry, rz);
#endif

    //if (_opengl->isPressed('-')) lp0._z += 15*_KM_;
    //if (_opengl->isPressed('=')) lp0._z -= 15*_KM_;
    //if (_opengl->isPressed('[')) lp0._y += 15*_KM_;
    //if (_opengl->isPressed(']')) lp0._y -= 15*_KM_;

    //if (_opengl->isPressed('j')) rx += 1e-2;
    //if (_opengl->isPressed('l')) rx -= 1e-2;
    //if (_opengl->isPressed('i')) ry += 1e-2;
    //if (_opengl->isPressed('k')) ry -= 1e-2;

    _camera->rotate(rx, ry);

    //_camera->updateSpeed(
    //    _opengl->isPressed('w') || _opengl->isPressed('W'),
    //    _opengl->isPressed('s') || _opengl->isPressed('S'),
    //    _opengl->isPressed('a') || _opengl->isPressed('A'),
    //    _opengl->isPressed('d') || _opengl->isPressed('D'));

    _camera->move();

    if (_opengl->isPressed('m')) _geometry->switchMipmapping();

    _geometry->setCameraDistance(_camera->getPos().l());
    _geometry->setCameraPos(_camera->getPos());
    //_camera->print();
  }

}
