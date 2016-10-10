#include "inc/chunk.h"
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

Chunk::Chunk()
{
  voxels.clear();
  changed=true;
  glGenVertexArrays(1, &vertexarray);
  glGenBuffers(4, vertexbuffer);
  glGenBuffers(1, &indexbuffer);

  Voxel dummy = Voxel(mat4());
  glBindVertexArray(vertexarray);

  //upload indices
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexbuffer);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, dummy.getnIndices() * sizeof(GLuint), dummy.getIndices(), GL_STATIC_DRAW);

  // POSITION
  glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer[0]);
  size_t position_nbytes = dummy.getnVerts() * sizeof(glm::vec3);
  glBufferData(GL_ARRAY_BUFFER, position_nbytes, glm::value_ptr(dummy.getVerts()[0]), GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0,
      3,
      GL_FLOAT,
      GL_FALSE,
      sizeof(glm::vec3),
      (void*)0);

  // NORMALS
  glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer[1]);
  size_t normal_nbytes = dummy.getnVerts() * sizeof(glm::vec3);
  glBufferData(GL_ARRAY_BUFFER, normal_nbytes, glm::value_ptr(dummy.getVerts()[0]), GL_STATIC_DRAW);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1,
      3,
      GL_FLOAT,
      GL_FALSE,
      sizeof(glm::vec3),
      (void*)0);

  // Color
  glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer[2]);
  size_t color_nbytes = dummy.getnVerts() * sizeof(glm::vec3);
  glBufferData(GL_ARRAY_BUFFER, normal_nbytes, glm::value_ptr(dummy.getVerts()[0]), GL_STATIC_DRAW);
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2,
      3,
      GL_FLOAT,
      GL_FALSE,
      sizeof(glm::vec3),
      (void*)0);

  glBindVertexArray(0);
}

Chunk::~Chunk()
{
  glDeleteBuffers(4, vertexbuffer);
  glDeleteBuffers(1, &indexbuffer);
  glDeleteVertexArrays(1, &vertexarray);
}

void Chunk::reset()
{
  voxels.clear();
  changed=true;
}

std::vector<Voxel> *Chunk::getVoxels()
{
  return &voxels;
}

void Chunk::addVoxel(Voxel newvox)
{
  voxels.push_back(newvox);
  changed=true;
}

void Chunk::setProgram(unsigned newprogram)
{
  program=program;
}

void Chunk::updateBuffers()
{
  glBindVertexArray(vertexarray);
  unsigned m_nvoxels = voxels.size();
  voxeltransforms.clear();
  for (unsigned int i=0; i<m_nvoxels; ++i)
  {
    voxeltransforms.push_back(voxels[i].getTransform());
  }
  // Voxel matrices
  glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer[3]);
  size_t mat_nbytes = voxeltransforms.size() * sizeof(glm::mat4);
  glBufferData(GL_ARRAY_BUFFER, mat_nbytes, glm::value_ptr(voxeltransforms[0]), GL_DYNAMIC_DRAW);
  GLsizei vec4Size = sizeof(glm::vec4);
  glEnableVertexAttribArray(3);
  glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (GLvoid*)0);
  glEnableVertexAttribArray(4);
  glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (GLvoid*)(vec4Size));
  glEnableVertexAttribArray(5);
  glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (GLvoid*)(2 * vec4Size));
  glEnableVertexAttribArray(6);
  glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (GLvoid*)(3 * vec4Size));

  glVertexAttribDivisor(3, 1);
  glVertexAttribDivisor(4, 1);
  glVertexAttribDivisor(5, 1);
  glVertexAttribDivisor(6, 1);
  glBindVertexArray(0);
}

void Chunk::draw()
{
  unsigned amount = voxels.size();
  if (amount > 0)
  {
    if (changed)
    {
      updateBuffers();
    }
    glUseProgram(program);
    glBindVertexArray(vertexarray);
    glDrawElementsInstanced(GL_TRIANGLES, voxels[0].getnVerts(), GL_UNSIGNED_INT, 0, amount);
    glUseProgram(0);
  }
}

VoxelGrid::VoxelGrid(int length, int width, int height) : length(length), width(width), height(height)
{
  for (int l=0; l<length; ++l)
  {
    for (int w=0; w<width; ++w)
    {
      for (int h=0; h<height; ++h)
      {
        addVoxel(Voxel(glm::translate(glm::mat4(),glm::vec3(float(l),float(w),float(h)))));
      }
    }
  }
}
