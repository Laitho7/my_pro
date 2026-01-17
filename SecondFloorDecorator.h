#ifndef SECONDFLOORDECORATOR_H
#define SECONDFLOORDECORATOR_H

#include <glm/glm.hpp>
#include "Shader.h"

class SecondFloorDecorator {
public:
    void Draw(Shader& shader, unsigned int VAO, float fY, float roomDim);

private:
    void drawDecoratedCeiling(Shader& shader, unsigned int VAO, float fY, float roomDim);
    void drawCrystalDecor(Shader& shader, glm::vec3 position);

};

#endif