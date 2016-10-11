#include "inc/input.h"
#include "inc/gl.h"
#include "GLFW/glfw3.h"
#include <vector>
#include <algorithm>
#include "stdio.h"

InputResponder *actors[100];
unsigned nactors=0;

InputResponder::InputResponder() : takesinput(false)
{
  actors[nactors]=this;
  ++nactors;
}

InputResponder::~InputResponder()
{
  for (unsigned i=0; i<nactors; ++i)
  {
    if (this == actors[i])
    {
      actors[i]=NULL;
    }
  }
}

void InputResponder::enableInput()
{
  takesinput=true;
}

void InputResponder::disableInput()
{
  takesinput=false;
}

bool InputResponder::takesInput()
{
  return takesinput;
}

void mouseCallback(GLFWwindow* window, double xpos, double ypos)
{
  for (unsigned int i=0; i<nactors; ++i)
  {
    InputResponder *actor = actors[i];
    if (actor->takesInput())
    {
      actor->doMouseInput(xpos, ypos);
    }
  }
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
  switch (key)
  {
   case GLFW_KEY_ESCAPE:
     glfwSetWindowShouldClose(window, GLFW_TRUE);
     return;
     break;
  }
  for (unsigned int i=0; i<nactors; ++i)
  {
    InputResponder *actor = actors[i];
    if (actor->takesInput())
    {
      actor->doKeyboardInput(key, scancode, action, mods);
    }
  }
}
