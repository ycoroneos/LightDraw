#pragma once
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "inc/chunk.h"
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
  unsigned program;     //program to draw with
  glm::mat4 transform;  //transform relative to parent

  std::vector<Node *> children;
};

class MeshNode : public Node
{
  public:
    MeshNode(Chunk *chunk);
    Chunk * getMesh();
  private:
    Chunk *mesh;
};

class SceneGraph
{
  public:
    Node *getRoot();
    void addNode(Node *parent, Node newnode);
  private:
    Node *root;
    std::vector<Node> graphnodes;
};

