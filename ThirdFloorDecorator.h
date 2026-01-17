#ifndef THIRDFLOORDECORATOR_H
#define THIRDFLOORDECORATOR_H

#include <glm/glm.hpp>
#include "Shader.h"

class ThirdFloorDecorator {
public:
    void Draw(Shader& shader, unsigned int VAO, float fY, float roomDim);
    bool curtainOpened = false; 
    float curtainWidthFactor = 0.25f;

private:
    void drawThirdFloorCeiling(Shader& shader, unsigned int VAO, float fY, float roomDim);
    void drawGroundPlatforms(Shader& shader, unsigned int VAO, float fY);
    void drawCarOnPlatform(Shader& shader, glm::vec3 pos, glm::vec4 color, float scale);
    void drawOfficeTable(Shader& shader, unsigned int VAO, glm::vec3 pos, float scale);
    void drawOfficeChair(Shader& shader, unsigned int VAO, glm::vec3 pos, float scale, float angleDegrees);
    void drawCrystal(Shader& shader, unsigned int VAO, glm::vec3 pos, glm::vec4 color, float scale);
    void drawCurtain(Shader& shader, unsigned int VAO, glm::vec3 pos, float fY);
};

#endif