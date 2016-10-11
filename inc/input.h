#pragma once
#include "inc/gl.h"
#include "GLFW/glfw3.h"
#include <vector>

void mouseCallback(GLFWwindow* window, double xpos, double ypos);
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

class InputResponder
{
  public:
    InputResponder();
    ~InputResponder();
    virtual void doMouseInput(double xpos, double ypos) = 0;
    virtual void doKeyboardInput(int key, int scancode, int action, int mods) = 0;
    void enableInput();
    void disableInput();
    bool takesInput();
  private:
    bool takesinput=false;
};
