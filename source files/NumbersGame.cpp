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
    std::vector<int> solution;
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
    bool findSolutionThreadStarted = false;
    float startThreadTime = 0.0;

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

    int getWidthIndex(const std::vector<int>& symbols, int index);

    int getTargetNumber();
    bool isInvalidExpressionPresent();

    bool isInvalidSymbol(int prev, int next);
    int getNumBrackets(bool closed);

    std::string convertExpression(const std::vector<int>& symbols);
    int precedence(int op);
    int applyOp(int a, int b, int op);
    std::vector<int> getPostfix(const std::vector<int>& symbols);
    int evaluatePostfix(const std::vector<int>& symbols);
    std::vector<std::vector<int>> generateSubsets(int subsetSize);
    std::vector<int> generateBestExpression(const std::vector<int>& subset);
    std::vector<int> findSolution();

    void drawNumbers(const std::vector<int>& symbols, int start);

    int getTargetNumber() {
        return targetNumber[0] * 100 + targetNumber[1] * 10 + targetNumber[2];
    }

    std::string convertExpression(const std::vector<int>& symbols) {
        std::string expression = "";
        for (int i = 0; i < symbols.size(); i++) {
            int symbol = symbols[i];
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
        std::string currentExpression = convertExpression(chosenSymbols);
        for (std::string expression : invalidExpressions) {
            if (expression == currentExpression) {
                return true;
            }
        }
        return false;
    }

    int getWidthIndex(const std::vector<int>& symbols, int index) {
        int widthIndex;
        int symbol = symbols[index];
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
            if (b == 0 || a < b || a % b != 0) return -1;
            return a / b;
        }
        default: return 0;
        }
    }

    std::vector<int> getPostfix(const std::vector<int>& symbols) {
        std::stack<char> operations;
        std::vector<int> output;
        for (int i = 0; i < symbols.size(); i++) {
            int symbol = symbols[i];
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

    int evaluatePostfix(const std::vector<int>& symbols) {
        std::vector<int> postfix = getPostfix(symbols);
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

    std::vector<std::vector<int>> generateSubsets(int subsetSize) {
        std::vector<int> nums = { 0, 1, 2, 3, 4, 5 };
        std::vector<std::vector<int>> subsets;

        std::vector<bool> mask(nums.size(), false);
        std::fill(mask.begin(), mask.begin() + subsetSize, true);

        do {
            std::vector<int> subset;
            for (int i = 0; i < nums.size(); i++) {
                if (mask[i]) {
                    subset.push_back(nums[i]);
                }
            }

            do {
                subsets.push_back(subset);
            } while (std::next_permutation(subset.begin(), subset.end()));

        } while (std::prev_permutation(mask.begin(), mask.end()));

        return subsets;
    }

    std::vector<int> generateBestExpression(const std::vector<int>& subset) {
        int targetNumber = getTargetNumber();
        int bestResult = 0;
        std::vector<int> bestExpression;

        int n = subset.size();

        std::vector<int> operations = { 6, 7, 8, 9 };

        std::vector<std::vector<int>> allOperationCombinations;
        std::vector<int> currentOps(n - 1, 6);

        do {
            allOperationCombinations.push_back(currentOps);

            for (int i = n - 2; i >= 0; i--) {
                if (currentOps[i] < 9) {
                    currentOps[i]++;
                    break;
                }
                else {
                    currentOps[i] = 6;
                }
            }
        } while (currentOps != std::vector<int>(n - 1, 6));

        for (const std::vector<int>& ops : allOperationCombinations) {
            std::vector<int> expression;
            expression.push_back(subset[0]);

            for (int i = 0; i < ops.size(); i++) {
                expression.push_back(ops[i]);
                expression.push_back(subset[i + 1]);
            }

            int result = evaluatePostfix(expression);
            if (result == targetNumber) return expression;
            else if (abs(result - targetNumber) < abs(bestResult - targetNumber)) {
                bestExpression = expression;
                bestResult = result;
            }
        }

        return bestExpression;
    }

    std::vector<int> findSolution() {
        int targetNumber = getTargetNumber();
        int bestResult = 0;
        std::vector<int> bestExpression;
        for (int i = 2; i < 7; i++) {
            std::vector<std::vector<int>> subsets = generateSubsets(i);
            for (std::vector<int> subset : subsets) {
                std::vector<int> expression = generateBestExpression(subset);
                //std::cout << convertExpression(expression) << std::endl;
                int result = evaluatePostfix(expression);
                if (result == targetNumber) return expression;
                else if (abs(result - targetNumber) < abs(bestResult - targetNumber)) {
                    bestExpression = expression;
                    bestResult = result;
                }
            }
        }
        return bestExpression;
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
                onClear = isOnClear();
                if (!onClear) onBackspace = isOnBackspace();
                if (!onBackspace && !onClear) onSubmit = !isCurrentExpressionInvalid && isOnSubmit();
                onButton = onClear || onBackspace || onSubmit;
            }
            if (!onButton) {
                for (int i = 0; i < 12; i++) {
                    if (isOnSymbol(i)) {
                        onButton = true;
                        onSymbol = i;
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
                if (chosenSymbols.size() > 0 && isOnClear()) {
                    if (action == GLFW_PRESS) {
                        clearPressed = true;
                    }
                    else if (clearPressed && action == GLFW_RELEASE) {
                        actionClear();
                    }
                }
                else if (chosenSymbols.size() > 0 &&
                    isOnBackspace()
                    ) {
                    if (action == GLFW_PRESS) {
                        backspacePressed = true;
                    }
                    else if (backspacePressed && action == GLFW_RELEASE) {
                        actionBackspace();
                    }
                }
                else if (!isCurrentExpressionInvalid && chosenSymbols.size() > 0 && isOnSubmit()) {
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
                        if (isOnSymbol(i)) {
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
            invalidExpressions.push_back(convertExpression(chosenSymbols));
            return;
        }
        int result = evaluatePostfix(chosenSymbols);
        if (result < 0 || result > 999) isCurrentExpressionInvalid = true;
        else {
            int target = getTargetNumber();
            if (target == result) score += 30;
            else if (abs(target - result) < 5) score += 25;
            gameEnded = true;
            chosenSymbols.push_back(35);
            int deg = 100;
            while (deg > 0) {
                int digit = (result / deg);
                if (digit > 0 || chosenSymbols.back() != 35) chosenSymbols.push_back(digit + 12);
                result -= digit * deg;
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

    void drawNumbers(const std::vector<int>& symbols, int start) {
        float textWidth = 0.0f;

        for (int i = 0; i < symbols.size(); i++) {
            textWidth += widths[getWidthIndex(symbols, i)];
        }

        textWidth *= screenWidth / 15.0;
        textWidth += PADDING / 2.0 * (symbols.size() - 1);
        float prevTextWidth = 0.0f;

        for (int i = 0; i < symbols.size(); i++) {
            int widthIndex = getWidthIndex(symbols, i);

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
            glDrawArrays(GL_TRIANGLE_FAN, start + i * 4, 4);
            glUniform1f(glGetUniformLocation(texShader, "kX"), 0);
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
            if (!findSolutionThreadStarted) {
                findSolutionThreadStarted = true;
                startThreadTime = glfwGetTime();
                std::thread findBestSolutionsSerialTask([&]() {
                    solution = findSolution();
                    std::cout << "Uspesno nasao resenje za " << glfwGetTime() - startThreadTime << " sekundi!" << std::endl;
                    solution.push_back(35);
                    int result = evaluatePostfix(solution);
                    int deg = 100;
                    while (deg > 0) {
                        int digit = (result / deg);
                        if (digit > 0 || solution.back() != 35) solution.push_back(digit + 12);
                        result -= digit * deg;
                        deg /= 10;
                    }
                    });
                findBestSolutionsSerialTask.detach();
            }

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

            drawNumbers(chosenSymbols, 124);
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

        else if (!solution.empty()) {
            drawNumbers(solution, 216);
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