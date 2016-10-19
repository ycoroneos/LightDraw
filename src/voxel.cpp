#include "inc/voxel.h"
#include "inc/cube.h"
#include <glm/glm.hpp>
using namespace glm;

Voxel::Voxel(glm::mat4 transform) : visible(true), m_transform(transform), m_nverts(cube_num_vertices), m_position(&cube_vertices[0]), m_nindices(cube_num_indices), m_indices(&cube_indices[0])
{
}

vec3 *Voxel::getVerts()
{
  return m_position;
}

GLuint Voxel::getnVerts()
{
  return m_nverts;
}

GLuint *Voxel::getIndices()
{
  return m_indices;
}

GLuint Voxel::getnIndices()
{
  return m_nindices;
}
mat4 Voxel::getTransform()
{
  return m_transform;
}

bool Voxel::getVisible()
{
  return visible;
}

void Voxel::setVisible()
{
  visible=true;
}

void Voxel::setInvisible()
{
  visible=false;
}
