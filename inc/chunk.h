#pragma once

#include <vector>
#include <glm/glm.hpp>
#include "inc/voxel.h"
using namespace glm;

class Chunk
{
  public:
    //create unit-spaced voxels
    Chunk();
    void reset();
    void addVoxel(Voxel newvox);
    std::vector<Voxel> *getVoxels();
    //void draw();
  protected:
    std::vector<Voxel> voxels;
    bool changed;

    //GL variables
    unsigned program;
};

class VoxelGrid : public Chunk
{
  public:
    VoxelGrid(int length, int width, int height);
  private:
    int length, width, height;
};
