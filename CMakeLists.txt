cmake_minimum_required (VERSION 2.8)
project (asteroid-generator)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE RELEASE)
endif()

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-deprecated-declarations -Wall")

#
## Dependencies
find_package(Threads REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)
find_package(OpenMP)

if(OPENMP_FOUND)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif()

#
## Logging parameters
option(CG_OUTPUT "Use cg-output" ON)
if (CG_OUTPUT)
    add_definitions(-DCG_OUTPUT -DCOLOR_OUTPUT)
endif()

add_definitions(-DCG_COLOR_OUTPUT) # Colored log messages

add_subdirectory(src)
