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
  //invert image along y axis
  unsigned short *flipped = (unsigned short*) malloc(width*height*sizeof(unsigned short)*3);
  for (int h=0; h<height; ++h)
  {
    for (int w=0; w<width; ++w)
    {
      for (int color=0; color<3; ++color)
      {
        int index = h*height + w;
        int flipped_index = (h)*height + w;
        flipped[flipped_index*3 + color]=image[index*3 + color];
      }
    }
  }
  SOIL_free_image_data(image);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, (void*)flipped);
  free(flipped);
  fprintf(stderr, "loaded texture %s\r\n", filename);
  refcount=0;
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glGenerateMipmap(GL_TEXTURE_2D);
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
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  //glGenerateMipmap(GL_TEXTURE_2D);
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
