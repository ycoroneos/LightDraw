#pragma once
#include <inc/gl.h>
#include <vector>
#include <glm/glm.hpp>
#include <inc/vertex.h>
using namespace glm;

class Light
{
  public:
    Light(const char *name_1, vec3 pos_1, vec3 ambient_1, vec3 diffuse_1, vec3 specular_1);
    virtual void updateUniforms(unsigned program)=0;
  protected:
    char name[25];
    vec3 pos;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

//class PointLight : public Light
//{
//  public:
//  private:
//};

class DirectionLight : public Light
{
  public:
    DirectionLight(const char *name_1, vec3 pos_1, vec3 ambient_1, vec3 diffuse_1, vec3 specular_1, vec3 direction_1);
    void updateUniforms(unsigned program) override;
  private:
    vec3 direction;
};

class DummyLight : public Light
{
  public:
    DummyLight();
    void updateUniforms(unsigned program) override;
  private:
};
