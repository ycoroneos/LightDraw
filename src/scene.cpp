#include "inc/scene.h"
#include "inc/chunk.h"
#include "inc/glt.h"
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "inc/recorder.h"
#include "inc/fpscamera.h"
#include "inc/cube.h"
#include "inc/voxel.h"
#include "inc/lidr.h"
#include "inc/shaderlib.h"
#include "stdio.h"
#include "inc/scenegraph.h"
#include <time.h>


FPSCamera *camera;
AssimpGraph *sgr;
LIDR *lidr;
ShaderLib shaderlib;
Camera *active_camera=NULL;

int voxelprog;
int default_mesh_prog;
int pointlight_shadowmap_program;
int spotlight_shadowmap_program;
int default_quad_program;
int lidr_z_program;
int lidr_lightvolume_program;
int viewport_program;
int mesh_lidr_prog;
int mesh_forward_prog;

int initScene(mat4 Projection, bool benchmark, bool uselidr, bool shadows)
{
  camera = new FPSCamera(vec3(5.0f,1.0f, 0.0f), vec2(0.0f,0.0f), Projection);
  camera->enableInput();

  voxelprog                         = shaderlib.loadShader("../shaders/voxel.vert", "../shaders/voxel.frag");
  default_mesh_prog                 = shaderlib.loadShader("../shaders/flat.vert", "../shaders/light.frag");
  pointlight_shadowmap_program      = shaderlib.loadShader("../shaders/point_shadow.vert", "../shaders/point_shadow.geom", "../shaders/point_shadow.frag");
  spotlight_shadowmap_program       = shaderlib.loadShader("../shaders/spotlight_shadow.vert", "../shaders/spotlight_shadow.frag");
  default_quad_program              = shaderlib.loadShader("../shaders/quad.vert", "../shaders/quad.geom", "../shaders/quad.frag");
  lidr_z_program                    = shaderlib.loadShader("../shaders/depth.vert", "../shaders/depth.frag");
  lidr_lightvolume_program          = shaderlib.loadShader("../shaders/lightvolume.vert", "../shaders/lightvolume.frag");
  viewport_program                  = shaderlib.loadShader("../shaders/viewport.vert", "../shaders/viewport.frag");
  mesh_lidr_prog                    = shaderlib.loadShader("../shaders/lidr.vert", "../shaders/lidr_light.frag");
  mesh_forward_prog                 = shaderlib.loadShader("../shaders/forward.vert", "../shaders/forward.frag");
  lidr = new LIDR(lidr_z_program, lidr_lightvolume_program);
  sgr = new AssimpGraph("../data/crytek-sponza-dragon/sponza_240lights.dae");
  sgr->enableInput();
  sgr->bake();
  if (shadows)
  {
    sgr->setAllShadowsOn();
  }
  else
  {
    sgr->setAllShadowsOff();
  }
  if (benchmark)
  {
    active_camera = sgr->getCamera(0);
  }
  if (active_camera == NULL)
  {
    active_camera = camera;
  }
  if (uselidr)
  {
    sgr->setAllMeshProgram(mesh_lidr_prog);
  }
  else
  {
    sgr->setAllMeshProgram(mesh_forward_prog);
  }
  return 0;
}

void drawScene(double timestep, bool uselidr)
{
  //animate
  sgr->animate(timestep);


  if (uselidr)
  {
    //first fill the z buffer
    int zprog = lidr->ZPrePass(active_camera);
    sgr->zPreBaked(zprog);

    //draw shadowmaps for each light
    sgr->drawShadowMaps();

    //draw lightvolumes
    int lightvolume_prog = lidr->LightVolumes();
    sgr->drawLightVolumes(lightvolume_prog, active_camera);
    lidr->LightVolumesEnd();

    //update the light textures
    lidr->packLightTextures(sgr->getLights());

    sgr->drawBaked(active_camera, active_camera->viewWire());
    //lidr->cornerWindow();
    //lidr->textureWindow(sgr->getLights()[0]->getDepthMap());
  }
  else
  {
    //depth pre-pass
    glColorMask(0,0,0,0);
    glUseProgram(lidr_z_program);
    active_camera->updateUniforms(lidr_z_program);
    sgr->zPreBaked(lidr_z_program);
    glColorMask(1,1,1,1);

    //shadow maps
    sgr->drawShadowMaps();

    //do regular forward pass
    sgr->drawForwardBaked(active_camera, active_camera->viewWire());
  }
}

void cleanupScene()
{
  delete camera;
  delete sgr;
  delete lidr;
}


