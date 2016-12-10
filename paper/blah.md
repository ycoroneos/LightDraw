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
