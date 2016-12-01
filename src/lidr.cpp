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
  glGenVertexArrays(1, &dummyvao);
  glGenFramebuffers(1, &depth_fbo);
  glGenTextures(1, &depth_map);
  glGenTextures(1, &volume_map);
  glGenTextures(1, &light_ambient_tex);
  glGenTextures(1, &light_diffuse_tex);
  glGenTextures(1, &light_specular_tex);
  glGenTextures(1, &light_position_tex);
  glActiveTexture(GL_TEXTURE0);

  //first generate light color lookup texture
  //ambient:96bits
  glBindTexture(GL_TEXTURE_1D, light_ambient_tex);
  glTexImage1D(GL_TEXTURE_1D, 0, GL_RGB32F, maxlights, 0, GL_RGB, GL_FLOAT, 0);
  glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  //diffuse:96bits
  glBindTexture(GL_TEXTURE_1D, light_diffuse_tex);
  glTexImage1D(GL_TEXTURE_1D, 0, GL_RGB32F, maxlights, 0, GL_RGB, GL_FLOAT, 0);
  glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  //specular:96bits
  glBindTexture(GL_TEXTURE_1D, light_specular_tex);
  glTexImage1D(GL_TEXTURE_1D, 0, GL_RGB32F, maxlights, 0, GL_RGB, GL_FLOAT, 0);
  glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);


  //then generate the light position texture
  //position:96bits
  //1/radius:32bits
  glBindTexture(GL_TEXTURE_1D, light_position_tex);
  glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA32F, maxlights, 0, GL_RGBA, GL_FLOAT, 0);
  glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  //depth
  glBindTexture(GL_TEXTURE_2D, depth_map);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, window_width, window_height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  //bind it
  glBindFramebuffer(GL_FRAMEBUFFER, depth_fbo);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth_map, 0);

  //color volumes
  glBindTexture(GL_TEXTURE_2D, volume_map);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, window_width, window_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

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
  glDeleteTextures(1, &light_position_tex);
  glDeleteTextures(1, &light_ambient_tex);
  glDeleteTextures(1, &light_diffuse_tex);
  glDeleteTextures(1, &light_specular_tex);
  glDeleteTextures(1, &volume_map);
  glDeleteTextures(1, &depth_map);
  glDeleteFramebuffers(1, &depth_fbo);
  glDeleteVertexArrays(1, &dummyvao);
}


int LIDR::ZPrePass(Camera *camera)
{
  glBindFramebuffer(GL_FRAMEBUFFER, depth_fbo);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glColorMaski(0, GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
  glUseProgram(z_program);
  camera->updateUniforms(z_program);
  return z_program;
}

int LIDR::LightVolumes()
{
  glBindFramebuffer(GL_FRAMEBUFFER, depth_fbo);
  glColorMaski(0, GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
  glUseProgram(lightvolume_program);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, depth_map);
  glDepthMask(GL_FALSE);
  return lightvolume_program;
}

void LIDR::LightVolumesEnd()
{
  glDepthMask(GL_TRUE);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  //bind the light index texture
  glActiveTexture(GL_TEXTURE2);
  glBindTexture(GL_TEXTURE_2D, volume_map);
}


//lets be silly and pack everything for now
void LIDR::packLightTextures(std::vector<Light *> lights)
{
  //update light colors and positions
  int nlights = lights.size();
  vec3 *light_ambient = new vec3[nlights+1];
  vec3 *light_diffuse = new vec3[nlights+1];
  vec3 *light_specular = new vec3[nlights+1];
  vec4 *light_pos_radius = new vec4[nlights+1];
  for (int i=0; i<nlights; ++i)
  {
    light_ambient[i+1] = lights[i]->getAmbient();
    light_diffuse[i+1] = lights[i]->getDiffuse();
    light_specular[i+1] = lights[i]->getSpecular();
    light_pos_radius[i+1] = vec4(lights[i]->getWorldPos(), lights[i]->getRadius());
  }
  glActiveTexture(GL_TEXTURE3);
  glBindTexture(GL_TEXTURE_1D, light_ambient_tex);
  glTexSubImage1D(GL_TEXTURE_1D, 0, 0, nlights+1, GL_RGB, GL_FLOAT, &light_ambient[0]);

  glActiveTexture(GL_TEXTURE4);
  glBindTexture(GL_TEXTURE_1D, light_diffuse_tex);
  glTexSubImage1D(GL_TEXTURE_1D, 0, 0, nlights+1, GL_RGB, GL_FLOAT, &light_diffuse[0]);

  glActiveTexture(GL_TEXTURE5);
  glBindTexture(GL_TEXTURE_1D, light_specular_tex);
  glTexSubImage1D(GL_TEXTURE_1D, 0, 0, nlights+1, GL_RGB, GL_FLOAT, &light_specular[0]);

  glActiveTexture(GL_TEXTURE6);
  glBindTexture(GL_TEXTURE_1D, light_position_tex);
  glTexSubImage1D(GL_TEXTURE_1D, 0, 0, nlights+1, GL_RGBA, GL_FLOAT, &light_pos_radius[0]);


  delete[] light_ambient;
  delete[] light_diffuse;
  delete[] light_specular;
  delete[] light_pos_radius;
}

void LIDR::cornerWindow()
{
  int smallwidth = window_width/4;
  int smallheight = window_height/4;

  //setup state
  glBindVertexArray(dummyvao);
  glUseProgram(viewport_program);
  //glEnable(GL_SCISSOR_TEST);
  glDisable(GL_DEPTH_TEST);

  //draw the z buffer
  glViewport(smallwidth, 0, smallwidth, smallheight);
  //glScissor(0, 0, smallwidth, smallheight);
  //glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, depth_map);
  glDrawArrays(GL_TRIANGLES, 0, 3);

  //draw the light volumes
  glViewport(smallwidth*2, 0, smallwidth, smallheight);
  //glScissor(smallwidth, 0, smallwidth, smallheight);
  //glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, volume_map);
  glDrawArrays(GL_TRIANGLES, 0, 3);

  //restore state
  glEnable(GL_DEPTH_TEST);
  //glDisable(GL_SCISSOR_TEST);
  glUseProgram(0);
  glViewport(0, 0, window_width, window_height);
  glBindVertexArray(0);
}

void LIDR::textureWindow(GLuint texture)
{
  int smallwidth = window_width/4;
  int smallheight = window_height/4;

  //setup state
  glBindVertexArray(dummyvao);
  glUseProgram(viewport_program);
  glDisable(GL_DEPTH_TEST);

  glViewport(0, 0, smallwidth, smallheight);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, texture);
  glDrawArrays(GL_TRIANGLES, 0, 3);
  //restore state
  glEnable(GL_DEPTH_TEST);
  glUseProgram(0);
  glViewport(0, 0, window_width, window_height);
  glBindVertexArray(0);
}
