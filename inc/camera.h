#pragma once
#include <glm/glm.hpp>
#include "inc/input.h"
using namespace glm;

class Camera : public InputResponder
{
  public:
    Camera(vec3 eye, vec2 pitchandyaw);
    mat4 getViewMatrix();
    void computeViewMatrix();
    void updatePitchYaw(vec2 pitchyawdelta);
    void updateTranslation(vec2 translation);
    vec3 getPos();
    void updateUniforms(mat4 P, unsigned program);

    void doMouseInput(double xpos, double ypos) override;
    void doKeyboardInput(int key, int scancode, int action, int mods) override;
  private:
    vec2 pitchyaw;
    vec3 pos;
    mat4 View;
};
