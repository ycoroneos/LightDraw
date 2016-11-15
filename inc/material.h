#pragma once
#include <inc/gl.h>
#include <vector>
#include <glm/glm.hpp>
#include <inc/vertex.h>
using namespace glm;

class Material
{
  public:
    Material();
    Material(const char *filename);
    ~Material();
    virtual void Use();
    void incRef();
    void decRef();
  private:
    char name[25];
    GLuint texID;
    unsigned refcount;
};

class DummyMat : public Material
{
  public:
  DummyMat();
  void Use() override;
};
