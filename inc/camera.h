#pragma once
#include <glm/glm.hpp>
#include "inc/input.h"
using namespace glm;

class Camera : public InputResponder
{
  public:
    Camera(vec3 eye, mat4 Projection, const char *name_1);
    void setProjection(mat4 newProjection);
    void updatePos(mat4 M);
    mat4 getViewMatrix();
    mat4 getProjectionMatrix();
    mat4 getProjectionViewInverse();
    vec3 getPos();
    void updateUniforms(unsigned program);
    bool viewWire();
  protected:
    char name[25];
    vec3 pos;
    mat4 View;
    mat4 local_View; //used for animation
    mat4 Projection;
    mat4 invProjection;
    bool wireframe=false;
};
