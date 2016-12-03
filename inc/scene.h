#pragma once
#include <glm/glm.hpp>

int initScene(glm::mat4 Projection, bool benchmark);

void drawScene(double timestep);

void cleanupScene();
