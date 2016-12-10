#Design and Implementation of the LightDraw Game Engine
Besides showcasing the Light Indexed Deferred Rendering algorithm,
LightDraw is a fairly complete game engine. Artists can model scenes in
Blender and LightDraw will directly import them. Textures,
keyframe animations, light properties, and the entire scene graph will
be imported. Really, all that's missing is a game.

##Scenegraph
The scenegraph is the primary data structure of the game engine. It
consists of a list of every node in the scene. Nodes are recursive data
structures that contain transformations relative to their parents. A
node can have lights, meshes, and cameras associated with it. In this
way, whenever a node is transformed during an animation, the objects attached to it can
reflect the transformation too.

Recomputing the world transforms of the scene graph in every single
frame is too expensive for real time rendering. The Crytek Sponza demo
scene consists of 582 triangle meshes. Each of these occupy a node in
the scene graph and recalculating their world matrices every frame would
incur the cost of 582 matrix multiplies on the CPU. Instead, the
scenegraph is baked on startup and whenever the transform of a node is
modifed, only it's children are re-baked. In this way, animation only
costs as much the number of objects that have moved.

##Meshes
Each mesh in LightDraw can have its own unique lighting model and
material properties. The unique lighting model is achieved by letting
each mesh have its own shader to draw with. Material properties are
handled by another data structure, the Material class. Mesh data
consists of vertex positions, texture coordinates, normals, and indices.
The vertex data is stored in an interleaved buffer on the GPU during
mesh instantiation.

##Materials
Materials in LightDraw are managed by the Material class. It is in
charge of binding textures and uploading material parameters to the
shader before the draw call. Naturally, each mesh contains a pointer to
a material object. Material loads textures and stores them in video
memory upon instantiation.

##Lights
Lights are extremely important because they are the objects of interest
in LIDR. LightDraw supports point lights, spot lights, and directional
lights. Each specific type of light inherits from an abstract base class
of Light. Each light is responsible for generating its own shadow maps
and uploading properties to the shaders. Each light has an ambient
color, diffuse color, specular color, position, direction, cone angle,
nd radius. Notice how this set of properties may seem specific to a
spotlight, but with some clever packing, these properties can represent
every light.

First of all, position and radius can be packed into a single vec4. This
is done.

For point lights the cone angle is set to 2\*PI. This essentially
removes the direction from the math calculations, so it left to be
garbage.

For directional lights, the radius is set to -1. When the shader sees
this, it knows that the light is a directional light so the position and
cone angle is ignored.

##Cameras
There are two kinds of cameras supported in LightDraw: FPS-like cameras
and animated, keyframed cameras. You can guess which one I used for the
benchmark.. Each camera is responsible for its own projection and view
matrices. It is also responsible for uploading these parameters to the
shaders.

##Keyframe Animations
LightDraw supports keyframe animations on any set of nodes in its
scenegraph. This is especially powerful because it allows for things
animated cutscenes and linear blend skinning. Each keyframe is a
snapshot of a node's local transform. The procedure for animation
consists of linearly interpolating the current keyframe, applying it to
the target node, and then recursively baking all children of that node.
Once again, it is the recursive nature of the Node structure that makes
this so easy.

##Keyboard and Mouse Input
There is an abstract base class called Input which can register derived
objects in the keyboard and mouse trap handler. Each derived instance of the
Input class (currently they are: Camera and Scenegraph) must override a
virtual function to receive and handle input. Each derived class can
also optionally mask its input functionality if desired. This is useful
for when the player-controlled object changes.

##ShaderLib
There is a database for loading and managing shaders. LightDraw
currently uses 17 different shaders for different purposes. GLSL has no
langauge support for naming a shader, so I added a small hack to enable
this. At the top of every vertex shader is a line like
````
//name=lidr
````
ShaderLib sees this and associates the shader with that name. That is
not the only thing ShaderLib does though! It also compiles the shaders
and prints out compiler errors when they don't compile. Shaderlib also
canonicalizes texture and uniform binding locations.

Updating a uniform in OpenGL is usually done by a two-part process:
````
glGetUniformLocation();
glUniform3fv(); //or others
````
This needs to happen because the programmer has no clue where OpenGL
bound the uniforms. Furthermore, glGetUniformLocation() is extremely
expensive when it's called millions of times per frame:

<img src="https://github.com/ycoroneos/LightDraw/blob/condensed/paper/call_stats.png">

OpenGL 4.3 and up introduced a feature to GLSL that allows programmers
to explicitly assign a binding location to uniforms and samplers in the
shader. Using this feature, all calls to glGetUniformLocation() can be avoided.
Unfortuneately, not all vendors support this functionality yet so I've
done the second-best thing. ShaderLib explicitly sets texture binding
locations upon loading a shader. ShaderLib can't set all uniform binding
locations because OpenGL doesn't support that but it can reduce the
overhead associated with binding textures. Now the programmer can
directly bind a texture to the correct binding spot.

##Assimp
In order to get any external object data into LightDraw, Assimp is used
because it presents a uniform interface for reading game assets in
almost any format! Instead of writing a seperate OBJ, collada, md5, 3ds,
and fbx importer I just wrote a single importer for the Assimp format!
This limits LightDraw's import capabilities to Assimp's but that hasn't
been a problem yet.
