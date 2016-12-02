#pragma once
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

class KeyframeAnimation
{
  public:
    KeyframeAnimation(double ticks_per_second_1, vector<vec3> poskeys_1, vector<vec3> scalekeys_1, vector<quat> rotationkeys_1, Node *target_1);
  private:
    double ticks_per_second;
    vector<vec3> poskeys;
    vector<vec3> scalekeys;
    vector<quat> rotationkeys;
    Node *target;
};
