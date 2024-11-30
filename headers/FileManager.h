#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <string>

unsigned int compileShader(GLenum type, std::string fileName);
unsigned int createShader(std::string vsSource, std::string fsSource);
unsigned loadTexture(std::string folderName, std::string fileName, bool filterLinear = false);
unsigned loadImageToTexture(const char* filePath);
GLFWcursor* loadImageToCursor(const char* filePath);