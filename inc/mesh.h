#pragma once
#include <inc/gl.h>
#include <vector>
#include <glm/glm.hpp>
#include <inc/vertex.h>
using namespace glm;

class Mesh
{
  public:
    Mesh(std::vector<Vertex> vertices, std::vector<unsigned> indices, const char *name, unsigned material);
    ~Mesh();
    void setProgram(unsigned newprogram);
    unsigned getProgram();
    void draw(bool lines=false);
  private:
    //for personality
    char name[25];

    //for culling <centroid, distance to furthest corner>
    vec4 bounding_sphere;

    //GL variables
    unsigned program=0;
    GLuint vertexarray;
    GLuint vertexbuffer[4];
    GLuint indexbuffer;
};

