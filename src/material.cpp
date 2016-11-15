#include "inc/material.h"
#include <inc/gl.h>
#include <vector>
#include <glm/glm.hpp>
#include <inc/vertex.h>
#include "stdio.h"
#include "string.h"
#include "SOIL/SOIL.h"
using namespace glm;

struct color
{
  unsigned char r;
  unsigned char g;
  unsigned char b;
};

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
  struct color *flipped = (struct color*) malloc(width*height*sizeof(struct color));
//  memcpy(flipped, image, width*height*3);
  fprintf(stderr, "size of color is %d\r\n", sizeof(struct color));
  for (int w=0; w<width; ++w)
  {
    for (int h=0; h<height; ++h)
    {
      int index = h*width + w;
      int flipped_index = (height-h-1)*width + w;
      struct color *cast = (struct color *)image;
      flipped[flipped_index]=cast[index];
    }
  }
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, (void*)flipped);
  SOIL_free_image_data(image);
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
