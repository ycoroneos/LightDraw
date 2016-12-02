#pragma once
#include <glm/glm.hpp>

int initScene(glm::mat4 Projection);

void drawScene(double timestep);

void cleanupScene();
