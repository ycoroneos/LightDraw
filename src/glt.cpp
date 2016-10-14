#include <inc/glt.h>
#include <stdio.h>
#include <cstdio>
#include <cstring>

GLFWwindow* createOpenGLWindow(int width, int height, const char* title) {
    // GLFW creates a window and OpenGL context
    // in a platform-independent manner.
    GLFWwindow* window;
    if (!glfwInit()) {
        printf("Could not init glfw\n");
        return nullptr;
    }

#ifdef __APPLE__
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#else
    //glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    //glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
#endif
    window = glfwCreateWindow(width, height, title, NULL, NULL);
    if (!window) {
        return nullptr;
    }
    // This must be called only once for each window
    glfwMakeContextCurrent(window);

    // You should be running an OpenGL 3.3 context
    printOpenGLVersion();

    // GLEW initializes the OpenGL functions
    // Without this, only a small subset of OpenGL's
    // functionality would be available
#ifndef __APPLE__
    GLenum err = glewInit();
    if (GLEW_OK != err) {
        //Problem: glewInit failed, something is wrong.
        fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
        return nullptr;
    }
#endif

    // This line is optional, but very useful for debugging
#ifndef __APPLE__
//    setupDebugPrint();
#endif
    return window;
}

static bool compileShader(GLuint handle, GLenum stype, const char* src)
{
	int shader_len = (int)strlen(src);
	glShaderSource(handle, 1, &src, &shader_len);
	glCompileShader(handle);

	int status;
	glGetShaderiv(handle, GL_COMPILE_STATUS, &status);
	if (!status) {
		char buff[2048];
		int nwritten;
		glGetShaderInfoLog(handle, 2048, &nwritten, buff);

		const char* typelabel = stype == GL_VERTEX_SHADER ? "vertex" : (stype == GL_FRAGMENT_SHADER ? "fragment" : "unknown");
		fprintf(stderr,"Error in %s shader\n%s\n", typelabel, buff);
		return false;
	}
	return true;
}

// caller must free returned shader object
static int compileShader(GLenum type, const char* src)
{
	GLuint handle = glCreateShader(type);
	compileShader(handle, type, src);
	return handle;
}

static bool linkProgram(GLuint handle, GLuint vshader, GLuint fshader)
{
	glAttachShader(handle, vshader);
	glAttachShader(handle, fshader);
	glLinkProgram(handle);
	int success;
	glGetProgramiv(handle, GL_LINK_STATUS, &success);
	if (!success) {
		char buff[2048];
		int nwritten;
		glGetProgramInfoLog(handle, 2048, &nwritten, buff);
		fprintf(stderr, "Program link error:\n%s\n", buff);
		return false;
	}
	return true;
}

unsigned compileProgram(const char* vshader_src_file, const char* fshader_src_file)
{
  FILE *f_vshader, *f_fshader;
  char *vshader_src, *fshader_src;
  unsigned length=0;
  //read vshader
  f_vshader = fopen(vshader_src_file, "rb");
  if (!f_vshader)
  {
    fprintf(stderr,"error reading vertex shader %s\n", vshader_src_file);
    return 0;
  }
  fseek(f_vshader, 0, SEEK_END);
  length=ftell(f_vshader);
  fseek(f_vshader, 0, SEEK_SET);
  vshader_src = new char[length];
  fread(vshader_src, 1, length, f_vshader);
  fclose(f_vshader);
  fprintf(stderr,"read vshader as:\n%s\n", vshader_src);

  //read fshader
  f_fshader = fopen(fshader_src_file, "rb");
  if (!f_fshader)
  {
    fprintf(stderr,"error reading fragment shader %s\n", fshader_src_file);
    return 0;
  }
  fseek(f_fshader, 0, SEEK_END);
  length=ftell(f_fshader);
  fseek(f_fshader, 0, SEEK_SET);
  fshader_src = new char[length];
  fread(fshader_src, 1, length, f_fshader);
  fclose(f_fshader);
  fprintf(stderr,"read fshader as:\n%s\n", fshader_src);

  //link program
	GLuint program = glCreateProgram();
	GLuint vshader = compileShader(GL_VERTEX_SHADER, vshader_src);
	GLuint fshader = compileShader(GL_FRAGMENT_SHADER, fshader_src);
	if (!linkProgram(program, vshader, fshader)) {
		glDeleteProgram(program);
		program = 0;
    fprintf(stderr, "killed program\n");
	}
	// once a program is linked
	// shader objects should be deleted
  delete [] vshader_src;
  delete [] fshader_src;
	glDeleteShader(vshader);
	glDeleteShader(fshader);
	return program;
}

void printOpenGLVersion()
{
    int major;
    int minor;
    glGetIntegerv(GL_MAJOR_VERSION, &major);
    glGetIntegerv(GL_MINOR_VERSION, &minor);
    printf("Running OpenGL %d.%d\n", major, minor);
}
