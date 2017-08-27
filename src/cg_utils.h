/*! \file cg_utils.h
 *  \brief Some helpful utils.
 *  \author Georgi Gerganov
 */

#ifndef __CG_UTILS_H__
#define __CG_UTILS_H__

#define SQR(A) (A)*(A)

#include "cg_config.h"

#include <cstdlib>
#include <cmath>

const CG_Float IRAND_MAX = 1.0/RAND_MAX;

namespace CG {
  class Utils {
    public:
      static CG_Float frand() { return ((CG_Float)(rand()%RAND_MAX)*IRAND_MAX); }
      static CG_Float frandGaussian(CG_Float sig);

      static void bloomFilter(float *img, int nx, int ny, int down, CG_Float weight);
      static void gaussianFilter(float *img, int nx, int ny, CG_Float window, CG_Float sigma, CG_Float weight);
      static void gaussianFilterGray(CG_Float *img, int nx, int ny, CG_Float window, CG_Float sigma, CG_Float weight);
  };
}

#endif
