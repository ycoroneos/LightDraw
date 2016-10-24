#include "inc/chunk.h"
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "stdio.h"
#include "inc/cube.h"
#include "inc/recorder.h"

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
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, dummy.getnIndices() * sizeof(GLuint), dummy.getIndices(), GL_DYNAMIC_DRAW);

  // POSITION
  glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer[0]);
  size_t position_nbytes = dummy.getnVerts() * sizeof(glm::vec3);
  glBufferData(GL_ARRAY_BUFFER, position_nbytes, dummy.getVerts(), GL_DYNAMIC_DRAW);
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
  glBufferData(GL_ARRAY_BUFFER, normal_nbytes, dummy.getVerts(), GL_DYNAMIC_DRAW);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1,
      3,
      GL_FLOAT,
      GL_FALSE,
      sizeof(glm::vec3),
      (void*)0);

  // Color
  extern vec3 colors[];
  glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer[2]);
  size_t color_nbytes = dummy.getnVerts() * sizeof(glm::vec3);
  glBufferData(GL_ARRAY_BUFFER, color_nbytes, dummy.getVerts(), GL_DYNAMIC_DRAW);
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
  program=newprogram;
}

unsigned Chunk::getProgram()
{
  return program;
}

void Chunk::updateBuffers()
{
  glBindVertexArray(vertexarray);
  unsigned m_nvoxels = voxels.size();
  voxeltransforms.clear();
  for (unsigned int i=0; i<m_nvoxels; ++i)
  {
    if (!voxels[i].getVisible())
      continue;
    voxeltransforms.push_back(voxels[i].getTransform());
  }
  fprintf(stderr, "have %d visible voxels\n", voxeltransforms.size());
  // Voxel matrices
  glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer[3]);
  size_t mat_nbytes = voxeltransforms.size() * sizeof(glm::mat4);
  glBufferData(GL_ARRAY_BUFFER, mat_nbytes, &voxeltransforms[0][0][0], GL_DYNAMIC_DRAW);
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

void Chunk::draw(bool lines)
{
  unsigned amount = voxels.size();
  if (amount > 0)
  {
    if (changed)
    {
      updateBuffers();
      changed=false;
    }
    unsigned draw_amount = voxeltransforms.size();
    glUseProgram(program);
    glBindVertexArray(vertexarray);
    if (lines)
    {
      glDrawElementsInstanced(GL_LINES, voxels[0].getnIndices(), GL_UNSIGNED_INT, 0, draw_amount);
    }
    else
    {
      glDrawElementsInstanced(GL_TRIANGLES, voxels[0].getnIndices(), GL_UNSIGNED_INT, 0, draw_amount);
    }
    glBindVertexArray(0);
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
        Voxel newvoxel = Voxel(glm::translate(glm::mat4(),glm::vec3(float(l),float(w),float(h))));
        addVoxel(newvoxel);
      }
    }
  }
  vec3 centroid = vec3(float(length), float(width), float(height));
  centroid = centroid / 2.0f;
  bounding_box = vec4(centroid, glm::length(centroid));
}
