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
using namespace std;
using namespace glm;
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

static vec3 aiColor3toVec3(aiColor3D in)
{
  return vec3(in.r, in.g, in.b);
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
AssimpGraph::AssimpGraph(const char *filename) : SceneGraph()
{
  nodes.clear();
  meshes.clear();
  Assimp::Importer importer;
  unsigned int ppsteps = aiProcess_CalcTangentSpace | // calculate tangents and bitangents if possible
    aiProcess_JoinIdenticalVertices    | // join identical vertices/ optimize indexing
    aiProcess_ValidateDataStructure    | // perform a full validation of the loader's output
    aiProcess_ImproveCacheLocality     | // improve the cache locality of the output vertices
   // aiProcess_RemoveRedundantMaterials | // remove redundant materials
    aiProcess_FindDegenerates          | // remove degenerated polygons from the import
    aiProcess_FindInvalidData          | // detect invalid model data, such as invalid normal vectors
    aiProcess_GenUVCoords              | // convert spherical, cylindrical, box and planar mapping to proper UVs
    aiProcess_TransformUVCoords        | // preprocess UV transformations (scaling, translation ...)
  //  aiProcess_FindInstances            | // search for instanced meshes and remove them by references to one master
    aiProcess_LimitBoneWeights         | // limit bone weights to 4 per vertex
    aiProcess_OptimizeMeshes           | // join small meshes, if possible;
    aiProcess_SplitByBoneCount         | // split meshes with too many bones. Necessary for our (limited) hardware skinning shader
    aiProcess_GenSmoothNormals         | // generate smooth normal vectors if not existing
    aiProcess_SplitLargeMeshes         | // split large, unrenderable meshes into submeshes
    aiProcess_Triangulate              | // triangulate polygons with more than 3 edges
    aiProcess_SortByPType              | // make 'clean' meshes which consist of a single typ of primitives
    0;
  fprintf(stderr, "assimp loading ...");
  const aiScene* scene = importer.ReadFile(filename,ppsteps);
  if (scene == NULL)
  {
    fprintf(stderr, "error loading assimp scene\r\n");
    return;
  }

  fprintf(stderr, "loading %s\r\n", filename);
  fprintf(stderr, "\t %d meshes\r\n", scene->mNumMeshes);
  const char *cwdspot = strrchr(filename, '/');
  int len = cwdspot - filename + 1;
  char *cwd = new char[len+1];
  memcpy(cwd, filename, len);
  cwd[len]=0;
  fprintf(stderr, "cwd is %s\r\n", cwd);
  //do materials first
  for (unsigned int i = 0 ; i < scene->mNumMaterials ; i++)
  {
      const aiMaterial* pMaterial = scene->mMaterials[i];
      aiString mname;
      pMaterial->Get(AI_MATKEY_NAME, mname);
      int shadingModel;
      pMaterial->Get(AI_MATKEY_SHADING_MODEL, shadingModel);
      if (shadingModel != aiShadingMode_Phong && shadingModel != aiShadingMode_Gouraud)
      {
          fprintf(stderr, "This mesh's shading model is not implemented in this loader, setting to default material");
          mname = "DefaultMaterial";
          materials.push_back(new DummyMat());
          continue;
      }
      aiColor3D dif(0.f,0.f,0.f);
      aiColor3D amb(0.f,0.f,0.f);
      aiColor3D spec(0.f,0.f,0.f);
      float shine = 0.0;

      pMaterial->Get(AI_MATKEY_COLOR_AMBIENT, amb);
      pMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, dif);
      pMaterial->Get(AI_MATKEY_COLOR_SPECULAR, spec);
      pMaterial->Get(AI_MATKEY_SHININESS, shine);

      char diffuse_texture[256];
      if (pMaterial->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
          aiString Path;

          if (pMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &Path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS) {
            //fprintf(stderr, "diffuse texture %s \r\n", Path.data);
            //I hope its never this big
            strcpy(diffuse_texture, cwd);
            strcat(diffuse_texture, Path.data);
            materials.push_back(new Material(mname.C_Str(), aiColor3toVec3(dif), aiColor3toVec3(amb), aiColor3toVec3(spec), shine, diffuse_texture));
          }
      }
      else
      {
        materials.push_back(new DummyMat());
      }
  }

  //load lights
  fprintf(stderr, "%d lights\r\n", scene->mNumLights);
  for (int i=0; i<scene->mNumLights; ++i)
  {
    aiLight *asslight = scene->mLights[i];
    fprintf(stderr, "light pos %f %f %f\r\n", asslight->mPosition[0], asslight->mPosition[1], asslight->mPosition[2]);
    switch(asslight->mType)
    {
      case aiLightSource_DIRECTIONAL:
        fprintf(stderr, "Directional light\r\n");
        //lights.push_back(new DirectionLight(asslight->mName.C_Str(), aiVec3toVec3(asslight->mPosition), aiColor3toVec3(asslight->mColorAmbient), aiColor3toVec3(asslight->mColorDiffuse),
        //      aiColor3toVec3(asslight->mColorSpecular), aiVec3toVec3(asslight->mDirection)));
        break;
      case aiLightSource_POINT:
        fprintf(stderr, "Point light\r\n");
        lights.push_back(new PointLight(asslight->mName.C_Str(), aiVec3toVec3(asslight->mPosition), aiColor3toVec3(asslight->mColorAmbient), aiColor3toVec3(asslight->mColorDiffuse),
              aiColor3toVec3(asslight->mColorSpecular)));
        break;
      case aiLightSource_SPOT:
        fprintf(stderr, "Spot light\r\n");
        //lights.push_back(new SpotLight(asslight->mName.C_Str(), aiVec3toVec3(asslight->mPosition), aiColor3toVec3(asslight->mColorAmbient), aiColor3toVec3(asslight->mColorDiffuse),
        //      aiColor3toVec3(asslight->mColorSpecular), aiVec3toVec3(asslight->mDirection), asslight->mAngleInnerCone));
        break;
      default:
        fprintf(stderr, "unknown light %d\r\n", asslight->mType);
        //lights.push_back(new DirectionLight(asslight->mName.C_Str(), aiVec3toVec3(asslight->mPosition), aiColor3toVec3(asslight->mColorAmbient), aiColor3toVec3(asslight->mColorDiffuse),
        //      aiColor3toVec3(asslight->mColorSpecular), aiVec3toVec3(asslight->mDirection)));
        //lights.push_back(new DummyLight());
        break;
    }
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
    if (mesh->HasVertexColors(0))
    {
      fprintf(stderr, "colors on %s\r\n", name);
    }
    for (int j=0; j<mesh->GetNumUVChannels(); ++j)
    {
      if (mesh->HasTextureCoords(j))
      {
        //fprintf(stderr, "texture coordinates on %s -> %d\r\n", name, j);
      }
    }
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
        fprintf(stderr, "face is not a triangle, abort: %d, %s\r\n", face.mNumIndices, name);
        continue;
        //return;
      }
      for (int f=0; f<3; ++f)
      {
        indices.push_back(face.mIndices[f]);
      }
    }
    //insert it
    if ((mat_index)>(materials.size()-1) || mat_index<0)
    {
      fprintf(stderr, "mat index %d but size is %d\r\n", mat_index, materials.size());
      return;
    }
    meshes.push_back(new Mesh(verts, indices, name, materials[mat_index]));
  }
  int nTextures = scene->mNumTextures;
  if (nTextures == 0)
  {
    fprintf(stderr, "no textures found\r\n");
  }
  for(int i=0; i<nTextures; ++i)
  {
    if (scene->mTextures[i]->mHeight == 0)
    {
      fprintf(stderr, "texture %d is compressed\r\n", i);
    }
    else
    {
      fprintf(stderr, "tex width: %d height: %d\r\n", scene->mTextures[i]->mWidth, scene->mTextures[i]->mHeight);
    }
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

  for (int i=0; i<lights.size(); ++i)
  {
    if (strcmp(lights[i]->getName(), root->getName())==0)
    {
      fprintf(stderr, "root has light\r\n");
      root->addLight(lights[i]);
    }
  }

  for (int i=0; i<ainode->mNumChildren; ++i)
  {
    root->addChild(recursive_copy(ainode->mChildren[i], root));
  }
  fprintf(stderr, "scene loaded %d lights\r\n", lights.size());
  delete[] cwd;
}


Node * AssimpGraph::recursive_copy(aiNode *curnode, Node *parent)
{
  Node *newnode = allocNode();
  aiNode *ainode=curnode;
  newnode->setName(ainode->mName.data);
  mat4 transform = aiMat4toMat4(ainode->mTransformation);
  newnode->setTransform(transform);
  for (int i=0; i<ainode->mNumMeshes; ++i)
  {
    newnode->addMesh(meshes[ainode->mMeshes[i]]);
  }
  for (int i=0; i<lights.size(); ++i)
  {
    if (strcmp(lights[i]->getName(), newnode->getName())==0)
    {
      fprintf(stderr, "%s has light, parent %s\r\n", newnode->getName(), parent->getName());
      newnode->addLight(lights[i]);
    }
  }
  newnode->setParent(parent);
  for (int i=0; i<ainode->mNumChildren; ++i)
  {
    newnode->addChild(recursive_copy(ainode->mChildren[i], newnode));
  }
  return newnode;
}

