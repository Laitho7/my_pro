#include "SecondFloorDesign.h"
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <cmath>

void SecondFloorDesign::Draw(Shader& shader, unsigned int VAO, float fY, float roomDim) {
  
    drawFirstFloorCeiling(shader, VAO, fY + 50.0f, roomDim);
}
void SecondFloorDesign::drawFirstFloorCeiling(Shader& shader, unsigned int VAO, float fY, float roomDim) {
    glBindVertexArray(VAO);

    glm::vec4 ceilBaseColor(0.01f, 0.01f, 0.02f, 1.0f); 
    glm::vec4 lightBlueWhite(0.7f, 0.85f, 1.0f, 0.9f);
    glm::vec4 pureWhiteNeon(2.5f, 2.5f, 3.0f, 1.0f);   
    float lightOffset = 90.0f;
    for (int side : {-1, 1}) {
        glm::mat4 lightM = glm::translate(glm::mat4(1.0f), glm::vec3(side * lightOffset, fY +17, 0.0f));
        shader.setMat4("model", glm::scale(lightM, glm::vec3(3.5f, 0.6f, roomDim)));
        shader.setVec4("ourColor", pureWhiteNeon);
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }
    float panelW = 120.0f;
    float panelD = 45.0f;
    float panelH = 0.8f;

    for (int i = 0; i < 3; i++) {
        float zPos = -65.0f + (i * 65.0f);
        glm::mat4 pPos = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, fY +17.5f, zPos));
        shader.setMat4("model", glm::scale(pPos, glm::vec3(panelW + 8.0f, panelH + 0.2f, panelD + 8.0f)));
        shader.setVec4("ourColor", pureWhiteNeon);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        shader.setMat4("model", glm::scale(pPos, glm::vec3(panelW, panelH + 0.4f, panelD)));
        shader.setVec4("ourColor", lightBlueWhite);
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }
}