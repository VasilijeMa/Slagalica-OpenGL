#include "LettersGame.h"
#include "View.h"
#include <string>
#include <vector>
#include <algorithm>
#include <random>
#include <thread>
#include <iostream>
#include <fstream>
#include <sstream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

namespace LettersGame {

    const std::wstring alphabet = L"абвгдђежзијклљмнњопрстћуфхцчџш";

    std::vector<int> generatedLetters;

    std::vector<int> chosenLetters;

    bool stopPressed = false;
    bool backspacePressed = false;
    bool clearPressed = false;
    bool submitPressed = false;
    int letterPressed = -1;
    bool gameEnded = false;
    bool findSolutionThreadStarted = false;
    bool loadDictThreadEnded = false;
    bool isCurrentWordInvalid = false;

    std::vector<std::wstring> invalidWords;
    std::vector<std::wstring> allWords;

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

    float startTimer = 0.0f;
    float endTime = 0.0f;
    float loadDictTime = 0.0f;
    int solutionLength = -1;
    std::vector<std::wstring> solutions;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, sizeof(probabilities) / sizeof(probabilities[0]));
    int random;

    bool isInvalidWordPresent();
    bool isSubset(std::wstring subset, std::wstring set);
    bool findMatchSerial(std::wstring userWord, const std::vector<std::wstring>& allWords);
    int findBestSolutionsSerial(std::wstring randomWord, const std::vector<std::wstring>& allWords, std::vector<std::wstring>& solutions);

    int convertLetter(wchar_t letter);

    void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);

    std::wstring getCurrentWord();
    std::wstring getGeneratedLetters();

    void updateCursor(GLFWwindow* window, GLFWcursor* cursorHover, GLFWcursor* cursorOpen, GLFWcursor* cursorPress);
    void updateError();

    bool isSelected(int index);
    void actionBackspace();
    void actionClear();
    void actionChooseLetter(int index);
    void actionStop();
    void actionSubmit();

    void run(int roundTime);

    bool isInvalidWordPresent() {
        std::wstring currentWord = getCurrentWord();
        for (std::wstring word : invalidWords) {
            if (word == currentWord) {
                return true;
            }
        }
        return false;
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

    int convertLetter(wchar_t letter) {
        for (int i = 0; i < 30; i++) {
            if (letter == alphabet[i]) return i;
        }
        return -1;
    }

    void actionBackspace() {
        chosenLetters.pop_back();
        if (chosenLetters.empty()) isCurrentWordInvalid = false;
        else updateError();
    }

    void actionClear() {
        chosenLetters.clear();
        isCurrentWordInvalid = false;
    }

    void actionSubmit() {
        if (isInvalidWordPresent()) {
            isCurrentWordInvalid = true;
            return;
        }
        std::wstring word = getCurrentWord();
        if (!findMatchSerial(word, allWords)) {
            invalidWords.push_back(word);
            isCurrentWordInvalid = true;
        }
        else gameEnded = true;
    }

    bool isSelected(int index) {
        for (int i = 0; i < chosenLetters.size(); i++) {
            if (chosenLetters[i] == index) {
                return true;
            }
        }
        return false;
    }

    void actionChooseLetter(int index) {
        chosenLetters.push_back(index);
        updateError();
    }

    void actionStop() {
        if (generatedLetters.size() == 12) return;
        generatedLetters.push_back(probabilities[random]);
    }

    std::wstring getCurrentWord() {
        std::wstring word = L"";
        for (int i = 0; i < chosenLetters.size(); i++) {
            word += alphabet[generatedLetters[chosenLetters[i]]];
        }
        return word;
    }

    std::wstring getGeneratedLetters() {
        std::wstring word = L"";
        for (int i = 0; i < 12; i++) {
            word += alphabet[generatedLetters[i]];
        }
        return word;
    }

    void updateCursor(GLFWwindow* window, GLFWcursor* cursorHover, GLFWcursor* cursorOpen, GLFWcursor* cursorPress) {
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
        if (generatedLetters.size() == 12) {
            if (chosenLetters.size() > 0) {
                onClear = isOnClear();
                if (!onClear) onBackspace = isOnBackspace();
                if (!onBackspace && !onClear) onSubmit = !isCurrentWordInvalid && loadDictThreadEnded && isOnSubmit();
                onButton = onClear || onBackspace || onSubmit;
            }
            if (!onButton) {
                for (int i = 0; i < 12; i++) {
                    if (isOnLetter(i)) {
                        onButton = true;
                        onLetter = i;
                        break;
                    }
                }
            }
        }
        else {
            onStop = isOnStop();
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
        if (isInvalidWordPresent()) {
            isCurrentWordInvalid = true;
            return;
        }
        isCurrentWordInvalid = false;
    }

    void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
    {
        if (gameEnded) return;
        if (generatedLetters.size() < 12) {
            if (key == GLFW_KEY_ENTER) {
                if (action == GLFW_PRESS) stopPressed = true;
                else if (stopPressed && action == GLFW_RELEASE) actionStop();
            }
        }

        else {
            if (key == GLFW_KEY_DELETE && chosenLetters.size() > 0) {
                if (action == GLFW_PRESS) clearPressed = true;
                else if (clearPressed && action == GLFW_RELEASE) actionClear();
            }
            else if (key == GLFW_KEY_BACKSPACE && chosenLetters.size() > 0) {
                if (action == GLFW_PRESS) backspacePressed = true;
                else if (backspacePressed && action == GLFW_RELEASE) actionBackspace();
            }
            else if (key == GLFW_KEY_ENTER && chosenLetters.size() > 0 && !isCurrentWordInvalid && loadDictThreadEnded) {
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

            if (generatedLetters.size() == 12) {
                if (chosenLetters.size() > 0 && isOnClear()) {
                    if (action == GLFW_PRESS) {
                        clearPressed = true;
                    }
                    else if (clearPressed && action == GLFW_RELEASE) {
                        actionClear();
                    }
                }
                else if (chosenLetters.size() > 0 && isOnBackspace()) {
                    if (action == GLFW_PRESS) {
                        backspacePressed = true;
                    }
                    else if (backspacePressed && action == GLFW_RELEASE) {
                        actionBackspace();
                    }
                }
                else if (!isCurrentWordInvalid && chosenLetters.size() > 0 && loadDictThreadEnded && isOnSubmit()) {
                    if (action == GLFW_PRESS) {
                        submitPressed = true;
                    }
                    else if (submitPressed && action == GLFW_RELEASE) {
                        actionSubmit();
                    }
                }
                else {
                    for (int i = 0; i < 12; i++) {
                        if (isOnLetter(i)) {
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
                if (isOnStop()) {
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

    void run(int roundTime) {
        double start = glfwGetTime();
        while ((glfwGetTime() - start) < fps) {}

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(window, GL_TRUE);
        updateCursor(window, cursorHover, cursorOpen, cursorPress);

        if (generatedLetters.size() == 11 && startTimer == 0.0) startTimer = glfwGetTime();

        glClear(GL_COLOR_BUFFER_BIT);
        glUseProgram(texShader);
        glUniform1f(glGetUniformLocation(texShader, "uX"), 0);

        drawUniversalElements();

        for (int i = 0; i < generatedLetters.size(); i++) {
            glBindTexture(GL_TEXTURE_2D, letters[generatedLetters[i]]);
            bool found = letterPressed == i || isSelected(i);
            if (!found) glDrawArrays(GL_TRIANGLE_FAN, 76 + 4 * i, 4);
            else {
                drawWithLens(76 + i * 4, lens);
                glActiveTexture(GL_TEXTURE1);
            }
        }

        if (generatedLetters.size() < 12) {

            random = dis(gen);
            glBindTexture(GL_TEXTURE_2D, letters[probabilities[random]]);
            glDrawArrays(GL_TRIANGLE_FAN, 76 + 4 * generatedLetters.size(), 4);
            glBindTexture(GL_TEXTURE_2D, 0);
            glActiveTexture(GL_TEXTURE0);

            glBindTexture(GL_TEXTURE_2D, stop);
            if (!stopPressed) glDrawArrays(GL_TRIANGLE_FAN, 56, 4);
            else {
                drawWithLens(56, stopLens);
                glActiveTexture(GL_TEXTURE0);
            }
        }
        else {

            if (!findSolutionThreadStarted && loadDictThreadEnded && solutionLength == -1) {
                findSolutionThreadStarted = true;
                std::thread findBestSolutionsSerialTask([&]() {
                    solutionLength = findBestSolutionsSerial(getGeneratedLetters(), allWords, solutions);
                    std::cout << "Uspesno nasao resenja za " << glfwGetTime() - loadDictTime << " sekundi!" << std::endl;
                    });
                findBestSolutionsSerialTask.detach();
            }

            glBindTexture(GL_TEXTURE_2D, 0);
            glActiveTexture(GL_TEXTURE0);
        }

        if (chosenLetters.size() > 0) {

            if (!gameEnded) {
                glBindTexture(GL_TEXTURE_2D, clear);
                if (!clearPressed) glDrawArrays(GL_TRIANGLE_FAN, 60, 4);
                else {
                    drawWithLens(60, lens);
                    glActiveTexture(GL_TEXTURE0);
                }

                glBindTexture(GL_TEXTURE_2D, backspace);
                if (!backspacePressed) glDrawArrays(GL_TRIANGLE_FAN, 64, 4);
                else {
                    drawWithLens(64, lens);
                    glActiveTexture(GL_TEXTURE0);
                }

                if (loadDictThreadEnded) {
                    if (!isCurrentWordInvalid) {
                        {
                            glBindTexture(GL_TEXTURE_2D, submit);
                            if (!submitPressed) glDrawArrays(GL_TRIANGLE_FAN, 68, 4);
                            else {
                                drawWithLens(68, submitLens);
                                glActiveTexture(GL_TEXTURE0);
                            }
                        }
                    }
                    else {
                        glBindTexture(GL_TEXTURE_2D, error);
                        glDrawArrays(GL_TRIANGLE_FAN, 72, 4);
                    }
                }
            }

            float textWidth = 0.0f;

            for (int i = 0; i < chosenLetters.size(); i++) {
                textWidth += widths[generatedLetters[chosenLetters[i]]];
            }

            textWidth *= screenWidth / 15.0;
            textWidth += PADDING / 2.0 * (chosenLetters.size() - 1);
            float prevTextWidth = 0.0f;

            for (int i = 0; i < chosenLetters.size(); i++) {
                int letter = generatedLetters[chosenLetters[i]];
                int xMiddle = int((screenWidth * (1 + (widths[letter] + 2 * prevTextWidth) / 15.0) - textWidth + PADDING * i) / 2.0);
                prevTextWidth += widths[letter];

                glBindTexture(GL_TEXTURE_2D, letters[letter]);
                glUniform1f(glGetUniformLocation(texShader, "uX"), convertX(xMiddle));
                glDrawArrays(GL_TRIANGLE_FAN, 124 + i * 4, 4);
            }
        }

        if (!gameEnded) {
            float red = 0.0f;
            glUseProgram(colShader);
            if (generatedLetters.size() == 12) {
                red = (glfwGetTime() - startTimer) / roundTime;
                if (red > 0.975) gameEnded = true;
            }
            float kY = 1.0f - red;

            glUniform1f(glGetUniformLocation(colShader, "red"), red);
            glUniform1f(glGetUniformLocation(colShader, "kY"), kY);
            glUniform1f(glGetUniformLocation(colShader, "minY"), convertY(int(screenHeight / 2.0) - 3 * PADDING));
            glDrawArrays(GL_TRIANGLE_FAN, 16, 4);
        }
        else {
            if (endTime == 0.0f) endTime = glfwGetTime();
            if (solutionLength != 0) {
                std::wstring solution = solutions[int(glfwGetTime() - endTime) % solutions.size()];

                float textWidth = 0.0f;

                for (int i = 0; i < solutionLength; i++) {
                    textWidth += widths[convertLetter(solution[i])];
                }

                textWidth *= screenWidth / 15.0;
                textWidth += PADDING / 2.0 * (solutionLength - 1);
                float prevTextWidth = 0.0f;

                for (int i = 0; i < solutionLength; i++) {
                    int letter = convertLetter(solution[i]);
                    int xMiddle = int((screenWidth * (1 + (widths[letter] + 2 * prevTextWidth) / 15.0) - textWidth + PADDING * i) / 2.0);
                    prevTextWidth += widths[letter];

                    glBindTexture(GL_TEXTURE_2D, letters[letter]);
                    glUniform1f(glGetUniformLocation(texShader, "uX"), convertX(xMiddle));
                    glDrawArrays(GL_TRIANGLE_FAN, 216 + i * 4, 4);
                }
            }
        }

        glBindTexture(GL_TEXTURE_2D, 0);
        glUseProgram(0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}

using namespace LettersGame;

void playLettersGame(int roundTime) {

    std::thread loadDictTask([&]() {
        std::wifstream inputFile("res/txt/dict.txt");
        inputFile.imbue(std::locale("en_US.UTF-8"));

        if (!inputFile.is_open())
        {
            std::cerr << "Greska pri otvaranju fajla!" << std::endl;
            return 1;
        }

        std::wstring line;
        while (std::getline(inputFile, line))
        {
            allWords.push_back(line);
        }
        loadDictTime = glfwGetTime();
        std::cout << "Uspesno procitao recnik za " << loadDictTime << " sekundi!" << std::endl;
        loadDictThreadEnded = true;

        inputFile.close();
        });
    loadDictTask.detach();

    glBindTexture(GL_TEXTURE_2D, 0);

    glfwSetKeyCallback(window, keyboardCallback);
    glfwSetMouseButtonCallback(window, mouseButtonCallback);

    while (!glfwWindowShouldClose(window))
    {
        run(roundTime);
        if (endTime > 0.0 && glfwGetTime() - endTime > solutions.size() * 2) break;
    }
    std::cout << solutions.size() << std::endl;
}