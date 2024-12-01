#include "View.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>

int screenHeight;
int screenWidth;

unsigned int colShader;
unsigned int texShader;

unsigned border;
unsigned row;
unsigned borderTimer;
unsigned rowTimer;
unsigned field;
unsigned clear;
unsigned backspace;
unsigned stop;
unsigned lens;
unsigned stopLens;
unsigned submitLens;
unsigned error;
unsigned submit;
unsigned logo;
unsigned letters[30];

GLFWwindow* window;

GLFWcursor* cursorHover;
GLFWcursor* cursorOpen;
GLFWcursor* cursorPress;

const double targetFrameTime = 1.0 / 60.0;

float convertX(int pixels) {
    return pixels * 2.0f / screenWidth - 1;
}

float convertY(int pixels) {
    return 1 - pixels * 2.0f / screenHeight;
}

bool isInSquare(GLFWwindow* window, float left, float up, float width, float height) {
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);
    return
        xpos >= left &&
        xpos <= left + width &&
        ypos >= up &&
        ypos <= up + height;
}

void drawWithLens(int start, unsigned texture) {
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, texture);
    glDrawArrays(GL_TRIANGLE_FAN, start, 4);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void drawUniversalElements() {
    glBindTexture(GL_TEXTURE_2D, logo);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

    glBindTexture(GL_TEXTURE_2D, borderTimer);
    glDrawArrays(GL_TRIANGLE_FAN, 4, 4);
    glDrawArrays(GL_TRIANGLE_FAN, 8, 4);
    glBindTexture(GL_TEXTURE_2D, rowTimer);
    glDrawArrays(GL_TRIANGLE_FAN, 12, 4);

    glBindTexture(GL_TEXTURE_2D, border);
    glDrawArrays(GL_TRIANGLE_FAN, 20, 4);
    glDrawArrays(GL_TRIANGLE_FAN, 24, 4);
    glDrawArrays(GL_TRIANGLE_FAN, 32, 4);
    glDrawArrays(GL_TRIANGLE_FAN, 36, 4);
    glBindTexture(GL_TEXTURE_2D, row);
    glDrawArrays(GL_TRIANGLE_FAN, 28, 4);
    glDrawArrays(GL_TRIANGLE_FAN, 40, 4);

    glBindTexture(GL_TEXTURE_2D, field);
}