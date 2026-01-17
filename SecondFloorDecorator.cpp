#include "SecondFloorDecorator.h"
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>

void SecondFloorDecorator::Draw(Shader& shader, unsigned int VAO, float fY, float roomDim) {
    drawDecoratedCeiling(shader, VAO, fY, roomDim);
}

void SecondFloorDecorator::drawDecoratedCeiling(Shader& shader, unsigned int VAO, float fY, float roomDim) {
    glBindVertexArray(VAO);

    // تعريف الألوان المطلوبة
    glm::vec4 oliveGreenCeil(0.23f, 0.28f, 0.10f, 1.0f); // لون زيتي للسقف
    glm::vec4 pinkPanel(1.0f, 0.41f, 0.70f, 1.0f);       // لون زهر للمتوازيات
    glm::vec4 whiteNeon(3.0f, 3.0f, 3.5f, 1.0f);         // نيون أبيض قوي

    // 1. رسم بلاطة السقف (باللون الزيتي)
    // نفترض أن ارتفاع السقف هو fY + ارتفاع الطابق (مثلاً 50)
    float ceilHeight = fY + 49.0f;
    glm::mat4 ceilM = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, ceilHeight, 0.0f));
    shader.setMat4("model", glm::scale(ceilM, glm::vec3(roomDim, 0.4f, roomDim)));
    shader.setVec4("ourColor", oliveGreenCeil);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    // 2. رسم الـ 4 متوازيات (صفين، كل صف فيه 2)
    float panelW = 80.0f;  // عرض المتوازي
    float panelD = 40.0f;  // عمق المتوازي
    float panelH = 0.8f;   // البروز

    // إحداثيات الصفوف والأعمدة لتوزيع المتوازيات الأربعة
    float xPositions[2] = { -60.0f, 60.0f }; // العمودين
    float zPositions[2] = { -50.0f, 50.0f }; // الصفين

    for (int r = 0; r < 2; r++) {     // الصفوف
        for (int c = 0; c < 2; c++) { // الأعمدة
            glm::mat4 pPos = glm::translate(glm::mat4(1.0f), glm::vec3(xPositions[c], ceilHeight - 0.6f, zPositions[r]));

            // أ- رسم برواز النيون الأبيض (أكبر قليلاً)
            shader.setMat4("model", glm::scale(pPos, glm::vec3(panelW + 8.0f, panelH + 0.1f, panelD + 8.0f)));
            shader.setVec4("ourColor", whiteNeon);
            glDrawArrays(GL_TRIANGLES, 0, 36);

            // ب- رسم المتوازي الزهري
            shader.setMat4("model", glm::scale(pPos, glm::vec3(panelW, panelH + 0.3f, panelD)));
            shader.setVec4("ourColor", pinkPanel);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
    }
}