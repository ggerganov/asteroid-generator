/*! \file cg_frame_buffer.h
 *  \brief Frame buffer for displaying at fixed frame rate.
 *  \author Georgi Gerganov
 */

#ifndef __CG_FRAME_BUFFER_H__
#define __CG_FRAME_BUFFER_H__

//#include "cg_io.h"

namespace CG {
  template <class ColorType>
  class FrameBuffer {
    public:
      FrameBuffer() {
        _nFrames = 0;

        _curFrame = 0;
        _lastFrame = 0;

        _nAvailableFrames = 0;
        _isFrameAvailable = false;

        _colorScale = 255.0;
        _icolorScale = 1.0/_colorScale;

        _frameData = 0;
      }

      ~FrameBuffer() {
        if (_frameData) delete [] _frameData;
      }

      void initialize(int nx, int ny, int nFrames) {
        _nx = nx;
        _ny = ny;
        _nFrames = nFrames;

        _curFrame = 0;
        _lastFrame = 0;

        _nAvailableFrames = 0;
        _isFrameAvailable = false;

        if (_frameData) delete [] _frameData;
        _frameData = new ColorType[3*_nx*_ny*_nFrames];
      }

      bool isFull() const { return ((_lastFrame == _curFrame) && (_isFrameAvailable)); }
      bool isFrameAvailable() const { return _isFrameAvailable; }

      void pushFrame(float *frame) {
        //DBG(1, "Pushing frame, cur = %d, last = %d, avail = %d\n", _curFrame, _lastFrame, _nAvailableFrames);
        int nn = 3*_nx*_ny;
        ColorType *curp = _frameData + _lastFrame*nn;
        for (int i = 0; i < nn; ++i) {
          curp[i] = (ColorType) (_colorScale*frame[i]);
        }
        if (++_lastFrame >= _nFrames) _lastFrame = 0;
        _nAvailableFrames++;
        _isFrameAvailable = true;
        //DBG(1, "Pushed frame, cur = %d, last = %d, avail = %d\n", _curFrame, _lastFrame, _nAvailableFrames);
      }

      void popFrame(float *frame) {
        //DBG(1, "Popping frame, cur = %d, last = %d, avail = %d\n", _curFrame, _lastFrame, _nAvailableFrames);
        int nn = 3*_nx*_ny;
        ColorType *curp = _frameData + _curFrame*nn;
        for (int i = 0; i < nn; ++i) {
          frame[i] = ((float)(curp[i]))*_icolorScale;
        }
        if (++_curFrame >= _nFrames) _curFrame = 0;
        _nAvailableFrames--;
        if (_curFrame == _lastFrame) _isFrameAvailable = false;
        //DBG(1, "Popped frame, cur = %d, last = %d, avail = %d\n", _curFrame, _lastFrame, _nAvailableFrames);
      }

      int getAllocatedMemory() const { return 3*_nx*_ny*_nFrames*sizeof(ColorType); }
      int getAvailableFrames() const { return _nAvailableFrames; }

    private:
      int _nx;
      int _ny;
      int _nFrames;

      int _curFrame;
      int _lastFrame;

      int _nAvailableFrames;
      bool _isFrameAvailable;

      float _colorScale;
      float _icolorScale;

      ColorType *_frameData;
  };

  typedef FrameBuffer<unsigned char> FrameBufferChar;

}

#endif
