#pragma once
#include <inc/gl.h>
#include <vector>
#include <map>
#include "inc/glt.h"
using namespace std;

struct Constraint
{
  const char *uniform_name;
  int location;
};

class ShaderLib
{
  public:
    ShaderLib();
    ~ShaderLib();
    int loadShader(const char *vshader, const char *gshader, const char *fshader);
    int loadShader(const char *vshader, const char *fshader);
    int getShaderByName(string name);
  private:
    int canonicalize(string shadername);
    map<string, int> programs;
};
