/*! \file cg_opengl.cpp
 *  \brief Implement the CG::OpenGLInitializator.
 *  \author Georgi Gerganov
 */

#include "cg_io.h"
#include "cg_opengl.h"

#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <GLUT/glut.h>
#else
#include <GL/gl.h>
#include <GL/glut.h>
#endif

#include <cstdlib>
#include <fstream>

namespace CG {
  CG::OpenGLInitializator *g_activeWindow = NULL;

  void OpenGLInitializator::displayFrameStatic(void) {
    if (g_activeWindow) g_activeWindow->displayFrame();
  }

  void OpenGLInitializator::redrawFrameCallbackStatic(int) {
    if (g_activeWindow) g_activeWindow->redrawFrameCallback(0);
  }

  void OpenGLInitializator::keyboardKeyStatic(unsigned char key, int x, int y) {
    if (g_activeWindow) g_activeWindow->keyboardKey(key, x, y);
  }

  void OpenGLInitializator::keyboardKeyUpStatic(unsigned char key, int x, int y) {
    if (g_activeWindow) g_activeWindow->keyboardKeyUp(key, x, y);
  }

  OpenGLInitializator::OpenGLInitializator() {
    _initialLoadingTime = 0.0;

    _windowID = -1;
    _windowSizeX = 0;
    _windowSizeY = 0;

    _displaySizeX = 0;
    _displaySizeY = 0;

    _timePerFrame = 41;
    _nTotalFrames = 0;
    _nLastFrames = 0;

    _screenRGB = NULL;
    _frameBuffer = NULL;

    for (int i = 0; i < 256; ++i) _key[i] = false;
    _shiftDown = false;

    _outputRAW = false;
  }

  OpenGLInitializator::~OpenGLInitializator() {
  }

  void OpenGLInitializator::start() {
    INFO(" [+] Starting OpenGL Main Loop ...\n");
    _timer.start();
    glutMainLoop();
  }

  void OpenGLInitializator::setActive() {
    g_activeWindow = this;
    redrawFrameCallbackStatic(0);
  }

  void OpenGLInitializator::setWindowSize(const int nx, const int ny) {
    _windowSizeX = nx;
    _windowSizeY = ny;

    _displaySizeX = nx;
    _displaySizeY = ny;
  }

  void OpenGLInitializator::initDisplay(int argc, char **argv, const char *title) {
    int nx = 800;
    int ny = 600;
    int nBufferFrames = 1;

    if (argc > 3) {
      nx = atoi(argv[2]);
      ny = atoi(argv[3]);
    }

    if (argc > 4) { nBufferFrames = atoi(argv[4]); }
    if (argc > 5) {
      _initialLoadingTime = atof(argv[5]);
      if (_initialLoadingTime > 0.0) {
        INFO(" [!] Buffering for %g seconds. Please wait ...\n", _initialLoadingTime);
      }
    }

    if (argc > 8) {
      int t = atoi(argv[8]);
      if (t == 1) _outputRAW = true;
      if (_outputRAW) {
        INFO(" [I] Outputing RAW RGB frame to 'video.raw'\n");
      }
    }

    setWindowSize(nx, ny);

    if (argc > 9)  { _displaySizeX = atoi(argv[9]);  if (_displaySizeX <= 0) _displaySizeX = _windowSizeX; }
    if (argc > 10) { _displaySizeY = atoi(argv[10]); if (_displaySizeY <= 0) _displaySizeY = _windowSizeY; }

    _frameBuffer = new FrameBufferChar();
    _frameBuffer->initialize(nx, ny, nBufferFrames);
    INFO(" [+] Frame buffer initialized. Memory = %d MB\n", _frameBuffer->getAllocatedMemory()/1024/1024);

    glutInit (&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowPosition(0,0);
    glutInitWindowSize(_displaySizeX, _displaySizeY);
    _windowID = glutCreateWindow(title);

    glClearColor(0.0, 0.0, 0.0, 0.0);
    glMatrixMode(GL_PROJECTION);
    //gluOrtho2D(0.0, _displaySizeX, 0.0, _displaySizeY);

    // register callback function to display graphics:
    glutDisplayFunc(displayFrameStatic);
    glutKeyboardFunc(keyboardKeyStatic);
    glutKeyboardUpFunc(keyboardKeyUpStatic);
  }

  void OpenGLInitializator::forceRedraw() {
    glutPostRedisplay();
  }

  void OpenGLInitializator::displayFrame(void) {
    if ((_nTotalFrames == 0) && (_timer.time() < _initialLoadingTime) && (!_frameBuffer->isFull())) return;
    if (!_screenRGB) return;
    if (!_frameBuffer->isFrameAvailable()) return;

    _frameBuffer->popFrame(_screenRGB);
    glDrawPixels(_displaySizeX, _displaySizeY, GL_RGB, GL_FLOAT, _screenRGB);
    glutSwapBuffers();

    if (_outputRAW) {
      static std::ofstream fout("video.raw", std::ios::binary);
      for (int i = 0; i < 3*_windowSizeX*_windowSizeY; ++i) {
        unsigned char v = 255*_screenRGB[i];
        fout.write((char *)(&v), 1);
      }
    }

    _nTotalFrames++;
    if (_nTotalFrames == 1) {
      INFO(" [!] First frame displayed at t = %g s ...\n", _timer.time());
    }

    _nLastFrames++;
    if (_nTotalFrames % 100 == 0) {
      INFO("     - Frames displayed = %d, FPS = %g\n", _nTotalFrames, _nLastFrames/_timer.time());
      _nLastFrames = 0;
      _timer.start();
    }
  }

  void OpenGLInitializator::redrawFrameCallback(int) {
    glutPostRedisplay();
    glutTimerFunc(_timePerFrame, redrawFrameCallbackStatic, 0);
  }

  void OpenGLInitializator::keyboardKey(unsigned char key, int x, int y) {
    //if (key == 27) { glutDestroyWindow(_windowID); }

    if (glutGetModifiers() == GLUT_ACTIVE_SHIFT) { _shiftDown = true; }
    else { _shiftDown = false; }

    _key[key] = true;
  }

  void OpenGLInitializator::keyboardKeyUp(unsigned char key, int x, int y) {
    _key[key] = false;
  }
}
