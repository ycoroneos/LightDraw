#include <inc/gl.h>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <inc/light.h>
#include "stdio.h"
#include "string.h"
using namespace glm;

#define SHADOW_WIDTH 2
#define SHADOW_HEIGHT 2

extern int window_width;
extern int window_height;
extern unsigned pointlight_shadowmap_program;
extern unsigned directionlight_shadowmap_program;

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
//////////////////////////////////////////////////////////////////////////////////////////////////////
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
  glGenFramebuffers(1, &depth_fbo);
  glGenTextures(1, &depth_map);
  glBindTexture(GL_TEXTURE_CUBE_MAP, depth_map);
  for (short i=0; i<6; ++i)
  {
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X+i, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
  }
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
  glBindFramebuffer(GL_FRAMEBUFFER, depth_fbo);
  glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depth_map, 0);
  glDrawBuffer(GL_NONE);
  glReadBuffer(GL_NONE);

  //check for completeness
  GLenum fbostatus = glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER);
  if (fbostatus != GL_FRAMEBUFFER_COMPLETE)
  {
    fprintf(stderr, "shadowmap FBO for %s is incomplete\r\n", name);
  }
  glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  shadowmap_program = pointlight_shadowmap_program;
}

PointLight::~PointLight()
{
  glDeleteTextures(1, &depth_map);
  glDeleteFramebuffers(1, &depth_fbo);
}

void PointLight::updateUniforms(unsigned program)
{
  //I hate doing this. Too bad apple doesn't support OGL4.3
  int lightpos_loc = glGetUniformLocation(program, "lightPos");
  int ambient_loc = glGetUniformLocation(program, "lightAmbient");
  int diffuse_loc = glGetUniformLocation(program, "lightDiffuse");
  int specular_loc = glGetUniformLocation(program, "lightSpecular");
  int cone_loc = glGetUniformLocation(program, "lightCone");
  vec4 lpos = vec4(worldpos, 1.0f);
  glUniform4fv(lightpos_loc, 1, &lpos[0]);
  glUniform3fv(ambient_loc, 1, &ambient[0]);
  glUniform3fv(diffuse_loc, 1, &diffuse[0]);
  glUniform3fv(specular_loc, 1, &specular[0]);
  float trash = 360.0f;
  glUniform1fv(cone_loc, 1, &trash);
}

int PointLight::shadowMap()
{
  glUseProgram(shadowmap_program);
  GLfloat aspect = (GLfloat)SHADOW_WIDTH/(GLfloat)SHADOW_HEIGHT;
  GLfloat near = 1.0f;
  GLfloat far = 25.0f;
  mat4 P = glm::perspective(90.0f, aspect, near, far);
  cubemats[0] = P* glm::lookAt(pos, pos + vec3(1.0,0.0,0.0), vec3(0.0,-1.0,0.0));
  cubemats[1] = P* glm::lookAt(pos, pos + vec3(-1.0,0.0,0.0), vec3(0.0,-1.0,0.0));
  cubemats[2] = P* glm::lookAt(pos, pos + vec3(0.0,1.0,0.0), vec3(0.0,0.0,1.0));
  cubemats[3] = P* glm::lookAt(pos, pos + vec3(0.0,-1.0,0.0), vec3(0.0,0.0,-1.0));
  cubemats[4] = P* glm::lookAt(pos, pos + vec3(0.0,0.0,1.0), vec3(0.0,-1.0,0.0));
  cubemats[5] = P* glm::lookAt(pos, pos + vec3(0.0,0.0,-1.0), vec3(0.0,-1.0,0.0));
  int PV_loc = glGetUniformLocation(shadowmap_program, "PV");
  int lightpos_loc = glGetUniformLocation(shadowmap_program, "lightPos");
  int farplane_loc = glGetUniformLocation(shadowmap_program, "far_plane");
  if (PV_loc < 0 || lightpos_loc < 0 || farplane_loc < 0)
  {
    fprintf(stderr, "shadowmap locs missing\r\n");
  }
  glUniformMatrix4fv(PV_loc, 6, false, &cubemats[0][0][0]);
  glUniform3fv(lightpos_loc, 1, &pos[0]);
  glUniform1fv(farplane_loc, 1, &far);

  glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
  glBindFramebuffer(GL_FRAMEBUFFER, depth_fbo);
  //glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth_map, 0);
  glDrawBuffer(GL_NONE);
  glReadBuffer(GL_NONE);
  glClear(GL_DEPTH_BUFFER_BIT);
  glCullFace(GL_FRONT);
  return shadowmap_program;

}

void PointLight::restore()
{
  glUseProgram(0);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glViewport(0, 0, window_width, window_height);
  //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glCullFace(GL_BACK);
}

unsigned PointLight::getType()
{
  return POINT_LIGHT;
}

////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////
void SpotLight::updatePos(mat4 *M)
{
  //vec4 base = *M * vec4(0.0f, 0.0f, 0.0f, 1.0f);
  //worldpos = vec3(base) + pos;
  //worldpos = vec3(*M * vec4(pos, 1.0f));
  //worldpos = vec3(*M * vec4(0.0f, 0.0f, 0.0f, 1.0f)) + pos;
  worldpos = vec3(-1.0f, 0.2f, -1.0f);
  direction = vec3(*M * vec4(1.0f, 1.0f, 1.0f, 0.0f)) + direction;
}

SpotLight::SpotLight(const char *name_1, vec3 pos_1, vec3 ambient_1, vec3 diffuse_1, vec3 specular_1, vec3 direction_1, float angle_1)
  : Light(name_1, pos_1, ambient_1, diffuse_1, specular_1), direction(direction_1), angle(angle_1)
{
  //worldpos = vec3(5.0f, 0.0f, 0.0f);
  //direction = -1.0f*worldpos;
  glGenFramebuffers(1, &depth_fbo);
  glGenTextures(1, &depth_map);
  glBindTexture(GL_TEXTURE_2D, depth_map);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
  glBindFramebuffer(GL_FRAMEBUFFER, depth_fbo);
  glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depth_map, 0);
  glDrawBuffer(GL_NONE);
  glReadBuffer(GL_NONE);

  //check for completeness
  GLenum fbostatus = glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER);
  if (fbostatus != GL_FRAMEBUFFER_COMPLETE)
  {
    fprintf(stderr, "shadowmap FBO for %s is incomplete\r\n", name);
  }
  glBindTexture(GL_TEXTURE_2D, 0);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  shadowmap_program = directionlight_shadowmap_program;
}

SpotLight::~SpotLight()
{
  glDeleteTextures(1, &depth_map);
  glDeleteFramebuffers(1, &depth_fbo);
}

void SpotLight::updateUniforms(unsigned program)
{
  //I hate doing this. Too bad apple doesn't support OGL4.3
  int lightpos_loc = glGetUniformLocation(program, "lightPos");
  int ambient_loc = glGetUniformLocation(program, "lightAmbient");
  int diffuse_loc = glGetUniformLocation(program, "lightDiffuse");
  int specular_loc = glGetUniformLocation(program, "lightSpecular");
  int cone_loc = glGetUniformLocation(program, "lightCone");
  int conedir_loc = glGetUniformLocation(program, "lightConeDirection");
  vec4 lpos = vec4(worldpos, 1.0f);
  glUniform4fv(lightpos_loc, 1, &lpos[0]);
  glUniform3fv(ambient_loc, 1, &ambient[0]);
  glUniform3fv(diffuse_loc, 1, &diffuse[0]);
  glUniform3fv(specular_loc, 1, &specular[0]);
  glUniform1fv(cone_loc, 1, &angle);
  glUniform3fv(conedir_loc, 1, &direction[0]);
}

int SpotLight::shadowMap()
{
  glUseProgram(shadowmap_program);
  GLfloat aspect = (GLfloat)SHADOW_WIDTH/(GLfloat)SHADOW_HEIGHT;
  GLfloat near = 1.0f;
  GLfloat far = 25.0f;
  mat4 P = glm::perspective(90.0f, aspect, near, far);
  shadowmat = P * glm::lookAt(pos, pos+direction, vec3(0.0f, 1.0f, 0.0f));
  int PV_loc = glGetUniformLocation(shadowmap_program, "PV");
  //int lightpos_loc = glGetUniformLocation(shadowmap_program, "lightPos");
  //int farplane_loc = glGetUniformLocation(shadowmap_program, "far_plane");
  //if (PV_loc < 0 || lightpos_loc < 0 || farplane_loc < 0)
  //{
  if (PV_loc < 0)
  {
    fprintf(stderr, "shadowmap locs missing\r\n");
  }
  glUniformMatrix4fv(PV_loc, 1, false, &shadowmat[0][0]);
  //glUniform3fv(lightpos_loc, 1, &pos[0]);
  //glUniform1fv(farplane_loc, 1, &far);

  glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
  glBindFramebuffer(GL_FRAMEBUFFER, depth_fbo);
  //glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth_map, 0);
  glDrawBuffer(GL_NONE);
  glReadBuffer(GL_NONE);
  glClear(GL_DEPTH_BUFFER_BIT);
  glCullFace(GL_FRONT);
  return shadowmap_program;

}

void SpotLight::restore()
{
  glUseProgram(0);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glViewport(0, 0, window_width, window_height);
  //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glCullFace(GL_BACK);
}

unsigned SpotLight::getType()
{
  return SPOT_LIGHT;
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
  glDeleteTextures(1, &depth_map);
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

int DirectionLight::shadowMap()
{
  glUseProgram(shadowmap_program);
  mat4 V = glm::lookAt(direction*-1.0f, vec3(0,0,0), vec3(0.0f, 1.0f, 0.0f));

  //this will be wrong
  mat4 P = glm::ortho<float>(-10,10,-10,10,-10,20);


  VP = V * P;
  int VP_loc = glGetUniformLocation(shadowmap_program, "VP");
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
  glCullFace(GL_FRONT);
  return shadowmap_program;
}

void DirectionLight::restore()
{
  glUseProgram(0);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glViewport(0, 0, window_width, window_height);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glCullFace(GL_BACK);
}

unsigned DirectionLight::getType()
{
  return DIRECTION_LIGHT;
}

//////////////////////////////////////////////////////
void DummyLight::updatePos(mat4 *M)
{
}

DummyLight::DummyLight() : Light("dummy", vec3(), vec3(), vec3(), vec3())
{
}

int DummyLight::shadowMap()
{
  glUseProgram(shadowmap_program);
  return shadowmap_program;
}

void DummyLight::restore()
{
  glUseProgram(0);
}

void DummyLight::updateUniforms(unsigned program)
{
}
unsigned DummyLight::getType()
{
  return DUMMY_LIGHT;
}
