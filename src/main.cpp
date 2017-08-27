#include "cg_io.h"
#include "cg_opengl.h"
#include "cg_raytracer.h"

#include <cstdlib>

using namespace std;

int main (int argc, char** argv) {
  INFO("Usage: %s seed sizeX sizeY nFramesToBuffer BufferTimeInSec nRaysPerPixelX nRaysPerPixelY OutputRAW DisplaySizeX DisplaySizeY Mipmapping TimeStep FPS SoftShadows\n\n", argv[0]);

  int seed = 0;
  if (argc > 1) seed = atoi(argv[1]);
  srand(seed);

  CG_Float fps = 25;
  if (argc > 13) fps = atof(argv[13]);

  CG::Raytracer *raytracer = new CG::Raytracer();
  raytracer->initialize(argc, argv);

  CG::OpenGLInitializator *opengl = new CG::OpenGLInitializator();
  opengl->initDisplay(argc, argv, "Asteroid Generator");
  opengl->setTimePerFrame(1000.0/fps);
  opengl->setActive();

  raytracer->setOpenGL(opengl);
  raytracer->start();

  opengl->start();

  delete raytracer;
  delete opengl;

  return 0;
}


