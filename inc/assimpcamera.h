#pragma once
#include <glm/glm.hpp>
#include "inc/input.h"
#include "inc/camera.h"
using namespace glm;

class AssimpCamera : public Camera
{
  public:
    AssimpCamera(mat4 Projection, vec3 local_pos_1, vec3 local_lookat_1, vec3 local_up_1, const char *name_1);
    void doMouseInput(double xpos, double ypos) override;
    void doKeyboardInput(int key, int scancode, int action, int mods) override;
  private:
};
