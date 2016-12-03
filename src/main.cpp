#include "inc/gl.h"
#include "GLFW/glfw3.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "inc/voxel.h"
#include "inc/glt.h"
#include "inc/scene.h"
#include "inc/input.h"
#include <stdio.h>
#include <cstring>

glm::mat4 Projection;

int window_width;
int window_height;

void setViewport(GLFWwindow* window)
{
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);

    window_width=width;
    window_height=height;
    glViewport(0,0,width, height);
}

void window_size_callback(GLFWwindow* window, int width, int height)
{

  setViewport(window);
  float fovy = 45.0;
  float nearz = 0.1f;
  float farz = 1000.0f;
  float aspect = float(width)/float(height);
  Projection = glm::perspective(fovy, aspect, nearz, farz);
}

int main(int argc, char **argv)
{
    //parse input arguments
    bool benchmark=false;
    for (int i=0; i<argc; ++i)
    {
      if (strcmp(argv[i], "benchmark")==0)
      {
        benchmark=true;
      }
    }
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    int width = 1920;
    int height = 1080;
    window_width = width;
    window_height = height;
    window = createOpenGLWindow(width, height,"tears_have_been_shed");
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    window_size_callback(window, width, height);
    glfwSetWindowSizeCallback(window, window_size_callback);
    glfwSetKeyCallback(window, keyCallback);
    glfwSetCursorPosCallback(window, mouseCallback);
    glfwSetWindowPos(window, 0, 0);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    /* Loop until the user closes the window */
    if (initScene(Projection, benchmark)<0)
    {
      cleanupScene();
      glfwTerminate();
      return -1;
    }
    double time = glfwGetTime();
    int framecount=0;
    double hundred_time = time;
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        double newtime = glfwGetTime();
        double diff = newtime - time;
        time=newtime;
        if (framecount>100)
        {
          fprintf(stderr, "drew 100 frames in %f seconds\r\n", newtime - hundred_time);
          hundred_time=newtime;
          framecount=0;
        }
        //draw
        drawScene(diff);

        /* Swap front and back buffers */
        glfwSwapBuffers(window);
        ++framecount;

        /* Poll for and process events */
        glfwPollEvents();
    }

    cleanupScene();
    glfwTerminate();
    return 0;
}
