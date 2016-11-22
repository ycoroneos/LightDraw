#include "inc/scenegraph.h"
#include "inc/mesh.h"
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flag
#include "stdio.h"
#include <inc/camera.h>
#include <inc/light.h>
//#include <glm/gtx/inverse_transpose.hpp>
using namespace std;
using namespace glm;

void Node::addChild(Node *child)
{
  children.push_back(child);
}

void Node::addLight(Light *light)
{
  lights.push_back(light);
}

void Node::setName(const char *newname)
{
  strncpy(name, newname, sizeof(name));
}

void Node::setTransform(mat4 transform_1)
{
  transform = transform_1;
}

void Node::setParent(Node *parent_1)
{
  parent = parent_1;
}

void Node::addMesh(Mesh *mesh)
{
  meshes.push_back(mesh);
}

mat4 Node::getTransform()
{
  return transform;
}


vector<Node *> Node::getChildren()
{
  return children;
}

vector<Mesh *> Node::getMeshes()
{
  return meshes;
}

vector<Light *> Node::getLights()
{
  return lights;
}

const char *Node::getName()
{
  return (const char *)name;
}
