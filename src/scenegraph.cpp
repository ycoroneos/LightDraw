#include "inc/scenegraph.h"
#include "inc/mesh.h"
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flag
#include "stdio.h"

MeshNode::MeshNode(Chunk *chunk) : mesh(chunk)
{
}

Chunk * MeshNode::getMesh()
{
  return mesh;
}

SceneGraph::SceneGraph()
{
}

void SceneGraph::addNode(Node *parent, Node newnode)
{
  if (parent == NULL)
  {
  }
  else
  {
  }
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

static aiMat4toMat4(aiMatrix4x4 in)
{
  mat4 out;
  for (int row=0; row<4; ++row)
  {
    for (int col=0; col<4; ++col)
    {
      out[row][col] = in[4*row + col];
    }
  }
  return out;
}

//load a scenegraph with assimp help
AssimpGraph::AssimpGraph(const char *filename)
{
  Assimp::Importer importer;
  const aiScene* scene = importer.ReadFile(filename, aiProcess_CalcTangentSpace|aiProcess_Triangulate|aiProcess_JoinIdenticalVertices|aiProcess_SortByPType);
  if (scene == NULL)
  {
    fprintf(2, "error loading assimp scene\r\n");
    return;
  }

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
    for (unsigned j=0; j<mNumVertices; ++j)
    {
      vec3 pos = aiVec3toVec3(mVertices[j]);
      if (mesh->HasNormals())
      {
        vec3 normal = aiVec3toVec3(mNormals[j]);
      }
      else
      {
        vec3 normal = vec3(0.0f, 0.0f, 0.0f);
      }
      vec2 texcoords = aiVec3toVec3(mTextureCoords[0][j]).xy();
      verts.push_back(Vertex(pos, normal, texcoords));
    }
    //faces
    for (unsigned j=0; j<mNumFaces; ++j)
    {
      aiFace face = mesh->mFaces[j];
      if (face.mNumIndices != 3)
      {
        fprintf(2, "face is not a triangle, abort");
        return;
      }
      for (int f=0; f<3; ++f)
      {
        indices.push_back(face.mIndices[f]);
      }
    }
    //do textures later
    //insert it
    meshes.push_back(Mesh(verts, indices, name, mat_index));
  }

  Node root;
  aiNode *ainode=scene->mRootNode;
  strcpy(root.name, ainode->mName.data, sizeof(root.name));
  root.transform = aiMat4toMat4(ainode->mTransformation);
  for (int i=0; i<ainode->mNumMeshes; ++i)
  {
    root.meshes.push_back(&meshes[ainode->mMeshes[i]]);
  }
  root.parent=NULL;
  nodes.push_back(root);
  Node * me = nodes.back();
  for (int i=0; i<ainode->mNumChildren; ++i)
  {
    me->children.push_back(recursive_copy(ainode->mChildren[i], me));
  }
}


Node * AssimpGraph::recursive_copy(aiNode *curnode, Node *parent);
{
  Node newnode;
  aiNode *ainode=curnode;
  strcpy(newnode.name, ainode->mName.data, sizeof(newnode.name));
  newnode.transform = aiMat4toMat4(ainode->mTransformation);
  for (int i=0; i<ainode->mNumMeshes; ++i)
  {
    newnode.meshes.push_back(&meshes[ainode->mMeshes[i]]);
  }
  newnode.parent=parent;
  nodes.push_back(newnode);
  Node * me = nodes.back();
  for (int i=0; i<ainode->mNumChildren; ++i)
  {
    me->children.push_back(recursive_copy(ainode->mChildren[i], me));
  }
  return me;
}

