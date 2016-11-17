#pragma once
#include <inc/gl.h>
#include <vector>
#include <glm/glm.hpp>
#include <inc/vertex.h>
using namespace glm;
#define POINT_LIGHT 0
#define DIRECTION_LIGHT 1
#define SPOT_LIGHT 2
#define DUMMY_LIGHT 3

class Light
{
  public:
    Light(const char *name_1, vec3 pos_1, vec3 ambient_1, vec3 diffuse_1, vec3 specular_1);
    virtual void updateUniforms(unsigned program)=0;
    const char* getName();
    virtual void updatePos(mat4 *M)=0;
    virtual int shadowMap()=0;
    virtual void restore()=0;
    virtual unsigned getType()=0;
    virtual void renderQuad()=0;
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
    int shadowmap_program;

    //for debugging
    int quad_program;
};

class PointLight : public Light
{
  public:
    ~PointLight();
    PointLight(const char *name_1, vec3 pos_1, vec3 ambient_1, vec3 diffuse_1, vec3 specular_1);
    void updateUniforms(unsigned program) override;
    void updatePos(mat4 *M) override;
    int shadowMap() override;
    void restore() override;
    unsigned getType() override;
    void renderQuad() override;
  private:
    mat4 cubemats[6];
};

class SpotLight : public Light
{
  public:
    ~SpotLight();
    SpotLight(const char *name_1, vec3 pos_1, vec3 ambient_1, vec3 diffuse_1, vec3 specular_1, vec3 direction_1, float angle_1);
    void updateUniforms(unsigned program) override;
    void updatePos(mat4 *M) override;
    int shadowMap() override;
    void restore() override;
    unsigned getType() override;
    void renderQuad() override;
  private:
    float angle;
    vec3 direction;
    mat4 shadowmat;
};

class DirectionLight : public Light
{
  public:
    ~DirectionLight();
    DirectionLight(const char *name_1, vec3 pos_1, vec3 ambient_1, vec3 diffuse_1, vec3 specular_1, vec3 direction_1);
    void updateUniforms(unsigned program) override;
    void updatePos(mat4 *M) override;
    int shadowMap() override;
    void restore() override;
    unsigned getType() override;
    void renderQuad() override;
  private:
    vec3 direction;
};

class DummyLight : public Light
{
  public:
    DummyLight();
    void updateUniforms(unsigned program) override;
    void updatePos(mat4 *M) override;
    int shadowMap() override;
    void restore() override;
    unsigned getType() override;
    void renderQuad() override;
  private:
};
