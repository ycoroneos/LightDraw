#Light Indexed Deferred Rendering
Is the main point of this project. LIDR is a linear time rendering
algorithm for scenes with many lights and objects. We can reason about
LIDR's performance by considering the runtime of a typical scene. Assume
there exists a scene with L lights, and M meshes. Also assume that the rendering
pipeline consists of a Z Pre Pass, shadow mapping, and, finally,
rendering the objects. The runtime breakdown is below in sequence of
operations.


| Method              |      Z-Pre    |      Shadowing|      Shading  |
|---------------------|:-------------:|:-------------:|:-------------:|
| Forward Rendering   |  O(M)         |  O(M\*L)      |  O(M\*L)      |
| G Buffer Deferred   |  O(M)         |  O(M\*L)      |  O(M+L)       |
| LIDR                |  O(M)         |  O(M\*L)      |  O(L+M)       |


Both G buffer deferred and LIDR shade all the objects in linear time but
LIDR can do it while consuming less memory and allowing more varied
material properties. With extremely large scenes that have many dynamic
shadows, both methods of deferred rendering are bottlenecked by
shadow mapping.


##Overview of the Algorithm
LIDR stores light properties, and the fragments they hit, in textures.
Later drawing passes can simply do a texture lookup to see which lights
shade a given fragment.


###Z Pre Pass
|Source             | Function  |
|-------------------|:---------:|
|src/lidr.cpp       |ZPrePass() |
|src/scenegraph.cpp |zPreBaked()|
|src/mesh.cpp       |quickdraw()|
|shaders/depth.vert |           |
|shaders/depth.frag |           |


Render the scene from the camera's point of view into an FBO which is
the same size as the window. I used a 32bit depth buffer for precision
but 24bits can probably work too.


<img src="https://github.com/ycoroneos/LightDraw/blob/master/paper/depth.png">


###Shadow Map Generation
|Source                         | Function        |
|-------------------------------|:---------------:|
|src/light.cpp                  |shadowMap()      |
|src/scenegraph.cpp             |drawShadowMaps() |
|shaders/spotlight\_shadow.vert |                 |
|shaders/spotlight\_shadow.frag |                 |
|shaders/point\_shadow.vert     |                 |
|shaders/point\_shadow.geom     |                 |
|shaders/point\_shadow.frag     |                 |


Render the scene from the light's point of view but only store the
depth. This is exactly the same as the Z Pre Pass, but it is done for
every light. For spotlights, construct a view matrix from where the
light is located and where it's looking at. For point lights, construct 6 view
matrices to represent each side of the cube that contains the light. In
order to avoid submitting 6 draw calls, the FBO for the point light
shadow map can be a cubemap and each side can be rendered to in a single
draw call with the help of a geometry shader. Shadow map resolution is
1024x1024 and there is no PCF so all shadows have hard edges.


There are two ways that shadow maps can be combined with LIDR. The first
way is deferring the shadow mapping algorithm until the final forward
pass. This approach allows for an easy implementation of soft shadows
because the final color of a fragment in shadow can multiplied by a
constant. The disadvantage of this method is that it can lead to an
artificially lower light count per fragment. Consider the scenario where a
fragment can be hit by 5 lights but one of them is completely blocked by
geometry. LIDR will evict one of the lights from the fragment, possibly
a light that actually affects the fragment. Then, in the final forward
pass, the fragment will actually only be lit by 3 lights instead of 4.


The alternative method of incorporating shadow maps that LightDraw uses
is to consider them in the light volume calculation. This way, if a certain
fragment is in shadow, the occluded light is not even considered to
affect it.
Returning to the previous example, the occluded light will not be
included in the list of lights affecting the fragment. This method
guarantees that a fragment is visually affected by the maximum amount of
lights. The downside to this method is that all shadows from individual
lights are now hard. Soft shadows can still be achieved in the case
where multiple shadowing lights hit a fragment.




###Light Map Generation
|Source                         | Function        |
|-------------------------------|:---------------:|
|src/lidr.cpp                  |lightVolumes()      |
|src/lidr.cpp                  |lightVolumesEnd()      |
|src/scenegraph.cpp             |drawLightVolumes()|
|src/camera.cpp             |getProjectionViewInverse()|
|shaders/lidr\_volume.vert |                 |
|shaders/lidr\_volume.frag |                 |




Determine which lights hit each fragment in the camera's view and write
this into a new FBO. Lights are identified by their index in a list and
index 0 means no light hit. Each light is represented by an implicit
volume. Spot lights are represented as cones, which have a radius and
cone angle. In order for a point to be inside a cone volume, it must be
inside the radius and also within the cone's light angle. Point lights
are just a special case of a cone with an angle of 2\*PI.


Light map on the bottom right:
![alt text](https://github.com/ycoroneos/LightDraw/blob/master/paper/lightvolumes.png)


In order to generate the Light Map, light volumes have to be rendered
into it. Since the light volumes are implicit, no real geometry must be
dispatched to the gpu in order to render the light volumes. Instead a
full-screen quad is rendered and, for every single fragment on the quad,
its world-space position is recovered from multiplying the clip space
coordinate with the camera's inverse ProjectionView matrix. Clip space depth
is recovered by looking it up from the depth buffer generated earlier.
This is a complicated step, so I will walk through the code below:


Reconstruct scenepos in clip space:
````
  //xyz scenepos in clip space
  //clip space goes from [-1,1] but NDC depth goes from [0,1] and
  //var_texcoords are (x,y) position of fragment on the full screen quad
  //var_texcoords go from [0,1]
  float scenedepth = texture(depthmap, var_texcoords).r * 2.0f - 1.0f;
  vec4 scenepos_clip = vec4((var_texcoords*2.0f) - 1.0f, scenedepth, 1.0f);
````


Transform from clip space to world space:
````
  //PV_inverse is from camera
  vec4 scenepos_world = PV_inverse * scenepos_clip;
  vec3 scenepos_world_fixed = scenepos_world.xyz/scenepos_world.w;
````


There is one more shortcut I took. Instead of rendering a full-screen
quad, which is 6 vertices the gpu must draw, I rendered an oversized
triangle. The gpu clips the triangle and interpolates the fragment
coordinates so it effectively becomes a quad in the fragment shader.
It is decribed in more detail [here](https://rauwendaal.net/2014/06/14/rendering-a-screen-covering-triangle-in-opengl/)


###Bit Packing and Render Buffer Size
|Source                         | Function        |
|-------------------------------|:---------------:|
|src/lidr.cpp                  |lightVolumes()      |
|src/lidr.cpp                  |lightVolumesEnd()      |
|src/scenegraph.cpp             |drawLightVolumes()|
|src/camera.cpp             |getProjectionViewInverse()|
|shaders/lidr\_volume.vert |                 |
|shaders/lidr\_volume.frag |                 |




Since we are storing light properties for later use, we cannot have
infinite lights. The constraints are set by the size of the render
buffer. I am using a single render buffer of type RGBA_32 which means it
can store 32bits of data per fragment. Partitioning these bits is sort
of arbitrary, but I chose to store 4 lights per fragment. This limits me
to 2^(32/4) = 256 total active lights in the scene. The light volume
shader takes a light index as input, and it outputs it for every
fragment that is inside the light's volume. The goal is to have
sequential runs of the light volume shader simply shift down
the bits of the previous output, packing the bits. This is illustrated
below for a series of fake lights.


Initial state of the framebuffer for a single fragment:


| Color Channel | 0x3<<6 | 0x3<<4 | 0x3<<2 | 0x3 |
|---------------|--------|--------|--------|-----|
| R             | 0      | 0      |    0   |   0 |
| G             | 0      | 0      |    0   |   0 |
| B             | 0      | 0      |    0   |   0 |
| A             | 0      | 0      | 0      | 0   |




After fragment is hit by light 1:


| Color Channel | 0x3<<6 | 0x3<<4 | 0x3<<2 | 0x3 |
|---------------|--------|--------|--------|-----|
| R             | 1      | 0      |    0   |   0 |
| G             | 0      | 0      |    0   |   0 |
| B             | 0      | 0      |    0   |   0 |
| A             | 0      | 0      | 0      | 0   |


After fragment is hit by light 200:


| Color Channel | 0x3<<6 | 0x3<<4 | 0x3<<2 | 0x3 |
|---------------|--------|--------|--------|-----|
| R             | 0      | 1      |    0   |   0 |
| G             | 2      | 0      |    0   |   0 |
| B             | 0      | 0      |    0   |   0 |
| A             | 3      | 0      | 0      | 0   |


After fragment is hit by light 75:


| Color Channel | 0x3<<6 | 0x3<<4 | 0x3<<2 | 0x3 |
|---------------|--------|--------|--------|-----|
| R             | 3      | 0      |    1   |   0 |
| G             | 2      | 2      |    0   |   0 |
| B             | 0      | 0      |    0   |   0 |
| A             | 1      | 3      | 0      | 0   |


In order to make the framebuffer obey these rules for bit packing, the
GPU's floating point unit must be abused in order to enable bit
manipulations. First, the light index must be normalized against the
total number of lights. This is because the fragment shader clips its
output value to [0,1]. The light index to be sent to the shader is then
constructed as follows:




````
    //Pack the RGBA channels of the color
    vec4 index = vec4((index&0x3) << 6, (index&0xC) << 4, (index&0x30) << 2, (index&0xC0) << 0)/255.0f;
    glUniform4fv(lightindex_loc, 1, &index[0]);
````


In order to make the bit shifting work, blending is enabled in
CONSTANT_COLOR mode with a value of (0.25f, 0.25f, 0.25f, 0.25f).
This blend mode produces:


````
framebuffer = new_framebuffer + 0.25*framebuffer
````


This is why light indices will be evicted out of the light map if there
are more than four lights hitting a single fragment.


The total amount of space consumed by the lightmap is
x\_pixels\*y\_pixels*4 bytes. For a resolution of 3840x2160 this is 33.18MB


###Light Property Packing
|Source                         | Function        |
|-------------------------------|:---------------:|
|src/lidr.cpp                  |packLightTextures()      |
The lightmap identifies the indices of the lights hitting a given
fragment but, in order to actually shade the fragment, light properties must be
fetched from a different table. LIDR uses 1D texture maps for the
property tables because texture lookups are very fast in the gpu.
Assembling these property tables is very straightforward and I used 4 1D
textures to store: ambient, diffuse, specular, position+radius. Position
and radius can be packed into a single vec4.
  ````
  glActiveTexture(GL_TEXTURE3);
  glBindTexture(GL_TEXTURE_1D, light_ambient_tex);
  glTexSubImage1D(GL_TEXTURE_1D, 0, 0, nlights+1, GL_RGB, GL_FLOAT, &light_ambient[0]);


  glActiveTexture(GL_TEXTURE4);
  glBindTexture(GL_TEXTURE_1D, light_diffuse_tex);
  glTexSubImage1D(GL_TEXTURE_1D, 0, 0, nlights+1, GL_RGB, GL_FLOAT, &light_diffuse[0]);


  glActiveTexture(GL_TEXTURE5);
  glBindTexture(GL_TEXTURE_1D, light_specular_tex);
  glTexSubImage1D(GL_TEXTURE_1D, 0, 0, nlights+1, GL_RGB, GL_FLOAT, &light_specular[0]);


  glActiveTexture(GL_TEXTURE6);
  glBindTexture(GL_TEXTURE_1D, light_position_tex);
  glTexSubImage1D(GL_TEXTURE_1D, 0, 0, nlights+1, GL_RGBA, GL_FLOAT, &light_pos_radius[0]);
````


The total amount of space used by all of these textures is 13.36kB


###Bit Unpacking and Shading in the Forward Pass
|Source                         | Function        |
|-------------------------------|:---------------:|
|src/scenegraph.cpp                   |drawBaked()      |
|src/mesh.cpp                         |draw()      |
|shaders/lidr.vert | |
|shaders/lidr_light.frag | |
Now that the light map has been generated and the light properties have
been packed, all the objects in the scene can be drawn. There is NO
restriction at all on the material properties of each object and, in
fact, each object can be drawn using a substantially different shader.
The only thing required for lighting is that the shader unpack light
index values from the lightmap and use them to index the light
properties table. The declarations in the shader look like:


````
//lidr supplied
uniform sampler2D lightindex_tex;     //2
uniform sampler1D lightambient_tex;   //3
uniform sampler1D lightdiffuse_tex;   //4
uniform sampler1D lightspecular_tex;  //5
uniform sampler1D lightposition_tex;  //6
````


Unpacking the light map is the opposite set of
operations that was used to pack it. Once again, the GPU doesn't support
bit operations so they must be faked with floating-point operations.


````
//Bit unpacking in the wild
#define NUM_LIGHTS 256.0
highp vec4 unpacklights(vec4 packedLight)
{
  //assemble the weights of different bits in the packed index
  highp vec4 unpackConst = vec4(4.0, 16.0, 64.0 , 256.0) / NUM_LIGHTS;


  //undo the scaling from [0,1]
  highp vec4 floorValues = ceil(packedLight * 254.5);
  highp vec4 lightIndex;
  for (int i=0; i<4; i++)
  {
    //shift out 1 column
    packedLight = floorValues * 0.25;


    //round down to integers
    floorValues = floor(packedLight);


    //extract what was shifted out
    highp vec4 fracpart = packedLight - floorValues;


    //re-assemble into the 8-bit light index but then divide by 256 in
    //order to scale for the texture lookup
    lightIndex[i]=dot(fracpart, unpackConst);
  }
  return lightIndex;
}
````


Each fragment can have 4 light indices so the final color output is the
sum of each light contribution. The index of each light is used to
sample the set of 1D property textures for the lights. Here it is with
the Blinn-Phong shading model.


````
        vec4 lightPos_att = texture(lightposition_tex, index).xyzw;
        vec3 lightPos = lightPos_att.xyz;
        float att = lightPos_att.w;
        vec3 L = normalize(lightPos.xyz - pos_world.xyz);
        vec3 V = normalize(camPos - pos_world.xyz);
        vec3 N = normalize(var_Normal);


        vec3 lightAmbient = texture(lightambient_tex, index).xyz;
        vec3 Iamb = ambient(lightAmbient);


        vec3 lightDiffuse = texture(lightdiffuse_tex, index).xyz;
        vec3 Idif = diffuse(N, L, lightDiffuse);


        vec3 lightSpecular = texture(lightspecular_tex, index).xyz;
        vec3 Ispe = specular(N, L, V, lightSpecular);


        float distance = 1.0f / length(lightPos - pos_world.xyz);
        out_Color.xyz += (Iamb * (Idif + Ispe)) * diffuseColor * distance * distance * att;
````


##Evaluation and Conclusion
Graphs are shown [here](https://github.com/ycoroneos/LightDraw/blob/master/README.md) at the bottom.
The benchmark scene was made by animating a camera on a predetermined
path. Every run of the benchmark consisted of the same fly-through and
animations so the computational variance is low. There are between 5 and
192 fully-dynamic, shadow-casting lights. This scene represents a
nightmare scenario for a videogame. The light count stops at 192 because
the performance was abysmal at the important resolutions of 1920x1080
and 3840x2160.


At resolutions below 3840x2160, LIDR without shadows can easily maintain
a smooth frame rate no matter how many lights. There are two interesting
observations that result from the data though:


1. Performance of LIDR collapses with shadow maps


2. Performance of LIDR collapses at high resolutions with many lights


The first observation can be explained by remembering the runtime of the
shadow mapping algorithm: O(M\*L). On a large scene, with many shadow
casting lights, there is almost no hope. A small trick you can play is
to only update shadow maps for lights that have moved. This is merely an
improvement by a constant factor though. Realistically, great care must
be taken to minimize the amount of shadow casting lights. By plotting
the log of the runtimes of LIDR with shadows and forward rendering with
shadows, the bottleneck becomes apparent: On scenes with many lights and
objects, their computational difficulties are within an order of magnitude.




<img src="https://github.com/ycoroneos/LightDraw/blob/master/paper/runtimes.png">


The performance collapse of LIDR with many lights is a more interesting
problem. First, the benchmark was rerun with V-Sync disabled in order
to see if the collapse is linear. It's not:


![alt text](https://github.com/ycoroneos/LightDraw/blob/master/paper/1024x768_novsync.png)
![alt text](https://github.com/ycoroneos/LightDraw/blob/master/paper/1920x1080_novsync.png)
![alt text](https://github.com/ycoroneos/LightDraw/blob/master/paper/3840x2160_novsync.png)


In order to characterize this, timers were added into the main rendering
loop to see where most of the time was spent. Impressively, most of the
time was spent by OpenGL swapping the front and back buffers. This is
not a good enough answer though, because the OpenGL driver tries to be
intelligent about when it submits commands to the GPU, so it may be
deferring the critical computations. In order to
bypass this, calls to glFlush() were added after every step in the LIDR
algorithm and time was measured again. Specifically, calls to glFlush() were added after:


-Animation


-Z Pre-Pass


-Shadow maps calculation


-Light volume map calculation


-Light property texture packing


-Forward rendering of scene objects


This test was run on resolutions of 1024x768 and 3840x2160


Here are the results for 1024x768, 196 lights:


|            | frame time (sec) | swap time (sec) | swap time % total |
|------------|------------------|-----------------|-------------------|
| no flush   | .00238           | .005919         | 71.32%            |
| with flush | .003617          | .000993         | 21.54%            |
|            |                  |                 |                   |


Here are the results for 3840x2160, 196 lights


|            | frame time (sec) | swap time (sec) | swap time % total |
|------------|------------------|-----------------|-------------------|
| no flush   | .002471          | .047278         | 95.032%           |
| with flush | .002389          | .03705          | 93.94%            |
|            |                  |                 |                   |


At the smaller resolution, flush has the desired effect of making the
draw time of the scene longer than the buffer swap time. This indicates
that the OpenGL driver does indeed defer computation until later.


At the larger resolution of 3840x2160, flush reduces the time it takes
to swap the buffers but buffer swapping still eats up most of the time.
Now, I will go ahead and shift blame to the OpenGL driver because that's
where the bottleneck is. It seems that the driver cannot efficiently
optimize command dispatch for the LightDraw game engine that I made. In
order to further increase performance, I will have to supply more driver
hints while rendering, switch to a new rendering algorithm, or abandon
OpenGL altogether and try out a different API like Vulkan.


In conclusion, LIDR is an effective algorithm for rendering lit scenes.
It dwarves forward rendering in performance when there are no shadows
(or few shadows) but, because the shadow mapping algorithm is polynomial
time, LIDR performance collapses with many shadows in the scene. An
extremely useful comparison that I didn't do would have been to compare
LIDR performance with traditional G buffer-based deferred rendering and with newer
algorithms such as tile-based deferred rendering. This goal was
unreasonable for this project because there was not enough time to
implement two more complicated renderers.


##Compared to G-Buffer Based Deferred Rendering
The G buffer typically stores all the material properties of the objects
in the scene, as well as their surface normals and depths.


At a resolution of 3840x2160:




G buffer stores depth, ambient, diffuse, specular, surface normal


|       | depth | ambient | diffuse | specular | surface normal | total   |
|-------|-------|---------|---------|----------|----------------|---------|
| bytes | 4     | 12      | 12      |    12    |             12 | 431.3MB |




LIDR stores depth, light index, ambient, diffuse, specular, position, radius.
Remember that the light properties are stored per-light, and not
per-pixel.


|       | depth | light index | ambient | diffuse | specular | position | radius | total   |
|-------|-------|-------------|---------|---------|----------|----------|--------|---------|
| bytes | 4     | 4           | 12      |    12   |       12 | 12       | 4      | 66.37MB |


LIDR also easily allows for transparent materials, even though I did not
demonstrate this ability in my demo. The reason for this is because all
of the objects in the scene are drawn during a forward pass and each
object's final color is determined in 1 pass. In a scene with
transparent objects: first render all opaque objects, then turn on
blending and render the transparent objects from back to front.


In G buffer deferred rendering, an object's final color is the
composition of its components in the G buffer. By the time an object is
determined to be transparent in the final pass, the information of what objects live
behind it has already been lost.



