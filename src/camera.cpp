#include "inc/camera.h"
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

Camera::Camera(vec3 eye, vec2 pitchandyaw)
{
  pos = eye;
  pitchyaw=pitchandyaw;
  computeViewMatrix();
}

void Camera::computeViewMatrix()
{
  mat4 pitchmatrix = rotate(pitchyaw[0], vec3(1.0,0,0));
  mat4 yawmatrix = rotate(pitchyaw[1], vec3(0,1.0,0));
  mat4 rotation = pitchmatrix * yawmatrix;
  View = rotation * translate(-1.0f*pos);
}

mat4 Camera::getViewMatrix()
{
  return View;
}

vec3 Camera::getPos()
{
  return pos;
}

// <w, s, a, d>
void Camera::updateTranslation(vec2 translation)
{

  vec4 row2 = transpose(View)*vec4(0,0,1,0);
  vec4 row0 = transpose(View)*vec4(1,0,0,0);
  vec3 forward = vec3(row2[0], row2[1], row2[2]);
  vec3 strafe = vec3(row0[0], row0[1], row0[2]);
  pos = pos +(-1.0f*translation[0]*forward +translation[1]*strafe*-1.0f);
  computeViewMatrix();
  return;
}

void Camera::updatePitchYaw(vec2 pitchyawdelta)
{
  pitchyaw = pitchyaw + pitchyawdelta;
  computeViewMatrix();
  return;
}
