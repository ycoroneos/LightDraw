#pragma once
#include <inc/gl.h>
#include <vector>
#include <glm/glm.hpp>
using namespace glm;

//  f[4]                 f[5]
//   -----------------------
//   |  f[0]          f[1] |
//   |  ----------------   |
//   |  |              |   |
//   |  |              |   |
//   |  |              |   |
//   |  |              |   |
//   |  |              |   |
//   |  |              |   |
//   |  |              |   |
//   |  ----------------   |
//   | f[2]          f[3]  |
//   -----------------------
//  f[7]                f[6]


//if the dot product with all of the inward normals is positive, then the point is inside the frustum

class Frustum
{
  public:
  Frustum(vec3 f_0, vec3 f_1, vec3 f_2, vec3 f_3, vec3 f_4, vec3 f_5, vec3 f_6, vec3 f_7);

  bool isInternal(vec3 point);

  private:
  vec3 f[8];
  vec3 near_normal, right_normal, left_normal, bottom_normal, top_normal, far_normal;
};
