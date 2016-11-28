#include <inc/shaderlib.h>
#include <vector>
#include <map>
#include "inc/glt.h"
#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include <iostream>
using namespace std;




static struct Constraint texture_constraints[] = {
  {"texmap", 0},
  {"texture_obj", 0},
  {"lightindex_tex",2},
  {"lightambient_tex", 3},
  {"lightdiffuse_tex", 4},
  {"lightspecular_tex", 5},
  {"lightposition_tex", 6},
};

#define NCONSTRAINTS sizeof(texture_constraints)/sizeof(Constraint)

ShaderLib::ShaderLib()
{
}

ShaderLib::~ShaderLib()
{
  map<string, int>::iterator it;
  for (it=programs.begin(); it!=programs.end(); ++it)
  {
    glDeleteProgram(it->second);
  }
}

//vshader must contain the name
int ShaderLib::loadShader(const char *vshader, const char *gshader, const char *fshader)
{
  char *vshader_src = read_file(vshader);
  char *name = strstr(vshader_src, "//name=");
  if (name==NULL)
  {
    fprintf(stderr, "[ShaderLib]: could not find the name of shader %s\r\n", vshader);
    free(vshader_src);
    exit(-1);
    return -1;
  }
  name = name + 7;
  char *name_end = strchr(name, '\n');
  char *_shader_name = (char *)malloc(name_end - name + 1);
  strncpy(_shader_name, name, name_end - name);
  _shader_name[name_end-name]='\0';
  string shader_name;
  shader_name.assign(_shader_name, strlen(_shader_name));
  free(_shader_name);
  free(vshader_src);
  int program = compileGProgram(vshader, gshader, fshader);
  if (program < 0)
  {
    fprintf(stderr, "[ShaderLib]: could not compile %s\r\n", shader_name.c_str());
    exit(-1);
    return -1;
  }
  programs[shader_name] = program;
  canonicalize(shader_name);
  return program;
}

//vshader must contain the name
int ShaderLib::loadShader(const char *vshader, const char *fshader)
{
  char *vshader_src = read_file(vshader);
  char *name = strstr(vshader_src, "//name=");
  if (name==NULL)
  {
    fprintf(stderr, "[ShaderLib]: could not find the name of shader %s\r\n", vshader);
    free(vshader_src);
    exit(-1);
    return -1;
  }
  name+=7;
  char *name_end = strchr(name, '\n');
  char *_shader_name = (char *)malloc(name_end - name + 1);
  strncpy(_shader_name, name, name_end - name);
  _shader_name[name_end-name]='\0';
  string shader_name;
  shader_name.assign(_shader_name, strlen(_shader_name));
  free(_shader_name);
  free(vshader_src);
  int program = compileProgram(vshader, fshader);
  if (program < 0)
  {
    fprintf(stderr, "[ShaderLib]: could not compile %s\r\n", shader_name.c_str());
    exit(-1);
    return -1;
  }
  programs[shader_name] = program;
  canonicalize(shader_name);
  return program;
}

int ShaderLib::getShaderByName(string name)
{
  if(programs.find(name) != programs.end())
  {
    return programs[name];
  }
  return -1;
}

int ShaderLib::canonicalize(string shadername)
{
  int program = programs[shadername];
  glUseProgram(program);
  for(int i=0; i<NCONSTRAINTS; ++i)
  {
    int texloc = glGetUniformLocation(program, texture_constraints[i].uniform_name);
    if (texloc < 0)
    {
      fprintf(stderr, "[ShaderLib]: warning could not find texture constraint %s in shader %s\r\n", texture_constraints[i].uniform_name, shadername.c_str());
    }
    else
    {
      glUniform1i(texloc, texture_constraints[i].location);
    }
  }
  glUseProgram(0);
  return 0;
}
