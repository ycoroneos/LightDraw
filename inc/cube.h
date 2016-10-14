#pragma once
#include <glm/glm.hpp>
using namespace glm;

//these cube vertices were ripped from https://github.com/in2gpu/in2gpu_tutorials/blob/fcdfe647183bf29b44bc9fa7e1b76d7a1dac27a2/in2gpu_tutorials/Chapter_2/c2_2_DrawCubeIndex/CubeIndex.cpp

const int cube_num_vertices = 24;
const int cube_num_indices = 36;
const float scaling=2.0f;

static unsigned int cube_indices[] = {
                       0,  1,  2,  0,  2,  3,   //front
										   4,  5,  6,  4,  6,  7,   //right
										   8,  9,  10, 8,  10, 11,  //back
										   12, 13, 14, 12, 14, 15,  //left
										   16, 17, 18, 16, 18, 19,  //upper
										   20, 21, 22, 20, 22, 23}; //bottom

static vec3 cube_vertices[] = {
	//front
	vec3(-1.0, -1.0, 1.0)/scaling,
	vec3( 1.0, -1.0, 1.0)/scaling,
	vec3( 1.0,  1.0, 1.0)/scaling,
	vec3(-1.0,  1.0, 1.0)/scaling,

  //right
	vec3(1.0,  1.0,   1.0)/scaling,
	vec3(1.0,  1.0,  -1.0)/scaling,
	vec3(1.0, -1.0,  -1.0)/scaling,
	vec3(1.0, -1.0,   1.0)/scaling,

	//back
	vec3(-1.0, -1.0, -1.0)/scaling,
	vec3( 1.0, -1.0, -1.0)/scaling,
	vec3( 1.0,  1.0, -1.0)/scaling,
	vec3(-1.0,  1.0, -1.0)/scaling,

	//left
	vec3(-1.0, -1.0, -1.0)/scaling,
	vec3(-1.0, -1.0,  1.0)/scaling,
	vec3(-1.0,  1.0,  1.0)/scaling,
	vec3(-1.0,  1.0, -1.0)/scaling,

	//upper
	vec3( 1.0, 1.0,  1.0)/scaling,
	vec3(-1.0, 1.0,  1.0)/scaling,
	vec3(-1.0, 1.0, -1.0)/scaling,
	vec3( 1.0, 1.0, -1.0)/scaling,


	//bottom
	vec3(-1.0, -1.0, -1.0)/scaling,
	vec3( 1.0, -1.0, -1.0)/scaling,
	vec3( 1.0, -1.0,  1.0)/scaling,
	vec3(-1.0, -1.0,  1.0)/scaling
};
