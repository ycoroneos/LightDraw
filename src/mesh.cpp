#include "inc/mesh.h"
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flag
#include "stdio.h"

extern int default_mesh_prog;
extern int mesh_lidr_prog;

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned> indices, const char *name_1, Material *material)
{
  strncpy(name, name_1, sizeof(name));
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
  //program = default_mesh_prog;
  program = mesh_lidr_prog;
  glUseProgram(program);
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
  //  return;
  }
  glUseProgram(0);
  drawmaterial = material;
  drawmaterial->incRef();
}

Mesh::~Mesh()
{
  glDeleteBuffers(1, &vertexbuffer);
  glDeleteBuffers(1, &indexbuffer);
  glDeleteVertexArrays(1, &vertexarray);
}

void Mesh::setWorldPos(mat4 pos)
{
  worldpos = pos;
  //fprintf(stderr, "mesh %s at world pos %f %f %f\r\n", name, worldpos[3].x, worldpos[3].y, worldpos[3].z);
  //fprintf(stderr, "M: ");
  //for (int i = 0; i<4; ++i)
  //{
  //  fprintf(stderr, "\tcol %d : %f %f %f %f\r\n", i, pos[i].x, pos[i].y, pos[i].z, pos[i].w);
  //}
}

mat4 Mesh::getWorldPos()
{
  return worldpos;
}

void Mesh::setProgram(unsigned newprogram)
{
  program = newprogram;
  glUseProgram(program);
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
  //  return;
  }
  glUseProgram(0);
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
  glBindVertexArray(vertexarray);
  glUniformMatrix4fv(M_loc, 1, false, M);
  glUniformMatrix3fv(N_loc, 1, false, N);
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
