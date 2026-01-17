#include "Showroom.h"
#include "Shader.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <cmath>

Showroom::Showroom() {
    platformPositions[0] = glm::vec3(-70.0f, 0.0f, -70.0f);
    platformPositions[1] = glm::vec3(70.0f, 0.0f, -70.0f);
    platformPositions[2] = glm::vec3(-70.0f, 0.0f, 70.0f);
    platformPositions[3] = glm::vec3(70.0f, 0.0f, 70.0f);

    platformColor = glm::vec4(0.05f, 0.05f, 0.07f, 1.0f);
    jewelColor = glm::vec4(0.0f, 0.6f, 1.0f, 0.4f);
    neonColor = glm::vec4(0.5f, 0.8f, 5.0f, 1.0f);
}

void Showroom::Draw(Shader& shader, unsigned int VAO) {
    float time = (float)glfwGetTime();

    drawModernCeiling(shader, VAO, 49.0f, 300.0f);
}

void Showroom::drawModernCeiling(Shader& shader, unsigned int VAO, float fY, float roomDim) {
    glBindVertexArray(VAO);
    glm::vec4 iceWhiteNeon(1.5f, 1.8f, 2.0f, 1.0f);
    int gridCount = 2; 
    float spacing = roomDim / gridCount;
    float cubeSize = 50.0f; 

    for (int x = 0; x < gridCount; x++) {
        for (int z = 0; z < gridCount; z++) {
            float xPos = -(roomDim / 2.0f) + (x * spacing) + (spacing / 2.0f);
            float zPos = -(roomDim / 2.0f) + (z * spacing) + (spacing / 2.0f);
            glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(xPos, fY + 6.0f, zPos));
            shader.setMat4("model", glm::scale(model, glm::vec3(cubeSize, 1.5f, cubeSize)));
            shader.setVec4("ourColor", iceWhiteNeon);
            glDrawArrays(GL_TRIANGLES, 0, 36);
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            shader.setMat4("model", glm::scale(model, glm::vec3(cubeSize + 0.1f, 1.6f, cubeSize + 0.1f)));
            shader.setVec4("ourColor", glm::vec4(2.0f, 2.5f, 3.0f, 1.0f));
            glDrawArrays(GL_TRIANGLES, 0, 36);
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }
    }
}