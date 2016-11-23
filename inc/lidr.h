#pragma once
#include <inc/gl.h>
#include <inc/camera.h>
#include <inc/light.h>

//LIDR has 3 steps
//1. Z pre pass into a depth buffer texture
//2. Render light volumes for every light in screen space into a different texture.
//3. Pack all light properties into another texture and do a forward pass
//   the forward pass must have the light properties texture as well as the light volumes texture bound
//   so that every fragment can be indexed to figure out which lights hit it
//
//   other notes: do everything in screen space?


//each lights has
//    vec3 ambient;
//    vec3 diffuse;
//    vec3 specular;
//    vec3 worldpos;
// this can be stored in RGBA32 texture
// 1d texture with length 256

class LIDR
{
  public:
    LIDR(int z_program_1, int lightvolume_program_1);
    ~LIDR();
    int ZPrePass(Camera *camera);
    int LightVolumes();
    void LightVolumesEnd();
    void packLightTextures(std::vector<Light *> lights);
    void cornerWindow();
  private:
    GLuint depth_fbo;
    GLuint depth_map;
    GLuint volume_map;
    GLuint light_map;
    int z_program;
    int lightvolume_program;

    int maxlights=256;
};
