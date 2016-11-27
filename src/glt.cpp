#include <inc/glt.h>
#include <stdio.h>
#include <cstdio>
#include <cstring>
#include "stdlib.h"


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
    setupDebugPrint();
#endif
    return window;
}

#ifndef __APPLE__
static void GLAPIENTRY gl_dbg_callback(GLenum source,
    GLenum type,
    GLuint id,
    GLenum severity,
    GLsizei length,
    const GLchar* message,
    const void* userParam)
{
   const char* msg = "Type = %d, id = %d, severity = %d, %s\n";
   //fprintf(stderr, msg, type, id, severity, message);
   //exit(-1);
    if (severity > GL_DEBUG_SEVERITY_NOTIFICATION) {
        // below some spammy ids that you might want to filter 
        //id != 131204 && id != 131076 && id != 131184 && 
        //id != 131186 && id != 131188 && id != 131154
        if (id != 131076) {
            const char* msg = "Type = %d, id = %d, severity = %d, %s\n";
            fprintf(stderr, msg, type, id, severity, message);
        }
    }
}

void setupDebugPrint()
{
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE,
        GL_DONT_CARE, 0, nullptr, GL_TRUE);
    glDebugMessageCallback(gl_dbg_callback, nullptr);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
}
#endif

char *read_file(const char *fn)
{
	FILE *fp;
	char *content = NULL;

	int count=0;

	if (fn != NULL) {

		fp = fopen(fn,"rt");
		if (fp != NULL) {

			fseek(fp, 0, SEEK_END);
			count = ftell(fp);
			rewind(fp);

			if (count > 0) {
				content = (char *)malloc(sizeof(char) * (count+1));
				count = fread(content,sizeof(char),count,fp);
				content[count] = '\0';
			}
			fclose(fp);
		}
	}
	return content;
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

static bool linkProgramGShader(GLuint handle, GLuint vshader, GLuint gshader, GLuint fshader)
{
	glAttachShader(handle, vshader);
  glAttachShader(handle, gshader);
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

int compileProgram(const char* vshader_src_file, const char* fshader_src_file)
{

  char *vshader_text = read_file(vshader_src_file);
  if (vshader_text == NULL)
  {
    fprintf(stderr,"error reading vertex shader %s\n", vshader_src_file);
    return -1;
  }

  char *fshader_text = read_file(fshader_src_file);
  if (fshader_text == NULL)
  {
    fprintf(stderr,"error reading fragment shader %s\n", fshader_src_file);
    return -1;
  }


  //link program
	GLuint program = glCreateProgram();
	GLuint vshader = compileShader(GL_VERTEX_SHADER, vshader_text);
	GLuint fshader = compileShader(GL_FRAGMENT_SHADER, fshader_text);
	if (!linkProgram(program, vshader, fshader)) {
  fprintf(stderr,"read vshader as:\n%s\n", vshader_text);
  fprintf(stderr,"read fshader as:\n%s\n", fshader_text);
		glDeleteProgram(program);
		program = 0;
    fprintf(stderr, "killed program\n");
	}
	// once a program is linked
	// shader objects should be deleted
  free(vshader_text);
  free(fshader_text);
	glDeleteShader(vshader);
	glDeleteShader(fshader);
	return program;
}

int compileGProgram(const char* vshader_src_file, const char* gshader_src_file, const char* fshader_src_file)
{
  char *vshader_text = read_file(vshader_src_file);
  if (vshader_text == NULL)
  {
    fprintf(stderr,"error reading vertex shader %s\n", vshader_src_file);
    return -1;
  }

  char *gshader_text = read_file(gshader_src_file);
  if (gshader_text == NULL)
  {
    fprintf(stderr,"error reading geometry shader %s\n", gshader_src_file);
    return -1;
  }

  char *fshader_text = read_file(fshader_src_file);
  if (fshader_text == NULL)
  {
    fprintf(stderr,"error reading fragment shader %s\n", fshader_src_file);
    return -1;
  }


  //link program
	GLuint program = glCreateProgram();
	GLuint vshader = compileShader(GL_VERTEX_SHADER, vshader_text);
	GLuint gshader = compileShader(GL_GEOMETRY_SHADER, gshader_text);
	GLuint fshader = compileShader(GL_FRAGMENT_SHADER, fshader_text);
	if (!linkProgramGShader(program, vshader, gshader, fshader)) {
  fprintf(stderr,"read vshader as:\n%s\n", vshader_text);
  fprintf(stderr,"read gshader as:\n%s\n", gshader_text);
  fprintf(stderr,"read fshader as:\n%s\n", fshader_text);
		glDeleteProgram(program);
		program = 0;
    fprintf(stderr, "killed program\n");
	}
	// once a program is linked
	// shader objects should be deleted
  free(vshader_text);
  free(gshader_text);
  free(fshader_text);
	glDeleteShader(vshader);
	glDeleteShader(gshader);
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
