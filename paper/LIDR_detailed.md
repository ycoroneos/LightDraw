#Light Indexed Deferred Rendering
Is the main point of this project. LIDR is a linear time rendering
algorithm for scenes with many lights and objects. It's possible to
reason about how long it takes to render a scene. Assume there exists a
scene with L lights, and M meshes. Also assume that the rendering
pipeline consists of a Z Pre Pass, shadowmapping, and, finally,
rendering the objects. The runtime breakdown is below in sequence of
operations.

Forward rendering: c1*O + c2*O*L + c3*O*L
G Buffer Deferred: c1*O + c2*O*L + c3*(O+L)
LIDR:              c1*O + c2*O*L + c3*(L+O)

Both G buffer deferred and LIDR shade all the objects in linear time but
LIDR can do it while consuming less memory and allowing more varied
material properties. With extremely large scenes that have many dynamic
shadows, both methods of deferred rendering are bottlenecked by
shadowmapping.

##Bit Packing and Render Buffer Size
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
