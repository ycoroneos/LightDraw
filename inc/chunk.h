#pragma once

#include <inc/gl.h>
#include <vector>
#include <glm/glm.hpp>
#include "inc/voxel.h"
#include "inc/binvox.h"
using namespace glm;

class Chunk
{
  public:
    //create unit-spaced voxels
    Chunk();
    ~Chunk();
    void reset();
    void addVoxel(Voxel newvox);
    std::vector<Voxel> *getVoxels();
    void setProgram(unsigned newprogram);
    unsigned getProgram();
    void updateBuffers();
    void draw(bool lines=false);
  protected:
    std::vector<Voxel> voxels;
    bool changed;

    //for culling <centroid, distance to corner>
    vec4 bounding_box;

    //GL variables
    unsigned program=0;
    GLuint vertexarray;
    GLuint vertexbuffer[4];
    GLuint indexbuffer;

  private:
    std::vector<mat4> voxeltransforms;
};

class VoxelGrid : public Chunk
{
  public:
    VoxelGrid(int length, int width, int height);
  private:
    int length, width, height;
};

class BinVox : public Chunk
{
  public:
    BinVox(const char *filename);
  private:
    int length, width, height;
    mat4 translate;
    mat4 scale;
};
