#ifndef SECONDFLOOR_H
#define SECONDFLOOR_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Shader.h"

class SecondFloorDesign {
public:
    void Draw(Shader& shader, unsigned int VAO, float fY, float roomDim);
    void drawFirstFloorCeiling(Shader& shader, unsigned int VAO, float fY, float roomDim);
    void drawCeilingPanels(Shader& shader, float fY, float roomDim);
};

#endif