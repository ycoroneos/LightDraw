#include "inc/scene.h"
#include "inc/chunk.h"
#include "inc/glt.h"
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "inc/recorder.h"
#include "inc/camera.h"
#include "inc/cube.h"
#include "inc/voxel.h"
#include "inc/lidr.h"
#include "inc/shaderlib.h"
#include "stdio.h"
#include "inc/scenegraph.h"
#include <time.h>


Camera *camera;
AssimpGraph *sgr;
LIDR *lidr;
ShaderLib shaderlib;

int voxelprog;
int default_mesh_prog;
int pointlight_shadowmap_program;
int spotlight_shadowmap_program;
int default_quad_program;
int lidr_z_program;
int lidr_lightvolume_program;
int viewport_program;
int mesh_lidr_prog;

int initScene(mat4 Projection)
{
  camera = new Camera(vec3(5.0f,1.0f, 0.0f), vec2(0.0f,0.0f), Projection);
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
  lidr = new LIDR(lidr_z_program, lidr_lightvolume_program);
  sgr = new AssimpGraph("../data/crytek-sponza-dragon/sponza.dae");
  sgr->enableInput();
  sgr->bake();
  return 0;
}

void drawScene()
{
  //first fill the z buffer
  int zprog = lidr->ZPrePass(camera);
  sgr->zPreBaked(zprog);

  //draw shadowmaps for each light
  sgr->drawShadowMaps();

  //draw lightvolumes
  int lightvolume_prog = lidr->LightVolumes();
  sgr->drawLightVolumes(lightvolume_prog, camera);
  lidr->LightVolumesEnd();

  //update the light textures
  lidr->packLightTextures(sgr->getLights());

  sgr->drawBaked(camera, camera->viewWire());
  lidr->cornerWindow();
  lidr->textureWindow(sgr->getLights()[0]->getDepthMap());
}

void cleanupScene()
{
  delete camera;
  delete sgr;
  delete lidr;
}


