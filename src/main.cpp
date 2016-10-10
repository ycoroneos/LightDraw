#include "inc/gl.h"
#include "GLFW/glfw3.h"
#include <glm/glm.hpp>
#include "inc/voxel.h"
#include "inc/glt.h"
#include "inc/scene.h"
#include "inc/input.h"
#include <stdio.h>

void window_size_callback(GLFWwindow* window, int width, int height)
{

  // make sure the viewport is square-shaped.
  if (width > height) {
      int offsetx = (width - height) / 2;
      glViewport(offsetx, 0, height, height);
  } else {
      int offsety = (height - width) / 2;
      glViewport(0, offsety, width, width);
  }
}

int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    window = createOpenGLWindow(640,480,"voxl");
    window_size_callback(window, 640, 480);
    glfwSetWindowSizeCallback(window, window_size_callback);
    glfwSetKeyCallback(window, keyCallback);
    glfwSetCursorPosCallback(window, mouseCallback);
    glEnable(GL_DEPTH_TEST);

    /* Loop until the user closes the window */
    initScene();
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        //draw
        drawScene();

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    cleanupScene();
    glfwTerminate();
    return 0;
}
