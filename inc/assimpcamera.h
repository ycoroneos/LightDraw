#pragma once
#include <glm/glm.hpp>
#include "inc/input.h"
#include "inc/camera.h"
using namespace glm;

class AssimpCamera : public Camera
{
  public:
    AssimpCamera(mat4 Projection, mat4 local_View_1, char *name_1)
    void doMouseInput(double xpos, double ypos) override;
    void doKeyboardInput(int key, int scancode, int action, int mods) override;
  private:
};
