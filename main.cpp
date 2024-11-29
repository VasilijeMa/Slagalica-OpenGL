
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <fstream>
#include <sstream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

unsigned int compileShader(GLenum type, const char* source);
unsigned int createShader(const char* vsSource, const char* fsSource);
//unsigned loadTexture(std::string folderName, std::string fileName, bool filterLinear = false);
unsigned loadImageToTexture(const char* filePath);
GLFWcursor* loadImageToCursor(const char* filePath);
#include <random>
#include <Windows.h>
#include <thread>
#include <locale>

using namespace std;

unsigned int compileShader(GLenum type, const char* source) {
    std::string content = "";
    std::ifstream file(source);
    std::stringstream ss;
    if (file.is_open()) {
        ss << file.rdbuf();
        file.close();
        std::cout << "Uspesno procitao fajl sa putanje \"" << source << "\"!" << std::endl;
    }
    else {
        ss << "";
        std::cout << "Greska pri citanju fajla sa putanje \"" << source << "\"!" << std::endl;
    }
    std::string temp = ss.str();
    const char* sourceCode = temp.c_str();

    int shader = glCreateShader(type);

    int success;
    char infoLog[512];
    glShaderSource(shader, 1, &sourceCode, NULL);
    glCompileShader(shader);

    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (success == GL_FALSE) {
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        if (type == GL_VERTEX_SHADER)
            printf("VERTEX");
        else if (type == GL_FRAGMENT_SHADER)
            printf("FRAGMENT");
        printf(" sejder ima gresku! Greska: \n");
        printf(infoLog);
    }
    return shader;
}

unsigned int createShader(const char* vsSource, const char* fsSource) {
    unsigned int program;
    unsigned int vertexShader;
    unsigned int fragmentShader;

    program = glCreateProgram();

    vertexShader = compileShader(GL_VERTEX_SHADER, vsSource);
    fragmentShader = compileShader(GL_FRAGMENT_SHADER, fsSource);

    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);

    glLinkProgram(program);
    glValidateProgram(program);

    int success;
    char infoLog[512];
    glGetProgramiv(program, GL_VALIDATE_STATUS, &success);
    if (success == GL_FALSE) {
        glGetShaderInfoLog(program, 512, NULL, infoLog);
        std::cout << "Objedinjeni sejder ima gresku! Greska: \n";
        std::cout << infoLog << std::endl;
    }

    glDetachShader(program, vertexShader);
    glDeleteShader(vertexShader);
    glDetachShader(program, fragmentShader);
    glDeleteShader(fragmentShader);

    return program;
}

unsigned loadImageToTexture(const char* filePath) {
    int TextureWidth;
    int TextureHeight;
    int TextureChannels;
    unsigned char* ImageData = stbi_load(filePath, &TextureWidth, &TextureHeight, &TextureChannels, 0);
    if (ImageData != NULL)
    {
        //Slike se osnovno ucitavaju naopako pa se moraju ispraviti da budu uspravne
        stbi__vertical_flip(ImageData, TextureWidth, TextureHeight, TextureChannels);

        // Provjerava koji je format boja ucitane slike
        GLint InternalFormat = -1;
        switch (TextureChannels) {
        case 1: InternalFormat = GL_RED; break;
        case 2: InternalFormat = GL_RG; break;
        case 3: InternalFormat = GL_RGB; break;
        case 4: InternalFormat = GL_RGBA; break;
        default: InternalFormat = GL_RGB; break;
        }

        unsigned int Texture;
        glGenTextures(1, &Texture);
        glBindTexture(GL_TEXTURE_2D, Texture);
        glTexImage2D(GL_TEXTURE_2D, 0, InternalFormat, TextureWidth, TextureHeight, 0, InternalFormat, GL_UNSIGNED_BYTE, ImageData);
        glBindTexture(GL_TEXTURE_2D, 0);
        // oslobadjanje memorije zauzete sa stbi_load posto vise nije potrebna
        stbi_image_free(ImageData);
        return Texture;
    }
    else
    {
        std::cout << "Tekstura nije ucitana! Putanja teksture: " << filePath << std::endl;
        stbi_image_free(ImageData);
        return 0;
    }
}

GLFWcursor* loadImageToCursor(const char* filePath) {
    int TextureWidth;
    int TextureHeight;
    int TextureChannels;

    unsigned char* ImageData = stbi_load(filePath, &TextureWidth, &TextureHeight, &TextureChannels, 0);

    if (ImageData != NULL)
    {
        GLFWimage image;
        image.width = TextureWidth;
        image.height = TextureHeight;
        image.pixels = ImageData;

        int hotspotX = TextureWidth / 5;
        int hotspotY = TextureHeight / 6;

        GLFWcursor* cursor = glfwCreateCursor(&image, hotspotX, hotspotY);
        stbi_image_free(ImageData);
        return cursor;
    }
    else {
        std::cout << "Kursor nije ucitana! Putanja kursora: " << filePath << std::endl;
        stbi_image_free(ImageData);
        return 0;
    }
}

bool isSubset(std::wstring subset, std::wstring set) {

    if (subset.length() > set.length()) {
        return false;
    }

    sort(set.begin(), set.end());
    sort(subset.begin(), subset.end());

    int i = 0;
    int j = 0;

    while (i < subset.length()) {
        if (j == set.length()) {
            return false;
        }
        if (subset[i] == set[j]) {
            ++i;
            ++j;
        }
        else if (subset[i] > set[j]) {
            ++j;
        }
        else {
            return false;
        }
    }
    return true;
}

bool findMatchSerial(std::wstring userWord, const std::vector<std::wstring>& allWords) {
    for (int i = 0; i < allWords.size(); i++) {
        if (userWord == allWords[i]) {
            return true;
        }
    }
    return false;
}

int findBestSolutionsSerial(std::wstring randomWord, const std::vector<std::wstring>& allWords, std::vector<std::wstring>& solutions) {
    int bestLength = 0;
    for (int i = 0; i < allWords.size();i++) {
        std::wstring word = allWords[i];
        if (word.length() > bestLength) {
            if (isSubset(word, randomWord)) {
                solutions.clear();
                solutions.push_back(word);
                bestLength = word.length();
            }
        }
        else if (word.length() == bestLength) {
            if (isSubset(word, randomWord)) {
                solutions.push_back(word);
            }
        }
    }
    return bestLength;
}

const int game = 1;
int roundTime = 120;

int formArray();
void userInteraction();
void draw();
float convertX(int pixels);
float convertY(int pixels);
int convertLetter(wchar_t letter);

void formBar(int start, int y, int height);
void formCell(int start, int x, int y, int width);
void formTimer(int start);
void formLogo(int start);

void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);

void actionBackspace();
void actionClear();
void actionChooseLetter(int index);
void actionStop();
void actionSubmit();

void drawWithLens(int start, unsigned texture);

void updateCursor();
void updateError();

bool isInSquare(float left, float up, float width, float height);
bool isSelected(int index);

wstring getCurrentWord();
wstring getGeneratedLetters();

float* vertices;
GLFWwindow* window;

unsigned int colShader;
unsigned int texShader;

int screenWidth;
int screenHeight;

const int padding = 5;

int generatedLetterCounter = 0;
int generatedLetters[12] = {
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1
};

bool stopPressed = false;
bool backspacePressed = false;
bool clearPressed = false;
bool submitPressed = false;
int letterPressed = -1;
bool gameEnded = false;
bool findSolutionThreadStarted = false;
bool loadDictThreadEnded = false;

bool isCurrentWordInvalid = false;

vector<wstring> invalidWords;

vector<wstring> allWords;

vector<wstring> solutions;
int solutionLength = -1;

unsigned field;
unsigned border;
unsigned row;
unsigned lens;
unsigned backspace;
unsigned clear;
unsigned stop;
unsigned stopLens;
unsigned submit;
unsigned submitLens;
unsigned borderTimer;
unsigned rowTimer;
unsigned error;
unsigned logo;

unsigned letters[30];

wstring alphabet = L"абвгдђежзијклљмнњопрстћуфхцчџш";

int letterKeys[12] = {
    GLFW_KEY_1,
    GLFW_KEY_2,
    GLFW_KEY_3,
    GLFW_KEY_4,
    GLFW_KEY_5,
    GLFW_KEY_6,
    GLFW_KEY_7,
    GLFW_KEY_8,
    GLFW_KEY_9,
    GLFW_KEY_0,
    GLFW_KEY_MINUS,
    GLFW_KEY_EQUAL
};

int probabilities[] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // A
    1, 1, 1, 1, // B
    2, 2, 2, 2, 2, 2, // V
    3, 3, 3, 3,  // G
    4, 4, 4, 4, // D
    5, // Đ
    6, 6, 6, 6, 6, 6, 6, 6, // E
    7, // Ž
    8, 8, 8, 8, // Z
    9, 9, 9, 9, 9, 9, 9, 9, 9, 9, // I
    10, 10, 10, 10, // J
    11, 11, 11, 11, 11, 11, // K
    12, 12, 12, 12, 12, // L
    13, // Lj
    14, 14, 14, 14, // M
    15, 15, 15, 15, 15, 15, // N
    16, // Nj
    17, 17, 17, 17, 17, 17, 17, 17, // O
    18, 18, 18, 18, 18, // P
    19, 19, 19, 19, 19, // R
    20, 20, 20, 20, 20, 20, // S
    21, 21, 21, 21, 21, 21, // T
    22, 22, // Ć
    23, 23, 23, 23, 23, 23, // U
    24, // F
    25, 25, // H
    26, 26, 26, // C
    27, 27, // Č
    28, // Dž
    29, 29 // Š
};

float widths[30] = {
    0.546875, // A
    0.421875, // B
    0.421875, // V
    0.328125, // G
    0.625, // D
    0.5625, // Đ
    0.34375, // E
    0.71875, // Ž
    0.390625, // Z
    0.484375, // I
    0.25, // J
    0.4375, // K
    0.515625, // L
    0.8125, // Lj
    0.6875, // M
    0.453125, // N
    0.75, // Nj
    0.5625, // O
    0.453125, // P
    0.40625, // R
    0.4375, // S
    0.453125, // T
    0.546875, // Ć
    0.5, // U
    0.609375, // F
    0.46875, // H
    0.53125, // C
    0.4375, // Č
    0.453125, // Dž
    0.703125, // Š
};

int chosenLetterCounter = 0;
int chosenLetters[12] = {
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1
};

float startTime = 0.0f;
float endTime = 0.0f;
float loadDictTime = 0.0f;

random_device rd;
mt19937 gen(rd());
uniform_int_distribution<> dis(0, sizeof(probabilities) / sizeof(probabilities[0]));

int random = 0;

GLFWcursor* cursorHover;
GLFWcursor* cursorOpen;
GLFWcursor* cursorPress;

int main(void)
{
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    wcout.imbue(locale("en_US.UTF-8"));
    thread loadDictTask([&]() {
            wifstream inputFile("res/txt/dict.txt");
            inputFile.imbue(std::locale("en_US.UTF-8"));

            if (!inputFile.is_open())
            {
                cerr << "Greska pri otvaranju fajla!" << endl;
                return 1;
            }

            wstring line;
            while (getline(inputFile, line))
            {
                allWords.push_back(line);
            }
            loadDictTime = glfwGetTime();
            cout << "Uspesno procitao recnik za " << loadDictTime << " sekundi!" << endl;
            loadDictThreadEnded = true;

            inputFile.close();
        });
    loadDictTask.detach();

    if (!glfwInit())
    {
        cout << "GLFW Biblioteka se nije ucitala! :(\n";
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

    const double targetFPS = 60.0;
    const double targetFrameTime = 1.0 / targetFPS;

    if (window == NULL)
    {
        cout << "Prozor nije napravljen! :(\n";
        glfwTerminate();
        return 2;
    }
    glfwMakeContextCurrent(window);

    if (glewInit() != GLEW_OK)
    {
        cout << "GLEW nije mogao da se ucita! :'(\n";
        return 3;
    }

    vertices = nullptr;
    int bufferSize;
    bufferSize = formArray();

    unsigned int VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    unsigned int VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, bufferSize * sizeof(float), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    colShader = createShader("col.vert", "col.frag");
    texShader = createShader("tex.vert", "tex.frag");

    field = loadImageToTexture("res/buttons/field.png");
    border = loadImageToTexture("res/box/border.png");
    row = loadImageToTexture("res/box/row.png");
    clear = loadImageToTexture("res/buttons/clear.png");
    backspace = loadImageToTexture("res/buttons/backspace.png");
    stop = loadImageToTexture("res/buttons/stop.png");
    lens = loadImageToTexture("res/buttons/lens.png");
    stopLens = loadImageToTexture("res/buttons/stop-lens.png");
    submitLens = loadImageToTexture("res/buttons/submit-lens.png");
    borderTimer = loadImageToTexture("res/box/border-timer.png");
    rowTimer = loadImageToTexture("res/box/row-timer.png");
    error = loadImageToTexture("res/buttons/error.png");
    submit = loadImageToTexture("res/buttons/submit.png");
    logo = loadImageToTexture("res/misc/logo.png");

    glBindTexture(GL_TEXTURE_2D, field);
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, border);
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, row);
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, backspace);
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, clear);
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, stop);
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, lens);
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, stopLens);
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, borderTimer);
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, rowTimer);
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, error);
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, submit);
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, submitLens);
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, logo);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    for (int i = 0; i < 30; i++) {
        letters[i] = loadImageToTexture(("res/letters/letter" + to_string(i + 1) + ".png").c_str());
        glBindTexture(GL_TEXTURE_2D, letters[i]);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    glBindTexture(GL_TEXTURE_2D, 0);
    

    glUseProgram(texShader);
    glUniform1i(glGetUniformLocation(texShader, "background"), 0);
    glUniform1i(glGetUniformLocation(texShader, "foreground"), 1);
    glUniform1i(glGetUniformLocation(texShader, "lens"), 2);

    cursorHover = loadImageToCursor("res/cursor/cursor-hover.png");
    cursorOpen = loadImageToCursor("res/cursor/cursor-open.png");
    cursorPress = loadImageToCursor("res/cursor/cursor-press.png");

    glfwSetKeyCallback(window, keyboardCallback);
    glfwSetMouseButtonCallback(window, mouseButtonCallback);
    while (!glfwWindowShouldClose(window))
    {

        double startTime = glfwGetTime();

        while ((glfwGetTime() - startTime) < targetFrameTime) {}
        /*double fps = 1.0 / (glfwGetTime() - startTime);
        cout << "FPS: " << fps << "\n";*/

        userInteraction();

        glBindVertexArray(VAO);

        draw();

        glBindTexture(GL_TEXTURE_2D, 0);
        glUseProgram(0);
        glBindVertexArray(0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
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

int formArray() {
    int vertexCount = 0;
    switch (game) {
    case 1: {      // slova    red 1   red 2   bs  clr typing  stop  timer  solution sbm,err logo
        vertexCount = 12 * 4 + 3 * 4 + 3 * 4 + 4 + 4 + 12 * 4 + 4 + 4 * 4 + 12 * 4 + 4 * 2 + 4;
        vertices = new float[vertexCount * 4] {
            };
        
        for (int i = 0; i < 12; i++) {
            formCell(i * 4, int(screenWidth / 2.0 - screenWidth / 15 * (6 - i)), screenHeight / 2, screenWidth / 15);
        }

        formBar(48, screenHeight / 2 + screenWidth / 15, screenWidth / 15);
        formBar(60, screenHeight / 2 + screenWidth / 15 * 2, screenWidth / 15);

        formCell(72, int(screenWidth / 2.0 - screenWidth / 15 * 7), screenHeight / 2 + screenWidth / 15, screenWidth / 15);
        formCell(76, int(screenWidth / 2.0 + screenWidth / 15 * 6), screenHeight / 2 + screenWidth / 15, screenWidth / 15);

        for (int i = 0; i < 12; i++) {
            formCell(80 + i * 4, int((screenWidth - screenWidth / 15) / 2.0), screenHeight / 2 + screenWidth / 15, screenWidth / 15);
        }

        formCell(128, int(screenWidth / 2.0) - screenWidth / 15, screenHeight / 2 + screenWidth / 15, 2 * screenWidth / 15);
        formTimer(132);

        for (int i = 0; i < 12; i++) {
            formCell(148 + i * 4, int((screenWidth - screenWidth / 15) / 2.0), screenHeight / 2 + screenWidth / 15 * 2, screenWidth / 15);
        }

        formCell(196, int(screenWidth / 2.0 - screenWidth / 15 * 1.5), screenHeight / 2 + screenWidth / 15 * 2, 3 * screenWidth / 15);
        formCell(200, int((screenWidth - screenWidth / 15) / 2.0), screenHeight / 2 + screenWidth / 15 * 2, screenWidth / 15);

        formLogo(204);

        glClearColor(0.05, 0.1, 0.4, 1.0);
        break;
    }
    case 2: {

        break;
    }
    }

    return vertexCount * 4;
}

void userInteraction() {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(window, GL_TRUE);

    switch (game) {
    case 1:
    {
        updateCursor();
        break;
    }
    case 2: {
        break;
    }
    }
}

void draw() {
    glClear(GL_COLOR_BUFFER_BIT);
    switch (game) {
    case 1:
    {
        glUseProgram(texShader);
        glUniform1f(glGetUniformLocation(texShader, "uX"), 0);
        glUniform1f(glGetUniformLocation(texShader, "kX"), 0);

        glActiveTexture(GL_TEXTURE0);


        glBindTexture(GL_TEXTURE_2D, logo);
        glDrawArrays(GL_TRIANGLE_FAN, 204, 4);

        glBindTexture(GL_TEXTURE_2D, border);
        glDrawArrays(GL_TRIANGLE_FAN, 4 * 12, 4);
        glDrawArrays(GL_TRIANGLE_FAN, 4 * 12 + 4, 4);
        glDrawArrays(GL_TRIANGLE_FAN, 4 * 12 + 12, 4);
        glDrawArrays(GL_TRIANGLE_FAN, 4 * 12 + 16, 4);

        glBindTexture(GL_TEXTURE_2D, row);
        glDrawArrays(GL_TRIANGLE_FAN, 4 * 12 + 8, 4);
        glDrawArrays(GL_TRIANGLE_FAN, 4 * 12 + 20, 4);

        glBindTexture(GL_TEXTURE_2D, field);
        glActiveTexture(GL_TEXTURE1);


        for (int i = 0; i < generatedLetterCounter; i++) {
            glBindTexture(GL_TEXTURE_2D, letters[generatedLetters[i]]);
            bool found = letterPressed == i;
            if (!found)
                for (int j=0; j<chosenLetterCounter; j++) {
                    if (chosenLetters[j] == i) {
                        found = true;
                        break;
                    }
                }
            if (!found) glDrawArrays(GL_TRIANGLE_FAN, 4 * i, 4);
            else {
                drawWithLens(i * 4, lens);
                glActiveTexture(GL_TEXTURE1);
            }
        }

        if (generatedLetterCounter < 12) {
            
            random = dis(gen);
            glBindTexture(GL_TEXTURE_2D, letters[probabilities[random]]);
            glDrawArrays(GL_TRIANGLE_FAN, 4 * generatedLetterCounter, 4);
            glBindTexture(GL_TEXTURE_2D, 0);
            glActiveTexture(GL_TEXTURE0);

            glBindTexture(GL_TEXTURE_2D, stop);
            if (!stopPressed) glDrawArrays(GL_TRIANGLE_FAN, 128, 4);
            else {
                drawWithLens(128, stopLens);
                glActiveTexture(GL_TEXTURE0);
            }
        } // Sta moze u teoriji da se desi? L1 L2 L3?
        else {

            if (!findSolutionThreadStarted && loadDictThreadEnded && solutionLength == -1) {
                findSolutionThreadStarted = true;
                thread findBestSolutionsSerialTask([&]() {
                    solutionLength = findBestSolutionsSerial(getGeneratedLetters(), allWords, solutions);
                    cout << "Uspesno nasao resenja za " << glfwGetTime() - loadDictTime << " sekundi!" << endl;
                    });
                findBestSolutionsSerialTask.detach();
            }

            glBindTexture(GL_TEXTURE_2D, 0);
            glActiveTexture(GL_TEXTURE0);
        }

        glBindTexture(GL_TEXTURE_2D, borderTimer);
        glDrawArrays(GL_TRIANGLE_FAN, 132, 4);
        glDrawArrays(GL_TRIANGLE_FAN, 136, 4);
        glBindTexture(GL_TEXTURE_2D, rowTimer);
        glDrawArrays(GL_TRIANGLE_FAN, 140, 4);


        if (chosenLetterCounter > 0) {

            if (!gameEnded) {
                glBindTexture(GL_TEXTURE_2D, clear);
                if (!clearPressed) glDrawArrays(GL_TRIANGLE_FAN, 72, 4);
                else {
                    drawWithLens(72, lens);
                    glActiveTexture(GL_TEXTURE0);
                }

                glBindTexture(GL_TEXTURE_2D, backspace);
                if (!backspacePressed) glDrawArrays(GL_TRIANGLE_FAN, 76, 4);
                else {
                    drawWithLens(76, lens);
                    glActiveTexture(GL_TEXTURE0);
                }

                if (loadDictThreadEnded) {
                    if (!isCurrentWordInvalid) {
                        {
                            glBindTexture(GL_TEXTURE_2D, submit);
                            if (!submitPressed) glDrawArrays(GL_TRIANGLE_FAN, 196, 4);
                            else {
                                drawWithLens(196, submitLens);
                                glActiveTexture(GL_TEXTURE0);
                            }
                        }
                    }
                    else {
                        glBindTexture(GL_TEXTURE_2D, error);
                        glDrawArrays(GL_TRIANGLE_FAN, 200, 4);
                    }
                }
            }
            

            float textWidth = 0.0f;

            for (int i = 0; i < chosenLetterCounter; i++) {
                textWidth += widths[generatedLetters[chosenLetters[i]]];
            }

            textWidth *= screenWidth / 15;
            textWidth += padding / 2.0 * (chosenLetterCounter - 1);
            float prevTextWidth = 0.0f;

            for (int i = 0; i < chosenLetterCounter; i++) {
                int letter = generatedLetters[chosenLetters[i]];
                int xMiddle = int((screenWidth - textWidth + widths[letter] * screenWidth / 15 + padding * i) / 2.0 + prevTextWidth * screenWidth / 15);
                prevTextWidth += widths[letter];

                glBindTexture(GL_TEXTURE_2D, letters[letter]);
                glUniform1f(glGetUniformLocation(texShader, "uX"), convertX(xMiddle));
                glUniform1f(glGetUniformLocation(texShader, "kX"), widths[letter]);
                glDrawArrays(GL_TRIANGLE_FAN, 80 + i * 4, 4);
            }
        }

        if (!gameEnded) {
            float red = 0.0f;
            glUseProgram(colShader);
            if (generatedLetterCounter == 12) {
                red = (glfwGetTime() - startTime) / roundTime;
                if (red > 0.975) gameEnded = true;
            }
            float kY = 1.0f - red;

            glUniform1f(glGetUniformLocation(colShader, "red"), red);
            glUniform1f(glGetUniformLocation(colShader, "kY"), kY);
            glUniform1f(glGetUniformLocation(colShader, "minY"), convertY(int(screenHeight / 2.0 - 3 * padding)));
            glDrawArrays(GL_TRIANGLE_FAN, 144, 4);
        }
        else {
            if (endTime == 0.0f) endTime = glfwGetTime();
            if (solutionLength == 0) break;

            wstring solution = solutions[int(glfwGetTime() - endTime) % solutions.size()];

            float textWidth = 0.0f;

            for (int i = 0; i < solutionLength; i++) {
                textWidth += widths[convertLetter(solution[i])];
            }

            textWidth *= screenWidth / 15;
            textWidth += padding / 2.0 * (solutionLength - 1);
            float prevTextWidth = 0.0f;

            for (int i = 0; i < solutionLength; i++) {
                int letter = convertLetter(solution[i]);
                int xMiddle = int((screenWidth - textWidth + widths[letter] * screenWidth / 15 + padding * i) / 2.0 + prevTextWidth * screenWidth / 15);
                prevTextWidth += widths[letter];

                glBindTexture(GL_TEXTURE_2D, letters[letter]);
                glUniform1f(glGetUniformLocation(texShader, "uX"), convertX(xMiddle));
                glUniform1f(glGetUniformLocation(texShader, "kX"), widths[letter]);
                glDrawArrays(GL_TRIANGLE_FAN, 148 + i * 4, 4);
            }
        }
        

        break;
    }
    case 2: {
        break;
    }
    }
}


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
            vertices[i * 4] = convertX(int(screenWidth / 2.0 - screenWidth / 15 * (6 - x / 4) + padding));
            if (i < start + 4) vertices[i * 4 + 2] = 0.0;
            else vertices[i * 4 + 2] = 1.0;
        }
        else {
            vertices[i * 4] = convertX(int(screenWidth / 2.0 - screenWidth / 15 * (5 - x / 4) - padding));
            if (i < start + 4) vertices[i * 4 + 2] = 1.0;
            else vertices[i * 4 + 2] = 0.0;
        }

        if (i % 4 < 2) {
            vertices[i * 4 + 1] = convertY(y + height - padding);
            vertices[i * 4 + 3] = 0.0;
        }
        else {
            vertices[i * 4 + 1] = convertY(y + padding);
            vertices[i * 4 + 3] = 1.0;
        }
    }

    for (int i = start + 8; i < start + 12; i++) {
        if ((i + 1) % 4 < 2) {
            vertices[i * 4] = convertX(int(screenWidth / 2.0 - screenWidth / 15 * 5 - padding));
            vertices[i * 4 + 2] = 0.0;
        }
        else {
            vertices[i * 4] = convertX(int(screenWidth / 2.0 + screenWidth / 15 * 5 + padding));
            vertices[i * 4 + 2] = 1.0;
        }

        if (i % 4 < 2) {
            vertices[i * 4 + 1] = convertY(y + height - padding);
            vertices[i * 4 + 3] = 0.0;
        }
        else {
            vertices[i * 4 + 1] = convertY(y + padding);
            vertices[i * 4 + 3] = 1.0;
        }
    }
}

void formCell(int start, int x, int y, int width) {
    for (int i = start; i < start + 4; i++) {
        if ((i + 1) % 4 < 2) {
            vertices[i * 4] = convertX(x + padding);
            vertices[i * 4 + 2] = 0.0;
        }
        else {
            vertices[i * 4] = convertX(x + width - padding);
            vertices[i * 4 + 2] = 1.0;
        }

        if (i % 4 < 2) {
            vertices[i * 4 + 1] = convertY(y + screenWidth / 15 - padding);
            vertices[i * 4 + 3] = 0.0;
        }
        else {
            vertices[i * 4 + 1] = convertY(y + padding);
            vertices[i * 4 + 3] = 1.0;
        }
    }
}

void formTimer(int start) {

    for (int i = start; i < start + 8; i++) {
        if ((i + 1) % 4 < 2) {
            vertices[i * 4] = convertX(int(screenWidth - screenWidth / 15 / 2) / 2 + padding);
            vertices[i * 4 + 2] = 0.0;
        }
        else {
            vertices[i * 4] = convertX(int(screenWidth + screenWidth / 15 / 2) / 2 - padding);
            vertices[i * 4 + 2] = 1.0;
        }

        if (i % 4 < 2) {
            if (i < start + 4) {
                vertices[i * 4 + 1] = convertY(int(screenWidth / 15 / 2.0 - padding));
                vertices[i * 4 + 3] = 0.0;
            }
            else {
                vertices[i * 4 + 1] = convertY(int(screenHeight / 2.0 - padding));
                vertices[i * 4 + 3] = 1.0;
            }
        }
        else {
            if (i < start + 4) {
                vertices[i * 4 + 1] = convertY(padding);
                vertices[i * 4 + 3] = 1.0;
            }
            else {
                vertices[i * 4 + 1] = convertY(int((screenHeight - screenWidth / 15) / 2.0 + padding));
                vertices[i * 4 + 3] = 0.0;
            }
        }
    }

    for (int i = start + 8; i < start + 12; i++) {
        if ((i + 1) % 4 < 2) {
            vertices[i * 4] = convertX(int(screenWidth - screenWidth / 15 / 2) / 2 + padding);
            vertices[i * 4 + 2] = 0.0;
        }
        else {
            vertices[i * 4] = convertX(int(screenWidth + screenWidth / 15 / 2) / 2 - padding);
            vertices[i * 4 + 2] = 1.0;
        }

        if (i % 4 < 2) {
            vertices[i * 4 + 1] = convertY(int((screenHeight - screenWidth / 15) / 2.0 + padding));
            vertices[i * 4 + 3] = 0.0;
        }
        else {
            vertices[i * 4 + 1] = convertY(int(screenWidth / 15 / 2.0 - padding));
            vertices[i * 4 + 3] = 1.0;
        }
    }

    for (int i = start + 12; i < start + 16; i++) {
        if ((i + 1) % 4 < 2) {
            vertices[i * 4] = convertX(int(screenWidth - screenWidth / 15 / 2) / 2 + padding * 2);
        }
        else {
            vertices[i * 4] = convertX(int(screenWidth + screenWidth / 15 / 2) / 2 - padding * 2);
        }

        if (i % 4 < 2) {
            vertices[i * 4 + 1] = convertY(int(screenHeight / 2.0 - 3 * padding));
        }
        else {
            vertices[i * 4 + 1] = convertY(3 * padding);
        }
        vertices[i * 4 + 2] = 0.0;
        vertices[i * 4 + 3] = 1.0;
    }

}

void formLogo(int start) {
    for (int i = start; i < start + 4; i++) {
        if ((i + 1) % 4 < 2) {
            vertices[i * 4] = convertX(int(screenWidth - 1.5 * 1.37 * screenWidth / 15 - 6 * padding));
            vertices[i * 4 + 2] = 0.0;
        }
        else {
            vertices[i * 4] = convertX(screenWidth - 3 * padding);
            vertices[i * 4 + 2] = 1.0;
        }

        if (i % 4 < 2) {
            vertices[i * 4 + 1] = convertY(1.5 * screenWidth / 15 + 6 * padding);
            vertices[i * 4 + 3] = 0.0;
        }
        else {
            vertices[i * 4 + 1] = convertY(3 * padding);
            vertices[i * 4 + 3] = 1.0;
        }
    }
}

void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (gameEnded) return;
    if (generatedLetterCounter < 12) {
        if (key == GLFW_KEY_ENTER) {
            if (action == GLFW_PRESS) stopPressed = true;
            else if (stopPressed && action == GLFW_RELEASE) actionStop();
        }
    }

    else {
        if (key == GLFW_KEY_DELETE && chosenLetterCounter > 0) {
            if (action == GLFW_PRESS) clearPressed = true;
            else if (clearPressed && action == GLFW_RELEASE) actionClear();
        }
        else if (key == GLFW_KEY_BACKSPACE && chosenLetterCounter > 0) {
            if (action == GLFW_PRESS) backspacePressed = true;
            else if (backspacePressed && action == GLFW_RELEASE) actionBackspace();
        }
        else if (key == GLFW_KEY_ENTER && chosenLetterCounter > 0 && !isCurrentWordInvalid && loadDictThreadEnded) {
            if (action == GLFW_PRESS) submitPressed = true;
            else if (submitPressed && action == GLFW_RELEASE) actionSubmit();
        }
        else {
            for (int i = 0; i < 12; i++) {
                if (key == letterKeys[i]) {
                    if (isSelected(i)) break;
                    if (action == GLFW_PRESS) {
                        letterPressed = i;
                        actionChooseLetter(i);
                    }
                }
            }
        }
    }

    if (action == GLFW_RELEASE) {
        stopPressed = false;
        clearPressed = false;
        backspacePressed = false;
        submitPressed = false;
        letterPressed = -1;
    }
}

void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    if (gameEnded) return;
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        
        if (generatedLetterCounter == 12) {
            if (chosenLetterCounter > 0 &&
                isInSquare(screenWidth / 2.0 - screenWidth / 15 * 7 + padding, screenHeight / 2 + screenWidth / 15 + padding, screenWidth / 15 - 2 * padding, screenWidth / 15 - 2 * padding)
                ) {
                if (action == GLFW_PRESS) {
                    clearPressed = true;
                }
                else if (clearPressed && action == GLFW_RELEASE) {
                    actionClear();
                }
            }
            else if (chosenLetterCounter > 0 &&
                isInSquare(screenWidth / 2.0 + screenWidth / 15 * 6 + padding, screenHeight / 2 + screenWidth / 15 + padding, screenWidth / 15 - 2 * padding, screenWidth / 15 - 2 * padding)
                ) {
                if (action == GLFW_PRESS) {
                    backspacePressed = true;
                }
                else if (backspacePressed && action == GLFW_RELEASE) {
                    actionBackspace();
                }
            }
            else if (!isCurrentWordInvalid && chosenLetterCounter > 0 && loadDictThreadEnded &&
                isInSquare(screenWidth / 2.0 - screenWidth / 15 * 1.5 + padding, screenHeight / 2 + screenWidth / 15 * 2 + padding * 2, 3 * screenWidth / 15 - 2 * padding, screenWidth / 15 - 4 * padding)
                ) {
                if (action == GLFW_PRESS) {
                    submitPressed = true;
                }
                else if (submitPressed && action == GLFW_RELEASE) {
                    actionSubmit();
                }
            }
            else {
                for (int i = 0; i < 12; i++) {
                    if (
                        isInSquare(screenWidth / 2.0 - screenWidth / 15 * (6 - i) + padding, screenHeight / 2 + padding, screenWidth / 15 - 2 * padding, screenWidth / 15 - 2 * padding)
                        ) {
                        if (isSelected(i)) break;
                        if (action == GLFW_PRESS) {
                            letterPressed = i;
                        }
                        else if (letterPressed == i && action == GLFW_RELEASE) {
                            actionChooseLetter(i);
                        }
                        break;
                    }
                }
            }
        }
        else {
            if (
                isInSquare(screenWidth / 2.0 - screenWidth / 15 + padding, screenHeight / 2 + screenWidth / 15 + padding * 2, 2 * (screenWidth / 15 - padding), screenWidth / 15 - 4 * padding)
                ) {
                if (action == GLFW_PRESS) {
                    stopPressed = true;
                }
                else if (stopPressed && action == GLFW_RELEASE) {
                    actionStop();
                }
            }
        }

        if (action == GLFW_RELEASE) {
            stopPressed = false;
            backspacePressed = false;
            clearPressed = false;
            submitPressed = false;
            letterPressed = -1;
        }
    }
}

void actionBackspace() {
    chosenLetters[--chosenLetterCounter] = -1;
    if (chosenLetterCounter == 0) isCurrentWordInvalid = false;
    else updateError();
}

void actionClear() {
    chosenLetterCounter = 0;
    for (int i = 0; i < 12; i++) {
        chosenLetters[i] = -1;
    }
    isCurrentWordInvalid = false;
}

void actionSubmit() {
    wstring word = getCurrentWord();
    if (!findMatchSerial(word, allWords)) {
        invalidWords.push_back(word);
        isCurrentWordInvalid = true;
    }
    else gameEnded = true;
}

bool isSelected(int index) {
    for (int i = 0; i < 12; i++) {
        if (chosenLetters[i] == index) {
            return true;
        }
    }
    return false;
}

void actionChooseLetter(int index) {
    chosenLetters[chosenLetterCounter++] = index;
    updateError();
}

void actionStop() {
    if (generatedLetterCounter >= 12) return;
    if (generatedLetterCounter == 11) startTime = glfwGetTime();
    generatedLetters[generatedLetterCounter++] = probabilities[random];
}

void drawWithLens(int start, unsigned texture) {
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, texture);
    glDrawArrays(GL_TRIANGLE_FAN, start, 4);
    glBindTexture(GL_TEXTURE_2D, 0);
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

void updateCursor() {
    if (gameEnded)
    {
        glfwSetCursor(window, cursorOpen);
        return;
    }

    bool onButton = false;
    bool onBackspace = false;
    bool onClear = false;
    bool onStop = false;
    bool onSubmit = false;
    int onLetter = -1;
    if (generatedLetterCounter == 12) {
        if (chosenLetterCounter > 0) {
            onClear = isInSquare(screenWidth / 2.0 - screenWidth / 15 * 7 + padding, screenHeight / 2 + screenWidth / 15 + padding, screenWidth / 15 - 2 * padding, screenWidth / 15 - 2 * padding);
            if (!onClear) onBackspace = isInSquare(screenWidth / 2.0 + screenWidth / 15 * 6 + padding, screenHeight / 2 + screenWidth / 15 + padding, screenWidth / 15 - 2 * padding, screenWidth / 15 - 2 * padding);
            if (!onBackspace && !onClear) onSubmit = !isCurrentWordInvalid && loadDictThreadEnded && isInSquare(screenWidth / 2.0 - screenWidth / 15 * 1.5 + padding, screenHeight / 2 + screenWidth / 15 * 2 + padding * 2, 3 * screenWidth / 15 - 2 * padding, screenWidth / 15 - 4 * padding);
            onButton = onClear || onBackspace || onSubmit;
        }
        if (!onButton) {
            for (int i = 0; i < 12; i++) {
                if (isInSquare(screenWidth / 2.0 - screenWidth / 15 * (6 - i) + padding, screenHeight / 2 + padding, screenWidth / 15 - 2 * padding, screenWidth / 15 - 2 * padding)) {
                    onButton = true;
                    onLetter = i;
                    break;
                }
            }
        }
    }
    else {
        onStop = isInSquare(screenWidth / 2.0 - screenWidth / 15 + padding, screenHeight / 2 + screenWidth / 15 + padding * 2, 2 * (screenWidth / 15 - padding), screenWidth / 15 - 4 * padding);;
        onButton = onStop;
    }
    if (onButton) {
        if ((stopPressed && onStop) || (clearPressed && onClear) || (backspacePressed && onBackspace) || (submitPressed && onSubmit) || (onLetter != -1 && letterPressed == onLetter)) {
            glfwSetCursor(window, cursorPress);
            
        }
        else if (onStop || onClear || onBackspace || onSubmit || (onLetter != -1)) {
            glfwSetCursor(window, cursorHover);
        }
    }
    else glfwSetCursor(window, cursorOpen);
}

void updateError() {
    wstring currentWord = getCurrentWord();
    for (wstring word : invalidWords) {
        if (word == currentWord) {
            isCurrentWordInvalid = true;
            return;
        }
    }
    isCurrentWordInvalid = false;
}

wstring getCurrentWord() {
    wstring word = L"";
    for (int i = 0; i < chosenLetterCounter; i++) {
        word += alphabet[generatedLetters[chosenLetters[i]]];
    }
    return word;
}

wstring getGeneratedLetters() {
    wstring word = L"";
    for (int i = 0; i < 12; i++) {
        word += alphabet[generatedLetters[i]];
    }
    return word;
}

int convertLetter(wchar_t letter) {
    for (int i = 0; i < 30; i++) {
        if (letter == alphabet[i]) return i;
    }
    return -1;
}