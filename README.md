# LightDraw
A game engine based on the light-indexed deferred rendering technique
described by Damien Trebilco. Check out https://github.com/dtrebilco/lightindexed-deferredrender

This used to be a minecraft-style voxel rendering engine. The pipeline is still there and it
actually works but its just unused for now

##Feature List
  -Light Indexed Deferred Rendering with up to 4 lights per fragment and
256 total scene lights

  -Standard forward rendering for comparison pruposes

  -Per-light shadow mapping (and point light omnidirectional shadow
maps)

  -Easily extensible Input class for all of your inputting needs (used by camera and scenegraph)

  -FPSCamera like counterstrike. Operates in clip mode right now. Press 'v' to toggle wireframe rendering.

  -Scenegraph implementation with things like Pre-baking, Z-prepass, and
efficient transform propagation

  -Keyframe animation for any node in the scenegraph. Check out out the
animation class in inc/!

  -ShaderLib: my shader library which can resolve named shader programs if you use my little syntax additions to GLSL. It canonicalizes
    all uniform locations with a constraint table so updating them at runtime now takes 1 function call instead of 2.

  -Light manager that supports point and cone lights. Directional lights
are implemented but never used for the demo so I'm not sure they work
perfectly.

  -Material library for the phong shading model

  -Uses ASSIMP as a first-step importer so any assets ASSIMP can handle
can theoretically be loaded into LightDraw. Realistically, I have used
it with collada, fbx, and 3ds so I know those work.

  -Uses SOIL to load textures

  -Uses glm for GLSL-like vector math

  -Uses lodePNG to record videos

  -Uses GLFW for rendering context

  -Uses GLEW for managing extensions

##How to Use
My benchmark scenes of the Crytek Sponza Atrium could not fit on github
so you must download them here and put them in the root of
this directory tree.

##Build Instructions
This is a CMake project with all dependencies included in the repo. If
you know CMake, then stop here and do the normal thing. Otherwise:

mkdir build

cd build

cmake ..

make -j4


##Basic Idea of LIDR
Instead of storing objects and material properties in a G buffer, like
traditional deferred rendering, LIDR stores light properties and the
fragments they hit. All geometry is then rendered in a forward manner
and a "lightmap" texture is sampled to see which lights hit the current
fragment. Lights are identified by a number, which is its index in my light
array and also its index in a 1D texture containing all of the light
properties of the scene. Check out the paper for more info.

