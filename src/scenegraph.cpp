#include "inc/scenegraph.h"
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>

MeshNode::MeshNode(Chunk *chunk) : mesh(chunk)
{
}

Chunk * MeshNode::getMesh()
{
  return mesh;
}
