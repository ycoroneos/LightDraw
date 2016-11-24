#include <inc/gl.h>
#include <inc/lidr.h>
#include <inc/light.h>
#include <inc/camera.h>
#include "stdio.h"
#include "string.h"
#include "stdlib.h"
extern int window_width;
extern int window_height;
extern int viewport_program;

LIDR::LIDR(int z_program_1, int lightvolume_program_1)
  :z_program(z_program_1), lightvolume_program(lightvolume_program_1)
{
  glGenFramebuffers(1, &depth_fbo);
  glGenTextures(1, &depth_map);
  glGenTextures(1, &volume_map);
  glGenTextures(1, &light_map);

  //first generate light lookup map
  glBindTexture(GL_TEXTURE_1D, light_map);
  glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA32F, maxlights*sizeof(vec4), 0, GL_RGBA, GL_FLOAT, 0);
  glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

  //depth
  glBindTexture(GL_TEXTURE_2D, depth_map);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, window_width, window_height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

  //bind it
  glBindFramebuffer(GL_FRAMEBUFFER, depth_fbo);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth_map, 0);

  //color volumes
  glBindTexture(GL_TEXTURE_2D, volume_map);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, window_width, window_height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

  //bind it
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, volume_map, 0);

  //check status
  GLenum fbostatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
  if (fbostatus != GL_FRAMEBUFFER_COMPLETE)
  {
    fprintf(stderr, "LIDR depth FBO is incomplete\r\n");
    exit(-1);
  }
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

LIDR::~LIDR()
{
  glDeleteTextures(1, &light_map);
  glDeleteTextures(1, &volume_map);
  glDeleteTextures(1, &depth_map);
  glDeleteFramebuffers(1, &depth_fbo);
}


int LIDR::ZPrePass(Camera *camera)
{
  glBindFramebuffer(GL_FRAMEBUFFER, depth_fbo);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glUseProgram(z_program);
  camera->updateUniforms(z_program);
  return z_program;
}

int LIDR::LightVolumes()
{
  glBindFramebuffer(GL_FRAMEBUFFER, depth_fbo);
  glUseProgram(lightvolume_program);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, depth_map);
  glDepthMask(GL_FALSE);
  return lightvolume_program;
}

void LIDR::LightVolumesEnd()
{
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glDepthMask(GL_TRUE);
}


void LIDR::packLightTextures(std::vector<Light *> lights)
{
}

void LIDR::cornerWindow()
{
  int smallwidth = window_width/4;
  int smallheight = window_height/4;

  //setup state
  glUseProgram(viewport_program);
  glDisable(GL_DEPTH_TEST);

  //draw the z buffer
  glViewport(0, 0, smallwidth, smallheight);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, depth_map);
  glDrawArrays(GL_TRIANGLES, 0, 3);

  //draw the light volumes
  glViewport(smallwidth, 0, smallwidth, smallheight);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, volume_map);
  glDrawArrays(GL_TRIANGLES, 0, 3);

  //restore state
  glEnable(GL_DEPTH_TEST);
  glUseProgram(0);
  glViewport(0, 0, window_width, window_height);
}
