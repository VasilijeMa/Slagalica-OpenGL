#include "View.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>

int screenHeight;
int screenWidth;

const int vertexCount = 308;
float* vertices = new float[vertexCount * 4] { };

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

void formBar(int start, int y, int height) {
    for (int i = start; i < start + 8; i++) {
        int x = i - start;
        if (x >= 4) x += 12 * 4 - 8;
        if ((i + 1) % 4 < 2) {
            vertices[i * 4] = convertX(int(screenWidth / 2.0 - screenWidth / 15 * (6 - x / 4) + PADDING));
            if (i < start + 4) vertices[i * 4 + 2] = 0.0;
            else vertices[i * 4 + 2] = 1.0;
        }
        else {
            vertices[i * 4] = convertX(int(screenWidth / 2.0 - screenWidth / 15 * (5 - x / 4) - PADDING));
            if (i < start + 4) vertices[i * 4 + 2] = 1.0;
            else vertices[i * 4 + 2] = 0.0;
        }

        if (i % 4 < 2) {
            vertices[i * 4 + 1] = convertY(y + height - PADDING);
            vertices[i * 4 + 3] = 0.0;
        }
        else {
            vertices[i * 4 + 1] = convertY(y + PADDING);
            vertices[i * 4 + 3] = 1.0;
        }
    }

    for (int i = start + 8; i < start + 12; i++) {
        if ((i + 1) % 4 < 2) {
            vertices[i * 4] = convertX(int(screenWidth / 2.0 - screenWidth / 15 * 5 - PADDING));
            vertices[i * 4 + 2] = 0.0;
        }
        else {
            vertices[i * 4] = convertX(int(screenWidth / 2.0 + screenWidth / 15 * 5 + PADDING));
            vertices[i * 4 + 2] = 1.0;
        }

        if (i % 4 < 2) {
            vertices[i * 4 + 1] = convertY(y + height - PADDING);
            vertices[i * 4 + 3] = 0.0;
        }
        else {
            vertices[i * 4 + 1] = convertY(y + PADDING);
            vertices[i * 4 + 3] = 1.0;
        }
    }
}

void formCell(int start, int x, int y, int width) {
    for (int i = start; i < start + 4; i++) {
        if ((i + 1) % 4 < 2) {
            vertices[i * 4] = convertX(x + PADDING);
            vertices[i * 4 + 2] = 0.0;
        }
        else {
            vertices[i * 4] = convertX(x + width - PADDING);
            vertices[i * 4 + 2] = 1.0;
        }

        if (i % 4 < 2) {
            vertices[i * 4 + 1] = convertY(y + screenWidth / 15 - PADDING);
            vertices[i * 4 + 3] = 0.0;
        }
        else {
            vertices[i * 4 + 1] = convertY(y + PADDING);
            vertices[i * 4 + 3] = 1.0;
        }
    }
}

void formTimer(int start) {
    for (int i = start; i < start + 8; i++) {
        if ((i + 1) % 4 < 2) {
            vertices[i * 4] = convertX(int(screenWidth - screenWidth / 15 / 2) / 2 + PADDING);
            vertices[i * 4 + 2] = 0.0;
        }
        else {
            vertices[i * 4] = convertX(int(screenWidth + screenWidth / 15 / 2) / 2 - PADDING);
            vertices[i * 4 + 2] = 1.0;
        }

        if (i % 4 < 2) {
            if (i < start + 4) {
                vertices[i * 4 + 1] = convertY(int(screenWidth / 15 / 2.0 - PADDING));
                vertices[i * 4 + 3] = 0.0;
            }
            else {
                vertices[i * 4 + 1] = convertY(int(screenHeight / 2.0 - PADDING));
                vertices[i * 4 + 3] = 1.0;
            }
        }
        else {
            if (i < start + 4) {
                vertices[i * 4 + 1] = convertY(PADDING);
                vertices[i * 4 + 3] = 1.0;
            }
            else {
                vertices[i * 4 + 1] = convertY(int((screenHeight - screenWidth / 15) / 2.0 + PADDING));
                vertices[i * 4 + 3] = 0.0;
            }
        }
    }

    for (int i = start + 8; i < start + 12; i++) {
        if ((i + 1) % 4 < 2) {
            vertices[i * 4] = convertX(int(screenWidth - screenWidth / 15 / 2) / 2 + PADDING);
            vertices[i * 4 + 2] = 0.0;
        }
        else {
            vertices[i * 4] = convertX(int(screenWidth + screenWidth / 15 / 2) / 2 - PADDING);
            vertices[i * 4 + 2] = 1.0;
        }

        if (i % 4 < 2) {
            vertices[i * 4 + 1] = convertY(int((screenHeight - screenWidth / 15) / 2.0 + PADDING));
            vertices[i * 4 + 3] = 0.0;
        }
        else {
            vertices[i * 4 + 1] = convertY(int(screenWidth / 15 / 2.0 - PADDING));
            vertices[i * 4 + 3] = 1.0;
        }
    }

    for (int i = start + 12; i < start + 16; i++) {
        if ((i + 1) % 4 < 2) {
            vertices[i * 4] = convertX(int(screenWidth - screenWidth / 15 / 2) / 2 + PADDING * 2);
        }
        else {
            vertices[i * 4] = convertX(int(screenWidth + screenWidth / 15 / 2) / 2 - PADDING * 2);
        }

        if (i % 4 < 2) {
            vertices[i * 4 + 1] = convertY(int(screenHeight / 2.0 - 3 * PADDING));
        }
        else {
            vertices[i * 4 + 1] = convertY(3 * PADDING);
        }
        vertices[i * 4 + 2] = 0.0;
        vertices[i * 4 + 3] = 1.0;
    }

}

void formLogo(int start) {
    for (int i = start; i < start + 4; i++) {
        if ((i + 1) % 4 < 2) {
            vertices[i * 4] = convertX(int(screenWidth - 1.5 * 1.37 * screenWidth / 15 - 6 * PADDING));
            vertices[i * 4 + 2] = 0.0;
        }
        else {
            vertices[i * 4] = convertX(screenWidth - 3 * PADDING);
            vertices[i * 4 + 2] = 1.0;
        }

        if (i % 4 < 2) {
            vertices[i * 4 + 1] = convertY(int(1.5 * screenWidth / 15 + 6 * PADDING));
            vertices[i * 4 + 3] = 0.0;
        }
        else {
            vertices[i * 4 + 1] = convertY(3 * PADDING);
            vertices[i * 4 + 3] = 1.0;
        }
    }
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