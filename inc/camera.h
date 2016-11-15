#pragma once
#include <glm/glm.hpp>
#include "inc/input.h"
using namespace glm;

class Camera : public InputResponder
{
  public:
    Camera(vec3 eye, vec2 pitchandyaw, mat4 Projection);
    mat4 getViewMatrix();
    mat4 getProjectionMatrix();
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
};
