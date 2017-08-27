/*! \file cg_camera.cpp
 *  \brief Enter description here.
 *  \author Georgi Gerganov
 */

#include "cg_io.h"
#include "cg_camera.h"

#include <cmath>

namespace CG {

  Camera::Camera() {
    _focalLength = 100*_MM_; // 100 mm
    _FOVX = 0.5*M_PI2;
    _FOVY = 0.5*M_PI2;

    _near = _focalLength;
    _far = 1e6*_KM_; // 1e6 km

    _pos._x  = 0.0; _pos._y  = 0.0; _pos._z  = 0.0;
    _rot._x  = 0.0; _rot._y  = 0.0; _rot._z  = 0.0;
    _dir._x  = 0.0; _dir._y  = 0.0; _dir._z  = 1.0;
    _dirx._x = 1.0; _dirx._y = 0.0; _dirx._z = 0.0;
    _diry._x = 0.0; _diry._y = 1.0; _diry._z = 0.0;

    _speed._x = 0.0; _speed._y = 0.0; _speed._z = 0.0;

    _screenWidth  = 2.0*_focalLength*sin(0.5*_FOVX);
    _screenHeight = 2.0*_focalLength*sin(0.5*_FOVY);

    setNumberOfPixels(100, 100);

    _rotFactor = 0.1;
    _speedFactor = 1.0*_KM_;
  }

  void Camera::move() { _pos += _speed; }

  void Camera::rotate(CG_Float dx, CG_Float dy) {
    _rot._x += dy*_rotFactor;
    _rot._y += dx*_rotFactor;

    if (_rot._x >  M_PI2) _rot._x = M_PI2;
    if (_rot._x < -M_PI2) _rot._x = -M_PI2;

    if (_rot._y < -M_PI) _rot._y += M_2PI;
    if (_rot._y >  M_PI) _rot._y -= M_2PI;

    setRot(_rot._x, _rot._y, _rot._z);
  }

  void Camera::updateSpeed(bool dirF, bool dirB, bool dirL, bool dirR) {
    CG_Float vx = 0.0;
    CG_Float vy = 0.0;
    CG_Float vz = 0.0;

    if (dirB) {
      vx += _sinry*_cosrx;
      vy -= _sinrx;
      vz -= _cosry*_cosrx;
    }

    if (dirF) {
      vx -= _sinry*_cosrx;
      vy += _sinrx;
      vz += _cosry*_cosrx;
    }

    if (dirL) {
      vx -= _cosry;
      vz -= _sinry;
    }

    if (dirR) {
      vx += _cosry;
      vz += _sinry;
    }

    _speed._x = _speedFactor*vx;
    _speed._y = _speedFactor*vy;
    _speed._z = _speedFactor*vz;

    if (_speed.l2() > 1e-6) _speed.norm(_speedFactor);
  }

  void Camera::setPos(CG_Float x, CG_Float y, CG_Float z) { _pos._x = x; _pos._y = y; _pos._z = z; }
  void Camera::getPos(CG_Float &x, CG_Float &y, CG_Float &z) const { x = _pos._x; y = _pos._y; z = _pos._z; }

  void Camera::setRot(CG_Float x, CG_Float y, CG_Float z) {
    _rot._x = x; _rot._y = y; _rot._z = z;

    _cosrx = cos(_rot._x);
    _sinrx = sin(_rot._x);
    _cosry = cos(_rot._y);
    _sinry = sin(_rot._y);

    _dir._x = -_sinry;
    _dir._y = _cosry*_sinrx;
    _dir._z = _cosry*_cosrx;

    _dirx._x = _cosry;
    _dirx._y = _sinrx*_sinry;
    _dirx._z = _cosrx*_sinry;
    //_dirx._y = _cosrx*_sinry;
    //_dirx._z = _sinrx*_sinry;

    _diry._x = 0.0;
    _diry._y = _cosrx;
    _diry._z = -_sinrx;
  }

  Vec3f Camera::getPixelDirection(CG_Float px, CG_Float py) const {
    Vec3f d = (_dir *_focalLength) +
              (_dirx*_pixelSizeX*px) +
              (_diry*_pixelSizeY*py);
    d.norm();
    return d;
  }

  void Camera::setNumberOfPixels(int nWidth, int nHeight) {
    _nPixelsWidth = nWidth;
    _nPixelsHeight = nHeight;

    _pixelSizeX = _screenWidth / _nPixelsWidth;
    _pixelSizeY = _screenHeight / _nPixelsHeight;
  }

  void Camera::print() {
    INFO("Camera: pos = [%g, %g, %g], rot = [%g, %g, %g], dir = [%g, %g, %g], dirx = [%g, %g, %g], diry = [%g %g %g] \n",
        _pos._x, _pos._y, _pos._z,
        _rot._x, _rot._y, _rot._z,
        _dir._x, _dir._y, _dir._z,
        _dirx._x, _dirx._y, _dirx._z,
        _diry._x, _diry._y, _diry._z);
  }

}
