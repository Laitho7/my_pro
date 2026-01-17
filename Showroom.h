#ifndef SHOWROOM_H
#define SHOWROOM_H

#include <glm/glm.hpp>
#include "Shader.h" 

class Showroom {
public:
    Showroom();
    void Draw(Shader& shader, unsigned int VAO);
    void drawModernCeiling(Shader& shader, unsigned int VAO, float fY, float roomDim);

private:
    glm::vec3 platformPositions[4];
    glm::vec4 platformColor;
    glm::vec4 jewelColor;
    glm::vec4 neonColor;
};

#endif