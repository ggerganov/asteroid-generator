/*! \file cg_filters.h
 *  \brief Enter description here.
 *  \author Georgi Gerganov
 */

#ifndef __CG_FILTERS_H__
#define __CG_FILTERS_H__

#include "cg_config.h"

namespace CG {
  namespace Filters {
    class Gaussian {
      public:
        Gaussian();
        ~Gaussian();

        void initialize(int nW, CG_Float sig);

        void filterRGB(float *img, int nx, int ny, CG_Float blend = 1000.0);
        void filterGray(CG_Float *img, int nx, int ny, CG_Float blend = 1000.0, bool cycleY = false);

      private:
        int _nW;
        CG_Float _sig;

        CG_Float *_kernel;

        int _lastN;
        CG_Float *_tmp;

        int _lastNRGB;
        float *_tmpRGB;
    };
  }
}


#endif
