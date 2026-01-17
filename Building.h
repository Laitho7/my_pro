#ifndef BUILDING_H
#define BUILDING_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Shader.h"
#include <vector>

class Building {
public:
    float elevatorY = 0.0f;
    float targetElevatorY = 0.0f;
    float elevatorDoorOffset = 0.0f;
    float doorOffset = 0.0f;
    const float floorHeight = 60.0f;

    void Update(float deltaTime);
    void Draw(Shader& shader, unsigned int VAO);

private:
    void drawFloorsAndWalls(Shader& shader, unsigned int VAO);
    void drawElevator(Shader& shader, unsigned int VAO);
    void drawRockArch(Shader& shader, glm::vec3 position);
    void drawSignboard(Shader& shader);
    void drawLetter(Shader& shader, char letter, glm::vec3 pos, float scale);
    // دوال الكريستال (يجب إضافتها هنا)
    void drawCrystalDecor(Shader& shader, glm::vec3 position);
    void drawCrystalWide(Shader& shader, glm::vec3 position);
    void drawCrystalTall(Shader& shader, glm::vec3 position);
    void drawCrystalCluster(Shader& shader, glm::vec3 position);
    void drawCrystalSpikes(Shader& shader, glm::vec3 position);
    void drawCrystalStar(Shader& shader, glm::vec3 position);
    void drawCrystalGiant(Shader& shader, glm::vec3 position);
    void drawCrystal(Shader& shader, glm::vec3 position);
};

#endif