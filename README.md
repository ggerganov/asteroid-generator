[![Build Status](https://travis-ci.org/ggerganov/asteroid-generator.svg?branch=master)](https://travis-ci.org/ggerganov/asteroid-generator?branch=master)

# asteroid-generator
The demo generates and renders asteroids floating in space. The shape and the texture of the generated asteroids are procedurally generated. The space background is procedurally generated as well.

<a href="http://www.youtube.com/watch?feature=player_embedded&v=bGKxsXbf358" target="_blank"><img src="http://img.youtube.com/vi/bGKxsXbf358/0.jpg" alt="CG++ Data over sound" width="360" height="270" border="10" /> </a>

## Features

- Procedurally generated space background with Perlin noise
- Procedurally generated asteroid shape and texture with Perlin noise
- Raytracing performed 100% on the CPU using modified “Relaxed Cone Stepping” technique for spherical objects.
- Hard and Soft (approximation) shadows
- Terrain and texture mipmapping
- Frame buffering for stable visualization

## Building & running

    git clone https://github.com/ggerganov/asteroid-generator
    cd asteroid-generator
    make
    ./bin/asteroid-generator
    
### Examples:

Change the **RNG seed** to get different asteroids/background

    ./bin/asteroid-generator 1234
    
Change the resolution to **400x400**:

    ./bin/asteroid-generator 1234 400 400 0 0.0 1 1 0 -1 -1 1 0.06 24 1
    
Increase the frame buffer size to **100** frames. Use this in case the frames are rendered faster than the specified Frame Rate. This way the newly generated frames will be buffered in the frame buffer, so they can be displayed later:

    ./bin/asteroid-generator 1234 800 600 100 0.0 1 1 0 -1 -1 1 0.06 24 1

Add initial buffering time. In the beginning of the execution, the rendered frames will be buffered for **10.0 seconds** before they start being displayed to the user:
    
    ./bin/asteroid-generator 1234 800 600 100 10.0 1 1 0 -1 -1 1 0.06 24 1
    
Reduce antialiasing artifacts by increasing the number of rays per pixel to **2x2 = 4**. This increases significantly the computation time.
    
    ./bin/asteroid-generator 1234 800 600 100 0.0 2 2 0 -1 -1 1 0.06 24 1
    
Increase the frame rate to **30 FPS**:
    
    ./bin/asteroid-generator 1234 800 600 100 0.0 1 1 0 -1 -1 1 0.06 30 1
    
Speed up the animation by a factor of 2 (i.e. increase time step to **0.12** from 0.06):
    
    ./bin/asteroid-generator 1234 800 600 100 0.0 1 1 0 -1 -1 1 0.12 24 1
    
## Dependencies

- OpenGL
- GLUT
- OpenMP
    
## Notes

The entire computation in the demo is performed on the CPU, therefore it is good to have more cores. I tried running the demo with default parameters on a 16 core machine Intel Xeon E5-2640 @ 2.00 GHz and it runs in real time. In case it does not run in real time on your machine, try increasing the frame buffer size and buffering time from the command line arguments. Another option is reducing the resolution. OpenGL is used only to open a window so I can draw my frames with glDrawPixels to it. No other OpenGL functionality is used.

During execution the shape of the asteroid changes periodically. Every time the camera is far from the surface of the asteroid, the shape of the asteroid is re-generated.
