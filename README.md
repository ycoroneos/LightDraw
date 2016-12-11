# LightDraw
[paper](https://github.com/ycoroneos/LightDraw/blob/condensed/paper/LIDR_detailed.md)
<img src="https://github.com/ycoroneos/LightDraw/blob/condensed/paper/shadows.png">

A game engine based on the light-indexed deferred rendering technique
described by Damien Trebilco. Check out https://github.com/dtrebilco/lightindexed-deferredrender

This project used to be a minecraft-style voxel rendering engine. The pipeline is still there and it
actually works but it's just unused for now. Check out inc/chunk.h if you
care about that.


##How to Use
My benchmark scenes of the Crytek Sponza Atrium could not fit on github
so you must download them [here](http://people.csail.mit.edu/spock/) and put them in the root of
this directory tree.

##Build Instructions
This is a CMake project with all dependencies included in the repo. If
you know CMake, then stop here and do the normal thing. Otherwise:

mkdir build

cd build

cmake ..

make -j4

##Feature List
  -Light Indexed Deferred Rendering with up to 4 lights per fragment and
256 total scene lights

  -Standard forward rendering for comparison purposes

  -Per-light shadow mapping (and point light omnidirectional shadow
maps)

  -Easily extensible Input class for all of your inputting needs (used by camera and scenegraph)

  -FPS Camera like counterstrike. Operates in clip mode right now. Press 'v' to toggle wireframe rendering.

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

  -Uses lodePNG to record videos and screenshots

  -Uses GLFW for rendering context

  -Uses GLEW for managing extensions

  -Can load voxel objects out of the binvox format


##Basic Idea of LIDR
Instead of storing objects and material properties in a G buffer, like
traditional deferred rendering, LIDR stores light properties and the
fragments they hit. All geometry is then rendered in a forward manner
and a "lightmap" texture is sampled to see which lights hit the current
fragment. Lights are identified by a number, which is its index in my light
array and also its index in a 1D texture containing all of the light
properties of the scene. The rendering steps of my implementation are:

1. Z Pre-Pass the entire scene into a depth FBO

2. (Optional) Render shadow maps for each light

3. Construct the Light Index Map by rendering and accumulating light
   volumes into a full screen quad. All light volumes are implicitly
defined as a sphere or a cone. If shadows are desired, apply
shadowmapping here by discarding fragments that are in the light's
shadow.

4. Pack light properties into 1D textures so that the light index can be
   used to sample the texture and retrieve the light properties

5. Draw scene in a forward manner. Make the light properties texture as
   well as the Light Index Map available to the shader in order to
extract which lights hit the current fragment.

Check out the paper for more detailed info.

##Performance and Graphs
My Sponza benchmark is basically the Crytek modified Sponza with 582
triangle meshes and varying amounts of lights. This is a stress test so
every single light is assumed to be dynamic and all light volumes and
shadowmaps are re-calculated on every frame.
This is why the performance collapses so
dramatically with shadows enabled.


I ran my benchmark scene on this computer:

CPU: intel 5820k

RAM: 64GB 2666MHz

GPU: nvidia gtx 980 ti

OpenGL version string: 4.5.0 NVIDIA 367.57

Linux Kernel: 4.4.0-53-generic

![alt text](https://github.com/ycoroneos/LightDraw/blob/condensed/paper/1024x768.png)
![alt text](https://github.com/ycoroneos/LightDraw/blob/condensed/paper/1920x1080.png)
![alt text](https://github.com/ycoroneos/LightDraw/blob/condensed/paper/3840x2160.png)
