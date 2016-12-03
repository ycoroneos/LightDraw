#pragma once
#include <glm/glm.hpp>
#include "inc/input.h"
#include "inc/camera.h"
using namespace glm;


class FPSCamera : public Camera
{
  public:
    FPSCamera(vec3 eye, vec2 pitchandyaw, mat4 Projection);
    void computeViewMatrix();
    void updatePitchYaw(vec2 pitchyawdelta);
    void updateTranslation(vec2 translation);
    void doMouseInput(double xpos, double ypos) override;
    void doKeyboardInput(int key, int scancode, int action, int mods) override;
  private:
    vec2 pitchyaw;
    bool sprint=false;
};
