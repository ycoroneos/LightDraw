#pragma once
#include <inc/gl.h>
#include <inc/camera.h>

//LIDR has 3 steps
//1. Z pre pass into a depth buffer texture
//2. Render light volumes for every light in screen space into a different texture.
//3. Pack all light properties into another texture and do a forward pass
//   the forward pass must have the light properties texture as well as the light volumes texture bound
//   so that every fragment can be indexed to figure out which lights hit it
//
//   other notes: do everything in screen space?


class LIDR
{
  public:
    LIDR(int z_program_1, int lightvolume_program_1);
    ~LIDR();
    int ZPrePass(Camera *camera);
    int LightVolumes(Camera *camera);
    //Forward();
  private:
    GLuint depth_fbo;
    GLuint depth_map;
    GLuint volume_map;
    int z_program;
    int lightvolume_program;
};
