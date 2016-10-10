#pragma once
#include <glm/glm.hpp>
using namespace glm;

class Camera
{
  public:
    Camera(vec3 eye, vec2 pitchandyaw);
    mat4 getViewMatrix();
    void computeViewMatrix();
    void updatePitchYaw(vec2 pitchyawdelta);
    void updateTranslation(vec2 translation);
    vec3 getPos();
  private:
    vec2 pitchyaw;
    vec3 pos;
    mat4 View;
};
