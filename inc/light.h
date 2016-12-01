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
    Light(const char *name_1, vec3 pos_1, vec3 ambient_1, vec3 diffuse_1, vec3 specular_1, bool shadows_1);
    virtual void updateUniforms(unsigned program)=0;
    virtual void updateShadowUniforms(unsigned program)=0;
    const char* getName();
    virtual void updatePos(mat4 *M)=0;
    virtual int shadowMap()=0;
    virtual void restore()=0;
    virtual unsigned getType()=0;
    virtual void renderQuad()=0;
    virtual vec3 getDirection()=0;
    //only spotlights and pointlights have radius and angle
    virtual float getRadius()=0;
    virtual float getAngle()=0;
    vec3 getWorldPos();
    vec3 getAmbient();
    vec3 getDiffuse();
    vec3 getSpecular();
    bool isShadowing();
    GLuint getDepthMap();
   // float getRadius();
   // float getAngle();
  protected:
    char name[25];
    vec3 pos;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    vec3 worldpos;
    bool shadows=0;
   // float angle;
   // float radius;

    //GL things for shadows
    GLuint depth_fbo;
    GLuint depth_map;
    mat4 PV;
    int shadowmap_program;

    //for debugging
    int quad_program;
};

class PointLight : public Light
{
  public:
    ~PointLight();
    PointLight(const char *name_1, vec3 pos_1, vec3 ambient_1, vec3 diffuse_1, vec3 specular_1, float radius_1);
    void updateUniforms(unsigned program) override;
    void updateShadowUniforms(unsigned program) override;
    void updatePos(mat4 *M) override;
    int shadowMap() override;
    void restore() override;
    unsigned getType() override;
    void renderQuad() override;
    vec3 getDirection() override;
    float getRadius() override;
    //2*pi
    float getAngle() override;
  private:
    float radius;
    mat4 cubemats[6];
};

class SpotLight : public Light
{
  public:
    ~SpotLight();
    SpotLight(const char *name_1, vec3 pos_1, vec3 ambient_1, vec3 diffuse_1, vec3 specular_1, vec3 direction_1, float radius_1, float angle_1);
    void updateUniforms(unsigned program) override;
    void updateShadowUniforms(unsigned program) override;
    void updatePos(mat4 *M) override;
    int shadowMap() override;
    void restore() override;
    unsigned getType() override;
    void renderQuad() override;
    vec3 getDirection() override;
    float getRadius() override;
    float getAngle() override;
  private:
    float radius;
    float angle;
    vec3 direction;
    vec3 world_direction;
    mat4 shadowmat;
};

class DirectionLight : public Light
{
  public:
    ~DirectionLight();
    DirectionLight(const char *name_1, vec3 pos_1, vec3 ambient_1, vec3 diffuse_1, vec3 specular_1, vec3 direction_1);
    void updateUniforms(unsigned program) override;
    void updateShadowUniforms(unsigned program) override;
    void updatePos(mat4 *M) override;
    int shadowMap() override;
    void restore() override;
    unsigned getType() override;
    void renderQuad() override;
    vec3 getDirection() override;
    float getRadius() override;
    float getAngle() override;
  private:
    vec3 direction;
    vec3 world_direction;
};

class DummyLight : public Light
{
  public:
    DummyLight();
    void updateUniforms(unsigned program) override;
    void updateShadowUniforms(unsigned program) override;
    void updatePos(mat4 *M) override;
    int shadowMap() override;
    void restore() override;
    unsigned getType() override;
    void renderQuad() override;
    vec3 getDirection() override;
    float getRadius() override;
    float getAngle() override;
  private:
};
