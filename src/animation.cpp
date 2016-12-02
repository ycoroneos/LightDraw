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
  last_tick_pos=0;
  last_tick_rot=0;
}

void KeyframeAnimation::stepAnimation(double timestep)
{
  //position
  double ticks = timestep * ticks_per_second;
  int next_tick = int(ceil(last_tick_pos + ticks)) % poskeys.size();
  mat4 translate = glm::translate(poskeys[last_tick_pos] + float(ticks)*(poskeys[next_tick] - poskeys[last_tick_pos]));
  last_tick_pos = next_tick;

  //rotation
  next_tick = int(ceil(last_tick_rot + ticks)) % rotationkeys.size();
  mat4 rotate = mat4_cast(rotationkeys[last_tick_rot]) + float(ticks)*(mat4_cast(rotationkeys[next_tick]) - mat4_cast(rotationkeys[last_tick_rot]));
  last_tick_rot = next_tick;

  //update the node transforms
  target->setTransform(translate * rotate);
  target->bakeLower();
}
