#pragma once
#define PADDING 5
#include <GL/glew.h>
#include <GLFW/glfw3.h>

extern int screenWidth;
extern int screenHeight;

extern unsigned int colShader;
extern unsigned int texShader;

extern unsigned border;
extern unsigned row;
extern unsigned borderTimer;
extern unsigned rowTimer;
extern unsigned field;
extern unsigned fieldL;
extern unsigned fieldXL;
extern unsigned clear;
extern unsigned backspace;
extern unsigned stop;
extern unsigned lens;
extern unsigned lensL;
extern unsigned lensXL;
extern unsigned stopLens;
extern unsigned submitLens;
extern unsigned error;
extern unsigned submit;
extern const double fps;
extern unsigned background;
extern unsigned logo;
extern unsigned player;

extern unsigned letters[30];

extern unsigned smallNumbers[10];
extern unsigned mediumNumbers[3];
extern unsigned largeNumbers[4];
extern unsigned operations[5];
extern unsigned brackets[2];

float convertX(int pixels);
float convertY(int pixels);

bool isInSquare(float left, float up, float width, float height);
void drawWithLens(int start, unsigned texture);
void drawUniversalElements();

//TODO: remove window
bool isOnStop();
bool isOnBackspace();
bool isOnClear();
bool isOnSubmit();

bool isOnLetter(int index);

bool isOnMediumNumber();
bool isOnLargeNumber();
bool isOnOperation(int index);
bool isOnBracket(bool closed);
bool isOnSymbol(int index);

extern GLFWwindow* window;

extern GLFWcursor* cursorHover;
extern GLFWcursor* cursorOpen;
extern GLFWcursor* cursorPress;