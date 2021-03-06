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
#include <inc/node.h>
#include <inc/animation.h>


class SceneGraph : public InputResponder
{
  public:
    SceneGraph();
    ~SceneGraph();
    Node *getRoot();
    void bake();
    void zPre();
    void zPreBaked(int program);
    void drawShadowMaps();
    void drawLightVolumes(int lightvolme_program, Camera *camera);
    void drawScene(Camera *camera, bool wireframe);
    void drawSceneShadowed(Camera *camera, bool wireframe);
    void drawBaked(Camera *camera, bool wireframe);
    void drawForwardBaked(Camera *camera, bool wireframe);
    void printGraph();
    std::vector<Light *> getLights();
    Node *findNodeByName(const char *name);
    void animate(double timestep);
    Camera *getCamera(unsigned num);
    void setAllMeshProgram(int program); //for benchmarking
    void setAllLightsOn();
    void setAllLightsOff();
    void setAllShadowsOn();
    void setAllShadowsOff();

    void doMouseInput(double xpos, double ypos) override;
    void doKeyboardInput(int key, int scancode, int action, int mods) override;
  protected:
    Node * allocNode();
    Node *root;
    std::vector<Node*> nodes;
    std::vector<Mesh*> meshes;
    std::vector<Material*> materials;
    std::vector<Light *> lights;
    std::vector<KeyframeAnimation *> animations;
    std::vector<Camera *> cameras;
    bool lightson=true;
    bool shadows=true;
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
