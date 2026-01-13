#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Shader.h" 
#include "Camera.h"  
#include <iostream>
#include <vector>
#include <string>

const unsigned int SCR_WIDTH = 1200;
const unsigned int SCR_HEIGHT = 900;
Camera camera(glm::vec3(0.0f, 30.0f, 250.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;
float deltaTime = 0.0f;
float lastFrame = 0.0f;
float doorOffset = 0.0f;

// متغيرات المصعد
float elevatorY = 0.0f;
float targetElevatorY = 0.0f;
float elevatorDoorOffset = 0.0f;
const float floorHeight = 60.0f;

void processInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void drawScene(Shader& shader, unsigned int VAO);
void drawRockArch(Shader& shader, glm::vec3 position);
void drawLetter(Shader& shader, char letter, glm::vec3 pos, float scale);
void drawBuildings(Shader& shader, unsigned int VAO);
void drawWorldCube(Shader& shader, unsigned int VAO);

int main() {
    if (!glfwInit()) return -1;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Mega Luxury Showroom 2026", NULL, NULL);
    if (!window) { glfwTerminate(); return -1; }

    glfwMakeContextCurrent(window);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) return -1;

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    Shader ourShader("shader.vs", "shader.fs");

    float vertices[] = {
        -0.5f,-0.5f,-0.5f,  0.5f,-0.5f,-0.5f,  0.5f, 0.5f,-0.5f,  0.5f, 0.5f,-0.5f, -0.5f, 0.5f,-0.5f, -0.5f,-0.5f,-0.5f,
        -0.5f,-0.5f, 0.5f,  0.5f,-0.5f, 0.5f,  0.5f, 0.5f, 0.5f,  0.5f, 0.5f, 0.5f, -0.5f, 0.5f, 0.5f, -0.5f,-0.5f, 0.5f,
        -0.5f, 0.5f, 0.5f, -0.5f, 0.5f,-0.5f, -0.5f,-0.5f,-0.5f, -0.5f,-0.5f,-0.5f, -0.5f,-0.5f, 0.5f, -0.5f, 0.5f, 0.5f,
         0.5f, 0.5f, 0.5f,  0.5f, 0.5f,-0.5f,  0.5f,-0.5f,-0.5f,  0.5f,-0.5f,-0.5f,  0.5f,-0.5f, 0.5f,  0.5f, 0.5f, 0.5f,
        -0.5f,-0.5f,-0.5f,  0.5f,-0.5f,-0.5f,  0.5f,-0.5f, 0.5f,  0.5f,-0.5f, 0.5f, -0.5f,-0.5f, 0.5f, -0.5f,-0.5f,-0.5f,
        -0.5f, 0.5f,-0.5f,  0.5f, 0.5f,-0.5f,  0.5f, 0.5f, 0.5f,  0.5f, 0.5f, 0.5f, -0.5f, 0.5f, 0.5f, -0.5f, 0.5f,-0.5f
    };

    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    while (!glfwWindowShouldClose(window)) {
        float currentFrame = (float)glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        processInput(window);

        if (elevatorY < targetElevatorY) elevatorY += 25.0f * deltaTime;
        if (elevatorY > targetElevatorY) elevatorY -= 25.0f * deltaTime;
        if (abs(elevatorY - targetElevatorY) < 0.2f) elevatorY = targetElevatorY;

        float elevX_pos = (300.0f / 2.0f) - 25.0f;
        float elevZ_pos = -(300.0f / 2.0f) + 25.0f;
        float distToElev = glm::distance(glm::vec2(camera.Position.x, camera.Position.z), glm::vec2(elevX_pos, elevZ_pos));
        if (distToElev < 22.0f) {
            camera.Position.y = elevatorY + 30.0f;
        }

        glClearColor(0.005f, 0.005f, 0.01f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        ourShader.use();
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 3000.0f);
        ourShader.setMat4("projection", projection);
        ourShader.setMat4("view", camera.GetViewMatrix());

        glBindVertexArray(VAO);
        drawWorldCube(ourShader, VAO);
        drawBuildings(ourShader, VAO);
        drawScene(ourShader, VAO);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glfwTerminate();
    return 0;
}

void drawBuildings(Shader& shader, unsigned int VAO) {
    struct BuildingData { glm::vec3 pos; glm::vec3 size; glm::vec4 color; };
    float unifiedHeight = 650.0f;
    BuildingData buildings[] = {
      {{-150, 0, -475}, {180, unifiedHeight, 150}, {0.04f, 0.04f, 0.07f, 1.0f}},
      {{ 150, 0, -475}, {180, unifiedHeight, 150}, {0.05f, 0.05f, 0.08f, 1.0f}},
      {{ 475, 0, -100}, {150, unifiedHeight, 180}, {0.03f, 0.03f, 0.06f, 1.0f}},
      {{ 460, 0,  220}, {120, unifiedHeight, 150}, {0.05f, 0.05f, 0.09f, 1.0f}},
      {{-475, 0, -100}, {150, unifiedHeight, 180}, {0.04f, 0.04f, 0.08f, 1.0f}},
      {{-460, 0,  220}, {120, unifiedHeight, 150}, {0.06f, 0.06f, 0.10f, 1.0f}},
      {{-280, 0,  475}, {150, unifiedHeight, 150}, {0.04f, 0.04f, 0.07f, 1.0f}},
      {{ 280, 0,  475}, {150, unifiedHeight, 150}, {0.05f, 0.05f, 0.08f, 1.0f}}
    };
    glm::vec4 buildingNeon(0.0f, 0.6f, 1.0f, 1.0f);
    glm::vec4 grassColor(0.1f, 0.4f, 0.12f, 1.0f);
    for (int i = 0; i < 8; i++) {
        glm::mat4 grassModel = glm::translate(glm::mat4(1.0f), glm::vec3(buildings[i].pos.x, -1.8f, buildings[i].pos.z));
        shader.setMat4("model", glm::scale(grassModel, glm::vec3(buildings[i].size.x + 100.0f, 0.5f, buildings[i].size.z + 100.0f)));
        shader.setVec4("ourColor", grassColor);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(buildings[i].pos.x, buildings[i].size.y / 2.0f - 2.0f, buildings[i].pos.z));
        shader.setMat4("model", glm::scale(model, buildings[i].size));
        shader.setVec4("ourColor", buildings[i].color);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        shader.setVec4("ourColor", buildingNeon);
        shader.setMat4("model", glm::scale(model, buildings[i].size * 1.005f));
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
}

void drawScene(Shader& shader, unsigned int VAO) {
    const float roomDim = 300.0f;
    const int numPanels = 20;
    const float panelWidth = roomDim / numPanels;

    glm::vec4 floorColor(0.08f, 0.08f, 0.1f, 0.3f);
    glm::vec4 ceilColor(0.01f, 0.01f, 0.02f, 1.0f);
    glm::vec4 groundCeilColor(0.4f, 0.7f, 1.0f, 0.45f);
    glm::vec4 whiteNeon(2.5f, 2.5f, 3.5f, 1.0f);
    glm::vec4 cyanNeon(0.0f, 2.5f, 3.0f, 1.0f);
    glm::vec4 sidewalkColor(0.12f, 0.12f, 0.14f, 1.0f);
    glm::vec4 grassColor(0.1f, 0.45f, 0.1f, 1.0f);
    glm::vec4 beamColor(0.3f, 0.3f, 0.32f, 1.0f);
    glm::vec4 roadColor(0.05f, 0.05f, 0.06f, 1.0f);
    glm::vec4 roadLineColor(1.0f, 0.8f, 0.0f, 1.0f);
    glm::vec4 plazaColor(0.7f, 0.7f, 0.72f, 1.0f);

    float sw = 80.0f;
    float rw = 120.0f;
   glm::mat4 plazaModel = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -0.5f, 190.0f)); 
shader.setMat4("model", glm::scale(plazaModel, glm::vec3(300.0f, 3.0f, 80.0f))); 
shader.setVec4("ourColor", plazaColor);
glDrawArrays(GL_TRIANGLES, 0, 36);;

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
        for (int s = 0; s < 10; s++) {
            if (side == 0 && (s == 4 || s == 5)) continue;
            float segmentLen = (roomDim + (sw * 2)) / 10.0f;
            float xS = -((roomDim + (sw * 2)) / 2.0f) + (s * segmentLen) + (segmentLen / 2.0f);
            glm::mat4 model = glm::translate(sideRot, glm::vec3(xS, -0.0f, (roomDim / 2.0f) + (sw / 2.0f)));
            shader.setMat4("model", glm::scale(model, glm::vec3(segmentLen, 2.0f, sw))); // زيادة سماكة الرصيف
            shader.setVec4("ourColor", sidewalkColor);
            glDrawArrays(GL_TRIANGLES, 0, 36);
            glm::mat4 grassModel = glm::translate(sideRot, glm::vec3(xS, 1.5f, (roomDim / 2.0f) + (sw / 2.0f)));
            shader.setMat4("model", glm::scale(grassModel, glm::vec3(segmentLen, 3.5f, sw))); // زيادة السماكة من 0.3 إلى 3.5
            shader.setVec4("ourColor", grassColor);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
    }
    glm::vec4 asphaltColor(0.06f, 0.06f, 0.07f, 1.0f);
    glm::vec4 linePColor(1.0f, 1.0f, 1.0f, 0.6f); 
    glm::vec4 stopperColor(0.25f, 0.25f, 0.28f, 1.0f);
    glm::mat4 parkBase = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -2.5f, 525.0f));
    shader.setMat4("model", glm::scale(parkBase, glm::vec3(300.0f, 2.0f, 350.0f)));
    shader.setVec4("ourColor", asphaltColor);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    for (int p = 0; p < 3; p++) {
        float px = -100.0f + (p * 100.0f);
        glm::mat4 lineM = glm::translate(glm::mat4(1.0f), glm::vec3(px, -1.6f, 525.0f));
        shader.setMat4("model", glm::scale(lineM, glm::vec3(8.0f, 0.8f, 320.0f))); 
        shader.setVec4("ourColor", linePColor);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glm::mat4 stopM = glm::translate(glm::mat4(1.0f), glm::vec3(px, -0.5f, 680.0f));
        shader.setMat4("model", glm::scale(stopM, glm::vec3(90.0f, 5.0f, 15.0f))); 
        shader.setVec4("ourColor", stopperColor);
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }
    for (int l = 0; l < 2; l++) {
        float lx = (l == 0) ? -145.0f : 145.0f;
        float lZ = 600.0f;
        glm::mat4 poleM = glm::translate(glm::mat4(1.0f), glm::vec3(lx, 45.0f, lZ));
        shader.setMat4("model", glm::scale(poleM, glm::vec3(10.0f, 100.0f, 10.0f))); 
        shader.setVec4("ourColor", beamColor);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glm::mat4 lightM = glm::translate(glm::mat4(1.0f), glm::vec3(lx, 90.0f, lZ - 20.0f));
        shader.setMat4("model", glm::scale(lightM, glm::vec3(25.0f, 8.0f, 45.0f)));
        shader.setVec4("ourColor", whiteNeon);
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }
    drawRockArch(shader, glm::vec3(0.0f, -14.0f, 144.5f));
    for (int f = 0; f < 4; f++) {
        float yPos = f * floorHeight;
        float ceilY = yPos + floorHeight - 1.0f;
        glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(0, yPos - 1.0f, 0));
        shader.setMat4("model", glm::scale(model, glm::vec3(roomDim, 0.5f, roomDim)));
        shader.setVec4("ourColor", floorColor);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        model = glm::translate(glm::mat4(1.0f), glm::vec3(0, ceilY, 0));
        shader.setMat4("model", glm::scale(model, glm::vec3(roomDim, 0.2f, roomDim)));
        shader.setVec4("ourColor", (f == 0) ? groundCeilColor : ceilColor);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        for (int i = 0; i < 4; i++) {
            glm::mat4 beamRot = glm::rotate(glm::mat4(1.0f), glm::radians(i * 90.0f), glm::vec3(0, 1, 0));
            glm::mat4 beamModel = glm::translate(beamRot, glm::vec3(0.0f, ceilY, (roomDim / 2) + 1.0f));
            shader.setMat4("model", glm::scale(beamModel, glm::vec3(roomDim + 4.0f, 3.0f, 2.0f)));
            shader.setVec4("ourColor", beamColor);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        if (f == 0) {
            for (int r = 0; r < 6; r++) {
                float zPos = -(roomDim / 2.5f) + (r * (roomDim / 6.0f));
                glm::mat4 rectM = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, ceilY - 0.4f, zPos));
                shader.setVec4("ourColor", cyanNeon);
                float w = 20.0f, d = 12.5f, thick = 0.6f;
                glm::vec3 eP[] = { {0,0,d}, {0,0,-d}, {w,0,0}, {-w,0,0} };
                glm::vec3 eS[] = { {40,0.2f,thick}, {40,0.2f,thick}, {thick,0.2f,25.0f}, {thick,0.2f,25.0f} };
                for (int e = 0; e < 4; e++) {
                    shader.setMat4("model", glm::scale(glm::translate(rectM, eP[e]), eS[e]));
                    glDrawArrays(GL_TRIANGLES, 0, 36);
                }
            }
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
                    float edgeOffset = (x == 9) ? (panelWidth / 2.0f) : -(panelWidth / 2.0f);
                    glm::mat4 doorNeonM = glm::translate(wallRot, glm::vec3(xOff + moveX + edgeOffset, yPos + (doorH / 2), (roomDim / 2) + 0.15f));
                    shader.setMat4("model", glm::scale(doorNeonM, glm::vec3(0.4f, doorH, 0.4f)));
                    glDrawArrays(GL_TRIANGLES, 0, 36);
                    if (x == 10) {
                        glm::mat4 sideNeon = glm::translate(wallRot, glm::vec3(xOff + (panelWidth / 2.0f), yPos + (floorHeight / 2.0f), (roomDim / 2) + 0.25f));
                        shader.setMat4("model", glm::scale(sideNeon, glm::vec3(0.8f, floorHeight, 0.8f)));
                        shader.setVec4("ourColor", whiteNeon);
                        glDrawArrays(GL_TRIANGLES, 0, 36);
                        glm::mat4 topNeon = glm::translate(wallRot, glm::vec3(xOff - (panelWidth / 2.0f), yPos + doorH, (roomDim / 2) + 0.25f));
                        shader.setMat4("model", glm::scale(topNeon, glm::vec3(panelWidth * 2.0f, 0.8f, 0.8f)));
                        glDrawArrays(GL_TRIANGLES, 0, 36);
                    }
                    float fixH = floorHeight - doorH;
                    glm::mat4 fixM = glm::translate(wallRot, glm::vec3(xOff, yPos + doorH + (fixH / 2), roomDim / 2));
                    shader.setMat4("model", glm::scale(fixM, glm::vec3(panelWidth, fixH, 0.2f)));
                    shader.setVec4("ourColor", glm::vec4(0.5f, 0.7f, 1.0f, 0.15f));
                    glDrawArrays(GL_TRIANGLES, 0, 36);
                    shader.setVec4("ourColor", whiteNeon);
                    glm::mat4 topNeon2 = glm::translate(wallRot, glm::vec3(xOff, yPos + doorH, (roomDim / 2) + 0.1f));
                    shader.setMat4("model", glm::scale(topNeon2, glm::vec3(panelWidth, 0.5f, 0.5f)));
                    glDrawArrays(GL_TRIANGLES, 0, 36);
                }
                else {
                    glm::mat4 gM = glm::translate(wallRot, glm::vec3(xOff, yPos + (floorHeight / 2), roomDim / 2));
                    shader.setMat4("model", glm::scale(gM, glm::vec3(panelWidth - 0.5f, floorHeight, 0.2f)));
                    shader.setVec4("ourColor", glm::vec4(0.5f, 0.7f, 1.0f, 0.15f));
                    glDrawArrays(GL_TRIANGLES, 0, 36);
                    glm::mat4 fM = glm::translate(wallRot, glm::vec3(xOff + (panelWidth / 2), yPos + (floorHeight / 2), (roomDim / 2) + 0.1f));
                    shader.setMat4("model", glm::scale(fM, glm::vec3(0.8f, floorHeight, 0.8f)));
                    shader.setVec4("ourColor", whiteNeon);
                    glDrawArrays(GL_TRIANGLES, 0, 36);
                }
            }
        }
    }

    float elevX = (300.0f / 2.0f) - 25.0f;
    float elevZ = -(300.0f / 2.0f) + 25.0f;
    float elevSize = 40.0f;
    float shaftTotalHeight = floorHeight * 4;

    glm::mat4 shaftM = glm::translate(glm::mat4(1.0f), glm::vec3(elevX, shaftTotalHeight / 2.0f - 2.0f, elevZ));
    shader.setMat4("model", glm::scale(shaftM, glm::vec3(elevSize + 2.0f, shaftTotalHeight, elevSize + 2.0f)));
    shader.setVec4("ourColor", glm::vec4(0.1f, 0.15f, 0.2f, 0.3f));
    glDrawArrays(GL_TRIANGLES, 0, 36);

    shader.setVec4("ourColor", glm::vec4(0.5f, 0.8f, 1.0f, 0.2f));
    glm::mat4 sideL = glm::translate(glm::mat4(1.0f), glm::vec3(elevX - elevSize / 2.0f, elevatorY + floorHeight / 2.0f, elevZ));
    shader.setMat4("model", glm::scale(sideL, glm::vec3(0.5f, floorHeight, elevSize)));
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glm::mat4 sideR = glm::translate(glm::mat4(1.0f), glm::vec3(elevX + elevSize / 2.0f, elevatorY + floorHeight / 2.0f, elevZ));
    shader.setMat4("model", glm::scale(sideR, glm::vec3(0.5f, floorHeight, elevSize)));
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glm::mat4 back = glm::translate(glm::mat4(1.0f), glm::vec3(elevX, elevatorY + floorHeight / 2.0f, elevZ - elevSize / 2.0f));
    shader.setMat4("model", glm::scale(back, glm::vec3(elevSize, floorHeight, 0.5f)));
    glDrawArrays(GL_TRIANGLES, 0, 36);

    shader.setVec4("ourColor", glm::vec4(0.2f, 0.2f, 0.25f, 1.0f));
    glm::mat4 eFloorM = glm::translate(glm::mat4(1.0f), glm::vec3(elevX, elevatorY + 0.5f, elevZ));
    shader.setMat4("model", glm::scale(eFloorM, glm::vec3(elevSize, 1.5f, elevSize)));
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glm::mat4 eCeilM = glm::translate(glm::mat4(1.0f), glm::vec3(elevX, elevatorY + floorHeight - 1.5f, elevZ));
    shader.setMat4("model", glm::scale(eCeilM, glm::vec3(elevSize, 1.5f, elevSize)));
    glDrawArrays(GL_TRIANGLES, 0, 36);

    for (int f = 0; f < 4; f++) {
        float fY = f * floorHeight;
        float openFactor = 0.0f;
        if (abs(elevatorY - fY) < 1.0f) openFactor = elevatorDoorOffset / 20.0f;
        float leafW = (elevSize / 2.0f);
        shader.setVec4("ourColor", glm::vec4(0.7f, 0.75f, 0.8f, 0.95f));
        float currentLeafW = leafW * (1.0f - openFactor);
        float offsetX = (leafW * openFactor) / 2.0f;
        glm::mat4 dL = glm::translate(glm::mat4(1.0f), glm::vec3(elevX - (leafW / 2.0f) - offsetX, fY + floorHeight / 2.0f, elevZ + elevSize / 2.0f + 0.5f));
        shader.setMat4("model", glm::scale(dL, glm::vec3(currentLeafW, floorHeight - 2.0f, 1.2f)));
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glm::mat4 dR = glm::translate(glm::mat4(1.0f), glm::vec3(elevX + (leafW / 2.0f) + offsetX, fY + floorHeight / 2.0f, elevZ + elevSize / 2.0f + 0.5f));
        shader.setMat4("model", glm::scale(dR, glm::vec3(currentLeafW, floorHeight - 2.0f, 1.2f)));
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }

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

void drawWorldCube(Shader& shader, unsigned int VAO) {
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

void drawRockArch(Shader& shader, glm::vec3 position) {
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

void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(window, true);
    float s = 120.0f * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) camera.Position += s * camera.Front;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) camera.Position -= s * camera.Front;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) camera.Position -= glm::normalize(glm::cross(camera.Front, camera.Up)) * s;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) camera.Position += glm::normalize(glm::cross(camera.Front, camera.Up)) * s;

    if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS) { doorOffset += 15.0f * deltaTime; if (doorOffset > 15.0f) doorOffset = 15.0f; }
    if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) { doorOffset -= 15.0f * deltaTime; if (doorOffset < 0.0f) doorOffset = 0.0f; }

    if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS) { elevatorDoorOffset += 20.0f * deltaTime; if (elevatorDoorOffset > 20.0f) elevatorDoorOffset = 20.0f; }
    if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS) { elevatorDoorOffset -= 20.0f * deltaTime; if (elevatorDoorOffset < 0.0f) elevatorDoorOffset = 0.0f; }

    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) { targetElevatorY = 0.0f; }
    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) { targetElevatorY = floorHeight; }
    if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) { targetElevatorY = floorHeight * 2.0f; }
    if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS) { targetElevatorY = floorHeight * 3.0f; }
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn) {
    float xpos = (float)xposIn; float ypos = (float)yposIn;
    if (firstMouse) { lastX = xpos; lastY = ypos; firstMouse = false; }
    float xoff = xpos - lastX; float yoff = lastY - ypos;
    lastX = xpos; lastY = ypos;
    camera.ProcessMouseMovement(xoff, yoff);
}

void drawLetter(Shader& shader, char letter, glm::vec3 pos, float scale) {
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
}