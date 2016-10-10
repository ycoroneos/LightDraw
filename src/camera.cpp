#include "inc/camera.h"
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "stdio.h"
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

void Camera::doMouseInput(double xpos, double ypos)
{
  static double oldx=-1;
  static double oldy=-1;

  //high pass initial condition
  if (oldx==-1 || oldy==-1)
  {
    oldx=xpos;
    oldy=ypos;
    return;
  }
  double sensitivity=0.01;
  double diffx=(xpos-oldx) * sensitivity;
  double diffy=(ypos-oldy) * sensitivity;
  updatePitchYaw(vec2(float(diffy), float(diffx)));
  oldx=xpos;
  oldy=ypos;
}

void Camera::doKeyboardInput(int key, int scancode, int action, int mods)
{
    if (action == GLFW_RELEASE) { // only handle PRESS and REPEAT
        return;
    }

    printf("camera: ");
    //printf("camera key input\n");
    // Special keys (arrows, CTRL, ...) are documented
    // here: http://www.glfw.org/docs/latest/group__keys.html
    switch (key)
    {
      case 'W':
        updateTranslation(vec2(0.1f,0.0f));
        break;
      case 'S':
        updateTranslation(vec2(-0.1f,0.0f));
        break;
      case 'A':
        updateTranslation(vec2(0.0f,0.1f));
        break;
      case 'D':
        updateTranslation(vec2(0.0f,-0.1f));
        break;
      default:
        printf("unhandled key press %d\n", key);
        break;
    }
}
