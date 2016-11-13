#pragma once
#include <vector>
#include <glm/glm.hpp>
#include <inc/gl.h>
using namespace glm;

//it will be slow to have getters/setters for the vertex but it makes for an easier (and pure) implementation

class Vertex
{
  public:
    Vertex(vec3 pos_1, vec3 normal_1, vec3 texcoords_1);
  private:
    vec3 pos;
    vec3 normal;
    vec2 texcoords;
};
