#pragma once
#include <inc/gl.h>
#include <vector>
#include <glm/glm.hpp>
#include <inc/vertex.h>
#include <inc/material.h>
using namespace glm;

class Mesh
{
  public:
    Mesh(std::vector<Vertex> vertices, std::vector<unsigned> indices, const char *name, Material *material);
    ~Mesh();
    void setProgram(unsigned newprogram);
    unsigned getProgram();
    void setQuickProgram(unsigned newprogram);
    unsigned getQuickProgram();
    void draw(bool lines=false, GLfloat *M=NULL, GLfloat *N=NULL);
    void quickdraw(GLfloat *M=NULL, int quickprog=0);
  private:
    //for personality
    char name[25];

    //for culling <centroid, distance to furthest corner>
    vec4 bounding_sphere;

    //GL variables
    unsigned program=0;
    unsigned quickprogram=0;
    GLuint vertexarray;
    GLuint vertexbuffer;
    GLuint indexbuffer;
    unsigned n_indices;

    int M_loc;
    int N_loc;

    //material
    Material *drawmaterial;
};

