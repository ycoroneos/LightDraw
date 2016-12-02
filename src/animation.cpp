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
  double curtick = fmod(last_tick + ticks, poskeys.size());
  last_tick = curtick;
  int curtick_bot = int(curtick);
  int curtick_top = curtick_bot+1;
  float a = curtick - double(curtick_bot);
  float b = 1.0f - a;
  //translate * rotate * scale
  //mat4 A = glm::translate(poskeys[curtick_bot]) * mat4_cast(rotationkeys[curtick_bot]) * glm::scale(scalekeys[curtick_bot]);
  //mat4 B = glm::translate(poskeys[curtick_top]) * mat4_cast(rotationkeys[curtick_top]) * glm::scale(scalekeys[curtick_top]);
  mat4 A = glm::translate(poskeys[curtick_bot]);
  mat4 B = glm::translate(poskeys[curtick_top]);
  mat4 transform = a*A + b*B;

  //update the node transforms
  target->setTransform(A);
  target->bakeLower();
}
