#pragma once
#include <glm/glm.hpp>
#include "inc/input.h"
using namespace glm;

class AssimpCamera : public InputResponder
{
  public:
    AssimpCamera(float aspect_1, float near_1, float far_1, float fov_1, char *name_1, vec3 local_lookAt_center_1, vec3 local_pos_1, vec3 local_up_1);
    mat4 getViewMatrix();
    mat4 getProjectionMatrix();
    mat4 getProjectionViewInverse();
    void computeViewMatrix();
    void updatePitchYaw(vec2 pitchyawdelta);
    void updateTranslation(vec2 translation);
    vec3 getPos();
    void updateUniforms(unsigned program);
    bool viewWire();

    void doMouseInput(double xpos, double ypos) override;
    void doKeyboardInput(int key, int scancode, int action, int mods) override;
  private:
    vec2 pitchyaw;
    vec3 pos;
    mat4 View;
    mat4 Projection;
    mat4 invProjection;
    bool wireframe;
    bool sprint;

    float aspect;
    float near;
    float far;
    float fov;
    char name[25];
    vec3 local_lookAt_center;
    vec3 local_pos;
    vec3 local_up;
};
