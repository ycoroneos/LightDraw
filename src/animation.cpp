#include <inc/animation.h>
#include <inc/gl.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <vector>
#include <inc/node.h>
using namespace std;
using namespace glm;

KeyframeAnimation::KeyframeAnimation(double ticks_per_second_1, vector<vec3> poskeys_1, vector<vec3> scalekeys_1, vector<quat> rotationkeys_1, Node *target_1)
  : ticks_per_second(ticks_per_second_1), poskeys(poskeys_1), scalekeys(scalekeys_1), rotationkeys(rotationkeys_1), target(target_1)
{
}

