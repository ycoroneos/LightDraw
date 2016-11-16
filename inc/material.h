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
    Material(const char *name_1, vec3 ambient_1, vec3 diffuse_1, vec3 specular_1, float shininess_1, const char *diffuse_texture);
    ~Material();
    virtual void Use(int program);
    void incRef();
    void decRef();
  private:
    char name[25];
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
    GLuint texID;
    unsigned refcount;
};

class DummyMat : public Material
{
  public:
  DummyMat();
  void Use(int program) override;
};
