#include <inc/gl.h>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <inc/light.h>
#include "stdio.h"
#include "string.h"
using namespace glm;

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
  worldpos = vec3(*M * vec4(pos, 1.0f));
}

PointLight::PointLight(const char *name_1, vec3 pos_1, vec3 ambient_1, vec3 diffuse_1, vec3 specular_1)
  : Light(name_1, pos_1, ambient_1, diffuse_1, specular_1)
{
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

DirectionLight::DirectionLight(const char *name_1, vec3 pos_1, vec3 ambient_1, vec3 diffuse_1, vec3 specular_1, vec3 direction_1)
  : Light(name_1, pos_1, ambient_1, diffuse_1, specular_1), direction(direction_1)
{
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

void DummyLight::updatePos(mat4 *M)
{
}

DummyLight::DummyLight() : Light("dummy", vec3(), vec3(), vec3(), vec3())
{
}

void DummyLight::updateUniforms(unsigned program)
{
}
