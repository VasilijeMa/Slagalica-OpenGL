#include "NumbersGame.h"
#include "View.h"
#include <string>
#include <vector>
#include <algorithm>
#include <iostream>
#include <random>
#include <thread>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

namespace NumbersGame {

    std::vector<int> targetNumber;
    std::vector<int> generatedNumbers;
    float startTimer = 0.0f;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> disDigit(0, 9);
    std::uniform_int_distribution<> disSmall(1, 9);
    std::uniform_int_distribution<> disMedium(0, 2);
    std::uniform_int_distribution<> disLarge(0, 3);
    int random;

    bool stopPressed = false;
    bool backspacePressed = false;
    bool clearPressed = false;
    bool submitPressed = false;
    int numberPressed = -1;
    bool gameEnded = false;
    bool isCurrentExpressionInvalid = false;

    void run(int roundTime);

    void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);

    void updateCursor(GLFWwindow* window, GLFWcursor* cursorHover, GLFWcursor* cursorOpen, GLFWcursor* cursorPress);

    void actionStop();

    void updateCursor(GLFWwindow* window, GLFWcursor* cursorHover, GLFWcursor* cursorOpen, GLFWcursor* cursorPress) {
        if (gameEnded)
        {
            glfwSetCursor(window, cursorOpen);
            return;
        }

        bool onButton = false;
        bool onStop = false;/*
        bool onBackspace = false;
        bool onClear = false;
        bool onSubmit = false;
        int onLetter = -1;*/
        if (generatedNumbers.size() == 6) {
            /*if (chosenLetters.size() > 0) {
                onClear = isOnClear(window);
                if (!onClear) onBackspace = isOnBackspace(window);
                if (!onBackspace && !onClear) onSubmit = !isCurrentWordInvalid && loadDictThreadEnded && isOnSubmit(window);
                onButton = onClear || onBackspace || onSubmit;
            }
            if (!onButton) {
                for (int i = 0; i < 12; i++) {
                    if (isOnLetter(window, i)) {
                        onButton = true;
                        onLetter = i;
                        break;
                    }
                }
            }*/
        }
        else {
            onStop = isOnStop(window);
            onButton = onStop;
        }/*
        if (onButton) {
            if ((stopPressed && onStop) || (clearPressed && onClear) || (backspacePressed && onBackspace) || (submitPressed && onSubmit) || (onLetter != -1 && letterPressed == onLetter)) {
                glfwSetCursor(window, cursorPress);

            }
            else if (onStop || onClear || onBackspace || onSubmit || (onLetter != -1)) {
                glfwSetCursor(window, cursorHover);
            }
        }
        else glfwSetCursor(window, cursorOpen);*/
        if (onButton) {
            if ((stopPressed && onStop)) {
                glfwSetCursor(window, cursorPress);
            }
            else if (onStop) {
                glfwSetCursor(window, cursorHover);
            }
        }
        else glfwSetCursor(window, cursorOpen);
    }

    void updateError() {
        /*std::wstring currentWord = getCurrentWord();
        for (std::wstring word : invalidWords) {
            if (word == currentWord) {
                isCurrentWordInvalid = true;
                return;
            }
        }
        isCurrentWordInvalid = false;*/
    }

    void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
    {
        if (gameEnded) return;
        if (generatedNumbers.size() < 6) {
            if (key == GLFW_KEY_ENTER) {
                if (action == GLFW_PRESS) stopPressed = true;
                else if (stopPressed && action == GLFW_RELEASE) actionStop();
            }
        }
        /*
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
        }*/

        if (action == GLFW_RELEASE) {
            stopPressed = false;
            clearPressed = false;
            backspacePressed = false;
            submitPressed = false;
            numberPressed = -1;
        }
    }

    void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
        if (gameEnded) return;
        if (button == GLFW_MOUSE_BUTTON_LEFT) {

            if (generatedNumbers.size() == 6) {
                /*if (chosenLetters.size() > 0 &&
                    isOnClear(window)
                    ) {
                    if (action == GLFW_PRESS) {
                        clearPressed = true;
                    }
                    else if (clearPressed && action == GLFW_RELEASE) {
                        actionClear();
                    }
                }
                else if (chosenLetters.size() > 0 &&
                    isOnBackspace(window)
                    ) {
                    if (action == GLFW_PRESS) {
                        backspacePressed = true;
                    }
                    else if (backspacePressed && action == GLFW_RELEASE) {
                        actionBackspace();
                    }
                }
                else if (!isCurrentWordInvalid && chosenLetters.size() > 0 && loadDictThreadEnded &&
                    isOnSubmit(window)
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
                            isOnLetter(window, i)
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
                }*/
            }
            else {
                if (isOnStop(window)) {
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
                numberPressed = -1;
            }
        }
    }

    void actionStop() {
        if (generatedNumbers.size() == 6) return;
        if (targetNumber.size() < 3) {
            targetNumber.push_back(random);
        }
        else if (generatedNumbers.size() < 4) {
            generatedNumbers.push_back(random);
        }
        else if (generatedNumbers.size() == 4) {
            generatedNumbers.push_back(10 + random * 5);
        }
        else if (generatedNumbers.size() == 5) {
            generatedNumbers.push_back(25 + random * 25);
        }
    }

    void run(int roundTime) {
        double start = glfwGetTime();
        while ((glfwGetTime() - start) < fps) {}

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(window, GL_TRUE);
        updateCursor(window, cursorHover, cursorOpen, cursorPress);

        if (generatedNumbers.size() == 6 && startTimer == 0.0) startTimer = glfwGetTime();

        drawUniversalElements();

        for (int i = 0; i < targetNumber.size() + generatedNumbers.size(); i++) {
            if (i < 3) {
                glBindTexture(GL_TEXTURE_2D, smallNumbers[targetNumber[i]]);
                glDrawArrays(GL_TRIANGLE_FAN, 308 + i * 4, 4);
            }
            else if (i < 7) {
                glBindTexture(GL_TEXTURE_2D, smallNumbers[generatedNumbers[i - 3]]);
                /*bool found = numberPressed == i;
                if (!found)
                    for (int j = 0; j < chosenLetterCounter; j++) {
                        if (chosenLetters[j] == i) {
                            found = true;
                            break;
                        }
                    }
                if (!found)*/glDrawArrays(GL_TRIANGLE_FAN, 76 + 4 * (i - 3), 4);/*
                else {
                    drawWithLens(76 + i * 4, lens);
                    glActiveTexture(GL_TEXTURE1);
                }*/
            }
            else if (i == 7) {
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, fieldL);
                glActiveTexture(GL_TEXTURE1);
                glBindTexture(GL_TEXTURE_2D, mediumNumbers[(generatedNumbers[i - 3] - 10) / 5]);
                glDrawArrays(GL_TRIANGLE_FAN, 320, 4);
            }
            else if (i == 8) {
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, fieldXL);
                glActiveTexture(GL_TEXTURE1);
                glBindTexture(GL_TEXTURE_2D, largeNumbers[(generatedNumbers[i - 3] - 25) / 25]);
                glDrawArrays(GL_TRIANGLE_FAN, 324, 4);
            }
        }

        if (generatedNumbers.size() < 6) {
            if (targetNumber.size() < 3) {
                random = disDigit(gen);
                glBindTexture(GL_TEXTURE_2D, smallNumbers[random]);
                glDrawArrays(GL_TRIANGLE_FAN, 308 + 4 * targetNumber.size(), 4);
            }
            else if (generatedNumbers.size() < 4) {
                random = disSmall(gen);
                glBindTexture(GL_TEXTURE_2D, smallNumbers[random]);
                glDrawArrays(GL_TRIANGLE_FAN, 76 + 4 * generatedNumbers.size(), 4);
            }
            else if (generatedNumbers.size() == 4) {
                random = disMedium(gen);
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, fieldL);
                glActiveTexture(GL_TEXTURE1);
                glBindTexture(GL_TEXTURE_2D, mediumNumbers[random]);
                glDrawArrays(GL_TRIANGLE_FAN, 320, 4);
            }
            else if (generatedNumbers.size() == 5) {
                random = disLarge(gen);
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, fieldXL);
                glActiveTexture(GL_TEXTURE1);
                glBindTexture(GL_TEXTURE_2D, largeNumbers[random]);
                glDrawArrays(GL_TRIANGLE_FAN, 324, 4);
            }

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
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, field);
            glActiveTexture(GL_TEXTURE1);

            for (int i = 0; i < 4; i++) {
                glBindTexture(GL_TEXTURE_2D, operations[i]);
                glDrawArrays(GL_TRIANGLE_FAN, 328 + i * 4, 4);
            }

            for (int i = 0; i < 2; i++) {
                glBindTexture(GL_TEXTURE_2D, brackets[i]);
                glDrawArrays(GL_TRIANGLE_FAN, 344 + i * 4, 4);
            }   
        }
        
        if (!gameEnded) {
            float red = 0.0f;
            glUseProgram(colShader);
            if (generatedNumbers.size() == 6) {
                red = (glfwGetTime() - startTimer) / roundTime;
                if (red > 0.975) gameEnded = true;
            }
            float kY = 1.0f - red;

            glUniform1f(glGetUniformLocation(colShader, "red"), red);
            glUniform1f(glGetUniformLocation(colShader, "kY"), kY);
            glUniform1f(glGetUniformLocation(colShader, "minY"), convertY(int(screenHeight / 2.0) - 3 * PADDING));
            glDrawArrays(GL_TRIANGLE_FAN, 16, 4);
        }

        glBindTexture(GL_TEXTURE_2D, 0);
        glUseProgram(0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}

using namespace NumbersGame;

void playNumbersGame(int roundTime) {

    glBindTexture(GL_TEXTURE_2D, 0);

    glfwSetKeyCallback(window, keyboardCallback);
    glfwSetMouseButtonCallback(window, mouseButtonCallback);

    while (!glfwWindowShouldClose(window))
    {
        run(roundTime);
    }
}