#include "inc/voxel.h"
#include "inc/cube.h"
#include <glm/glm.hpp>
using namespace glm;

// cube ///////////////////////////////////////////////////////////////////////
//    v6----- v5
//   /|      /|
//  v1------v0|
//  | |     | |
//  | |v7---|-|v4
//  |/      |/
//  v2------v3
//
//vec3 colors[]=    { vec3(1.0f,0.0f,0.0f),      //v0
//                    vec3(1.0f,0.0f,0.0f),
//                    vec3(1.0f,0.0f,0.0f),
//                    vec3(1.0f,0.0f,0.0f),
//                    vec3(1.0f,0.0f,0.0f),
//                    vec3(1.0f,0.0f,0.0f),
//                    vec3(1.0f,0.0f,0.0f),
//                    vec3(1.0f,0.0f,0.0f)};
//
//static vec3 vertices[]= { vec3(0.5,0.5,-0.5),      //v0
//                   vec3(-0.5, 0.5, -0.5),   //v1
//                   vec3(-0.5, -0.5, -0.5),  //v2
//                   vec3(0.5, -0.5, -0.5),   //v3
//                   vec3(0.5, -0.5, 0.5),    //v4
//                   vec3(0.5, 0.5, 0.5),     //v5
//                   vec3(-0.5, 0.5, 0.5),    //v6
//                   vec3(-0.5, -0.5, 0.5)};  //v7
//
//static GLuint indices[] = { 0,1,2, //front
//                     0,2,3,
//                     5,0,3, //right
//                     5,3,4,
//                     6,5,4, //back
//                     6,4,7,
//                     3,2,7, //bottom
//                     3,7,4,
//                     1,6,7, //left
//                     1,7,2,
//                     5,6,1, //top
//                     5,1,0};

Voxel::Voxel(glm::mat4 transform) : m_transform(transform), m_nverts(cube_num_vertices), m_position(&cube_vertices[0]), m_nindices(cube_num_indices), m_indices(&cube_indices[0])
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
