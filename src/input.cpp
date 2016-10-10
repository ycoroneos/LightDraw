#include "inc/input.h"
#include "inc/gl.h"
#include "GLFW/glfw3.h"
#include <vector>
#include <algorithm>
#include "stdio.h"

static std::vector<InputResponder*> actors;
InputResponder *test;

InputResponder::InputResponder() : takesinput(false)
{
//  printf("added actor\n");
  actors.push_back(this);
//  printf("%d actors\n", actors.size());
  test=this;
 // printf(" p %x\n", test);
}

InputResponder::~InputResponder()
{
//  printf("bye bye actor\n");
  std::vector<InputResponder*>::iterator position = std::find(actors.begin(), actors.end(), this);
  if (position != actors.end())
  {
    actors.erase(position);
  }
}

void InputResponder::enableInput()
{
//  printf("takes input\n");
//  printf("%d actors\n", actors.size());
//  printf("p %x\n", test);
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
  for (unsigned int i=0; i<actors.size(); ++i)
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
//  printf("key callback, %d actors\n", actors.size());
  for (unsigned int i=0; i<actors.size(); ++i)
  {
    InputResponder *actor = actors[i];
    if (actor->takesInput())
    {
      actor->doKeyboardInput(key, scancode, action, mods);
    }
  }
  if (test->takesInput())
  {
    test->doKeyboardInput(key, scancode, action, mods);
  }
}
