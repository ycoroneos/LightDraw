#include <inc/gl.h>
#include <inc/lidr.h>
#include <inc/camera.h>
#include "stdio.h"
#include "string.h"
#include "stdlib.h"
extern int window_width;
extern int window_height;

LIDR::LIDR(int z_program_1, int lightvolume_program_1)
  :z_program(z_program_1), lightvolume_program(lightvolume_program_1)
{
  glGenFramebuffers(1, &depth_fbo);
  glGenTextures(1, &depth_map);
  glGenTextures(1, &volume_map);

  //depth
  glBindTexture(GL_TEXTURE_2D, depth_map);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, window_width, window_height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glBindFramebuffer(GL_FRAMEBUFFER, depth_fbo);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth_map, 0);

  //color volumes
  glBindTexture(GL_TEXTURE_2D, volume_map);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, window_width, window_height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

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
  return lightvolume_program;
}
