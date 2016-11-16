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
    const char* getName();
    virtual void updatePos(mat4 *M)=0;
    virtual void shadowMap(unsigned program)=0;
    virtual void restore()=0;
  protected:
    char name[25];
    vec3 pos;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    vec3 worldpos;

    //GL things for shadows
    GLuint depth_fbo;
    GLuint depth_map;
    mat4 VP;
};

//class PointLight : public Light
//{
//  public:
//  private:
//};

class PointLight : public Light
{
  public:
    PointLight(const char *name_1, vec3 pos_1, vec3 ambient_1, vec3 diffuse_1, vec3 specular_1);
    void updateUniforms(unsigned program) override;
    void updatePos(mat4 *M) override;
    void shadowMap(unsigned program) override;
    void restore() override;
  private:
};

class DirectionLight : public Light
{
  public:
    ~DirectionLight();
    DirectionLight(const char *name_1, vec3 pos_1, vec3 ambient_1, vec3 diffuse_1, vec3 specular_1, vec3 direction_1);
    void updateUniforms(unsigned program) override;
    void updatePos(mat4 *M) override;
    void shadowMap(unsigned program) override;
    void restore() override;
  private:
    vec3 direction;
};

class DummyLight : public Light
{
  public:
    DummyLight();
    void updateUniforms(unsigned program) override;
    void updatePos(mat4 *M) override;
    void shadowMap(unsigned program) override;
    void restore() override;
  private:
};
