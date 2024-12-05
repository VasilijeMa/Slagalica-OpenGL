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
unsigned fieldL;
unsigned fieldXL;
unsigned clear;
unsigned backspace;
unsigned stop;
unsigned lens;
unsigned lensL;
unsigned lensXL;
unsigned stopLens;
unsigned submitLens;
unsigned error;
unsigned submit;
unsigned logo;
unsigned letters[30];

unsigned smallNumbers[10];
unsigned mediumNumbers[3];
unsigned largeNumbers[4];
unsigned operations[5];
unsigned brackets[2];

GLFWwindow* window;

GLFWcursor* cursorHover;
GLFWcursor* cursorOpen;
GLFWcursor* cursorPress;

const double fps = 1.0 / 60.0;

bool isInSquare(GLFWwindow* window, float left, float up, float width, float height);

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
    glClear(GL_COLOR_BUFFER_BIT);
    glUseProgram(texShader);
    glUniform1f(glGetUniformLocation(texShader, "uX"), 0);
    glUniform1f(glGetUniformLocation(texShader, "kX"), 0);

    glActiveTexture(GL_TEXTURE0);

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
    glActiveTexture(GL_TEXTURE1);
}

bool isOnStop(GLFWwindow* window) {
    return isInSquare(window,
        int(13 * screenWidth / 30.0) + PADDING,
        int(screenHeight / 2.0 + screenWidth * 3 / 15.0) + PADDING * 2,
        int(2 * screenWidth / 15.0) - 2 * PADDING,
        int(screenWidth / 15.0) - 4 * PADDING);
}

bool isOnBackspace(GLFWwindow* window) {
    return isInSquare(window,
        int(9 * screenWidth / 10.0) + PADDING,
        int(screenHeight / 2.0 + screenWidth * 2 / 15.0) + PADDING,
        int(screenWidth / 15.0) - 2 * PADDING,
        int(screenWidth / 15.0) - 2 * PADDING);
}

bool isOnClear(GLFWwindow* window) {
    return isInSquare(window,
        int(screenWidth / 30.0) + PADDING,
        int(screenHeight / 2.0 + screenWidth * 2 / 15.0) + PADDING,
        int(screenWidth / 15.0) - 2 * PADDING,
        int(screenWidth / 15.0) - 2 * PADDING);
}

bool isOnLetter(GLFWwindow* window, int index) {
    return isInSquare(window,
        int(screenWidth * (3 + 2 * index) / 30.0) + PADDING,
        int(screenHeight / 2.0 + screenWidth / 15.0) + PADDING,
        int(screenWidth / 15.0) - 2 * PADDING,
        int(screenWidth / 15.0) - 2 * PADDING);
}

bool isOnSubmit(GLFWwindow* window) {
    return isInSquare(window,
        int(4 * screenWidth / 10.0) + PADDING,
        int(screenHeight / 2.0 + screenWidth * 3 / 15.0) + PADDING * 2,
        int(screenWidth / 5.0) - 2 * PADDING,
        int(screenWidth / 15.0) - 4 * PADDING);
}