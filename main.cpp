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

    /*screenWidth = mode->width;
    screenHeight = mode->height;
    window = glfwCreateWindow(screenWidth, screenHeight, "Slagalica", monitor, NULL);*/

    screenWidth = 800;
    screenHeight = 600;
    window = glfwCreateWindow(screenWidth, screenHeight, "Slagalica", NULL, NULL);

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

    vertices = nullptr;
    vertices = new float[vertexCount * 4] {
        0.2, 0, 0, 0,
        0.4, 0, 1, 0,
        0.3, 0.5, 0.5, 1
        };

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
    // playDemo();
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