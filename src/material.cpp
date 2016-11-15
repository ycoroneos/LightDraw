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
  //glGenTextures(1,&texID);
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

void Material::Use()
{
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, texID);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
}

DummyMat::DummyMat()
{
}

void DummyMat::Use()
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
