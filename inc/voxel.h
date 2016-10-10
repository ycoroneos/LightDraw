#pragma once
#include <vector>
#include <glm/glm.hpp>
#include <inc/gl.h>
using namespace glm;

class Voxel
{
  public:
    Voxel(mat4 transform);
  private:
    mat4 m_transform;

    GLuint m_nverts;
    glm::vec3 *m_position;


    GLuint m_nindices;
    GLuint *m_indices;
};
