#include <inc/gl.h>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <inc/light.h>
#include "stdio.h"
#include "string.h"
using namespace glm;

#define SHADOW_WIDTH 1024
#define SHADOW_HEIGHT 1024

extern int window_width;
extern int window_height;

Light::Light(const char *name_1, vec3 pos_1, vec3 ambient_1, vec3 diffuse_1, vec3 specular_1)
  : pos(pos_1), ambient(ambient_1), diffuse(diffuse_1), specular(specular_1)
{
  strncpy(name, name_1, sizeof(name));
  worldpos = pos;
}

const char *Light::getName()
{
  return name;
}

void PointLight::updatePos(mat4 *M)
{
  //vec4 base = *M * vec4(0.0f, 0.0f, 0.0f, 1.0f);
  //worldpos = vec3(base) + pos;
  //worldpos = vec3(*M * vec4(pos, 1.0f));
  worldpos = vec3(5.0f, 5.0f, 0.0f);
}

PointLight::PointLight(const char *name_1, vec3 pos_1, vec3 ambient_1, vec3 diffuse_1, vec3 specular_1)
  : Light(name_1, pos_1, ambient_1, diffuse_1, specular_1)
{
  worldpos = vec3(5.0f, 5.0f, 0.0f);
}

void PointLight::updateUniforms(unsigned program)
{
  //I hate doing this. Too bad apple doesn't support OGL4.3
  int lightpos_loc = glGetUniformLocation(program, "lightPos");
  int ambient_loc = glGetUniformLocation(program, "lightAmbient");
  int diffuse_loc = glGetUniformLocation(program, "lightDiffuse");
  int specular_loc = glGetUniformLocation(program, "lightSpecular");
  vec4 lpos = vec4(worldpos, 1.0f);
  glUniform4fv(lightpos_loc, 1, &lpos[0]);
  glUniform3fv(ambient_loc, 1, &ambient[0]);
  glUniform3fv(diffuse_loc, 1, &diffuse[0]);
  glUniform3fv(specular_loc, 1, &specular[0]);
}

void PointLight::shadowMap(unsigned program)
{
}

void PointLight::restore()
{
}

////////////////////////////////////////////////////
DirectionLight::DirectionLight(const char *name_1, vec3 pos_1, vec3 ambient_1, vec3 diffuse_1, vec3 specular_1, vec3 direction_1)
  : Light(name_1, pos_1, ambient_1, diffuse_1, specular_1), direction(direction_1)
{
  glGenFramebuffers(1, &depth_fbo);
  glGenTextures(1, &depth_map);
  glBindTexture(GL_TEXTURE_2D, depth_map);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

DirectionLight::~DirectionLight()
{
  glDeleteFramebuffers(1, &depth_fbo);
}

void DirectionLight::updatePos(mat4 *M)
{
}

void DirectionLight::updateUniforms(unsigned program)
{
  //I hate doing this. Too bad apple doesn't support OGL4.3
  int lightpos_loc = glGetUniformLocation(program, "lightPos");
  int ambient_loc = glGetUniformLocation(program, "lightAmbient");
  int diffuse_loc = glGetUniformLocation(program, "lightDiffuse");
  int specular_loc = glGetUniformLocation(program, "lightSpecular");
  vec4 lpos = vec4(direction, 0.0f);
  glUniform4fv(lightpos_loc, 1, &lpos[0]);
  glUniform3fv(ambient_loc, 1, &ambient[0]);
  glUniform3fv(diffuse_loc, 1, &diffuse[0]);
  glUniform3fv(specular_loc, 1, &specular[0]);
}

void DirectionLight::shadowMap(unsigned program)
{
  mat4 V = glm::lookAt(direction*-1.0f, vec3(0,0,0), vec3(0.0f, 1.0f, 0.0f));

  //this will be wrong
  mat4 P = glm::ortho<float>(-10,10,-10,10,-10,20);


  VP = V * P;
  int VP_loc = glGetUniformLocation(program, "VP");
  glUniform4fv(VP_loc, 1, &VP[0][0]);
  glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
  glBindTexture(GL_TEXTURE_2D, depth_map);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glBindFramebuffer(GL_FRAMEBUFFER, depth_fbo);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth_map, 0);
  glDrawBuffer(GL_NONE);
  glReadBuffer(GL_NONE);
  glClear(GL_DEPTH_BUFFER_BIT);
}

void DirectionLight::restore()
{
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glViewport(0, 0, window_width, window_height);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

//////////////////////////////////////////////////////
void DummyLight::updatePos(mat4 *M)
{
}

DummyLight::DummyLight() : Light("dummy", vec3(), vec3(), vec3(), vec3())
{
}

void DummyLight::shadowMap(unsigned program)
{
}

void DummyLight::restore()
{
}

void DummyLight::updateUniforms(unsigned program)
{
}
