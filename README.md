# voxl
used to be a minecraft-style voxel rendering engine. The pipeline is still there and it
actually works but its just commented for now

Really, this is a light-indexed deferred rendering engine.
It uses the Assimp library to import scenes from just about any format
(but it seems to work best with collada).

how to build:
 This is a CMake project with all dependencies included in the repo. If
  you know CMake, then stop here and do the normal thing. Otherwise:
    mkdir build
    cd build
    cmake ..
    make -j4

features include:
  -easily extensible Input class for all of your inputting needs (used by camera and scenegraph)
  -FPSCamera like counterstrike. Operates in clip mode right now. Press 'v' to toggle wireframe rendering.
  -Scenegraph implementation with things like Pre-baking and Z-prepass
  -ShaderLib: my shader library which can resolve named shader programs if you use my little syntax additions to GLSL. It canonicalizes
    all uniform locations with a  constraint table so updating them at runtime now takes 1 function call instead of 2.
  -Light manager that supports point, directional, and cone lights
  -Material library for the phong shading model
  -Toggle-able rendering model. Press 'f' for forward shading. Press 'l'
    for light-indexed deferred

TODO:
  -physics engine
  -multithreading / performance optimization
  -more damn lights so you can actually tell a difference
