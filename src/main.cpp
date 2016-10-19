#include "inc/gl.h"
#include "GLFW/glfw3.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "inc/voxel.h"
#include "inc/glt.h"
#include "inc/scene.h"
#include "inc/input.h"
#include <stdio.h>

glm::mat4 Projection;

void setViewport(GLFWwindow* window)
{
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);

    glViewport(0,0,width, height);
}

void window_size_callback(GLFWwindow* window, int width, int height)
{

  setViewport(window);
  float fovy = 45.0;
  float nearz = 0.1f;
  float farz = 100.0f;
  float aspect = float(width)/float(height);
  Projection = glm::perspective(fovy, aspect, nearz, farz);
}

int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    window = createOpenGLWindow(800,600,"voxl");
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    window_size_callback(window, 800, 600);
    glfwSetWindowSizeCallback(window, window_size_callback);
    glfwSetKeyCallback(window, keyCallback);
    glfwSetCursorPosCallback(window, mouseCallback);
    glEnable(GL_DEPTH_TEST);
    //glEnable(GL_CULL_FACE);

    /* Loop until the user closes the window */
    initScene();
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        //draw
        drawScene(Projection);

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    cleanupScene();
    glfwTerminate();
    return 0;
}
