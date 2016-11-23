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
  private:
  char name[25];        //name for debugging
  glm::mat4 transform;  //transform relative to parent
  Node *parent;
  std::vector<Node *> children;
  std::vector<Mesh *> meshes;
  std::vector<Light *> lights;
};

class SceneGraph
{
  public:
    SceneGraph();
    ~SceneGraph();
    Node *getRoot();
    void bake();
    void zPre();
    void zPreBaked(int program);
    void drawLightVolumes(int lightvolme_program, Camera *camera);
    void drawScene(Camera *camera, bool wireframe);
    void drawSceneShadowed(Camera *camera, bool wireframe);
    void drawBaked(Camera *camera, bool wireframe);
    void printGraph();
  protected:
    Node * allocNode();
    Node *root;
    std::vector<Node*> nodes;
    std::vector<Mesh*> meshes;
    std::vector<Material*> materials;
    std::vector<Light *> lights;
  private:
    GLuint lightvolume_vao;
    GLuint lightvolume_vertex_buffer;
};

class AssimpGraph : public SceneGraph
{
  public:
    AssimpGraph(const char *filename);
  private:
    Node *recursive_copy(aiNode *curnode, Node *parent);
};
