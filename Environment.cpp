#include "Environment.h"
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
void Environment::Draw(Shader& shader, unsigned int VAO) {
    drawWorldCube(shader, VAO);
    drawBuildings(shader, VAO);
    drawParkingLot(shader);
    const float roomDim = 300.0f;
    float sw = 80.0f;
    float rw = 120.0f;

    glm::vec4 roadColor(0.05f, 0.05f, 0.06f, 1.0f);
    glm::vec4 roadLineColor(1.0f, 0.8f, 0.0f, 1.0f);

    for (int side = 0; side < 4; side++) {
        glm::mat4 sideRot = glm::rotate(glm::mat4(1.0f), glm::radians(side * 90.0f), glm::vec3(0, 1, 0));
        glm::mat4 roadModel = glm::translate(sideRot, glm::vec3(0.0f, -2.5f, (roomDim / 2.0f) + sw + (rw / 2.0f)));
        shader.setMat4("model", glm::scale(roadModel, glm::vec3(roomDim + (sw * 2) + (rw * 2), 4.0f, rw)));
        shader.setVec4("ourColor", roadColor);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        for (int i = 0; i < 8; i++) {
            float lineX = -300.0f + (i * 60.0f);
            glm::mat4 lineModel = glm::translate(sideRot, glm::vec3(lineX, -0.5f, (roomDim / 2.0f) + sw + (rw / 2.0f)));
            shader.setMat4("model", glm::scale(lineModel, glm::vec3(25.0f, 0.4f, 4.0f)));
            shader.setVec4("ourColor", roadLineColor);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
    }
    drawTrafficLight(shader, glm::vec3(220.0f, 0.0f, 160.0f));
    glm::vec3 parkingPos(0.0f, -2.5f, 450.0f);
    float spaceWidth = 300.0f / 5.0f;
    float startX = -(300.0f / 2.0f) + (spaceWidth / 2.0f);

    drawCar(shader, parkingPos + glm::vec3(startX, 3.0f, 0.0f), glm::vec4(0.02f, 0.02f, 0.02f, 1.0f), 4.0f);
    drawCar(shader, parkingPos + glm::vec3(startX + spaceWidth, 3.0f, 0.0f), glm::vec4(0.0f, 0.1f, 0.5f, 1.0f), 4.0f);
    drawCar(shader, parkingPos + glm::vec3(startX + (spaceWidth * 3), 3.0f, 0.0f), glm::vec4(0.6f, 0.0f, 0.0f, 1.0f), 4.0f);
}
void Environment::drawWorldCube(Shader& shader, unsigned int VAO) {
    float worldSize = 1500.0f;
    glm::vec4 groundColor(0.1f, 0.5f, 0.1f, 1.0f);
    glm::vec4 skyColor(0.4f, 0.7f, 1.0f, 0.25f);

    glBindVertexArray(VAO);

    glm::mat4 groundModel = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -2.0f, 0.0f));
    groundModel = glm::scale(groundModel, glm::vec3(worldSize, 0.1f, worldSize));
    shader.setMat4("model", groundModel);
    shader.setVec4("ourColor", groundColor);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glm::mat4 skyModel = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, (worldSize / 2.0f) - 2.0f, 0.0f));
    skyModel = glm::scale(skyModel, glm::vec3(worldSize));
    shader.setMat4("model", skyModel);

    glDepthMask(GL_FALSE);
    shader.setVec4("ourColor", skyColor);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glDepthMask(GL_TRUE);
}
void Environment::drawBuildings(Shader& shader, unsigned int VAO) {
    struct BuildingData { glm::vec3 pos; glm::vec3 size; glm::vec4 color; };
    float unifiedHeight = 650.0f;

    BuildingData buildings[] = {
      {{-150, 0, -475}, {180, unifiedHeight, 150}, {0.04f, 0.04f, 0.07f, 0.4f}},
      {{ 150, 0, -475}, {180, unifiedHeight, 150}, {0.05f, 0.05f, 0.08f, 0.4f}},
      {{ 475, 0, -100}, {150, unifiedHeight, 180}, {0.03f, 0.03f, 0.06f, 0.4f}},
      {{ 460, 0,  220}, {120, unifiedHeight, 150}, {0.05f, 0.05f, 0.09f, 0.4f}},
      {{-475, 0, -100}, {150, unifiedHeight, 180}, {0.04f, 0.04f, 0.08f, 0.4f}},
      {{-460, 0,  220}, {120, unifiedHeight, 150}, {0.06f, 0.06f, 0.10f, 0.4f}},
      {{-280, 0,  475}, {150, unifiedHeight, 150}, {0.04f, 0.04f, 0.07f, 0.4f}},
      {{ 280, 0,  475}, {150, unifiedHeight, 150}, {0.05f, 0.05f, 0.08f, 0.4f}}
    };

    glm::vec4 grassColor(0.1f, 0.4f, 0.12f, 1.0f);
    glm::vec4 neonColor(1.0f, 1.0f, 1.0f, 1.0f); 
    glm::vec4 yellowNeon(1.0f, 1.0f, 0.0f, 1.0f);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    for (int i = 0; i < 8; i++) {
        glm::mat4 grassModel = glm::translate(glm::mat4(1.0f), glm::vec3(buildings[i].pos.x, -1.8f, buildings[i].pos.z));
        shader.setMat4("model", glm::scale(grassModel, glm::vec3(buildings[i].size.x + 100.0f, 0.5f, buildings[i].size.z + 100.0f)));
        shader.setVec4("ourColor", grassColor);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(buildings[i].pos.x, buildings[i].size.y / 2.0f - 2.0f, buildings[i].pos.z));
        shader.setMat4("model", glm::scale(model, buildings[i].size));
        shader.setVec4("ourColor", buildings[i].color);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        float neonWidth = 10.0f;
        float offset_X = buildings[i].size.x / 2.0f;
        float offset_Z = buildings[i].size.z / 2.0f;
        glm::vec3 cornerOffsets[] = {
            { -offset_X, 0, -offset_Z }, {  offset_X, 0, -offset_Z },
            { -offset_X, 0,  offset_Z }, {  offset_X, 0,  offset_Z }
        };
        shader.setVec4("ourColor", neonColor);
        for (int j = 0; j < 4; j++) {
            glm::mat4 neonModel = glm::translate(glm::mat4(1.0f), glm::vec3(buildings[i].pos.x + cornerOffsets[j].x, buildings[i].size.y / 2.0f, buildings[i].pos.z + cornerOffsets[j].z));
            shader.setMat4("model", glm::scale(neonModel, glm::vec3(neonWidth, buildings[i].size.y, neonWidth)));
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
        shader.setVec4("ourColor", yellowNeon);
        float gridThickness = 2.0f; 
        int horizontalDivisions = 5; 
        int verticalDivisions = 3; 
        for (int h = 1; h < horizontalDivisions; h++) {
            float yPos = (buildings[i].size.y / horizontalDivisions) * h;
            glm::mat4 horizModel = glm::translate(glm::mat4(1.0f), glm::vec3(buildings[i].pos.x, yPos, buildings[i].pos.z));
            shader.setMat4("model", glm::scale(horizModel, glm::vec3(buildings[i].size.x + 1.0f, gridThickness, buildings[i].size.z + 1.0f)));
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
        for (int v = 1; v < verticalDivisions; v++) {
            float xOffset = (buildings[i].size.x / verticalDivisions) * v - (buildings[i].size.x / 2.0f);
            float zOffset = (buildings[i].size.z / verticalDivisions) * v - (buildings[i].size.z / 2.0f);
            glm::mat4 vertX = glm::translate(glm::mat4(1.0f), glm::vec3(buildings[i].pos.x + xOffset, buildings[i].size.y / 2.0f, buildings[i].pos.z));
            shader.setMat4("model", glm::scale(vertX, glm::vec3(gridThickness, buildings[i].size.y, buildings[i].size.z + 1.5f)));
            glDrawArrays(GL_TRIANGLES, 0, 36);
            glm::mat4 vertZ = glm::translate(glm::mat4(1.0f), glm::vec3(buildings[i].pos.x, buildings[i].size.y / 2.0f, buildings[i].pos.z + zOffset));
            shader.setMat4("model", glm::scale(vertZ, glm::vec3(buildings[i].size.x + 1.5f, buildings[i].size.y, gridThickness)));
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
    }
}

void Environment::drawTrafficLight(Shader& shader, glm::vec3 pos) {
    float currentTime = glfwGetTime();
    int cycle = (int)currentTime % 18; 
    glm::mat4 model = glm::translate(glm::mat4(1.0f), pos + glm::vec3(0, 20.0f, 0));
    shader.setMat4("model", glm::scale(model, glm::vec3(2.0f, 40.0f, 2.0f)));
    shader.setVec4("ourColor", glm::vec4(0.1f, 0.1f, 0.1f, 1.0f));
    glDrawArrays(GL_TRIANGLES, 0, 36);
    model = glm::translate(glm::mat4(1.0f), pos + glm::vec3(0, 45.0f, 0));
    shader.setMat4("model", glm::scale(model, glm::vec3(6.0f, 14.0f, 6.0f)));
    shader.setVec4("ourColor", glm::vec4(0.05f, 0.05f, 0.05f, 1.0f));
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glm::vec4 offColor = glm::vec4(0.2f, 0.2f, 0.2f, 1.0f);
    glm::vec4 red = (cycle < 6) ? glm::vec4(1.0f, 0.0f, 0.0f, 1.0f) : offColor;
    glm::mat4 redModel = glm::translate(glm::mat4(1.0f), pos + glm::vec3(0, 49.0f, 3.1f));
    shader.setMat4("model", glm::scale(redModel, glm::vec3(3.0f, 3.0f, 1.0f)));
    shader.setVec4("ourColor", red);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glm::vec4 yellow = (cycle >= 6 && cycle < 12) ? glm::vec4(1.0f, 1.0f, 0.0f, 1.0f) : offColor;
    glm::mat4 yellowModel = glm::translate(glm::mat4(1.0f), pos + glm::vec3(0, 45.0f, 3.1f));
    shader.setMat4("model", glm::scale(yellowModel, glm::vec3(3.0f, 3.0f, 1.0f)));
    shader.setVec4("ourColor", yellow);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glm::vec4 green = (cycle >= 12) ? glm::vec4(0.0f, 1.0f, 0.0f, 1.0f) : offColor;
    glm::mat4 greenModel = glm::translate(glm::mat4(1.0f), pos + glm::vec3(0, 41.0f, 3.1f));
    shader.setMat4("model", glm::scale(greenModel, glm::vec3(3.0f, 3.0f, 1.0f)));
    shader.setVec4("ourColor", green);
    glDrawArrays(GL_TRIANGLES, 0, 36);
}
void Environment::drawParkingLot(Shader& shader) {
    float parkingWidth = 300.0f;
    float parkingDepth = 200.0f;
    glm::vec3 parkingPos(0.0f, -2.5f, 450.0f);
    glm::mat4 model = glm::translate(glm::mat4(1.0f), parkingPos);
    shader.setMat4("model", glm::scale(model, glm::vec3(parkingWidth, 1.5f, parkingDepth)));
    shader.setVec4("ourColor", glm::vec4(0.15f, 0.15f, 0.15f, 1.0f));
    glDrawArrays(GL_TRIANGLES, 0, 36);

    int numSpaces = 5;
    float spaceWidth = parkingWidth / numSpaces;
    shader.setVec4("ourColor", glm::vec4(0.8f, 0.8f, 0.8f, 1.0f));
    for (int i = 0; i <= numSpaces; i++) {
        float xOff = -(parkingWidth / 2.0f) + (i * spaceWidth);
        glm::mat4 lineM = glm::translate(glm::mat4(1.0f), parkingPos + glm::vec3(xOff, 0.8f, 0.0f));
        shader.setMat4("model", glm::scale(lineM, glm::vec3(1.2f, 0.1f, parkingDepth - 10.0f)));
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }

    shader.setVec4("ourColor", glm::vec4(0.3f, 0.3f, 0.32f, 1.0f));
    for (int i = 0; i < numSpaces; i++) {
        float xOff = -(parkingWidth / 2.0f) + (i * spaceWidth) + (spaceWidth / 2.0f);
        glm::mat4 bumperM = glm::translate(glm::mat4(1.0f), parkingPos + glm::vec3(xOff, 4.5f, (parkingDepth / 2.0f) - 10.0f));
        shader.setMat4("model", glm::scale(bumperM, glm::vec3(spaceWidth * 0.6f, 5.2f, 4.0f)));
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }
    float lampHeight = 60.0f;
    float lampSideOffset = (parkingWidth / 2.0f) + 5.0f;
    float xPositions[] = { -lampSideOffset, lampSideOffset };

    for (int j = 0; j < 2; j++) {
        glm::vec3 lampPos = parkingPos + glm::vec3(xPositions[j], lampHeight / 2.0f, 0.0f);
        glm::mat4 poleModel = glm::translate(glm::mat4(1.0f), lampPos);
        shader.setMat4("model", glm::scale(poleModel, glm::vec3(2.5f, lampHeight, 2.5f)));
        shader.setVec4("ourColor", glm::vec4(0.1f, 0.1f, 0.1f, 1.0f));
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glm::vec3 headPos = lampPos + glm::vec3((j == 0 ? 5.0f : -5.0f), (lampHeight / 2.0f), 0.0f);
        glm::mat4 headModel = glm::translate(glm::mat4(1.0f), headPos);
        shader.setMat4("model", glm::scale(headModel, glm::vec3(12.0f, 3.0f, 6.0f)));
        shader.setVec4("ourColor", glm::vec4(2.5f, 2.5f, 2.5f, 1.0f));
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }
    glm::vec4 iceColor(0.9f, 0.95f, 1.0f, 0.8f);
    glm::vec3 icePos(0.0f, -2.0f, 185.0f);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glm::mat4 iceModel = glm::translate(glm::mat4(1.0f), icePos);
    shader.setMat4("model", glm::scale(iceModel, glm::vec3(100.0f, 0.5f, 90.0f)));
    shader.setVec4("ourColor", iceColor);
    glDrawArrays(GL_TRIANGLES, 0, 36);
}
void Environment::drawCar(Shader& shader, glm::vec3 pos, glm::vec4 color, float scale) {
    glm::vec4 wheelColor(0.02f, 0.02f, 0.02f, 1.0f);
    glm::vec4 glassColor(0.1f, 0.1f, 0.15f, 0.9f);
    glm::vec4 bumperColor(0.1f, 0.1f, 0.1f, 1.0f);
    glm::vec4 plateColor(0.9f, 0.9f, 0.8f, 1.0f); 
    glm::mat4 base = glm::translate(glm::mat4(1.0f), pos + glm::vec3(0, 0.8f * scale, 0));
    shader.setMat4("model", glm::scale(base, glm::vec3(8.2f * scale, 1.0f * scale, 21.0f * scale)));
    shader.setVec4("ourColor", bumperColor);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glm::mat4 body = glm::translate(glm::mat4(1.0f), pos + glm::vec3(0, 1.6f * scale, 0));
    shader.setMat4("model", glm::scale(body, glm::vec3(8.0f * scale, 1.2f * scale, 20.0f * scale)));
    shader.setVec4("ourColor", color);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glm::mat4 cabinLower = glm::translate(glm::mat4(1.0f), pos + glm::vec3(0, 2.6f * scale, -0.5f * scale));
    shader.setMat4("model", glm::scale(cabinLower, glm::vec3(7.0f * scale, 1.0f * scale, 12.0f * scale)));
    shader.setVec4("ourColor", glassColor);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glm::mat4 roof = glm::translate(glm::mat4(1.0f), pos + glm::vec3(0, 3.2f * scale, -1.0f * scale));
    shader.setMat4("model", glm::scale(roof, glm::vec3(6.5f * scale, 0.3f * scale, 10.0f * scale)));
    shader.setVec4("ourColor", color);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    float xOff[4] = { -3.8f, 3.8f, -3.8f, 3.8f };
    float zOff[4] = { -6.5f, -6.5f, 6.5f, 6.5f };
    for (int i = 0; i < 4; i++) {
        glm::mat4 wheel = glm::translate(glm::mat4(1.0f), pos + glm::vec3(xOff[i] * scale, 0.8f * scale, zOff[i] * scale));
        shader.setMat4("model", glm::scale(wheel, glm::vec3(1.6f * scale, 2.0f * scale, 2.8f * scale)));
        shader.setVec4("ourColor", wheelColor);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glm::mat4 rim = glm::translate(glm::mat4(1.0f), pos + glm::vec3(xOff[i] * 1.05f * scale, 0.8f * scale, zOff[i] * scale));
        shader.setMat4("model", glm::scale(rim, glm::vec3(1.4f * scale, 1.2f * scale, 1.6f * scale)));
        shader.setVec4("ourColor", glm::vec4(0.5f, 0.5f, 0.5f, 1.0f));
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }
    for (float s : {-4.2f, 4.2f}) {
        glm::mat4 mirror = glm::translate(glm::mat4(1.0f), pos + glm::vec3(s * scale, 2.4f * scale, -4.0f * scale));
        shader.setMat4("model", glm::scale(mirror, glm::vec3(0.8f * scale, 0.5f * scale, 0.4f * scale)));
        shader.setVec4("ourColor", color);
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }
    float plateZ[2] = { -10.6f, 10.1f };
    for (float z : plateZ) {
        glm::mat4 plate = glm::translate(glm::mat4(1.0f), pos + glm::vec3(0, 1.0f * scale, z * scale));
        shader.setMat4("model", glm::scale(plate, glm::vec3(2.5f * scale, 0.8f * scale, 0.1f * scale)));
        shader.setVec4("ourColor", plateColor);
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }
    glm::mat4 grill = glm::translate(glm::mat4(1.0f), pos + glm::vec3(0, 1.4f * scale, -10.1f * scale));
    shader.setMat4("model", glm::scale(grill, glm::vec3(4.0f * scale, 0.6f * scale, 0.1f * scale)));
    shader.setVec4("ourColor", bumperColor);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    shader.setVec4("ourColor", glm::vec4(3.0f, 3.0f, 2.0f, 1.0f));
    for (float s : {-2.8f, 2.8f}) {
        glm::mat4 light = glm::translate(glm::mat4(1.0f), pos + glm::vec3(s * scale, 1.6f * scale, -10.1f * scale));
        shader.setMat4("model", glm::scale(light, glm::vec3(1.8f * scale, 0.6f * scale, 0.2f * scale)));
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }
    shader.setVec4("ourColor", glm::vec4(2.0f, 0.0f, 0.0f, 1.0f));
    for (float s : {-2.8f, 2.8f}) {
        glm::mat4 light = glm::translate(glm::mat4(1.0f), pos + glm::vec3(s * scale, 1.6f * scale, 10.1f * scale));
        shader.setMat4("model", glm::scale(light, glm::vec3(1.8f * scale, 0.4f * scale, 0.2f * scale)));
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }
}  
