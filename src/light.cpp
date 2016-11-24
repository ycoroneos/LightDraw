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
extern int  pointlight_shadowmap_program;
extern int  directionlight_shadowmap_program;
extern int  default_quad_program;

Light::Light(const char *name_1, vec3 pos_1, vec3 ambient_1, vec3 diffuse_1, vec3 specular_1)
  : pos(pos_1), ambient(ambient_1), diffuse(diffuse_1), specular(specular_1)
{
  strncpy(name, name_1, sizeof(name));
  worldpos = pos;
  quad_program = default_quad_program;
  if (ambient == vec3(0.0f, 0.0f, 0.0f))
  {
    ambient = vec3(1.0f, 1.0f, 1.0f);
  }
  fprintf(stderr, "light %s\r\n", name);
  fprintf(stderr, "\t amb %f %f %f\r\n", ambient.x, ambient.y, ambient.z);
  fprintf(stderr, "\t diff %f %f %f", diffuse.x, diffuse.y, diffuse.z);
  fprintf(stderr, "\t spec %f %f %f\r\n", specular.x, specular.y, specular.z);
  fprintf(stderr, "\t local pos %f %f %f\r\n", pos.x, pos.y, pos.z);
}

const char *Light::getName()
{
  return name;
}

vec3 Light::getWorldPos()
{
  return worldpos;
}

float Light::getRadius()
{
  return 5.0f;
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
  fprintf(stderr, "point light named %s\r\n", name);
  worldpos = vec3(5.0f, 5.0f, 0.0f);
  glGenFramebuffers(1, &depth_fbo);
  glGenTextures(1, &depth_map);
//  glBindTexture(GL_TEXTURE_CUBE_MAP, depth_map);
//  for (short i=0; i<6; ++i)
//  {
//    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X+i, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
//  }
//  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
//  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
//  glBindFramebuffer(GL_FRAMEBUFFER, depth_fbo);
//  glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depth_map, 0);
//  glDrawBuffer(GL_NONE);
//  glReadBuffer(GL_NONE);
//
//  //check for completeness
//  GLenum fbostatus = glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER);
//  if (fbostatus != GL_FRAMEBUFFER_COMPLETE)
//  {
//    fprintf(stderr, "shadowmap FBO for %s is incomplete\r\n", name);
//  }
//  glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
//  glBindFramebuffer(GL_FRAMEBUFFER, 0);
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
  if (ambient_loc<0)
  {
    fprintf(stderr, "cant find ambient loc for light %s\r\n", name);
  }
  //int farplane_loc = glGetUniformLocation(shadowmap_program, "far_plane");
 /// if (farplane_loc < 0)
 // {
 //   fprintf(stderr, "cant find farplane loc\r\n");
 // }
  vec4 lpos = vec4(worldpos, 1.0f);
  glUniform4fv(lightpos_loc, 1, &lpos[0]);
  glUniform3fv(ambient_loc, 1, &ambient[0]);
  glUniform3fv(diffuse_loc, 1, &diffuse[0]);
  vec4 combined = vec4(specular, 0.0f);
  glUniform4fv(specular_loc, 1, &combined[0]);
  //GLfloat far = 250.0f;
  //glUniform1fv(farplane_loc, 1, &far);
//  glBindTexture(GL_TEXTURE_CUBE_MAP, depth_map);
//  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
//  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}

int PointLight::shadowMap()
{
  glUseProgram(shadowmap_program);
  GLfloat aspect = (GLfloat)SHADOW_WIDTH/(GLfloat)SHADOW_HEIGHT;
  GLfloat near = 1.0f;
  GLfloat far = 250.0f;
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
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth_map, 0);
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
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glCullFace(GL_BACK);
}

unsigned PointLight::getType()
{
  return POINT_LIGHT;
}

void PointLight::renderQuad()
{
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
  //int cone_loc = glGetUniformLocation(program, "lightCone");
  int conedir_loc = glGetUniformLocation(program, "lightConeDirection");
  int shadow_loc = glGetUniformLocation(program, "single_depthMap");
  if (shadow_loc < 0)
    fprintf(stderr, "shadow map loc missing\r\n");
  int BPV_loc = glGetUniformLocation(program, "BPV");
  if (BPV_loc < 0)
    fprintf(stderr, "BPV loc missing\r\n");
  if (lightpos_loc<0)
  {
    fprintf(stderr, "spot light: couldn't find lightpos uniform\r\n");
  }
  if (ambient_loc<0)
  {
    fprintf(stderr, "spot light: couldn't find ambient uniform\r\n");
  }
  if (diffuse_loc<0)
  {
    fprintf(stderr, "spot light: couldn't find diffuse uniform\r\n");
  }
  if (specular_loc<0)
  {
    fprintf(stderr, "spot light: couldn't find specular uniform\r\n");
  }
 // if (cone_loc<0)
 // {
 //   fprintf(stderr, "spot light: couldn't find cone uniform\r\n");
 // }
  if (conedir_loc<0)
  {
    fprintf(stderr, "spot light: couldn't find cone dir uniform\r\n");
  }
  vec4 lpos = vec4(worldpos, 1.0f);
  glUniform4fv(lightpos_loc, 1, &lpos[0]);
  glUniform3fv(ambient_loc, 1, &ambient[0]);
  glUniform3fv(diffuse_loc, 1, &diffuse[0]);
  vec4 combined = vec4(specular, angle);
  glUniform4fv(specular_loc, 1, &combined[0]);
  //glUniform1fv(cone_loc, 1, &angle);
  glUniform3fv(conedir_loc, 1, &direction[0]);
  glUniform1i(shadow_loc, 4);
  glActiveTexture(GL_TEXTURE0 + 4);
  glBindTexture(GL_TEXTURE_2D, depth_map);
  glm::mat4 biasMatrix(
  0.5, 0.0, 0.0, 0.0,
  0.0, 0.5, 0.0, 0.0,
  0.0, 0.0, 0.5, 0.0,
  0.5, 0.5, 0.5, 1.0
  );
  glm::mat4 BPV = biasMatrix*PV;
  glUniform4fv(BPV_loc, 1, &BPV[0][0]);
  glBindTexture(GL_TEXTURE_2D, depth_map);
}

int SpotLight::shadowMap()
{
  glUseProgram(shadowmap_program);
  GLfloat aspect = (GLfloat)SHADOW_WIDTH/(GLfloat)SHADOW_HEIGHT;
  GLfloat near = 1.0f;
  GLfloat far = 250.0f;
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
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth_map, 0);
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
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glCullFace(GL_BACK);
}

unsigned SpotLight::getType()
{
  return SPOT_LIGHT;
}

void SpotLight::renderQuad()
{
  glUseProgram(quad_program);
  GLfloat near = 1.0f;
  GLfloat far = 250.0f;
  int near_loc = glGetUniformLocation(quad_program, "near_plane");
  int far_loc = glGetUniformLocation(quad_program, "far_plane");
  glUniform1fv(near_loc, 1, &near);
  glUniform1fv(far_loc, 1, &far);
  glDisable(GL_DEPTH_TEST);
  //glViewport(0, 0, window_width/4, window_height/4);
  glBindTexture(GL_TEXTURE_2D, depth_map);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
  glDrawArrays(GL_POINTS, 0, 1);
  glEnable(GL_DEPTH_TEST);
  glUseProgram(0);
}

////////////////////////////////////////////////////
DirectionLight::DirectionLight(const char *name_1, vec3 pos_1, vec3 ambient_1, vec3 diffuse_1, vec3 specular_1, vec3 direction_1)
  : Light(name_1, pos_1, ambient_1, diffuse_1, specular_1), direction(direction_1)
{
  glGenFramebuffers(1, &depth_fbo);
  glGenTextures(1, &depth_map);
  glBindTexture(GL_TEXTURE_2D, depth_map);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depth_map, 0);
  glDrawBuffer(GL_NONE);
  if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
  {
    fprintf(stderr, "%s fbo is incomplete\r\n", name);
  }
  glBindTexture(GL_TEXTURE_2D, 0);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  shadowmap_program = directionlight_shadowmap_program;
}

DirectionLight::~DirectionLight()
{
  glDeleteTextures(1, &depth_map);
  glDeleteFramebuffers(1, &depth_fbo);
}

void DirectionLight::updatePos(mat4 *M)
{
  //direction = vec3(*M * vec4(1.0f, 1.0f, 1.0f, 0.0f)) + direction;
  direction = vec3(-1.0f, -1.0f, -1.0f);
  fprintf(stderr, "sun direction: %f %f %f\r\n", direction.x, direction.y, direction.z);
}

void DirectionLight::updateUniforms(unsigned program)
{
  //I hate doing this. Too bad apple doesn't support OGL4.3
  int lightpos_loc = glGetUniformLocation(program, "lightPos");
  int ambient_loc = glGetUniformLocation(program, "lightAmbient");
  int diffuse_loc = glGetUniformLocation(program, "lightDiffuse");
  int specular_loc = glGetUniformLocation(program, "lightSpecular");
  int shadow_loc = glGetUniformLocation(program, "single_depthMap");
  if (shadow_loc < 0)
    fprintf(stderr, "shadow map loc missing\r\n");
  int BPV_loc = glGetUniformLocation(program, "BPV");
  if (BPV_loc < 0)
    fprintf(stderr, "BPV loc missing\r\n");
  vec4 lpos = vec4(direction*-1.0f, 0.0f);
  glUniform4fv(lightpos_loc, 1, &lpos[0]);
  glUniform3fv(ambient_loc, 1, &ambient[0]);
  glUniform3fv(diffuse_loc, 1, &diffuse[0]);
  glUniform3fv(specular_loc, 1, &specular[0]);
  glUniform1i(shadow_loc, 4);
  glActiveTexture(GL_TEXTURE0 + 4);
  glBindTexture(GL_TEXTURE_2D, depth_map);
  glm::mat4 biasMatrix(
  0.5, 0.0, 0.0, 0.0,
  0.0, 0.5, 0.0, 0.0,
  0.0, 0.0, 0.5, 0.0,
  0.5, 0.5, 0.5, 1.0
  );
  glm::mat4 BPV = biasMatrix*PV;
  glUniform4fv(BPV_loc, 1, &BPV[0][0]);
}

int DirectionLight::shadowMap()
{
  glUseProgram(shadowmap_program);
  mat4 V = glm::lookAt(direction, vec3(0,0,0), vec3(0.0f, 1.0f, 0.0f));

  //this will be wrong
  mat4 P = glm::ortho<float>(-100,100,-100,100,-100,100);


  PV = P*V;
  int PV_loc = glGetUniformLocation(shadowmap_program, "PV");
  glUniform4fv(PV_loc, 1, &PV[0][0]);
  glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
//  glBindTexture(GL_TEXTURE_2D, depth_map);
//  glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
//  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
//  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glBindFramebuffer(GL_FRAMEBUFFER, depth_fbo);
//  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth_map, 0);
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

void DirectionLight::renderQuad()
{
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

void DummyLight::renderQuad()
{
}
