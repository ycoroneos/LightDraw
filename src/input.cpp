#include "inc/input.h"
#include "inc/gl.h"
#include "GLFW/glfw3.h"
#include <vector>
#include <algorithm>

static std::vector<InputResponder*> actors;

InputResponder::InputResponder() : takesinput(false)
{
  actors.push_back(this);
}

InputResponder::~InputResponder()
{
  std::vector<InputResponder*>::iterator position = std::find(actors.begin(), actors.end(), this);
  if (position != actors.end())
  {
    actors.erase(position);
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
  for (unsigned int i=0; i<actors.size(); ++i)
  {
    InputResponder *actor = actors[i];
    if (actor->takesInput())
    {
      actor->doKeyboardInput(key, scancode, action, mods);
    }
  }
}
