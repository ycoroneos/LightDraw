#include "inc/material.h"
#include <inc/gl.h>
#include <vector>
#include <glm/glm.hpp>
#include <inc/vertex.h>
#include "stdio.h"
#include "string.h"
#include "SOIL/SOIL.h"
using namespace glm;

Material::Material()
{
}

Material::Material(const char *filename)
{
  strncpy(name, filename, sizeof(name));
  fprintf(stderr, "loading texture %s\r\n", filename);
  glGenTextures(1,&texID);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, texID);
  int width, height;
  unsigned char *image = SOIL_load_image(filename, &width, &height, 0, SOIL_LOAD_RGB);
  if (image == NULL)
  {
    fprintf(stderr,"failed to load texture\r\n");
    return;
  }
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
  SOIL_free_image_data(image);
  texID = SOIL_load_OGL_texture(filename, SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y);
  if (texID<0)
  {
    fprintf(stderr, "failed to load texture %s\r\n", filename);
    return;
  }
  fprintf(stderr, "loaded texture %s\r\n", filename);
  refcount=0;
}

Material::~Material()
{
  glDeleteTextures(1, &texID);
}

void Material::Use(int program)
{
  int texLoc = glGetUniformLocation(program, "texture_obj");
  if (texLoc<0)
  {
    fprintf(stderr,"cant find texture uniform\r\n");
  }
  glUniform1i(texLoc, 0);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, texID);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

DummyMat::DummyMat()
{
}

void DummyMat::Use(int program)
{
}

void Material::incRef()
{
  refcount+=1;
}

void Material::decRef()
{
  refcount-=1;
}
