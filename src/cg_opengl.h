/*! \file cg_opengl.h
 *  \brief Initialize OpenGL shits
 *  \author Georgi Gerganov
*/

#ifndef __CG_OPENGL_H__
#define __CG_OPENGL_H__

#include "cg_timer.h"
#include "cg_config.h"
#include "cg_frame_buffer.h"

namespace CG {

  class OpenGLInitializator {
    public:
      OpenGLInitializator();
      virtual ~OpenGLInitializator();

      void start();
      void setActive();

      bool isPressed(char key) const { return _key[(int) key]; }
      bool isShiftPressed() const { return _shiftDown; }

      void setWindowSize(const int nx, const int ny);
      int getWindowSizeX() const { return _windowSizeX; }
      int getWindowSizeY() const { return _windowSizeY; }

      int getWindowID() const { return _windowID; }

      void setDisplayBuffer(float *screenRGB) { _screenRGB = screenRGB; }

      void setTimePerFrame(const int timeInMs) { _timePerFrame = timeInMs; }

      void initDisplay(int argc, char **argv, const char *title);

      inline CG_Float getTime() { return _timer.time(); }

      void forceRedraw();

      FrameBufferChar *getFrameBufferPtr() const { return _frameBuffer; }

    protected:
      virtual void displayFrame(void);
      virtual void redrawFrameCallback(int);
      virtual void keyboardKey(unsigned char key, int x, int y);
      virtual void keyboardKeyUp(unsigned char key, int x, int y);

    private:
      static void displayFrameStatic(void);
      static void redrawFrameCallbackStatic(int);
      static void keyboardKeyStatic(unsigned char key, int x, int y);
      static void keyboardKeyUpStatic(unsigned char key, int x, int y);

      Timer _timer;
      CG_Float _initialLoadingTime;

      int _windowID;
      int _windowSizeX;
      int _windowSizeY;

      int _displaySizeX;
      int _displaySizeY;

      int _timePerFrame;
      int _nTotalFrames;
      int _nLastFrames;

      float *_screenRGB;
      FrameBufferChar *_frameBuffer;

      // Keyboard stuff
      bool _key[256];
      bool _shiftDown;

      bool _outputRAW;
  };

}

#endif
