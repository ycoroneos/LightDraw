#include <inc/animation.h>
#include <inc/gl.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <vector>
#include <inc/node.h>
#include "math.h"
using namespace std;
using namespace glm;

KeyframeAnimation::KeyframeAnimation(double ticks_per_second_1, vector<vec3> poskeys_1, vector<vec3> scalekeys_1, vector<quat> rotationkeys_1, Node *target_1)
  : ticks_per_second(ticks_per_second_1), poskeys(poskeys_1), scalekeys(scalekeys_1), rotationkeys(rotationkeys_1), target(target_1)
{
  last_tick=0;
}

void KeyframeAnimation::stepAnimation(double timestep)
{
  double ticks = timestep * ticks_per_second;
  last_tick += ticks;
  double curtick_pos = fmod(last_tick, poskeys.size());
  double curtick_rot = fmod(last_tick, rotationkeys.size());
  //double curtick_scale = fmod(last_tick, poskeys.size());
  int curtick_bot = int(curtick_pos);
  int curtick_top = curtick_bot+1;
  float a = curtick_pos - double(curtick_bot);
  float b = 1.0f - a;
  mat4 A = glm::translate(poskeys[curtick_bot]);// * mat4_cast(rotationkeys[curtick_bot]);
  mat4 B = glm::translate(poskeys[curtick_top]);// * mat4_cast(rotationkeys[curtick_top]);
  mat4 translate = a*A + b*B;

  curtick_bot = int(curtick_rot);
  curtick_top = curtick_bot+1;
  a = curtick_rot - double(curtick_bot);
  b = 1.0f - a;
  A = mat4_cast(rotationkeys[curtick_bot]);
  B = mat4_cast(rotationkeys[curtick_top]);
  mat4 rotation = a*A + b*B;

  mat4 transform = translate;// * rotation;

  //update the node transforms
  target->setTransform(transform);
  target->bakeLower();
}
