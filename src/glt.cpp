#include <inc/glt.h>
#include <inc/gl.h>
#include <stdio.h>
#include <cstdio>
#include <cstring>

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
		printf("Error in %s shader\n%s\n", typelabel, buff);
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
    printf("error reading vertex shader %s\n", vshader_src_file);
    return 0;
  }
  fseek(f_vshader, 0, SEEK_END);
  length=ftell(f_vshader);
  fseek(f_vshader, 0, SEEK_SET);
  vshader_src = new char[length];
  fread(vshader_src, 1, length, f_vshader);
  fclose(f_vshader);

  //read fshader
  f_fshader = fopen(fshader_src_file, "rb");
  if (!f_fshader)
  {
    printf("error reading fragment shader %s\n", fshader_src_file);
    return 0;
  }
  fseek(f_fshader, 0, SEEK_END);
  length=ftell(f_fshader);
  fseek(f_fshader, 0, SEEK_SET);
  fshader_src = new char[length];
  fread(fshader_src, 1, length, f_fshader);
  fclose(f_fshader);

  //link program
	GLuint program = glCreateProgram();
	GLuint vshader = compileShader(GL_VERTEX_SHADER, vshader_src);
	GLuint fshader = compileShader(GL_FRAGMENT_SHADER, fshader_src);
	if (!linkProgram(program, vshader, fshader)) {
		glDeleteProgram(program);
		program = 0;
	}
	// once a program is linked
	// shader objects should be deleted
  delete [] vshader_src;
  delete [] fshader_src;
	glDeleteShader(vshader);
	glDeleteShader(fshader);
	return program;
}
