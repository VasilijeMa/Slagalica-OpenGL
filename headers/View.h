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
extern unsigned clear;
extern unsigned backspace;
extern unsigned stop;
extern unsigned lens;
extern unsigned stopLens;
extern unsigned submitLens;
extern unsigned error;
extern unsigned submit;
extern unsigned logo;
extern const double targetFrameTime;

float convertX(int pixels);
float convertY(int pixels);

bool isInSquare(GLFWwindow* window, float left, float up, float width, float height);
void drawWithLens(int start, unsigned texture);
void drawUniversalElements();

extern GLFWwindow* window;

extern GLFWcursor* cursorHover;
extern GLFWcursor* cursorOpen;
extern GLFWcursor* cursorPress;