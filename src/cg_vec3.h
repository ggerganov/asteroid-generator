/*! \file cg_vec3.h
 *  \brief 3D Vector Class
 *  \author Georgi Gerganov
*/

#ifndef __CG_VEC3_H__
#define __CG_VEC3_H__

#include "cg_config.h"

#include <cmath>

namespace CG {

  template <class T>
  class Vec3 {
    public:
      inline Vec3() : _x(0.0), _y(0.0), _z(0.0) {}
      inline Vec3(T x, T y, T z) : _x(x), _y(y), _z(z) {}

      inline Vec3(const Vec3 &b) : _x(b._x), _y(b._y), _z(b._z) {}
      inline Vec3& operator=(const Vec3 &b) { _x = b._x; _y = b._y; _z = b._z; return *this; }

      inline Vec3 operator+(const Vec3 &b) const { return Vec3(_x + b._x, _y + b._y, _z + b._z); }
      inline Vec3 operator-(const Vec3 &b) const { return Vec3(_x - b._x, _y - b._y, _z - b._z); }
      inline Vec3 operator*(const Vec3 &b) const { return Vec3(_x * b._x, _y * b._y, _z * b._z); }
      inline Vec3 operator/(const Vec3 &b) const { return Vec3(_x / b._x, _y / b._y, _z / b._z); }

      inline Vec3& operator+=(const Vec3 &b) { _x += b._x; _y += b._y; _z += b._z; return *this; }
      inline Vec3& operator-=(const Vec3 &b) { _x -= b._x; _y -= b._y; _z -= b._z; return *this; }
      inline Vec3& operator*=(const Vec3 &b) { _x *= b._x; _y *= b._y; _z *= b._z; return *this; }
      inline Vec3& operator/=(const Vec3 &b) { _x /= b._x; _y /= b._y; _z /= b._z; return *this; }

      inline Vec3 operator+(const T b) const { return Vec3(_x + b, _y + b, _z + b); }
      inline Vec3 operator-(const T b) const { return Vec3(_x - b, _y - b, _z - b); }
      inline Vec3 operator*(const T b) const { return Vec3(_x * b, _y * b, _z * b); }
      inline Vec3 operator/(const T b) const { return Vec3(_x / b, _y / b, _z / b); }

      inline Vec3& operator+=(const T b) { _x += b; _y += b; _z += b; }
      inline Vec3& operator-=(const T b) { _x -= b; _y -= b; _z -= b; }
      inline Vec3& operator*=(const T b) { _x *= b; _y *= b; _z *= b; }
      inline Vec3& operator/=(const T b) { _x /= b; _y /= b; _z /= b; }

      inline Vec3 cross(const Vec3 &b) const { return Vec3(_y*b._z - _z*b._y, _z*b._x - _x*b._z, _x*b._y - _y*b._x); }
      inline T dot(const Vec3 &b) const { return _x*b._x + _y*b._y + _z*b._z; }
      inline T l() const { return sqrt(_x*_x + _y*_y + _z*_z); }
      inline T l2() const { return _x*_x + _y*_y + _z*_z; }
      inline void norm() { T ll = 1.0/sqrt(_x*_x + _y*_y + _z*_z); _x *= ll; _y *= ll; _z *= ll; }
      inline void norm(const T to) { T ll = to/sqrt(_x*_x + _y*_y + _z*_z); _x *= ll; _y *= ll; _z *= ll; }

      T _x, _y, _z;
  };

  typedef Vec3<CG_Float> Vec3f;

  inline Vec3f operator+(CG_Float a, const Vec3f &b) { return b + a; }
  inline Vec3f operator*(CG_Float a, const Vec3f &b) { return b * a; }
}

#endif
