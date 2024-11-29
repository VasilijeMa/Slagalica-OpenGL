#include "FileManager.h"


//unsigned loadTexture(std::string folderName, std::string fileName, bool filterLinear) {
//    unsigned texture = loadImageToTexture(("res/" + folderName + "/" + fileName + ".png").c_str());
//    glBindTexture(GL_TEXTURE_2D, texture);
//    glGenerateMipmap(GL_TEXTURE_2D);
//    if (filterLinear) {
//        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//    }
//    return texture;
//}