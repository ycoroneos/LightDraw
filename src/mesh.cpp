#include "inc/mesh.h"
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flag
#include "stdio.h"

extern unsigned default_mesh_prog;

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned> indices, const char *name, Material *material)
{
  glGenVertexArrays(1, &vertexarray);
  glGenBuffers(1, &vertexbuffer);
  glGenBuffers(1, &indexbuffer);

  glBindVertexArray(vertexarray);

  //upload indices
  n_indices = indices.size();
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexbuffer);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &indices[0], GL_DYNAMIC_DRAW);

  //ALL THE VERTICES ARE INTERLEAVED
  glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
  size_t vert_bytes = vertices.size() * sizeof(Vertex);
  glBufferData(GL_ARRAY_BUFFER, vert_bytes, &vertices[0], GL_DYNAMIC_DRAW);
  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, pos));
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texcoords));
  glBindVertexArray(0);
  program = default_mesh_prog;
  M_loc = glGetUniformLocation(program, "M");
  if (M_loc<0)
  {
    fprintf(stderr, "error binding mesh: could not find M matrix location\r\n");
    return;
  }
  N_loc = glGetUniformLocation(program, "N");
  if (N_loc<0)
  {
    fprintf(stderr, "error binding mesh: could not find N matrix location\r\n");
    return;
  }
  drawmaterial = material;
  drawmaterial->incRef();
}

Mesh::~Mesh()
{
  glDeleteBuffers(1, &vertexbuffer);
  glDeleteBuffers(1, &indexbuffer);
  glDeleteVertexArrays(1, &vertexarray);
}

void Mesh::setProgram(unsigned newprogram)
{
  program = newprogram;
}

unsigned Mesh::getProgram()
{
  return program;
}

void Mesh::setQuickProgram(unsigned newprogram)
{
  quickprogram = newprogram;
}

unsigned Mesh::getQuickProgram()
{
  return quickprogram;
}

void Mesh::draw(bool lines, GLfloat *M, GLfloat *N)
{
  //glUseProgram(program);
  glBindVertexArray(vertexarray);
  glUniformMatrix4fv(M_loc, 1, false, M);
  glUniformMatrix3fv(N_loc, 1, false, N);
  //update shading uniforms for the material
  drawmaterial->Use(program);
  if (lines)
  {
     glDrawElements(GL_LINES, n_indices, GL_UNSIGNED_INT, 0);
  }
  else
  {
     glDrawElements(GL_TRIANGLES, n_indices, GL_UNSIGNED_INT, 0);
  }
  glBindVertexArray(0);
  //glUseProgram(0);
}


//used for Z pre pass
void Mesh::quickdraw(GLfloat *M, int quickprog)
{
  glBindVertexArray(vertexarray);
  int this_M_loc = glGetUniformLocation(quickprog, "M");
  glUniformMatrix4fv(this_M_loc, 1, false, M);
  glDrawElements(GL_TRIANGLES, n_indices, GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);
}
