#pragma once
#include "inc/gl.h"
#include "GLFW/glfw3.h"

GLFWwindow* createOpenGLWindow(int width, int height, const char* title);
unsigned compileProgram(const char* vshader_src_file, const char* fshader_src_file);
unsigned compileGProgram(const char* vshader_src_file, const char* gshader_src_file, const char* fshader_src_file);
void printOpenGLVersion();
