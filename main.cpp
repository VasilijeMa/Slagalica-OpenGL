#include "LettersGame.h"
#include "View.h"
#include "FileManager.h"
#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

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

    /*screenWidth = 800;
    screenHeight = 600;
    window = glfwCreateWindow(screenWidth, screenHeight, "Slagalica", NULL, NULL);*/

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

    formLogo(0); // logo

    formTimer(4); //timer

    formBar(20, // row 1
        screenHeight / 2 + screenWidth / 15,
        screenWidth / 15);

    formBar(32, // row 2
        screenHeight / 2 + screenWidth / 15 * 2,
        screenWidth / 15);

    formCell(44, // background TODO
        0,
        0,
        0
    );
    formCell(48, // pic1 TODO
        0,
        0,
        0
    );
    formCell(52, // pic2 TODO
        0,
        0,
        0
    );

    formCell(56, // stop
        int(screenWidth / 2.0) - screenWidth / 15,
        screenHeight / 2 + screenWidth / 15,
        2 * screenWidth / 15);

    formCell(60, // clear
        int(screenWidth / 2.0 - screenWidth / 15 * 7),
        screenHeight / 2 + screenWidth / 15,
        screenWidth / 15);

    formCell(64, // backspace
        int(screenWidth / 2.0 + screenWidth / 15 * 6),
        screenHeight / 2 + screenWidth / 15,
        screenWidth / 15);

    formCell(68, // submit
        int(screenWidth / 2.0 - screenWidth / 15 * 1.5),
        screenHeight / 2 + screenWidth / 15 * 2,
        3 * screenWidth / 15);

    formCell(72, // error
        int((screenWidth - screenWidth / 15) / 2.0),
        screenHeight / 2 + screenWidth / 15 * 2,
        screenWidth / 15);


    for (int i = 0; i < 12; i++) { // letters + prva 4 broja
        formCell(76 + i * 4,
            int(screenWidth / 2.0 - screenWidth / 15 * (6 - i)),
            screenHeight / 2,
            screenWidth / 15);
    }

    for (int i = 0; i < 23; i++) { // typing
        formCell(124 + i * 4,
            int((screenWidth - screenWidth / 15) / 2.0),
            screenHeight / 2 + screenWidth / 15,
            screenWidth / 15);
    }

    for (int i = 0; i < 23; i++) { // solution
        formCell(216 + i * 4,
            int((screenWidth - screenWidth / 15) / 2.0),
            screenHeight / 2 + screenWidth / 15 * 2,
            screenWidth / 15);
    }

    // TODO: 3 cifre, 2 velika broja, 6 operatora = 11 * 4 cvorova
    // TODO: 4 score baferi, 2 cifre score * 2 = 8 * 4 cvorova

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

    colShader = createShader("col.vert", "col.frag");
    texShader = createShader("tex.vert", "tex.frag");

    border = loadTexture("box", "border");
    row = loadTexture("box", "row");
    borderTimer = loadTexture("box", "border-timer");
    rowTimer = loadTexture("box", "row-timer");
    field = loadTexture("buttons", "field");
    clear = loadTexture("buttons", "clear");
    backspace = loadTexture("buttons", "backspace");
    stop = loadTexture("buttons", "stop");
    lens = loadTexture("buttons", "lens");
    stopLens = loadTexture("buttons", "stop-lens");
    submitLens = loadTexture("buttons", "submit-lens");
    error = loadTexture("buttons", "error");
    submit = loadTexture("buttons", "submit");
    logo = loadTexture("misc", "logo", true);

    glBindTexture(GL_TEXTURE_2D, 0);

    glUseProgram(texShader);
    glUniform1i(glGetUniformLocation(texShader, "background"), 0);
    glUniform1i(glGetUniformLocation(texShader, "foreground"), 1);
    glUniform1i(glGetUniformLocation(texShader, "lens"), 2);

    cursorHover = loadImageToCursor("res/cursor/cursor-hover.png");
    cursorOpen = loadImageToCursor("res/cursor/cursor-open.png");
    cursorPress = loadImageToCursor("res/cursor/cursor-press.png");

    glClearColor(0.05, 0.1, 0.4, 1.0);


    glBindVertexArray(VAO);
    playLettersGame(roundTime);
    glBindVertexArray(0);
    //TODO play numbers game

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