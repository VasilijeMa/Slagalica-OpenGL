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
unsigned background;
unsigned logo;
unsigned player;

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

bool isInSquare(float left, float up, float width, float height);

float convertX(int pixels) {
    return pixels * 2.0f / screenWidth - 1;
}

float convertY(int pixels) {
    return 1 - pixels * 2.0f / screenHeight;
}

bool isInSquare(float left, float up, float width, float height) {
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
    glBindTexture(GL_TEXTURE_2D, background);
    glDrawArrays(GL_TRIANGLE_FAN, 44, 4);

    glUniform1f(glGetUniformLocation(texShader, "alpha"), 0.4);
    glBindTexture(GL_TEXTURE_2D, logo);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

    glUniform1f(glGetUniformLocation(texShader, "alpha"), 0);
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
    glBindTexture(GL_TEXTURE_2D, player);
    glDrawArrays(GL_TRIANGLE_FAN, 48, 4);
    glDrawArrays(GL_TRIANGLE_FAN, 52, 4);

    glBindTexture(GL_TEXTURE_2D, field);
    glActiveTexture(GL_TEXTURE1);
}

bool isOnStop() {
    return isInSquare(
        int(13 * screenWidth / 30.0) + PADDING,
        int(screenHeight / 2.0 + screenWidth * 3 / 15.0) + PADDING * 2,
        int(2 * screenWidth / 15.0) - 2 * PADDING,
        int(screenWidth / 15.0) - 4 * PADDING);
}

bool isOnBackspace() {
    return isInSquare(
        int(9 * screenWidth / 10.0) + PADDING,
        int(screenHeight / 2.0 + screenWidth * 2 / 15.0) + PADDING,
        int(screenWidth / 15.0) - 2 * PADDING,
        int(screenWidth / 15.0) - 2 * PADDING);
}

bool isOnClear() {
    return isInSquare(
        int(screenWidth / 30.0) + PADDING,
        int(screenHeight / 2.0 + screenWidth * 2 / 15.0) + PADDING,
        int(screenWidth / 15.0) - 2 * PADDING,
        int(screenWidth / 15.0) - 2 * PADDING);
}

bool isOnLetter(int index) {
    return isInSquare(
        int(screenWidth * (3 + 2 * index) / 30.0) + PADDING,
        int(screenHeight / 2.0 + screenWidth / 15.0) + PADDING,
        int(screenWidth / 15.0) - 2 * PADDING,
        int(screenWidth / 15.0) - 2 * PADDING);
}

bool isOnSubmit() {
    return isInSquare(
        int(4 * screenWidth / 10.0) + PADDING,
        int(screenHeight / 2.0 + screenWidth * 3 / 15.0) + PADDING * 2,
        int(screenWidth / 5.0) - 2 * PADDING,
        int(screenWidth / 15.0) - 4 * PADDING);
}

bool isOnMediumNumber() {
    return isInSquare(
        screenWidth * 27 / 60.0 + PADDING,
        screenHeight / 2.0 + screenWidth / 15.0 + PADDING,
        screenWidth / 10.0 - 2 * PADDING,
        screenWidth / 15.0 - 2 * PADDING
    );
}

bool isOnLargeNumber() {
    return isInSquare(
        screenWidth * 23 / 30.0 + PADDING,
        screenHeight / 2.0 + screenWidth / 15.0 + PADDING,
        screenWidth * 2 / 15.0 - 2 * PADDING,
        screenWidth / 15.0 - 2 * PADDING
    );
}

bool isOnOperation(int index) {
    return isInSquare(
        screenWidth * (3 + 2 * index) / 30.0 + PADDING,
        screenHeight / 2.0 + PADDING,
        screenWidth / 15.0 - 2 * PADDING,
        screenWidth / 15.0 - 2 * PADDING
    );
}

bool isOnBracket(bool closed) {
    return isInSquare(
        screenWidth * (23 + 2 * closed) / 30.0 + PADDING,
        screenHeight / 2.0 + PADDING,
        screenWidth / 15.0 - 2 * PADDING,
        screenWidth / 15.0 - 2 * PADDING
    );
}

bool isOnSymbol(int index) {
    return (index < 4 && isOnLetter(index)) ||
        (index == 4 && isOnMediumNumber()) ||
        (index == 5 && isOnLargeNumber()) ||
        (index > 5 && index < 10 && isOnOperation(index - 6)) ||
        (index >= 10 && isOnBracket(index == 11));
}