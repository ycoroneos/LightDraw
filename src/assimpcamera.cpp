#include "inc/assimpcamera.h"
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "stdio.h"
using namespace glm;

AssimpCamera::AssimpCamera(mat4 Projection, vec3 local_pos_1, vec3 local_lookat_1, vec3 local_up_1, const char *name_1)
  : Camera(local_pos_1, Projection, name_1)//, local_pos(local_pos_1), local_lookat(local_lookat_1), local_up(local_up_1)
{
  local_pos = local_pos_1;
  local_lookat = local_lookat_1;
  local_up = local_up_1;
  //mat4 id = mat4(1.0f);
  //animateView(&id);
}

void AssimpCamera::doMouseInput(double xpos, double ypos)
{
}

void AssimpCamera::doKeyboardInput(int key, int scancode, int action, int mods)
{
}
