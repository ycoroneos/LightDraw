#pragma once
#include <inc/gl.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "inc/chunk.h"
#include "inc/mesh.h"
#include "inc/material.h"
#include <vector>
#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flag
#include <inc/camera.h>
#include <inc/light.h>

//transform is relative to parent
//so do push(transform)
//then draw()
class Node
{
  public:
    void addChild(Node *child);
    void addLight(Light *light);
    void setName(const char *newname);
    const char* getName();
    void setTransform(mat4 transform_1);
    void setParent(Node *parent_1);
    glm::mat4 getTransform();
    void addMesh(Mesh *mesh);
    std::vector<Node *> getChildren();
    std::vector<Mesh *> getMeshes();
    std::vector<Light *> getLights();
    void setM(glm::mat4 M_1);
    Node *getParent();
    glm::mat4 getM();
    void bakeLower();
  private:
  bool baked=false;
  char name[25];        //name for debugging
  glm::mat4 transform;  //transform relative to parent
  glm::mat4 M; //a baked M used for animation
  Node *parent;
  std::vector<Node *> children;
  std::vector<Mesh *> meshes;
  std::vector<Light *> lights;
};
