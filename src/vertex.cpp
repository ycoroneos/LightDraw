#include <inc/vertex.h>
#include <glm/glm.hpp>
#include <inc/gl.h>
using namespace glm;

Vertex::Vertex(vec3 pos_1, vec3 normal_1, vec2 texcoords_1) : pos(pos_1), normal(normal_1), texcoords(texcoords_1)
{
}
