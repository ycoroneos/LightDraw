#include "inc/camera.h"
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "stdio.h"
using namespace glm;

extern int window_width;
extern int window_height;

Camera::Camera(vec3 eye, mat4 Projection, const char *name_1)
  : pos(eye), Projection(Projection), invProjection(glm::inverse(Projection))
{
  strncpy(name, name_1, sizeof(name));
}

mat4 Camera::getProjectionMatrix()
{
  return Projection;
}

mat4 Camera::getViewMatrix()
{
  return View;
}

vec3 Camera::getPos()
{
  return pos;
}

mat4 Camera::getProjectionViewInverse()
{
  return glm::inverse(getProjectionMatrix()*getViewMatrix());
}

void Camera::updateUniforms(unsigned program)
{
  //projection matrix
  int loc = glGetUniformLocation(program, "P");
  if (loc==-1)
  {
    perror("P not found\n");
  }
  else
  {
    glUniformMatrix4fv(loc, 1, false, &Projection[0][0]);
  }
  loc = glGetUniformLocation(program, "V");
  if (loc==-1)
  {
    perror("V not found\n");
  }
  else
  {
    glUniformMatrix4fv(loc, 1, false, &View[0][0]);
  }
  loc = glGetUniformLocation(program, "camPos");
  if (loc==-1)
  {
    //perror("camPos not found\n");
  }
  else
  {
    glUniform3fv(loc, 1, &pos[0]);
  }
}

void Camera::updatePos(mat4 M)
{
  View = M * local_View;
}

bool Camera::viewWire()
{
  return wireframe;
}

