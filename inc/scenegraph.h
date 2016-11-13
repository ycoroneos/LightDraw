#pragma once
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "inc/chunk.h"
#include "inc/mesh.h"
#include <vector>
/*
 * Scenegraph will traverse the graph and generate deferred draw calls that contain all of the transforms required for the chunk to draw correctly.
 * Before drawing, the deferred draw calls will be sorted by centroid distance from the camera. This will allow the gl depth test to quickly throw out some vertices.
 *
 */


//transform is relative to parent
//so do push(transform)
//then draw()
class Node
{
  public:
  private:
  char name[25];        //name for debugging
  glm::mat4 transform;  //transform relative to parent
  Node *parent;
  std::vector<Node *> children;
  std::vector<Mesh *> meshes;
};

class SceneGraph
{
  public:
    Node *getRoot();
    void addNode(Node *parent, Node newnode);
  protected:
    Node *root;
    std::vector<Node> nodes;
    std::vector<Mesh> meshes;
};

class AssimpGraph : public SceneGraph
{
  public:
    AssimpGraph(const char *filename);
  private:
    Node *recursive_copy(aiNode *curnode, Node *parent);
};
