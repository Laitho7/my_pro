#pragma once 
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Shader.h"

class Environment {
public:
    void Draw(Shader& shader, unsigned int VAO); 

private:

    void drawWorldCube(Shader& shader, unsigned int VAO);
    void drawBuildings(Shader& shader, unsigned int VAO);
    void drawTrafficLight(Shader& shader, glm::vec3 pos);
    void drawParkingLot(Shader& shader);
    void drawCar(Shader& shader, glm::vec3 pos, glm::vec4 color, float scale);
};