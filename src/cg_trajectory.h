/*! \file cg_trajectory.h
 *  \brief Some basic trajectories.
 *  \author Georgi Gerganov
 */

#ifndef __CG_TRAJECTORY_H__
#define __CG_TRAJECTORY_H__

#include "cg_config.h"

namespace CG {
  namespace Trajectory {
    // zoom in, zoom out along Z
    static void P1(CG_Float t,
                   CG_Float cz, CG_Float az, CG_Float T,
                   CG_Float &x, CG_Float &y, CG_Float &z) {
      t += 1.5*T;
      t = fmod(t, 1.333*T);
      if (t < 0.333*T) { t = 3.0*t; } else { t += 0.666*T; }

      CG_Float sa = sin(M_PI*t/T);
      if (sa > 0.0) sa *= 0.25;
      z = sa*az + cz;
    }

    // camera rotate pos
    static void P2(CG_Float t,
                   CG_Float cz, CG_Float az, CG_Float T,
                   CG_Float &x, CG_Float &y, CG_Float &z) {
      //CG_Float angx = 0.33*sin(-0.64*M_PI*t/T)*az + cz;
      //CG_Float angx = 0.13*sin(-0.44*M_PI*t/T)*az + cz;
      //CG_Float angy =      sin(-M_PI*t/T)*az + cz;
      CG_Float angx = 0.33*sin(-0.64*M_PI*t/T)*az + cz;
      CG_Float angy =      sin(-M_PI*t/(T+0.1*sin(M_PI*t/100.0)))*az + cz;

      CG_Float cosrx = cos(angx);
      CG_Float sinrx = sin(angx);
      CG_Float cosry = cos(angy);
      CG_Float sinry = sin(angy);

      CG_Float sinrxsinry = sinrx*sinry;
      CG_Float cosrxsinry = cosrx*sinry;
      CG_Float sinrxcosry = sinrx*cosry;
      CG_Float cosrxcosry = cosrx*cosry;

      CG_Float tx = x*cosry - z*sinry;
      CG_Float ty = x*sinrxsinry + y*cosrx + z*sinrxcosry;
      z = x*cosrxsinry - y*sinrx + z*cosrxcosry;
      x = tx;
      y = ty;
    }

    // camera float
    static void R1(CG_Float t,
                   CG_Float ax, CG_Float ay, CG_Float Tx, CG_Float Ty,
                   CG_Float &rx, CG_Float &ry, CG_Float &rz) {
      rx += sin(-M_PI*t/Tx)*ax;
      ry += sin(-M_PI*t/Ty)*ay;
    }

    // camera rotate
    static void R2(CG_Float t,
                   CG_Float cz, CG_Float az, CG_Float T,
                   CG_Float &rx, CG_Float &ry, CG_Float &rz) {
      rx = 0.33*sin(-0.64*M_PI*t/T)*az + cz;
      ry =      sin(-M_PI*t/(T+0.1*sin(M_PI*t/100.0)))*az + cz;
    }

  }
}

#endif
