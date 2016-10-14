#include "inc/scene.h"
#include "inc/chunk.h"
#include "inc/glt.h"
#include <glm/glm.hpp>
#include "inc/recorder.h"
#include "inc/camera.h"
#include "inc/teapot.h"
#include "inc/cube.h"
#include "inc/voxel.h"

void drawTeapot();

Camera camera = Camera(vec3(0.0f,0.0f,10.0f),vec2(0.0f,0.0f));
VoxelGrid *vxg;
//Chunk *ch;
unsigned voxelprog;

void initScene()
{
  camera.enableInput();
  voxelprog = compileProgram("../shaders/voxel.vs", "../shaders/voxel.fs");
  vxg = new VoxelGrid(10,10,10);
  vxg->setProgram(voxelprog);
  //ch.setProgram(voxelprog);
  //ch.addVoxel(Voxel(glm::mat4()));
  //ch = new Chunk();
  //ch->setProgram(voxelprog);
  //ch->addVoxel(Voxel(glm::mat4()));
}

void drawScene(mat4 Projection)
{
  camera.updateUniforms(Projection, voxelprog);
  //glUseProgram(voxelprog);
  vxg->draw();
  //drawTeapot();
  //ch->draw();
  //glUseProgram(0);
}

void cleanupScene()
{
  delete vxg;
}


//debug
void drawTeapot()
{
    // set the required buffer size exactly.
    GeometryRecorder rec(teapot_num_faces * 3);
    for (int idx : teapot_indices) {
        vec3 position(teapot_positions[idx * 3 + 0],
            teapot_positions[idx * 3 + 1],
            teapot_positions[idx * 3 + 2]);

        vec3 normal(teapot_normals[idx * 3 + 0],
            teapot_normals[idx * 3 + 1],
            teapot_normals[idx * 3 + 2]);

        rec.record(position, normal);
    }
    rec.draw();
}
