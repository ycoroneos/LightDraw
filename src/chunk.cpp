#include "inc/chunk.h"
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>

Chunk::Chunk()
{
  voxels.clear();
  changed=true;
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
