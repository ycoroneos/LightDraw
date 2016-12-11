#Light Indexed Deferred Rendering
Is the main point of this project. LIDR is a linear time rendering
algorithm for scenes with many lights and objects. We can reason about
LIDR's performance by considering the runtime of a typical scene. Assume
there exists a scene with L lights, and M meshes. Also assume that the rendering
pipeline consists of a Z Pre Pass, shadowmapping, and, finally,
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
shadowmapping.

##Overview of the Algorithm
LIDR stores light properties, and the fragments they hit, in textures.
Later drawing passes can simply do a texture lookup to see which lights
shade a given fragment.

###Z Pre Pass
|Source             | Function  |
|-------------------|:---------:|
|src/lidr.cpp       |ZPrePass() |
|src/scenegraph.cpp |zPreBaked()|
|shaders/depth.vert |           |
|shaders/depth.frag |           |

Render the scene from the camera's point of view into an FBO which is
the same size as the window. I used a 32bit depth buffer for precision
but 24bits can probably work too.

<img src="https://github.com/ycoroneos/LightDraw/blob/condensed/paper/depth.png">

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
every light. For spotlights, construct a view matrix from where it's
located and where it's looking at. For point lights, construct 6 view
matrices to represent each side of the cube that contains the it. In
order to avoid submitting 6 draw calls, the FBO for the point light
shadowmap can be a cubemap and each side can be rendered to with the
help of a geometry shader.


###Light Map Generation
Determine which lights hit each fragment in the camera's view and write
this into a new FBO. Lights are identified by their index in a list and
index 0 means no light hit. Each light is represented by an implicit
volume. Spot lights are represented as cones, which have a radius and
cone angle. In order for a point to be inside a cone volume, it must be
inside the radius and also within the cone's light angle. Point lights
are just a special case of a cone with an angle of 2\*PI.
<img src="https://github.com/ycoroneos/LightDraw/blob/condensed/paper/lightvolumes.png">

In order to generate the Light Map, light volumes have to be rendered
into it. Since the light volumes are implicit, no real geometry must be
dispatched to the gpu in order to render the light volumes. Instead a
full-screen quad is rendered and, for every single fragment on the quad,
its world-space position is recovered from multiplying the NDC
coordinated with the camera's inverse ProjectionView matrix. World-space
depth is recovered by simply looking it up from the depth buffer that
was generated earlier. This is a complicated step, so I will walk
through the code below:

Reconstruct scenepos in clip space:
````
  //xyz scenepos in clip space
  //clip space goes from [0,1]
  //var_texcoords are (x,y) position of fragment on the full screen quad
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
coordinates so it becomes the same thing in the fragment shader.

###Bit Packing and Render Buffer Size
Since we are storing light properties for later use, we cannot have
infinite lights. The constraints are set by the size of the render
buffer. I am using a single render buffer of type RGBA_32 which means I
can store 32bits of data per fragment. Partitioning these bits is sort
of arbitrary, but I chose to store 4 lights per fragment. This limits me
to 2^8 = 256 total active lights in the scene.

LIDR has numerous advantages compared to G buffer deferred rendering.
There are typically less lights in a scene than objects. Also, each
light usually has less shading parameters than an arbitrary scene
object. Because of this, the amount of video memory used to store light
properties in LIDR is much less than the amount the G buffer consumes.

Since all objects are drawn sequentially in a forward pass, transparency
and varied material properties are easy to implement. Transparency is
done exactly the same way as in a standard forward renderer: Z-PrePass
all opaque objects
