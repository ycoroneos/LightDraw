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
#include "stdio.h"
#include "inc/scenegraph.h"
#include <time.h>


Camera *camera;// = Camera(vec3(0.0f,0.0f,10.0f),vec2(0.0f,0.0f));
VoxelGrid *vxg;
VoxelGrid *gnd;
BinVox *garg;
AssimpGraph *sgr;
LIDR *lidr;
glm::mat4 floormat = glm::translate(glm::mat4(), vec3(-20.0,-1.0, -20.0));
glm::mat4 ident;

int voxelprog;
int default_mesh_prog;
int pointlight_shadowmap_program;
int directionlight_shadowmap_program;
int default_quad_program;
int lidr_z_program;
int lidr_lightvolume_program;

int initScene(mat4 Projection)
{
  camera = new Camera(vec3(5.0f,1.0f, 0.0f), vec2(0.0f,0.0f), Projection);
  camera->enableInput();
//  voxelprog = compileProgram("../shaders/voxel.vs", "../shaders/voxel.fs");
  //default_mesh_prog = compileProgram("../shaders/flat.vs", "../shaders/flat.fs");
  default_mesh_prog = compileProgram("../shaders/flat.vs", "../shaders/light.fs");
  pointlight_shadowmap_program = compileGProgram("../shaders/point_shadow.vs", "../shaders/point_shadow.gs", "../shaders/point_shadow.fs");
  directionlight_shadowmap_program = compileProgram("../shaders/shadow.vs", "../shaders/shadow.fs");
  default_quad_program = compileGProgram("../shaders/quad.vs", "../shaders/quad.gs", "../shaders/quad.fs");
  lidr_z_program = compileProgram("../shaders/depth.vs", "../shaders/depth.fs");
  lidr_lightvolume_program = compileProgram("../shaders/lightvolume.vs", "../shaders/lightvolume.fs");
  if (!default_mesh_prog || !pointlight_shadowmap_program || !directionlight_shadowmap_program || !default_quad_program)
  {
    return -1;
  }
  lidr = new LIDR(lidr_z_program, lidr_lightvolume_program);
//  vxg = new VoxelGrid(10,10,10);
//  vxg->setProgram(voxelprog);
//  gnd = new VoxelGrid(100,1,100);
//  gnd->setProgram(voxelprog);
  //garg = new BinVox("../data/garg.binvox");
  //garg->setProgram(voxelprog);
  sgr = new AssimpGraph("../data/crytek-sponza-dragon/sponza.dae");
  sgr->bake();
  return 0;
}

void drawScene()
{
//  int loc = glGetUniformLocation(voxelprog, "M");
//  camera->updateUniforms(voxelprog);
//  glUseProgram(voxelprog);
//  glUniformMatrix4fv(loc, 1, false, &floormat[0][0]);
//  gnd->draw();

//  sgr->zPreBaked();
// / sgr->drawBaked(camera, camera->viewWire());
  int zprog = lidr->ZPrePass(camera);
  sgr->zPreBaked(zprog);
  sgr->drawBaked(camera, camera->viewWire());
  //sgr->zPre();
  //sgr->drawScene(camera, camera->viewWire());
  //sgr->drawSceneShadowed(camera, camera->viewWire());
//
//  glUseProgram(voxelprog);
//  if (loc<0)
//  {
//    fprintf(stderr, "M not found\n");
//  }
//  glUniformMatrix4fv(loc, 1, false, &ident[0][0]);
//  //vxg->draw();
//  //garg->draw();
}

void cleanupScene()
{
//  delete vxg;
//  delete gnd;
  //delete garg;
  delete camera;
  delete sgr;
  delete lidr;
}


