﻿#include "LettersGame.h"
#include "NumbersGame.h"
#include "View.h"
#include "FileManager.h"
#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

float* vertices;

void loadFiles();
void formBar(int start, int y, int height);
void formCell(int start, int x, int y, int width);
void formTimer(int start);
void formLogo(int start);
void formVertices();

int main(void)
{
    int roundTime = 120;

    if (!glfwInit())
    {
        std::cout << "GLFW Biblioteka se nije ucitala! :(\n";
        return 1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);

    screenWidth = mode->width;
    screenHeight = mode->height;
    window = glfwCreateWindow(screenWidth, screenHeight, "Slagalica", monitor, NULL);

    if (window == NULL)
    {
        std::cout << "Prozor nije napravljen! :(\n";
        glfwTerminate();
        return 2;
    }
    glfwMakeContextCurrent(window);

    if (glewInit() != GLEW_OK)
    {
        std::cout << "GLEW nije mogao da se ucita! :'(\n";
        return 3;
    }

    const int vertexCount = 364;
    vertices = new float[vertexCount * 4];

    formVertices();

    unsigned int VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    unsigned int VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertexCount * 4 * sizeof(float), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    loadFiles();

    glUseProgram(texShader);
    glUniform1i(glGetUniformLocation(texShader, "background"), 0);
    glUniform1i(glGetUniformLocation(texShader, "foreground"), 1);
    glUniform1i(glGetUniformLocation(texShader, "lens"), 2);

    glClearColor(0.05, 0.1, 0.4, 1.0);

    glBindVertexArray(VAO);

    playLettersGame(roundTime);
    playNumbersGame(roundTime);

    glBindVertexArray(0);

    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &VAO);

    delete[] vertices;
    glDeleteProgram(colShader);
    glDeleteProgram(texShader);
    glfwDestroyWindow(window);
    glfwDestroyCursor(cursorHover);
    glfwDestroyCursor(cursorOpen);
    glfwDestroyCursor(cursorPress);
    glfwTerminate();
    return 0;
}

void loadFiles() {
    colShader = createShader("col.vert", "col.frag");
    texShader = createShader("tex.vert", "tex.frag");

    border = loadTexture("box", "border");
    row = loadTexture("box", "row");
    borderTimer = loadTexture("box", "border-timer");
    rowTimer = loadTexture("box", "row-timer");
    field = loadTexture("buttons", "field");
    fieldL = loadTexture("buttons", "field-L");
    fieldXL = loadTexture("buttons", "field-XL");
    clear = loadTexture("buttons", "clear");
    backspace = loadTexture("buttons", "backspace");
    stop = loadTexture("buttons", "stop");
    lens = loadTexture("buttons", "lens");
    lensL = loadTexture("buttons", "lens-L");
    lensXL = loadTexture("buttons", "lens-XL");
    stopLens = loadTexture("buttons", "stop-lens");
    submitLens = loadTexture("buttons", "submit-lens");
    error = loadTexture("buttons", "error");
    submit = loadTexture("buttons", "submit");
    background = loadTexture("misc", "background");
    logo = loadTexture("misc", "logo", true);
    player = loadTexture("misc", "player");
    
    for (int i = 0; i < 30; i++) {
        letters[i] = loadTexture("letters", ("letter" + std::to_string(i + 1)).c_str());
    }
    
    for (int i = 0; i < 10; i++) {
        smallNumbers[i] = loadTexture("numbers", ("number" + std::to_string(i)).c_str());
    }
    
    for (int i = 0; i < 3; i++) {
        mediumNumbers[i] = loadTexture("numbers", ("number" + std::to_string(10 + i * 5)).c_str());
    }

    for (int i = 0; i < 4; i++) {
        largeNumbers[i] = loadTexture("numbers", ("number" + std::to_string(25 + i * 25)).c_str());
    }

    operations[0] = loadTexture("operations", "plus");
    operations[1] = loadTexture("operations", "minus");
    operations[2] = loadTexture("operations", "times");
    operations[3] = loadTexture("operations", "over");
    operations[4] = loadTexture("operations", "equals");
    brackets[0] = loadTexture("operations", "bracket-open");
    brackets[1] = loadTexture("operations", "bracket-closed");

    glBindTexture(GL_TEXTURE_2D, 0);

    cursorHover = loadImageToCursor("res/cursor/cursor-hover.png");
    cursorOpen = loadImageToCursor("res/cursor/cursor-open.png");
    cursorPress = loadImageToCursor("res/cursor/cursor-press.png");
}

void formBar(int start, int y, int height) {
    for (int i = start; i < start + 8; i++) {
        int x = i - start;
        if (x >= 4) x += 12 * 4 - 8;
        if ((i + 1) % 4 < 2) {
            vertices[i * 4] = convertX(int(screenWidth * (3 + x / 4 * 2) / 30.0 + PADDING));
            if (i < start + 4) vertices[i * 4 + 2] = 0.0;
            else vertices[i * 4 + 2] = 1.0;
        }
        else {
            vertices[i * 4] = convertX(int(screenWidth * (5 + x / 4 * 2) / 30.0 - PADDING));
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
            vertices[i * 4] = convertX(int(screenWidth / 6.0 - PADDING));
            vertices[i * 4 + 2] = 0.0;
        }
        else {
            vertices[i * 4] = convertX(int(screenWidth * 5 / 6.0 + PADDING));
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
            vertices[i * 4] = convertX(int(screenWidth * 29 / 60.0) + PADDING);
            vertices[i * 4 + 2] = 0.0;
        }
        else {
            vertices[i * 4] = convertX(int(screenWidth * 31 / 60.0) - PADDING);
            vertices[i * 4 + 2] = 1.0;
        }

        if (i % 4 < 2) {
            if (i < start + 4) {
                vertices[i * 4 + 1] = convertY(int(screenWidth / 10.0) - PADDING);
                vertices[i * 4 + 3] = 0.0;
            }
            else {
                vertices[i * 4 + 1] = convertY(int(screenHeight / 2.0) - PADDING);
                vertices[i * 4 + 3] = 1.0;
            }
        }
        else {
            if (i < start + 4) {
                vertices[i * 4 + 1] = convertY(int(screenWidth / 15.0) + PADDING);
                vertices[i * 4 + 3] = 1.0;
            }
            else {
                vertices[i * 4 + 1] = convertY(int((screenHeight - screenWidth / 15.0) / 2.0) + PADDING);
                vertices[i * 4 + 3] = 0.0;
            }
        }
    }

    for (int i = start + 8; i < start + 12; i++) {
        if ((i + 1) % 4 < 2) {
            vertices[i * 4] = convertX(int(screenWidth * 29 / 60.0) + PADDING);
            vertices[i * 4 + 2] = 0.0;
        }
        else {
            vertices[i * 4] = convertX(int(screenWidth * 31 / 60.0) - PADDING);
            vertices[i * 4 + 2] = 1.0;
        }

        if (i % 4 < 2) {
            vertices[i * 4 + 1] = convertY(int((screenHeight - screenWidth / 15.0) / 2.0) + PADDING);
            vertices[i * 4 + 3] = 0.0;
        }
        else {
            vertices[i * 4 + 1] = convertY(int(screenWidth / 10.0) - PADDING);
            vertices[i * 4 + 3] = 1.0;
        }
    }

    for (int i = start + 12; i < start + 16; i++) {
        if ((i + 1) % 4 < 2) {
            vertices[i * 4] = convertX(int(screenWidth * 29 / 60.0) + PADDING * 2);
        }
        else {
            vertices[i * 4] = convertX(int(screenWidth * 31 / 60.0) - PADDING * 2);
        }

        if (i % 4 < 2) {
            vertices[i * 4 + 1] = convertY(int(screenHeight / 2.0) - 3 * PADDING);
        }
        else {
            vertices[i * 4 + 1] = convertY(int(screenWidth / 15.0) + 3 * PADDING);
        }
        vertices[i * 4 + 2] = 0.0;
        vertices[i * 4 + 3] = 1.0;
    }

}

void formLogo(int start) {
    for (int i = start; i < start + 4; i++) {
        if ((i + 1) % 4 < 2) {
            vertices[i * 4] = convertX(int(0.863 * screenWidth) - 6 * PADDING);
            vertices[i * 4 + 2] = 0.0;
        }
        else {
            vertices[i * 4] = convertX(screenWidth - 3 * PADDING);
            vertices[i * 4 + 2] = 1.0;
        }

        if (i % 4 < 2) {
            vertices[i * 4 + 1] = convertY(int(screenWidth / 10.0) + 6 * PADDING);
            vertices[i * 4 + 3] = 0.0;
        }
        else {
            vertices[i * 4 + 1] = convertY(3 * PADDING);
            vertices[i * 4 + 3] = 1.0;
        }
    }
}

void formPlayer(int start, int left, int up, int orientation) {
    for (int i = start; i < start + 4; i++) {
        if ((i + 1) % 4 < 2) {
            vertices[i * 4] = convertX(left + orientation * PADDING);
            vertices[i * 4 + 2] = 0.0;
        }
        else {
            vertices[i * 4] = convertX(left + orientation * int(screenWidth * 6 / 30.0) - orientation * PADDING);
            vertices[i * 4 + 2] = 1.0;
        }

        if (i % 4 < 2) {
            vertices[i * 4 + 1] = convertY(up + int(screenWidth * 6 / 30.0) - PADDING);
            vertices[i * 4 + 3] = 0.0;
        }
        else {
            vertices[i * 4 + 1] = convertY(up + PADDING);
            vertices[i * 4 + 3] = 1.0;
        }
    }
}

void formBackground(int start) {
    for (int i = start; i < start + 4; i++) {
        if ((i + 1) % 4 < 2) {
            vertices[i * 4] = -1.0;
            vertices[i * 4 + 2] = 0.0;
        }
        else {
            vertices[i * 4] = 1.0;
            vertices[i * 4 + 2] = 1.0;
        }

        if (i % 4 < 2) {
            vertices[i * 4 + 1] = -1.0;
            vertices[i * 4 + 3] = 0.0;
        }
        else {
            vertices[i * 4 + 1] = 1.0;
            vertices[i * 4 + 3] = 1.0;
        }
    }
}

void formVertices() {
    formLogo(0); // logo

    formTimer(4); //timer

    formBar(20, // row 1
        int(screenHeight / 2.0 + screenWidth * 2 / 15.0),
        int(screenWidth / 15.0));

    formBar(32, // row 2
        int(screenHeight / 2.0 + screenWidth * 3 / 15.0),
        int(screenWidth / 15.0));

    formBackground(44); // background

    formPlayer(48, int(screenWidth * 5 / 30.0), int(screenWidth / 30.0), 1); // pic1

    formPlayer(52, int(screenWidth * 25 / 30.0), int(screenWidth / 30.0), -1); // pic2

    formCell(56, // stop
        int(screenWidth * 13 / 30.0),
        int(screenHeight / 2.0 + 3 * screenWidth / 15.0),
        int(2 * screenWidth / 15.0));

    formCell(60, // clear
        int(screenWidth / 30.0),
        int(screenHeight / 2.0 + 2 * screenWidth / 15.0),
        int(screenWidth / 15.0));

    formCell(64, // backspace
        int(27 * screenWidth / 30.0),
        int(screenHeight / 2.0 + 2 * screenWidth / 15.0),
        int(screenWidth / 15.0));

    formCell(68, // submit
        int(2 * screenWidth / 5.0),
        int(screenHeight / 2.0 + 3 * screenWidth / 15.0),
        int(3 * screenWidth / 15.0));

    formCell(72, // error
        int(7 * screenWidth / 15.0),
        int(screenHeight / 2.0 + 3 * screenWidth / 15.0),
        int(screenWidth / 15.0));

    for (int i = 0; i < 12; i++) { // letters + small numbers
        formCell(76 + i * 4,
            int(screenWidth * (3 + 2 * i) / 30.0),
            int(screenHeight / 2.0 + screenWidth / 15.0),
            int(screenWidth / 15.0));
    }

    for (int i = 0; i < 23; i++) { // typing
        formCell(124 + i * 4,
            int(7 * screenWidth / 15.0),
            int(screenHeight / 2.0 + screenWidth * 2 / 15.0),
            int(screenWidth / 15.0));
    }

    for (int i = 0; i < 23; i++) { // solution
        formCell(216 + i * 4,
            int(7 * screenWidth / 15.0),
            int(screenHeight / 2.0 + 3 * screenWidth / 15.0),
            int(screenWidth / 15.0));
    }

    for (int i = 0; i < 3; i++) { // target number digits
        formCell(308 + i * 4,
            int(screenWidth * (i + 6) / 15.0),
            int(screenHeight / 2.0),
            int(screenWidth / 15.0));
    }

    formCell(320, // medium number
        int(screenWidth * 27 / 60.0),
        int(screenHeight / 2.0 + screenWidth / 15.0),
        int(screenWidth / 10.0));

    formCell(324, // large number
        int(screenWidth * 23 / 30.0),
        int(screenHeight / 2.0 + screenWidth / 15.0),
        int(screenWidth * 2 / 15.0));

    for (int i = 0; i < 4; i++) { // operations
        formCell(328 + i * 4,
            int(screenWidth * (3 + 2 * i) / 30.0),
            int(screenHeight / 2.0),
            int(screenWidth / 15.0));
    }

    formCell(344, // open bracket
        int(screenWidth * 23 / 30.0),
        int(screenHeight / 2.0),
        int(screenWidth / 15.0));

    formCell(348, // closed bracket
        int(screenWidth * 5 / 6.0),
        int(screenHeight / 2.0),
        int(screenWidth / 15.0));

    formCell(352, // score field
        int(screenWidth * 27 / 60.0),
        0,
        int(screenWidth / 10.0));

    formCell(356, // score digit 1
        int(screenWidth * 7 / 15.0),
        0,
        int(screenWidth / 15.0));

    formCell(360, // scored digit 2
        int(screenWidth * 7 / 15.0),
        0,
        int(screenWidth / 15.0));
}