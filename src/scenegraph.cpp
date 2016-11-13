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
using namespace std;
using namespace glm;

void Node::addChild(Node *child)
{
  children.push_back(child);
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

const char *Node::getName()
{
  return (const char *)name;
}


void SceneGraph::addNode(Node *parent, Node newnode)
{
//  if (parent == NULL)
//  {
//    fprintf(stderr, "parent cannot be null when adding node\r\n");
//  }
//  else
//  {
//    nodes.push_back(newnode);
//    parent->addChild(&nodes[nodes.size()-1]);
//  }
}

void SceneGraph::printGraph()
{
  for (int i=0; i<nodes.size(); ++i)
  {
    fprintf(stderr, "node %s\r\n", nodes[i]->getName());
    fprintf(stderr, "\tchildren: ");
    vector<Node *> children = nodes[i]->getChildren();
    for (int j=0; j<children.size(); ++j)
    {
      fprintf(stderr, "%s ", children[j]->getName());
    }
  }
}

void SceneGraph::drawScene(Camera *camera, bool wireframe)
{
  // do DFS with a while loop so its faster
  struct state_variables
  {
    Node *N;
    mat4 M;
  };
  vector <struct state_variables> Nstack;
  Nstack.push_back((struct state_variables){root, root->getTransform()});
  while (Nstack.size()>0)
  {
    struct state_variables cur_depth = Nstack.back();
    Nstack.pop_back();
    Node *curN = cur_depth.N;
    mat4 M = cur_depth.M;
    vector<Mesh*> meshes = curN->getMeshes();
    for (unsigned i=0; i<meshes.size(); ++i)
    {
      int program = meshes[i]->getProgram();
      camera->updateUniforms(program);
      if (strncmp(curN->getName(), "dragon", 6)==0)
      {
        meshes[i]->draw(wireframe, &M[0][0]);
      }
    }

    vector<Node *> children = curN->getChildren();
    for (unsigned i=0; i<children.size(); ++i)
    {
      Nstack.push_back((struct state_variables){children[i], M * children[i]->getTransform()});
    }
  }
}

Node *SceneGraph::allocNode()
{
  Node *newnode = new Node();
  nodes.push_back(newnode);
  return newnode;
}

/////////////////////////////////////////////////////////////////
//
//
// ASSIMP GLUE
//
////////////////////////////////////////////////////////////////
static vec3 aiVec3toVec3(aiVector3D in)
{
  return vec3(in.x, in.y, in.z);
}

static mat4 aiMat4toMat4(aiMatrix4x4 in)
{
  mat4 out;
  for (int row=0; row<4; ++row)
  {
    for (int col=0; col<4; ++col)
    {
      out[row][col] = in[row][col];
    }
  }
  return out;
}

//load a scenegraph with assimp help
AssimpGraph::AssimpGraph(const char *filename)
{
  nodes.clear();
  meshes.clear();
  Assimp::Importer importer;
  const aiScene* scene = importer.ReadFile(filename, aiProcess_CalcTangentSpace|aiProcess_Triangulate|aiProcess_JoinIdenticalVertices|aiProcess_SortByPType);
  if (scene == NULL)
  {
    fprintf(stderr, "error loading assimp scene\r\n");
    return;
  }

  fprintf(stderr, "loading %s\r\n", filename);
  fprintf(stderr, "\t %d meshes\r\n", scene->mNumMeshes);
  //load all meshes into the list
  for (unsigned i=0; i<scene->mNumMeshes; ++i)
  {
    aiMesh* mesh = scene->mMeshes[i];
    int nfaces = mesh->mNumFaces;
    int mat_index = mesh->mMaterialIndex;
    const char *name = mesh->mName.C_Str();
    vector<Vertex> verts;
    vector<unsigned> indices;
    //vertices
    for (unsigned j=0; j<mesh->mNumVertices; ++j)
    {
      vec3 pos = aiVec3toVec3(mesh->mVertices[j]);
      vec3 normal = mesh->HasNormals() ? aiVec3toVec3(mesh->mNormals[j]) : vec3(0.0f, 0.0f, 0.0f);
      vec2 texcoords = mesh->HasTextureCoords(0) ? vec2(aiVec3toVec3(mesh->mTextureCoords[0][j])) : vec2(0.0f, 0.0f);
      verts.push_back(Vertex(pos, normal, texcoords));
    }
    //faces
    for (unsigned j=0; j<nfaces; ++j)
    {
      aiFace face = mesh->mFaces[j];
      if (face.mNumIndices != 3)
      {
        fprintf(stderr, "face is not a triangle, abort");
        return;
      }
      for (int f=0; f<3; ++f)
      {
        indices.push_back(face.mIndices[f]);
      }
    }
    //do textures later
    //insert it
    meshes.push_back(new Mesh(verts, indices, name, mat_index));
  }

  fprintf(stderr,"\t loading scene graph\r\n");
  root = allocNode();
  aiNode *ainode=scene->mRootNode;
  fprintf(stderr, "\t root node %s\r\n", ainode->mName.data);
  root->setName(ainode->mName.data);
  root->setTransform(aiMat4toMat4(ainode->mTransformation));
  for (int i=0; i<ainode->mNumMeshes; ++i)
  {
    root->addMesh(meshes[ainode->mMeshes[i]]);
  }
  for (int i=0; i<ainode->mNumChildren; ++i)
  {
    root->addChild(recursive_copy(ainode->mChildren[i], root));
  }
}


Node * AssimpGraph::recursive_copy(aiNode *curnode, Node *parent)
{
  Node *newnode = allocNode();
  aiNode *ainode=curnode;
  newnode->setName(ainode->mName.data);
  fprintf(stderr, "\t node %s, parent %s\r\n", newnode->getName(), parent->getName());
  newnode->setTransform(aiMat4toMat4(ainode->mTransformation));
  for (int i=0; i<ainode->mNumMeshes; ++i)
  {
    newnode->addMesh(meshes[ainode->mMeshes[i]]);
  }
  newnode->setParent(parent);
  for (int i=0; i<ainode->mNumChildren; ++i)
  {
    newnode->addChild(recursive_copy(ainode->mChildren[i], newnode));
  }
  return newnode;
}

