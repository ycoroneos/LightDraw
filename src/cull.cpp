#include "inc/cull.h"
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


Frustum::Frustum(vec3 f_0, vec3 f_1, vec3 f_2, vec3 f_3, vec3 f_4, vec3 f_5, vec3 f_6, vec3 f_7)
{
  f[0]=f_0;
  f[1]=f_1;
  f[2]=f_2;
  f[3]=f_3;
  f[4]=f_4;
  f[5]=f_5;
  f[6]=f_6;
  f[7]=f_7;
  near_normal = glm::normalize(glm::cross(f[1]-f[0], f[2]-f[0]));
  right_normal = glm::normalize(glm::cross(f[5]-f[1], f[3]-f[1]));
  left_normal = glm::normalize(glm::cross(f[2]-f[0], f[4]-f[0]));
  bottom_normal = glm::normalize(glm::cross(f[6]-f[3], f[2]-f[3]));
  top_normal = glm::normalize(glm::cross(f[5]-f[1], f[0]-f[1]));
  far_normal = glm::normalize(glm::cross(f[7]-f[4], f[5]-f[4]));
}

bool Frustum::isInternal(vec3 point)
{
  if (glm::dot(point, near_normal)<0) {return false;};
  if (glm::dot(point, right_normal)<0) {return false;};
  if (glm::dot(point, left_normal)<0) {return false;};
  if (glm::dot(point, bottom_normal)<0) {return false;};
  if (glm::dot(point, top_normal)<0) {return false;};
  if (glm::dot(point, far_normal)<0) {return false;};
  return true;
}
