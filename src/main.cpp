#include "inc/gl.h"
#include "GLFW/glfw3.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "inc/voxel.h"
#include "inc/glt.h"
#include "inc/scene.h"
#include "inc/input.h"
#include "lodepng.h"
#include <stdio.h>
#include <cstring>
void encodeOneStep(const char* filename, const unsigned char* image, unsigned width, unsigned height);

glm::mat4 Projection;

int window_width;
int window_height;

bool screenshot=false;

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
    bool uselidr=true;
    bool shadows=false;
    bool record=false;
    for (int i=0; i<argc; ++i)
    {
      if (strcmp(argv[i], "benchmark")==0)
      {
        benchmark=true;
      }
      if (strcmp(argv[i], "forward")==0)
      {
        uselidr=false;
      }
      if (strcmp(argv[i], "shadows")==0)
      {
        shadows=true;
      }
      if (strcmp(argv[i], "record")==0)
      {
        record=true;
      }
    }
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    int width = 3840;
    int height = 2160;
    window_width = width;
    window_height = height;
    window = createOpenGLWindow(width, height,"tears_have_been_shed");
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    window_size_callback(window, width, height);
    glfwSetWindowSizeCallback(window, window_size_callback);
    glfwSetKeyCallback(window, keyCallback);
    glfwSetCursorPosCallback(window, mouseCallback);
    glfwSetWindowPos(window, 0, 0);
    glfwSwapInterval(0);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    /* Loop until the user closes the window */
    if (initScene(Projection, benchmark, uselidr, shadows)<0)
    {
      cleanupScene();
      glfwTerminate();
      return -1;
    }
    unsigned char *pixels = new unsigned char[width*height*4*4];
    double time = glfwGetTime();
    int framecount=0;
    double hundred_time = time;
    int writeframe=0;
    while (!glfwWindowShouldClose(window))
    {
        /* Poll for and process events */
        glfwPollEvents();

        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        double newtime = glfwGetTime();
        double diff = newtime - time;
        time=newtime;
       // if (framecount>100)
       // {
       //   fprintf(stderr, "drew 100 frames in %f seconds -> %f fps\r\n", newtime - hundred_time, 100.0f/(newtime - hundred_time));
       //   hundred_time=newtime;
       //   framecount=0;
       // }
        //draw
        if (record)
        {
          drawScene(1.0/60.0f, uselidr);
        }
        //draw
        if (record)
        {
          drawScene(1.0/60.0f, uselidr);
        }
        else
        {
          drawScene(diff, uselidr);
        }
        double frametime = glfwGetTime();
        //record maybe
        if (record || screenshot)
        {
          double distime = glfwGetTime();
          glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
          glReadPixels(0, 0, width*2, height*2, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
          //glReadPixels(0, 0, width*2, height*2, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, pixels);
          char name[50];
          sprintf(name, "frame_%d.png", writeframe);
          encodeOneStep(name, (const unsigned char*)pixels, width*2, height*2);
          fprintf(stderr, "screenshot %d %d\r\n", width, height);
          ++writeframe;
          screenshot=false;
          glfwSetTime(distime);
        }

        /* Swap front and back buffers */
        glfwSwapBuffers(window);
        //fprintf(stderr, "frametime: %f swaptime: %f\r\n", frametime-newtime, glfwGetTime()-frametime);
        ++framecount;

    }

    if (record)
    {
      delete[] pixels;
    }
    cleanupScene();
    glfwTerminate();
    return 0;
}

void encodeOneStep(const char* filename, const unsigned char* image, unsigned width, unsigned height)
{
  /*Encode the image*/
  unsigned error = lodepng_encode32_file(filename, image, width, height);

  /*if there's an error, display it*/
  if(error) printf("error %u: %s\n", error, lodepng_error_text(error));
}

