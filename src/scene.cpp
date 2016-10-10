#include "inc/scene.h"
#include "inc/chunk.h"
#include "inc/glt.h"
#include <glm/glm.hpp>
#include "inc/camera.h"

Camera camera = Camera(vec3(0.0f,0.0f,0.0f),vec2(0.0f,0.0f));
VoxelGrid *vxg;
unsigned voxelprog;

void initScene()
{
  camera.enableInput();
  voxelprog = compileProgram("../shaders/voxel.vs", "../shaders/voxel.fs");
  vxg = new VoxelGrid(10,10,10);
}

void drawScene(mat4 *Projection)
{
}

void cleanupScene()
{
  delete vxg;
}
