#include "Building.h"
#include <GLFW/glfw3.h>
#include <iostream>
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

void Building::Update(float deltaTime) {
    if (elevatorY < targetElevatorY) elevatorY += 25.0f * deltaTime;
    if (elevatorY > targetElevatorY) elevatorY -= 25.0f * deltaTime;
    if (abs(elevatorY - targetElevatorY) < 0.2f) elevatorY = targetElevatorY;
    if (doorOffset > 15.0f) doorOffset = 15.0f;
    if (doorOffset < 0.0f) doorOffset = 0.0f;
    if (elevatorDoorOffset > 20.0f) elevatorDoorOffset = 20.0f;
    if (elevatorDoorOffset < 0.0f) elevatorDoorOffset = 0.0f;
}

void Building::Draw(Shader& shader, unsigned int VAO) {
    glBindVertexArray(VAO);
    drawFloorsAndWalls(shader, VAO);
     drawElevator(shader, VAO);
    drawRockArch(shader, glm::vec3(0.0f, -14.0f, 144.5f));
    drawSignboard(shader);

}
//راما
void Building::drawFloorsAndWalls(Shader& shader, unsigned int VAO) {

    const float roomDim = 300.0f;
    const int numPanels = 20;
    const float panelWidth = roomDim / numPanels;
    glm::vec4 floorColor(0.08f, 0.08f, 0.1f, 1.0f); 
    glm::vec4 leadGrayFloor(0.35f, 0.35f, 0.38f, 1.0f);
    glm::vec4 ceilColorOuter(0.85f, 0.93f, 1.0f, 1.0f);
    glm::vec4 ceilColorInner(0.01f, 0.01f, 0.01f, 1.0f);
    glm::vec4 whiteNeon(2.5f, 2.5f, 3.5f, 1.0f);
    glm::vec4 beamColor(0.3f, 0.3f, 0.32f, 1.0f);
    glm::vec4 pillarGray(0.25f, 0.25f, 0.28f, 1.0f);

    for (int f = 0; f < 4; f++) {
        float yPos = f * floorHeight;
        float ceilY = yPos + floorHeight - 1.0f;
        if (f == 0) { // الطابق الأرضي
            float floorY = 0.0f;

            glm::vec3 platformScale(50.0f, 1.2f, 120.0f);
            glm::vec4 whitePlate(1.0f, 1.0f, 1.0f, 1.0f);
            glm::vec4 whiteGlow(18.0f, 18.0f, 20.0f, 1.0f);
            float xOffsets[] = { -50.0f, 50.0f };

            for (float offset : xOffsets) {
                glm::vec3 currentPos(offset, floorY + 0.5f, 0.0f);
                glm::mat4 mPlate = glm::translate(glm::mat4(1.0f), currentPos);

                // رسم المنصة (المصمتة)
                shader.setVec4("ourColor", whitePlate);
                shader.setMat4("model", glm::scale(mPlate, platformScale));
                glDrawArrays(GL_TRIANGLES, 0, 36);

                // رسم إطار التوهج (Wireframe)
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                shader.setVec4("ourColor", whiteGlow);
                shader.setMat4("model", glm::scale(mPlate, platformScale * 1.005f));
                glDrawArrays(GL_TRIANGLES, 0, 36);
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            }

            // بقية كود الكريستالات يبقى كما هو تماماً
            struct CryPos { glm::vec3 pos; int type; float lift; };
            CryPos positions[] = {
                {{ 85.0f, floorY, -90.0f }, 2, 14.5f}, {{ 110.0f, floorY, -50.0f }, 0, 14.0f},
                {{ -90.0f, floorY, -130.0f }, 1, 13.5f}, {{ -125.0f, floorY, -70.0f }, 3, 14.0f},
                {{ -110.0f, floorY, 120.0f }, 4, 14.5f}, {{ -130.0f, floorY, 85.0f }, 5, 13.5f},
                {{ 115.0f, floorY, 115.0f }, 0, 14.5f}, {{ 85.0f, floorY, 135.0f }, 3, 13.5f},
                {{ -135.0f, floorY, 10.0f }, 2, 13.5f}
            };
            for (int i = 0; i < 9; i++) {
                glm::vec3 p = positions[i].pos; p.y += positions[i].lift;
                switch (positions[i].type) {
                case 0: drawCrystalTall(shader, p); break;
                case 1: drawCrystalStar(shader, p); break;
                case 2: drawCrystalSpikes(shader, p); break;
                case 3: drawCrystalDecor(shader, p); break;
                case 4: drawCrystal(shader, p); break;
                case 5: drawCrystalGiant(shader, p); break;
                }
            }
        }

        if (f == 1) { // الطابق الأول
            float floorY = f * floorHeight;
            glm::vec3 platformPos(0.0f, floorY + 0.5f, 0.0f);
            glm::vec3 platformScale(100.0f, 1.0f, 100.0f);
            glm::vec4 silverColor(0.85f, 0.88f, 0.9f, 0.85f);
            glm::vec4 silverGlow(10.0f, 10.0f, 12.0f, 1.0f);

            glm::mat4 mPlate = glm::translate(glm::mat4(1.0f), platformPos);
            shader.setMat4("model", glm::scale(mPlate, platformScale));
            shader.setVec4("ourColor", silverColor);
            glDrawArrays(GL_TRIANGLES, 0, 36);

            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            shader.setVec4("ourColor", silverGlow);
            shader.setMat4("model", glm::scale(mPlate, platformScale * 1.01f));
            glDrawArrays(GL_TRIANGLES, 0, 36);
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

            struct CrystalPos { glm::vec3 pos; int type; float lift; };
            CrystalPos positions[] = {
                {{ 85.0f, floorY, -90.0f }, 4, 14.5f}, {{ 110.0f, floorY, -50.0f }, 3, 14.0f},
                {{ -120.0f, floorY, -110.0f }, 0, 14.5f}, {{ -90.0f, floorY, -130.0f }, 5, 13.5f},
                {{ -125.0f, floorY, -70.0f }, 4, 14.0f}, {{ -110.0f, floorY, 120.0f }, 5, 14.5f},
                {{ -130.0f, floorY, 85.0f }, 2, 13.5f}, {{ 115.0f, floorY, 115.0f }, 0, 14.5f},
                {{ 85.0f, floorY, 135.0f }, 1, 13.5f}, {{ -135.0f, floorY, 10.0f }, 3, 13.5f},
                {{ 130.0f, floorY, 30.0f }, 1, 13.5f}, {{ 0.0f, floorY, 120.0f }, 2, 14.0f}
            };
            for (int i = 0; i < 12; i++) {
                glm::vec3 p = positions[i].pos; p.y += positions[i].lift;
                switch (positions[i].type) {
                case 0: drawCrystalGiant(shader, p); break;
                case 1: drawCrystalStar(shader, p); break;
                case 2: drawCrystalSpikes(shader, p); break;
                case 3: drawCrystalDecor(shader, p); break;
                case 4: drawCrystal(shader, p); break;
                case 5: drawCrystalTall(shader, p); break;
                }
            }
        }

        if (f == 2) { // الطابق الثاني
            float floorY = f * floorHeight;
            glm::vec3 platformPos(0.0f, floorY + 0.5f, 0.0f);
            glm::vec3 platformScale(100.0f, 1.0f, 100.0f);
            glm::vec4 platformColor(0.0f, 0.9f, 0.8f, 0.9f);
            glm::vec4 platformGlow(0.0f, 15.0f, 12.0f, 1.0f);

            glm::mat4 mPlate = glm::translate(glm::mat4(1.0f), platformPos);
            shader.setMat4("model", glm::scale(mPlate, platformScale));
            shader.setVec4("ourColor", platformColor);
            glDrawArrays(GL_TRIANGLES, 0, 36);

            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            shader.setVec4("ourColor", platformGlow);
            shader.setMat4("model", glm::scale(mPlate, platformScale * 1.01f));
            glDrawArrays(GL_TRIANGLES, 0, 36);
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

            struct CrystalPos { glm::vec3 pos; int type; float lift; };
            CrystalPos positions[] = {
                {{ 85.0f, floorY, -90.0f }, 4, 14.5f}, {{ 110.0f, floorY, -50.0f }, 3, 14.0f},
                {{ -120.0f, floorY, -110.0f }, 0, 14.5f}, {{ -90.0f, floorY, -130.0f }, 2, 13.5f},
                {{ -125.0f, floorY, -70.0f }, 3, 14.0f}, {{ -110.0f, floorY, 120.0f }, 4, 14.5f},
                {{ -130.0f, floorY, 85.0f }, 1, 13.5f}, {{ 115.0f, floorY, 115.0f }, 2, 14.5f},
                {{ 85.0f, floorY, 135.0f }, 0, 13.5f}, {{ -135.0f, floorY, 10.0f }, 0, 13.5f},
                {{ 130.0f, floorY, 30.0f }, 1, 13.5f}, {{ 0.0f, floorY, 120.0f }, 3, 14.0f}
            };
            for (int i = 0; i < 12; i++) {
                glm::vec3 p = positions[i].pos; p.y += positions[i].lift;
                switch (positions[i].type) {
                case 0: drawCrystalStar(shader, p); break;
                case 1: drawCrystalWide(shader, p); break;
                case 2: drawCrystalTall(shader, p); break;
                case 3: drawCrystalSpikes(shader, p); break;
                case 4: drawCrystalDecor(shader, p); break;
                }
            }
        }

        glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(0, yPos - 1.0f, 0));
        shader.setMat4("model", glm::scale(model, glm::vec3(roomDim, 0.5f, roomDim)));
        shader.setVec4("ourColor", (f == 0) ? leadGrayFloor : floorColor);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glm::mat4 modelCeilOuter = glm::translate(glm::mat4(1.0f), glm::vec3(0, ceilY, 0));
        shader.setMat4("model", glm::scale(modelCeilOuter, glm::vec3(roomDim, 0.2f, roomDim)));
        shader.setVec4("ourColor", ceilColorOuter);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glm::mat4 modelCeilInner = glm::translate(glm::mat4(1.0f), glm::vec3(0, ceilY - 0.15f, 0));
        shader.setMat4("model", glm::scale(modelCeilInner, glm::vec3(roomDim - 1.0f, 0.05f, roomDim - 1.0f)));
        shader.setVec4("ourColor", ceilColorInner);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        for (int i = 0; i < 4; i++) {
            glm::mat4 beamRot = glm::rotate(glm::mat4(1.0f), glm::radians(i * 90.0f), glm::vec3(0, 1, 0));
            glm::mat4 beamModel = glm::translate(beamRot, glm::vec3(0.0f, ceilY, (roomDim / 2) + 1.0f));
            shader.setMat4("model", glm::scale(beamModel, glm::vec3(roomDim + 4.0f, 3.0f, 2.0f)));
            shader.setVec4("ourColor", beamColor);
            glDrawArrays(GL_TRIANGLES, 0, 36);
            shader.setVec4("ourColor", whiteNeon);
            glm::mat4 horizontalNeonM = glm::translate(beamRot, glm::vec3(0.0f, ceilY, (roomDim / 2) + 2.1f));
            shader.setMat4("model", glm::scale(horizontalNeonM, glm::vec3(roomDim + 4.0f, 0.8f, 0.5f)));
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
        for (int i = 0; i < 4; i++) {
            glm::mat4 wallRot = glm::rotate(glm::mat4(1.0f), glm::radians(i * 90.0f), glm::vec3(0, 1, 0));
            for (int x = 0; x < numPanels; x++) {
                float xOff = -(roomDim / 2) + (x * panelWidth) + (panelWidth / 2);

                if (f == 0 && i == 0 && (x == 9 || x == 10)) {
                    float moveX = (x == 9) ? -doorOffset : doorOffset;
                    float doorH = floorHeight * 0.7f;
                    glm::mat4 dM = glm::translate(wallRot, glm::vec3(xOff + moveX, yPos + doorH / 2, roomDim / 2));
                    shader.setMat4("model", glm::scale(dM, glm::vec3(panelWidth - 0.5f, doorH, 0.2f)));
                    shader.setVec4("ourColor", glm::vec4(0.5f, 0.7f, 1.0f, 0.25f));
                    glDrawArrays(GL_TRIANGLES, 0, 36);

                    shader.setVec4("ourColor", whiteNeon);
                    glm::mat4 doorNeonM = glm::translate(wallRot, glm::vec3(xOff + moveX + ((x == 9) ? (panelWidth / 2.0f) : -(panelWidth / 2.0f)), yPos + (doorH / 2), (roomDim / 2) + 0.15f));
                    shader.setMat4("model", glm::scale(doorNeonM, glm::vec3(0.4f, doorH, 0.4f)));
                    glDrawArrays(GL_TRIANGLES, 0, 36);

                    float fixedH = floorHeight - doorH;
                    glm::mat4 fixedM = glm::translate(wallRot, glm::vec3(xOff, yPos + doorH + (fixedH / 2), roomDim / 2));
                    shader.setMat4("model", glm::scale(fixedM, glm::vec3(panelWidth, fixedH, 0.2f)));
                    shader.setVec4("ourColor", glm::vec4(0.5f, 0.7f, 1.0f, 0.15f));
                    glDrawArrays(GL_TRIANGLES, 0, 36);

                    if (x == 9) {
                        glm::mat4 hBarM = glm::translate(wallRot, glm::vec3(xOff + (panelWidth / 2.0f), yPos + doorH, (roomDim / 2) + 0.4f));
                        shader.setMat4("model", glm::scale(hBarM, glm::vec3(panelWidth * 2.0f, 1.8f, 1.2f)));
                        shader.setVec4("ourColor", pillarGray);
                        glDrawArrays(GL_TRIANGLES, 0, 36);
                    }
                }
                else {
                    glm::mat4 gM = glm::translate(wallRot, glm::vec3(xOff, yPos + (floorHeight / 2), roomDim / 2));
                    shader.setMat4("model", glm::scale(gM, glm::vec3(panelWidth - 0.5f, floorHeight, 0.2f)));
                    shader.setVec4("ourColor", glm::vec4(0.5f, 0.7f, 1.0f, 0.15f));
                    glDrawArrays(GL_TRIANGLES, 0, 36);
                }

                float pillarX = xOff + (panelWidth / 2.0f);
                bool isDoorArea = (f == 0 && i == 0 && (x == 9));
                if (x < numPanels - 1 && !isDoorArea) {
                    glm::mat4 pM = glm::translate(wallRot, glm::vec3(pillarX, yPos + (floorHeight / 2), (roomDim / 2) + 0.3f));
                    shader.setMat4("model", glm::scale(pM, glm::vec3(1.0f, floorHeight, 1.0f)));
                    shader.setVec4("ourColor", pillarGray);
                    glDrawArrays(GL_TRIANGLES, 0, 36);

                    shader.setVec4("ourColor", whiteNeon);
                    glm::mat4 neonM = glm::translate(wallRot, glm::vec3(pillarX, yPos + (floorHeight / 2), (roomDim / 2) + 0.85f));
                    shader.setMat4("model", glm::scale(neonM, glm::vec3(0.4f, floorHeight, 0.2f)));
                    glDrawArrays(GL_TRIANGLES, 0, 36);
                }
            }
        }
    }
}

void Building::drawElevator(Shader& shader, unsigned int VAO) {
    glBindVertexArray(VAO);
    glm::vec4 shaftColor(0.12f, 0.12f, 0.14f, 1.0f);
    glm::vec4 cabinColor(0.22f, 0.22f, 0.25f, 1.0f);
    glm::vec4 doorColor(0.5f, 0.7f, 1.0f, 0.4f);
    glm::vec4 lightColor(3.0f, 3.0f, 4.0f, 1.0f);

    float elevX = 125.0f;
    float elevZ = -125.0f;
    float elevSize = 25.0f;
    float shaftSize = 27.0f;
    float h = floorHeight - 3.0f;
    float halfH = h / 2.0f;

    float currentDoorWidth = 12.5f - elevatorDoorOffset;
    if (currentDoorWidth < 0.0f) currentDoorWidth = 0.0f;
    float doorXOffset = elevatorDoorOffset / 2.0f;
    for (int f = 0; f < 4; f++) {
        float floorY = f * floorHeight;

        glm::mat4 sBack = glm::translate(glm::mat4(1.0f), glm::vec3(elevX, floorY + floorHeight / 2, elevZ - shaftSize / 2));
        shader.setMat4("model", glm::scale(sBack, glm::vec3(shaftSize, floorHeight, 0.2f)));
        shader.setVec4("ourColor", shaftColor);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glm::mat4 sLeft = glm::translate(glm::mat4(1.0f), glm::vec3(elevX - shaftSize / 2, floorY + floorHeight / 2, elevZ));
        shader.setMat4("model", glm::scale(sLeft, glm::vec3(1.5f, floorHeight, shaftSize)));
        glDrawArrays(GL_TRIANGLES, 0, 36);

        glm::mat4 sRight = glm::translate(glm::mat4(1.0f), glm::vec3(elevX + shaftSize / 2, floorY + floorHeight / 2, elevZ));
        shader.setMat4("model", glm::scale(sRight, glm::vec3(1.5f, floorHeight, shaftSize)));
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glm::mat4 sDoorL = glm::translate(glm::mat4(1.0f), glm::vec3(elevX - 6.25f - doorXOffset, floorY + floorHeight / 2, elevZ + shaftSize / 2 - 0.2f));
        shader.setMat4("model", glm::scale(sDoorL, glm::vec3(currentDoorWidth, floorHeight, 0.4f)));
        shader.setVec4("ourColor", doorColor);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        glm::mat4 sDoorR = glm::translate(glm::mat4(1.0f), glm::vec3(elevX + 6.25f + doorXOffset, floorY + floorHeight / 2, elevZ + shaftSize / 2 - 0.2f));
        shader.setMat4("model", glm::scale(sDoorR, glm::vec3(currentDoorWidth, floorHeight, 0.4f)));
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }
    glm::mat4 elevFloor = glm::translate(glm::mat4(1.0f), glm::vec3(elevX, elevatorY, elevZ));
    shader.setMat4("model", glm::scale(elevFloor, glm::vec3(elevSize, 0.5f, elevSize)));
    shader.setVec4("ourColor", cabinColor);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    glm::mat4 elevCeil = glm::translate(glm::mat4(1.0f), glm::vec3(elevX, elevatorY + h, elevZ));
    shader.setMat4("model", glm::scale(elevCeil, glm::vec3(elevSize, 0.5f, elevSize)));
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glm::mat4 cLeft = glm::translate(glm::mat4(1.0f), glm::vec3(elevX - elevSize / 2, elevatorY + halfH, elevZ));
    shader.setMat4("model", glm::scale(cLeft, glm::vec3(1.0f, h, elevSize)));
    glDrawArrays(GL_TRIANGLES, 0, 36);

    glm::mat4 cRight = glm::translate(glm::mat4(1.0f), glm::vec3(elevX + elevSize / 2, elevatorY + halfH, elevZ));
    shader.setMat4("model", glm::scale(cRight, glm::vec3(1.0f, h, elevSize)));
    glDrawArrays(GL_TRIANGLES, 0, 36);

    glm::mat4 backWall = glm::translate(glm::mat4(1.0f), glm::vec3(elevX, elevatorY + halfH, elevZ - elevSize / 2));
    shader.setMat4("model", glm::scale(backWall, glm::vec3(elevSize, h, 0.5f)));
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glm::mat4 doorL = glm::translate(glm::mat4(1.0f), glm::vec3(elevX - 6.25f - doorXOffset, elevatorY + halfH, elevZ + elevSize / 2 - 0.5f));
    shader.setMat4("model", glm::scale(doorL, glm::vec3(currentDoorWidth, h, 0.3f)));
    shader.setVec4("ourColor", doorColor);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    glm::mat4 doorR = glm::translate(glm::mat4(1.0f), glm::vec3(elevX + 6.25f + doorXOffset, elevatorY + halfH, elevZ + elevSize / 2 - 0.5f));
    shader.setMat4("model", glm::scale(doorR, glm::vec3(currentDoorWidth, h, 0.3f)));
    glDrawArrays(GL_TRIANGLES, 0, 36);
    shader.setVec4("ourColor", lightColor);
    glm::mat4 lightM = glm::translate(glm::mat4(1.0f), glm::vec3(elevX, elevatorY + h - 0.7f, elevZ));
    shader.setMat4("model", glm::scale(lightM, glm::vec3(elevSize - 2.0f, 0.1f, elevSize - 2.0f)));
    glDrawArrays(GL_TRIANGLES, 0, 36);
}

void Building::drawRockArch(Shader& shader, glm::vec3 position) {
    float factor = 2.2f;
    glm::vec4 cryCol(0.8f, 0.9f, 1.0f, 0.15f);
    glm::vec4 neoCol(2.8f, 3.2f, 4.0f, 1.0f);
    struct Seg { glm::vec3 o, r, s; };
    Seg segs[] = {
        {{-8.5f,9.5f,0}, {0,0,0}, {4,7,1.5f}}, {{-8,10.5f,0}, {0,0,15}, {3.4f,6.5f,1.4f}},
        {{-7,16,0}, {0,0,30}, {3.2f,6,1.3f}}, {{-5,20.5f,0}, {0,0,45}, {3,6,1.2f}},
        {{-2.5f,24,0}, {0,0,65}, {2.8f,6,1.1f}}, {{0,26,0}, {0,0,90}, {2.8f,7,1.1f}},
        {{2.5f,24,0}, {0,0,-65}, {2.8f,6,1.1f}}, {{5,20.5f,0}, {0,0,-45}, {3,6,1.2f}},
        {{7,16,0}, {0,0,-30}, {3.2f,6,1.3f}}, {{8,10.5f,0}, {0,0,-15}, {3.4f,6.5f,1.4f}},
        {{8.5f,9.5f,0}, {0,0,0}, {4,7,1.5f}}
    };
    for (int i = 0; i < 11; i++) {
        glm::mat4 m = glm::translate(glm::mat4(1.0f), position + segs[i].o * factor);
        m = glm::rotate(m, glm::radians(segs[i].r.z), glm::vec3(0, 0, 1));
        shader.setVec4("ourColor", cryCol);
        shader.setMat4("model", glm::scale(m, segs[i].s * factor));
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        shader.setVec4("ourColor", neoCol);
        shader.setMat4("model", glm::scale(m, segs[i].s * factor * 1.005f));
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
}

void Building::drawSignboard(Shader& shader) {
    float sY = (3 * floorHeight) + (floorHeight / 2.0f);
    glm::mat4 bM = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, sY, 150.0f));
    shader.setMat4("model", glm::scale(bM, glm::vec3(180.0f, 28.0f, 1.5f)));
    shader.setVec4("ourColor", glm::vec4(1.0f, 1.0f, 1.0f, 0.4f));
    glDrawArrays(GL_TRIANGLES, 0, 36);

    std::string txt = "CAR SHOWROOM";
    for (int i = 0; i < (int)txt.length(); i++) {
        if (txt[i] != ' ') drawLetter(shader, txt[i], glm::vec3(-80.0f + (i * 12.0f), sY, 153.0f), 2.5f);
    }
}

void Building::drawLetter(Shader& shader, char letter, glm::vec3 pos, float scale) {
    float th = 0.5f * scale;
    glm::vec4 cry(0.8f, 0.9f, 1.0f, 0.15f);
    glm::vec4 neo(2.8f, 3.2f, 4.0f, 1.0f);
    auto p = [&](float ox, float oy, float w, float h) {
        glm::mat4 m = glm::translate(glm::mat4(1.0f), pos + glm::vec3(ox * scale, oy * scale, 0.0f));
        shader.setVec4("ourColor", cry);
        shader.setMat4("model", glm::scale(m, glm::vec3(w * scale, h * scale, th)));
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        shader.setVec4("ourColor", neo);
        shader.setMat4("model", glm::scale(m, glm::vec3(w * scale, h * scale, th) * 1.01f));
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    };

    switch (toupper(letter)) {
    case 'C': p(-1, 0, 0.5f, 4); p(0, 1.75f, 2.5f, 0.5f); p(0, -1.75f, 2.5f, 0.5f); break;
    case 'A': p(-1, 0, 0.5f, 4); p(1, 0, 0.5f, 4); p(0, 1.75f, 2.5f, 0.5f); p(0, 0, 2, 0.5f); break;
    case 'R': p(-1, 0, 0.5f, 4); p(0, 1.75f, 2.5f, 0.5f); p(1, 1, 0.5f, 2); p(0, 0.25f, 2, 0.5f); p(0.8f, -1, 0.6f, 2.5f); break;
    case 'S': p(0, 1.75f, 2.5f, 0.5f); p(-1, 1, 0.5f, 2); p(0, 0, 2.5f, 0.5f); p(1, -1, 0.5f, 2); p(0, -1.75f, 2.5f, 0.5f); break;
    case 'H': p(-1, 0, 0.5f, 4); p(1, 0, 0.5f, 4); p(0, 0, 2, 0.5f); break;
    case 'O': p(-1, 0, 0.5f, 4); p(1, 0, 0.5f, 4); p(0, 1.75f, 2.5f, 0.5f); p(0, -1.75f, 2.5f, 0.5f); break;
    case 'W': p(-1.2f, 0, 0.5f, 4); p(1.2f, 0, 0.5f, 4); p(0, -0.5f, 0.4f, 3); p(-0.6f, -1.75f, 1.2f, 0.5f); p(0.6f, -1.75f, 1.2f, 0.5f); break;
    case 'M': p(-1.2f, 0, 0.5f, 4); p(1.2f, 0, 0.5f, 4); p(0, 0.5f, 0.4f, 3); p(-0.6f, 1.75f, 1.2f, 0.5f); p(0.6f, 1.75f, 1.2f, 0.5f); break;
    case 'U': p(-1, 0, 0.5f, 4); p(1, 0, 0.5f, 4); p(0, -1.75f, 2.5f, 0.5f); break;
    }
}void Building::drawCrystalDecor(Shader& shader, glm::vec3 position) {
    float factor = 6.0f;
    glm::vec4 baseCol(0.3f, 0.9f, 1.0f, 0.5f);
    glm::vec4 glowCol(0.0f, 8.0f, 10.0f, 1.0f);
    struct Seg { glm::vec3 o, r, s; };
    Seg segs[] = {
        {{0,0,0},       {0,0,0},    {1.2f,4.5f,1.2f}},
        {{0.6f,0.8f,0}, {0,0,15},   {0.8f,3.5f,0.8f}},
        {{-0.7f,0.6f,0},{0,0,-20},  {0.9f,4.0f,0.9f}},
        {{0.3f,1.8f,0}, {0,0,35},   {0.6f,2.5f,0.6f}},
        {{-0.4f,2.2f,0},{0,0,-30},  {0.5f,2.2f,0.5f}},
    };
    for (int i = 0; i < 5; i++) {
        glm::mat4 m = glm::translate(glm::mat4(1.0f), position + segs[i].o * factor);
        m = glm::rotate(m, glm::radians(segs[i].r.z), glm::vec3(0, 0, 1));
        shader.setVec4("ourColor", baseCol);
        shader.setMat4("model", glm::scale(m, segs[i].s * factor));
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        shader.setVec4("ourColor", glowCol);
        shader.setMat4("model", glm::scale(m, segs[i].s * factor * 1.07f));
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
}

// --- دالة الكريستال Wide ---
void Building::drawCrystalWide(Shader& shader, glm::vec3 position) {
    float f = 8.0f;
    glm::vec4 col(1.0f, 0.6f, 0.9f, 0.4f);
    glm::vec4 glow(6.0f, 1.5f, 4.0f, 1.0f);
    struct Seg { glm::vec3 o, r, s; };
    Seg segs[] = {
        {{0,0,0},{0,0,0},{2.2f,2.2f,1.2f}},
        {{1.2f,0.4f,0},{0,0,35},{1.5f,1.5f,1.0f}},
        {{-1.0f,0.6f,0},{0,0,-30},{1.3f,1.3f,1.0f}},
    };
    for (int i = 0; i < 3; i++) {
        glm::mat4 m = glm::translate(glm::mat4(1.0f), position + segs[i].o * f);
        m = glm::rotate(m, glm::radians(segs[i].r.z), glm::vec3(0, 0, 1));
        shader.setVec4("ourColor", col);
        shader.setMat4("model", glm::scale(m, segs[i].s * f));
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        shader.setVec4("ourColor", glow);
        shader.setMat4("model", glm::scale(m, segs[i].s * f * 1.06f));
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
}

// --- دالة الكريستال Tall ---
void Building::drawCrystalTall(Shader& shader, glm::vec3 position) {
    float f = 4.5f;
    glm::vec4 col(0.4f, 1.0f, 0.7f, 0.6f);
    glm::vec4 glow(0.2f, 4.0f, 1.8f, 1.0f);
    struct Seg { glm::vec3 o, r, s; };
    Seg segs[] = {
        {{0,0,0},{0,0,0},{0.8f,5.5f,0.8f}},
        {{0.4f,1.2f,0},{0,0,20},{0.6f,3.5f,0.6f}},
        {{-0.5f,2.2f,0},{0,0,-25},{0.5f,3.0f,0.5f}},
    };
    for (int i = 0; i < 3; i++) {
        glm::mat4 m = glm::translate(glm::mat4(1.0f), position + segs[i].o * f);
        m = glm::rotate(m, glm::radians(segs[i].r.z), glm::vec3(0, 0, 1));
        shader.setVec4("ourColor", col);
        shader.setMat4("model", glm::scale(m, segs[i].s * f));
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        shader.setVec4("ourColor", glow);
        shader.setMat4("model", glm::scale(m, segs[i].s * f * 1.06f));
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
}

// --- دالة الكريستال Cluster ---
void Building::drawCrystalCluster(Shader& shader, glm::vec3 position) {
    float f = 6.0f;
    glm::vec4 baseCol(0.0f, 1.0f, 0.85f, 0.7f);
    glm::vec4 glowCol(0.0f, 8.0f, 7.0f, 1.0f);
    struct Seg { glm::vec3 o, r, s; };
    Seg segs[] = {
        {{0, 0, 0},           {0, 0, 0},    {2.2f, 1.5f, 2.2f}},
        {{0.6f, 2.0f, 0.6f},  {15, 0, 15},  {0.7f, 6.0f, 0.7f}},
        {{-0.7f, 1.5f, -0.6f},{-20, 0, -10},{0.8f, 4.8f, 0.8f}},
        {{0.8f, 1.2f, -0.8f}, {12, 0, -25}, {0.6f, 4.0f, 0.6f}},
        {{-0.8f, 1.0f, 0.8f}, {-10, 0, 20}, {0.5f, 3.5f, 0.5f}},
        {{0.0f, 3.0f, 0.0f},  {0, 0, 0},    {0.5f, 7.5f, 0.5f}}
    };
    for (int i = 0; i < 6; i++) {
        glm::mat4 m = glm::translate(glm::mat4(1.0f), position + segs[i].o * f);
        m = glm::rotate(m, glm::radians(segs[i].r.x), glm::vec3(1, 0, 0));
        m = glm::rotate(m, glm::radians(segs[i].r.z), glm::vec3(0, 0, 1));
        shader.setVec4("ourColor", baseCol);
        shader.setMat4("model", glm::scale(m, segs[i].s * f));
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        shader.setVec4("ourColor", glowCol);
        shader.setMat4("model", glm::scale(m, segs[i].s * f * 1.06f));
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
}

// --- دالة الكريستال Spikes ---
void Building::drawCrystalSpikes(Shader& shader, glm::vec3 position) {
    float f = 4.5f;
    glm::vec4 col(0.9f, 0.7f, 1.0f, 0.7f);
    glm::vec4 glow(3.5f, 2.0f, 4.5f, 1.0f);
    struct Seg { glm::vec3 o, r, s; };
    Seg segs[] = {
        {{0, 0, 0},         {0, 0, 0},    {2.2f, 1.2f, 2.2f}},
        {{0.5f, 1.5f, 0.5f}, {15, 0, 15}, {0.7f, 4.5f, 0.7f}},
        {{-0.5f, 1.2f, -0.5f},{-20, 0, -10},{0.6f, 3.8f, 0.6f}},
        {{0.6f, 1.0f, -0.6f}, {10, 0, -25}, {0.5f, 3.2f, 0.5f}},
        {{-0.2f, 2.0f, 0.1f}, {0, 0, 0},    {0.4f, 5.2f, 0.4f}}
    };
    for (int i = 0; i < 5; i++) {
        glm::mat4 m = glm::translate(glm::mat4(1.0f), position + segs[i].o * f);
        m = glm::rotate(m, glm::radians(segs[i].r.x), glm::vec3(1, 0, 0));
        m = glm::rotate(m, glm::radians(segs[i].r.z), glm::vec3(0, 0, 1));
        shader.setVec4("ourColor", col);
        shader.setMat4("model", glm::scale(m, segs[i].s * f));
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        shader.setVec4("ourColor", glow);
        shader.setMat4("model", glm::scale(m, segs[i].s * f * 1.08f));
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
}

// --- دالة الكريستال Star ---
void Building::drawCrystalStar(Shader& shader, glm::vec3 position) {
    float f = 6.5f;
    glm::vec4 col(0.9f, 0.95f, 1.0f, 0.4f);
    glm::vec4 glow(2.5f, 2.5f, 2.5f, 1.0f);
    struct Seg { glm::vec3 o, r, s; };
    Seg segs[] = {
        {{0, 1.0f, 0},    {0, 0, 0},    {1.0f, 6.0f, 1.0f}},
        {{0, 1.0f, 0},    {45, 45, 0},  {0.6f, 4.0f, 0.6f}},
        {{0, 1.0f, 0},    {-45, -45, 0},{0.6f, 4.0f, 0.6f}},
        {{0, 1.0f, 0},    {45, -45, 0}, {0.6f, 4.0f, 0.6f}},
        {{0, 1.0f, 0},    {-45, 45, 0}, {0.6f, 4.0f, 0.6f}}
    };
    for (int i = 0; i < 5; i++) {
        glm::mat4 m = glm::translate(glm::mat4(1.0f), position + segs[i].o * f);
        m = glm::rotate(m, glm::radians(segs[i].r.x), glm::vec3(1, 0, 0));
        m = glm::rotate(m, glm::radians(segs[i].r.y), glm::vec3(0, 1, 0));
        shader.setVec4("ourColor", col);
        shader.setMat4("model", glm::scale(m, segs[i].s * f));
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        shader.setVec4("ourColor", glow);
        shader.setMat4("model", glm::scale(m, segs[i].s * f * 1.05f));
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
}

// --- دالة الكريستال Giant ---
void Building::drawCrystalGiant(Shader& shader, glm::vec3 position) {
    float f = 4.0f;
    glm::vec4 yellowColor(1.0f, 1.0f, 0.4f, 0.8f);
    glm::vec4 glowCol(10.0f, 10.0f, 0.0f, 1.0f);
    struct Seg { glm::vec3 o, r, s; };
    Seg segs[] = {
        {{0,0,0},         {0,0,0},    {3.0f, 5.5f, 3.0f}},
        {{1.5f, 1.0f, 0},  {0,0,25},   {1.2f, 3.0f, 1.2f}},
        {{-1.5f, 0.8f,0},  {0,0,-20},  {1.3f, 3.5f, 1.3f}},
        {{0, 1.2f, 1.5f},  {15,0,0},   {1.0f, 2.5f, 1.0f}},
    };
    for (int i = 0; i < 4; i++) {
        glm::mat4 m = glm::translate(glm::mat4(1.0f), position + segs[i].o * f);
        m = glm::rotate(m, glm::radians(segs[i].r.x), glm::vec3(1, 0, 0));
        m = glm::rotate(m, glm::radians(segs[i].r.z), glm::vec3(0, 0, 1));
        shader.setVec4("ourColor", yellowColor);
        shader.setMat4("model", glm::scale(m, segs[i].s * f));
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        shader.setVec4("ourColor", glowCol);
        shader.setMat4("model", glm::scale(m, segs[i].s * f * 1.05f));
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
}

// --- دالة الكريستال العادية ---
void Building::drawCrystal(Shader& shader, glm::vec3 position) {
    float f = 4.5f;
    glm::vec4 lavenderColor(0.7f, 0.5f, 1.0f, 0.75f);
    glm::vec4 glowCol(4.0f, 2.0f, 8.0f, 1.0f);
    struct Seg { glm::vec3 o, r, s; };
    Seg segs[] = {
        {{0,0,0},           {0,0,0},     {2.0f, 4.0f, 2.0f}},
        {{1.0f, 0.5f, 1.0f}, {20,0,20},   {0.8f, 2.5f, 0.8f}},
        {{-1.0f, 0.5f, 1.0f}, {20,0,-20}, {0.8f, 2.5f, 0.8f}},
        {{1.0f, 0.5f, -1.0f}, {-20,0,20}, {0.8f, 2.5f, 0.8f}},
        {{-1.0f, 0.5f, -1.0f},{-20,0,-20}, {0.8f, 2.5f, 0.8f}},
    };
    for (int i = 0; i < 5; i++) {
        glm::mat4 m = glm::translate(glm::mat4(1.0f), position + segs[i].o * f);
        m = glm::rotate(m, glm::radians(segs[i].r.x), glm::vec3(1, 0, 0));
        m = glm::rotate(m, glm::radians(segs[i].r.z), glm::vec3(0, 0, 1));
        shader.setVec4("ourColor", lavenderColor);
        shader.setMat4("model", glm::scale(m, segs[i].s * f));
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        shader.setVec4("ourColor", glowCol);
        shader.setMat4("model", glm::scale(m, segs[i].s * f * 1.06f));
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
}