#include "ThirdFloorDecorator.h"
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>

void ThirdFloorDecorator::Draw(Shader& shader, unsigned int VAO, float fY, float roomDim) {
    glBindVertexArray(VAO);

    glm::vec4 floorColor(0.55f, 0.40f, 0.30f, 1.0f);
    glm::mat4 mainFloorM = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, fY + 0.5f, 0.0f));
    shader.setMat4("model", glm::scale(mainFloorM, glm::vec3(roomDim, 0.2f, roomDim)));
    shader.setVec4("ourColor", floorColor);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    drawThirdFloorCeiling(shader, VAO, fY, roomDim);
    drawGroundPlatforms(shader, VAO, fY);
    drawCarOnPlatform(shader, glm::vec3(70.0f, fY + 2.0f, 0.0f), glm::vec4(0.35f, 0.20f, 0.10f, 1.0f), 5.0f);
    drawCarOnPlatform(shader, glm::vec3(-70.0f, fY + 2.0f, 0.0f), glm::vec4(0.0f, 0.2f, 0.8f, 1.0f), 5.0f);
    glm::vec3 tablePos(0.0f, fY, 130.0f);
    drawOfficeTable(shader, VAO, tablePos, 1.0f);
    drawOfficeTable(shader, VAO, glm::vec3(0.0f, fY, 130.0f), 1.0f);
    drawOfficeChair(shader, VAO, tablePos + glm::vec3(-12.0f, 0.0f, 10.0f), 1.0f, -90);
    drawOfficeChair(shader, VAO, tablePos + glm::vec3(12.0f, 0.0f, 10.0f), 1.0f, 90);
    for (int i = 0; i < 5; i++) {
        float zOffset = -90.0f + (i * 50.0f);
        glm::vec3 cPos(130.0f, fY, zOffset);
        float angle = 90.0f;

        drawOfficeChair(shader, VAO, cPos, 1.0f, angle);
    }
    for (int i = 0; i < 5; i++) {
        float zOffset = -110.0f + (i * 50.0f);
        glm::vec3 cPos(-130.0f, fY, zOffset);
        float angle = -90.0f;


        drawOfficeChair(shader, VAO, cPos, 1.0f, angle);
    }
    float cornerPos = (roomDim / 2.0f) - 15.0f; 
    float liftAmount = 30.0f; 

    drawCrystal(shader, VAO, glm::vec3(cornerPos, fY + liftAmount-2, cornerPos), glm::vec4(0.0f, 0.5f, 1.0f, 0.7f), 15.0f);
    drawCrystal(shader, VAO, glm::vec3(-cornerPos, fY + liftAmount-2, cornerPos-2), glm::vec4(0.0f, 1.0f, 0.5f, 0.7f), 15.0f);
    drawCrystal(shader, VAO, glm::vec3(-cornerPos, fY + liftAmount-2, -cornerPos+7), glm::vec4(0.6f, 0.2f, 1.0f, 0.7f), 15.0f);
    drawCrystal(shader, VAO, glm::vec3(-cornerPos+205, fY + liftAmount-2, -cornerPos + 7), glm::vec4(0.0f, 0.5f, 1.0f, 0.7f), 15.0f);
    drawCrystal(shader, VAO, glm::vec3(-cornerPos + 105, fY + liftAmount - 2, -cornerPos + 7), glm::vec4(0.31f, 0.78f, 0.47f, 0.75f), 15.0f);
    drawCrystal(shader, VAO, glm::vec3(-cornerPos + 125, fY + liftAmount - 2, -cornerPos + 200), glm::vec4(0.95f, 0.98f, 1.0f, 0.6f), 15.0f);


    glm::vec3 curtainPos(0.0f, fY+5, 148.0f);
    drawCurtain(shader, VAO, curtainPos, fY);
}

void ThirdFloorDecorator::drawOfficeTable(Shader& shader, unsigned int VAO, glm::vec3 pos, float scale) {
    glBindVertexArray(VAO);
    glm::vec4 woodLegColor(0.35f, 0.20f, 0.10f, 1.0f); 
    float xPos[4] = { -24.0f, 24.0f, -24.0f, 24.0f };
    float zPos[4] = { -4.0f, -4.0f, 4.0f, 4.0f };

    for (int i = 0; i < 4; i++) {
        glm::mat4 leg = glm::translate(glm::mat4(1.0f), pos + glm::vec3(xPos[i] * scale, 4.0f * scale, zPos[i] * scale));
        shader.setMat4("model", glm::scale(leg, glm::vec3(1.0f * scale, 8.0f * scale, 1.0f * scale)));
        shader.setVec4("ourColor", woodLegColor);
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }
    glm::mat4 tableTop = glm::translate(glm::mat4(1.0f), pos + glm::vec3(0.0f, 8.0f * scale, 0.0f));
    shader.setMat4("model", glm::scale(tableTop, glm::vec3(50.0f * scale, 0.5f * scale, 10.0f * scale)));
    shader.setVec4("ourColor", woodLegColor);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDrawArrays(GL_TRIANGLES, 0, 36);
  

}
void ThirdFloorDecorator::drawOfficeChair(Shader& shader, unsigned int VAO, glm::vec3 pos, float scale, float angleDegrees) {
    glBindVertexArray(VAO);
    glm::vec4 woodColor(0.35f, 0.20f, 0.10f, 1.0f);
    glm::vec4 seatColor(0.15f, 0.15f, 0.15f, 1.0f);
    glm::mat4 baseModel = glm::translate(glm::mat4(1.0f), pos);
    baseModel = glm::rotate(baseModel, glm::radians(angleDegrees), glm::vec3(0.0f, 1.0f, 0.0f));

    float legX[4] = { -2.5f, 2.5f, -2.5f, 2.5f }; 
    float legZ[4] = { -2.5f, -2.5f, 2.5f, 2.5f };
    for (int i = 0; i < 4; i++) {
        glm::mat4 leg = glm::translate(baseModel, glm::vec3(legX[i] * scale, 2.5f * scale, legZ[i] * scale));
        shader.setMat4("model", glm::scale(leg, glm::vec3(0.8f * scale, 5.0f * scale, 0.8f * scale))); 
        shader.setVec4("ourColor", woodColor);
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }
    glm::mat4 seat = glm::translate(baseModel, glm::vec3(0.0f, 5.2f * scale, 0.0f));
    shader.setMat4("model", glm::scale(seat, glm::vec3(6.5f * scale, 1.0f * scale, 6.5f * scale))); 
    shader.setVec4("ourColor", seatColor);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glm::mat4 back = glm::translate(baseModel, glm::vec3(0.0f, 9.5f * scale, 3.0f * scale));
    shader.setMat4("model", glm::scale(back, glm::vec3(6.5f * scale, 8.5f * scale, 0.8f * scale))); 
    shader.setVec4("ourColor", seatColor);
    glDrawArrays(GL_TRIANGLES, 0, 36);
}

void ThirdFloorDecorator::drawThirdFloorCeiling(Shader& shader, unsigned int VAO, float fY, float roomDim) {
    glBindVertexArray(VAO);
    glm::vec4 woodColor(0.35f, 0.20f, 0.10f, 1.0f);
    glm::vec4 yellowPanel(1.0f, 0.85f, 0.0f, 1.0f);
    glm::vec4 pureWhiteNeon(3.0f, 3.0f, 3.5f, 1.0f);
    float ceilHeight = fY + 49.0f;
    glm::mat4 ceilM = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, ceilHeight + 10, 0.0f));
    shader.setMat4("model", glm::scale(ceilM, glm::vec3(roomDim, 0.4f, roomDim)));
    shader.setVec4("ourColor", woodColor);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    float beamW = 220.0f;
    float beamD = 20.0f;
    float beamH = 0.8f;

    for (int i = 0; i < 4; i++) {
        float zPos = -90.0f + (i * 60.0f);
        glm::mat4 pPos = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, ceilHeight + 9.4f, zPos));
        shader.setMat4("model", glm::scale(pPos, glm::vec3(beamW + 10.0f, beamH + 0.2f, beamD + 8.0f)));
        shader.setVec4("ourColor", pureWhiteNeon);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        shader.setMat4("model", glm::scale(pPos, glm::vec3(beamW, beamH + 0.4f, beamD)));
        shader.setVec4("ourColor", yellowPanel);
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }
}

void ThirdFloorDecorator::drawGroundPlatforms(Shader& shader, unsigned int VAO, float fY) {
    glBindVertexArray(VAO);
    glm::vec4 woodColor(0.70f, 0.55f, 0.40f, 1.0f);
    glm::vec4 neonWhite(3.5f, 3.5f, 4.0f, 1.0f);

    for (float side : {-70.0f, 70.0f}) {
        glm::mat4 lightModel = glm::translate(glm::mat4(1.0f), glm::vec3(side, fY - 0.1f, 0.0f));
        shader.setMat4("model", glm::scale(lightModel, glm::vec3(64.0f, 1.5f, 124.0f)));
        shader.setVec4("ourColor", neonWhite);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        glm::mat4 platformModel = glm::translate(glm::mat4(1.0f), glm::vec3(side, fY, 0.0f));
        shader.setMat4("model", glm::scale(platformModel, glm::vec3(60.0f, 2.0f, 120.0f)));
        shader.setVec4("ourColor", woodColor);
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }
}

void ThirdFloorDecorator::drawCarOnPlatform(Shader& shader, glm::vec3 pos, glm::vec4 color, float scale) {
    glm::mat4 model = glm::translate(glm::mat4(1.0f), pos + glm::vec3(0, 1.0f * scale, 0));
    shader.setMat4("model", glm::scale(model, glm::vec3(8.0f * scale, 1.5f * scale, 22.0f * scale)));
    shader.setVec4("ourColor", color);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    glm::vec4 glassColor(0.6f, 0.8f, 1.0f, 0.5f);
    glm::mat4 cabin = glm::translate(glm::mat4(1.0f), pos + glm::vec3(0, 2.3f * scale, 0.0f));
    shader.setMat4("model", glm::scale(cabin, glm::vec3(6.5f * scale, 1.2f * scale, 16.0f * scale)));
    shader.setVec4("ourColor", glassColor);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    float pillarPositions[] = { -6.0f, -2.0f, 2.0f, 6.0f };
    for (float zOffset : pillarPositions) {
        glm::mat4 pillar = glm::translate(glm::mat4(1.0f), pos + glm::vec3(0, 2.3f * scale, zOffset * scale));
        shader.setMat4("model", glm::scale(pillar, glm::vec3(6.8f * scale, 1.3f * scale, 0.6f * scale)));
        shader.setVec4("ourColor", color);
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }
    glm::mat4 roof = glm::translate(glm::mat4(1.0f), pos + glm::vec3(0, 2.9f * scale, 0.0f));
    shader.setMat4("model", glm::scale(roof, glm::vec3(6.6f * scale, 0.2f * scale, 16.2f * scale)));
    shader.setVec4("ourColor", color);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glm::vec4 wheelColor(0.05f, 0.05f, 0.05f, 1.0f);
    glm::vec4 rimColor(1.0f, 1.0f, 1.0f, 1.0f);

    float xOff[4] = { -3.8f, 3.8f, -3.8f, 3.8f };
    float zOff[4] = { -8.5f, -8.5f, 8.5f, 8.5f };

    for (int i = 0; i < 4; i++) {
        glm::vec3 wheelPos = pos + glm::vec3(xOff[i] * scale, 0.7f * scale, zOff[i] * scale);
        glm::mat4 wheel = glm::translate(glm::mat4(1.0f), wheelPos);
        shader.setMat4("model", glm::scale(wheel, glm::vec3(1.5f * scale, 1.8f * scale, 2.5f * scale)));
        shader.setVec4("ourColor", wheelColor);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        float sideSign = (xOff[i] > 0) ? 0.1f : -0.1f;
        glm::mat4 rim = glm::translate(glm::mat4(1.0f), wheelPos + glm::vec3(sideSign * scale, 0.0f, 0.0f));
        shader.setMat4("model", glm::scale(rim, glm::vec3(1.6f * scale, 1.2f * scale, 1.5f * scale)));
        shader.setVec4("ourColor", rimColor);
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }
    glm::vec4 headlightColor(3.0f, 3.0f, 2.0f, 1.0f);
    for (float s : {-2.5f, 2.5f}) {
        glm::mat4 headlight = glm::translate(glm::mat4(1.0f), pos + glm::vec3(s * scale, 1.2f * scale, -11.1f * scale));
        shader.setMat4("model", glm::scale(headlight, glm::vec3(1.5f * scale, 0.8f * scale, 0.2f * scale)));
        shader.setVec4("ourColor", headlightColor);
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }
}
void ThirdFloorDecorator::drawCrystal(Shader& shader, unsigned int VAO, glm::vec3 pos, glm::vec4 color, float scale) {
    glBindVertexArray(VAO);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  float time = (float)glfwGetTime();
    float pulse = (sin(time * 2.0f) * 0.5f + 0.5f); 
    glm::vec4 emissiveColor = color * (2.0f + pulse * 3.0f);
    emissiveColor.a = 1.0f; 
    float angles[] = { 0.0f, 120.0f, 240.0f };

    for (int i = 0; i < 3; i++) {
        glm::mat4 model = glm::translate(glm::mat4(1.0f), pos);
        model = glm::rotate(model, glm::radians(angles[i]), glm::vec3(0, 1, 0));
        model = glm::rotate(model, glm::radians(15.0f + (pulse * 5.0f)), glm::vec3(1, 0, 0)); 

        float sX = (i == 0) ? 0.8f : 0.5f;
        float sY = (i == 0) ? 3.5f : 2.2f;

        shader.setMat4("model", glm::scale(model, glm::vec3(sX * scale, sY * scale, sX * scale)));
        shader.setVec4("ourColor", color);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glLineWidth(3.0f); 
        shader.setVec4("ourColor", emissiveColor);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
}
void ThirdFloorDecorator::drawCurtain(Shader& shader, unsigned int VAO, glm::vec3 pos, float fY) {
    glBindVertexArray(VAO);
    glm::vec4 iceMirrorColor(0.95f, 0.98f, 1.0f, 0.7f);
    float targetWidth = curtainOpened ? 1.0f : 0.25f;
    if (curtainWidthFactor < targetWidth) curtainWidthFactor += 0.01f;
    if (curtainWidthFactor > targetWidth) curtainWidthFactor -= 0.01f;
    int segments = 20; 
    float totalMaxWidth = 170.0f;
    float currentTotalWidth = totalMaxWidth * curtainWidthFactor;
    float segmentWidth = currentTotalWidth / segments;
    float rightEdgeX = pos.x + (totalMaxWidth / 2.0f);

    for (int i = 0; i < segments; i++) {
        float xPos = rightEdgeX - (i * segmentWidth) - (segmentWidth / 2.0f);
        float zWave = (i % 2 == 0) ? 0.2f : -0.2f;
        glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(xPos, pos.y + 25.0f, pos.z + zWave));
        model = glm::rotate(model, glm::radians(zWave * 10.0f), glm::vec3(0, 1, 0));

        shader.setMat4("model", glm::scale(model, glm::vec3(segmentWidth, 50.0f, 0.1f)));
        shader.setVec4("ourColor", iceMirrorColor);

        glDrawArrays(GL_TRIANGLES, 0, 36);
    }
}