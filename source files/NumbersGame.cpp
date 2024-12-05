#include "NumbersGame.h"
#include "View.h"
#include <string>
#include <vector>
#include <stack>
#include <algorithm>
#include <iostream>
#include <random>
#include <thread>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

namespace NumbersGame {

    std::vector<int> targetNumber;
    std::vector<int> generatedNumbers;
    std::vector<int> chosenSymbols;
    std::vector<std::string> invalidExpressions;
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
    int symbolPressed = -1;
    bool gameEnded = false;
    bool isCurrentExpressionInvalid = false;

    float widths[24] = {
        0.421875, // 0
        0.34375, // 1
        0.390625, // 2
        0.375, // 3
        0.4375, // 4
        0.390625, // 5
        0.40625, // 6
        0.390625, // 7
        0.421875, // 8
        0.390625, // 9
        0.84375, // 10
        0.84375, // 15
        0.875, // 20
        0.859375, // 25
        0.875, // 50
        0.859375, // 75 
        1.3125, // 100
        0.40625, // +
        0.234375, // -
        0.375, // *
        0.125, // :
        0.1875, // (
        0.1875, // )
        0.375 // =
    };

    int symbolKeys[12] = {
        GLFW_KEY_1, // small num 1
        GLFW_KEY_2, // small num 2
        GLFW_KEY_3, // small num 3
        GLFW_KEY_4, // small num 4
        GLFW_KEY_5, // medium num
        GLFW_KEY_6, // large num
        GLFW_KEY_7, // +
        GLFW_KEY_8, // -
        GLFW_KEY_9, // *
        GLFW_KEY_0, // :
        GLFW_KEY_MINUS, // (
        GLFW_KEY_EQUAL  // )
    };

    void run(int roundTime);

    void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);

    void updateCursor(GLFWwindow* window, GLFWcursor* cursorHover, GLFWcursor* cursorOpen, GLFWcursor* cursorPress);

    bool isSelected(int index);
    void actionStop();
    void actionBackspace();
    void actionClear();
    void actionChooseSymbol(int index);
    void actionStop();
    void actionSubmit();

    int getWidthIndex(int index);

    std::string getCurrentExpression();
    bool isInvalidExpressionPresent();

    bool isInvalidSymbol(int prev, int next);
    int getNumBrackets(bool closed);

    int precedence(int op);
    int applyOp(int a, int b, int op);
    std::vector<int> getPostfix();
    int evaluatePostfix();

    std::string getCurrentExpression() {
        std::string expression = "";
        for (int i = 0; i < chosenSymbols.size(); i++) {
            int symbol = chosenSymbols[i];
            if (symbol < 6) {
                expression += std::to_string(generatedNumbers[symbol]);
            }
            else switch (symbol) {
                case 6: {
                    expression += '+';
                    break;
                }
                case 7: {
                    expression += '-';
                    break;
                }
                case 8: {
                    expression += '*';
                    break;
                }
                case 9: {
                    expression += ':';
                    break;
                }
                case 10: {
                    expression += '(';
                    break;
                }
                case 11: {
                    expression += ')';
                    break;
                }
            }
        }
        return expression;
    }

    bool isInvalidExpressionPresent() {
        std::string currentExpression = getCurrentExpression();
        for (std::string expression : invalidExpressions) {
            if (expression == currentExpression) {
                return true;
            }
        }
        return false;
    }

    int getWidthIndex(int index) {
        int widthIndex;
        int symbol = chosenSymbols[index];
        if (symbol < 4) widthIndex = generatedNumbers[symbol];
        else if (symbol == 4) widthIndex = 10 + (generatedNumbers[symbol] - 10) / 5;
        else if (symbol == 5) widthIndex = 13 + (generatedNumbers[symbol] - 25) / 25;
        else if (symbol < 12) widthIndex = symbol + 11;
        else widthIndex = symbol - 12;
        return widthIndex;
    }

    bool isInvalidSymbol(int prev, int next) {
        return
            ((prev < 6 || prev == 11) && (next < 6 || next == 10)) ||
            (prev >= 6 && prev <= 10 && ((next >= 6 && next < 10) || next == 11));
    }

    int getNumBrackets(bool closed) {
        int num = 0;
        for (int i = 0; i < chosenSymbols.size(); i++) {
            if (chosenSymbols[i] == 10 + closed) num++;
        }
        return num;
    }

    int precedence(int op) {
        if (op == 6 || op == 7) return 1;
        if (op == 8 || op == 9) return 2;
        return 0;
    }

    int applyOp(int a, int b, int op) {
        switch (op) {
        case 6: return a + b;
        case 7: return a - b;
        case 8: return a * b;
        case 9: {
            if (a < b || a % b != 0) return -1;
            return a / b;
        }
        default: return 0;
        }
    }

    std::vector<int> getPostfix() {
        std::stack<char> operations;
        std::vector<int> output;
        for (int i = 0; i < chosenSymbols.size(); i++) {
            int symbol = chosenSymbols[i];
            if (symbol < 6) {
                output.push_back(symbol);
            }
            else if (symbol == 10) {
                operations.push(symbol);
            }
            else if (symbol == 11) {
                while (!operations.empty() && operations.top() != 10) {
                    output.push_back(operations.top());
                    operations.pop();
                }
                operations.pop();
            }
            else if (symbol > 5 && symbol < 10) {
                while (!operations.empty() && precedence(operations.top()) >= precedence(symbol)) {
                    output.push_back(operations.top());
                    operations.pop();
                }
                operations.push(symbol);
            }
        }
        while (!operations.empty()) {
            output.push_back(operations.top());
            operations.pop();
        }
        return output;
    }

    int evaluatePostfix() {
        std::vector<int> postfix = getPostfix();
        std::stack<int> values;
        for (int symbol : postfix) {
            if (symbol < 6) {
                values.push(generatedNumbers[symbol]);
            }
            else {
                int b = values.top();
                values.pop();
                int a = values.top();
                values.pop();
                int result = applyOp(a, b, symbol);
                if (result == -1) return -1;
                values.push(result);
            }
        }
        return values.top();
    }

    void updateCursor(GLFWwindow* window, GLFWcursor* cursorHover, GLFWcursor* cursorOpen, GLFWcursor* cursorPress) {
        if (gameEnded)
        {
            glfwSetCursor(window, cursorOpen);
            return;
        }

        bool onButton = false;
        bool onStop = false;
        bool onBackspace = false;
        bool onClear = false;
        bool onSubmit = false;
        int onSymbol = -1;
        if (generatedNumbers.size() == 6) {
            if (chosenSymbols.size() > 0) {
                onClear = isOnClear(window);
                if (!onClear) onBackspace = isOnBackspace(window);
                if (!onBackspace && !onClear) onSubmit = !isCurrentExpressionInvalid && isOnSubmit(window);
                onButton = onClear || onBackspace || onSubmit;
            }
            if (!onButton) {
                for (int i = 0; i < 12; i++) {
                    if (isOnSymbol(window, i)) {
                        onButton = true;
                        onSymbol = i;
                        break;
                    }
                }
            }
        }
        else {
            onStop = isOnStop(window);
            onButton = onStop;
        }
        if (onButton) {
            if ((stopPressed && onStop) || (clearPressed && onClear) || (backspacePressed && onBackspace) || (submitPressed && onSubmit) || (onSymbol != -1 && symbolPressed == onSymbol)) {
                glfwSetCursor(window, cursorPress);

            }
            else if (onStop || onClear || onBackspace || onSubmit || (onSymbol != -1)) {
                glfwSetCursor(window, cursorHover);
            }
        }
        else glfwSetCursor(window, cursorOpen);
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
        if (isInvalidExpressionPresent()) {
            isCurrentExpressionInvalid = true;
            return;
        }
        isCurrentExpressionInvalid = false;
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

        else {
            if (key == GLFW_KEY_DELETE && chosenSymbols.size() > 0) {
                if (action == GLFW_PRESS) clearPressed = true;
                else if (clearPressed && action == GLFW_RELEASE) actionClear();
            }
            else if (key == GLFW_KEY_BACKSPACE && chosenSymbols.size() > 0) {
                if (action == GLFW_PRESS) backspacePressed = true;
                else if (backspacePressed && action == GLFW_RELEASE) actionBackspace();
            }
            else if (key == GLFW_KEY_ENTER && chosenSymbols.size() > 0 && !isCurrentExpressionInvalid) {
                if (action == GLFW_PRESS) submitPressed = true;
                else if (submitPressed && action == GLFW_RELEASE) actionSubmit();
            }
            else {
                for (int i = 0; i < 12; i++) {
                    if (key == symbolKeys[i]) {
                        if (i < 6 && isSelected(i)) break;
                        if (action == GLFW_PRESS) {
                            symbolPressed = i;
                            actionChooseSymbol(i);
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
            symbolPressed = -1;
        }
    }

    void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
        if (gameEnded) return;
        if (button == GLFW_MOUSE_BUTTON_LEFT) {

            if (generatedNumbers.size() == 6) {
                if (chosenSymbols.size() > 0 && isOnClear(window)) {
                    if (action == GLFW_PRESS) {
                        clearPressed = true;
                    }
                    else if (clearPressed && action == GLFW_RELEASE) {
                        actionClear();
                    }
                }
                else if (chosenSymbols.size() > 0 &&
                    isOnBackspace(window)
                    ) {
                    if (action == GLFW_PRESS) {
                        backspacePressed = true;
                    }
                    else if (backspacePressed && action == GLFW_RELEASE) {
                        actionBackspace();
                    }
                }
                else if (!isCurrentExpressionInvalid && chosenSymbols.size() > 0 && isOnSubmit(window)) {
                    if (action == GLFW_PRESS) {
                        submitPressed = true;
                    }
                    else if (submitPressed && action == GLFW_RELEASE) {
                        actionSubmit();
                    }
                }
                else {
                    bool found = false;
                    for (int i = 0; i < 12; i++) {
                        if (isOnSymbol(window, i)) {
                            if (i < 6 && isSelected(i)) break;
                            if (action == GLFW_PRESS) {
                                symbolPressed = i;
                            }
                            else if (symbolPressed == i && action == GLFW_RELEASE) {
                                actionChooseSymbol(i);
                            }
                            break;
                        }
                    }
                }
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
                symbolPressed = -1;
            }
        }
    }

    void actionBackspace() {
        int number = chosenSymbols.back();
        chosenSymbols.pop_back();
        if (chosenSymbols.empty()) isCurrentExpressionInvalid = false;
        else updateError();
    }

    void actionClear() {
        chosenSymbols.clear();
        isCurrentExpressionInvalid = false;
    }

    void actionSubmit() {
        if (isInvalidExpressionPresent()) {
            isCurrentExpressionInvalid = true;
            return;
        }
        int back = chosenSymbols.back();
        if (getNumBrackets(false) != getNumBrackets(true) ||
            (back > 5 && back < 10)) {
            isCurrentExpressionInvalid = true;
            invalidExpressions.push_back(getCurrentExpression());
            return;
        }
        int result = evaluatePostfix();
        if (result < 0 || result > 999) isCurrentExpressionInvalid = true;
        else {
            gameEnded = true;
            chosenSymbols.push_back(35);
            int deg = 100;
            while (result > 0) {
                int digit = (result / deg);
                if (digit > 0) chosenSymbols.push_back(digit + 12);
                result -= digit * deg;
                deg /= 10;
            }
            while (deg > 0) {
                chosenSymbols.push_back(12);
                deg /= 10;
            }
        }
    }

    bool isSelected(int index) {
        for (int i = 0; i < chosenSymbols.size(); i++) {
            if (chosenSymbols[i] == index) {
                return true;
            }
        }
        return false;
    }

    void actionChooseSymbol(int index) {
        int prevSymbol = 10;
        if (!chosenSymbols.empty()) prevSymbol = chosenSymbols.back();
        if (isInvalidSymbol(prevSymbol, index) ||
            (getNumBrackets(false) == 4 && index == 10) ||
            (getNumBrackets(false) == getNumBrackets(true) && index == 11)) return;
        chosenSymbols.push_back(index);
        updateError();
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
            else {
                bool found = symbolPressed == i - 3 || isSelected(i - 3);
                if (i < 7) {
                    glBindTexture(GL_TEXTURE_2D, smallNumbers[generatedNumbers[i - 3]]);

                    if (!found) glDrawArrays(GL_TRIANGLE_FAN, 76 + 4 * (i - 3), 4);
                    else {
                        drawWithLens(76 + 4 * (i - 3), lens);
                        glActiveTexture(GL_TEXTURE1);
                    }
                }
                else if (i == 7) {
                    glActiveTexture(GL_TEXTURE0);
                    glBindTexture(GL_TEXTURE_2D, fieldL);
                    glActiveTexture(GL_TEXTURE1);
                    glBindTexture(GL_TEXTURE_2D, mediumNumbers[(generatedNumbers[i - 3] - 10) / 5]);
                    if (!found) glDrawArrays(GL_TRIANGLE_FAN, 320, 4);
                    else {
                        drawWithLens(320, lensL);
                        glActiveTexture(GL_TEXTURE1);
                    }
                }
                else if (i == 8) {
                    glActiveTexture(GL_TEXTURE0);
                    glBindTexture(GL_TEXTURE_2D, fieldXL);
                    glActiveTexture(GL_TEXTURE1);
                    glBindTexture(GL_TEXTURE_2D, largeNumbers[(generatedNumbers[i - 3] - 25) / 25]);
                    if (!found) glDrawArrays(GL_TRIANGLE_FAN, 324, 4);
                    else {
                        drawWithLens(324, lensXL);
                        glActiveTexture(GL_TEXTURE1);
                    }
                }
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
                if (symbolPressed != i + 6) glDrawArrays(GL_TRIANGLE_FAN, 328 + i * 4, 4);
                else {
                    drawWithLens(328 + i * 4, lens);
                    glActiveTexture(GL_TEXTURE1);
                }
            }

            for (int i = 0; i < 2; i++) {
                glBindTexture(GL_TEXTURE_2D, brackets[i]);
                glDrawArrays(GL_TRIANGLE_FAN, 344 + i * 4, 4);
                if (symbolPressed != i + 10) glDrawArrays(GL_TRIANGLE_FAN, 344 + i * 4, 4);
                else {
                    drawWithLens(344 + i * 4, lens);
                    glActiveTexture(GL_TEXTURE1);
                }
            }
        }

        if (!chosenSymbols.empty()) {

            glBindTexture(GL_TEXTURE_2D, 0);
            glActiveTexture(GL_TEXTURE0);

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
                if (!isCurrentExpressionInvalid) {
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

            float textWidth = 0.0f;

            for (int i = 0; i < chosenSymbols.size(); i++) {
                textWidth += widths[getWidthIndex(i)];
            }

            textWidth *= screenWidth / 15.0;
            textWidth += PADDING / 2.0 * (chosenSymbols.size() - 1);
            float prevTextWidth = 0.0f;

            for (int i = 0; i < chosenSymbols.size(); i++) {
                int widthIndex = getWidthIndex(i);

                int xMiddle = int((screenWidth * (1 + (widths[widthIndex] + 2 * prevTextWidth) / 15.0) - textWidth + PADDING * i) / 2.0);
                prevTextWidth += widths[widthIndex];

                unsigned texture{};
                if (widthIndex < 10) texture = smallNumbers[widthIndex];
                else if (widthIndex < 13) texture = mediumNumbers[widthIndex - 10];
                else if (widthIndex < 17) texture = largeNumbers[widthIndex - 13];
                else if (widthIndex < 21) texture = operations[widthIndex - 17];
                else if (widthIndex < 23) texture = brackets[widthIndex - 21];
                else texture = operations[4];

                float width = 0.0f;
                if (widthIndex >= 10 && widthIndex < 13) width = 2 / 3.0;
                else if (widthIndex >= 13 && widthIndex < 17) width = 0.5;

                glBindTexture(GL_TEXTURE_2D, texture);
                glUniform1f(glGetUniformLocation(texShader, "uX"), convertX(xMiddle));
                glUniform1f(glGetUniformLocation(texShader, "kX"), width);
                glDrawArrays(GL_TRIANGLE_FAN, 124 + i * 4, 4);
                glUniform1f(glGetUniformLocation(texShader, "kX"), 0);
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