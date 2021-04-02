//
// Created by korpatiy on 02.04.2021.
//

#ifndef KGLAB2_MAINCREATOR_H
#define KGLAB2_MAINCREATOR_H

#include <GL/glew.h>
#include "iostream"

GLuint createShader(const GLchar *code, GLenum type);
GLuint createProgram(GLuint vsh, GLuint fsh);

#endif //KGLAB2_MAINCREATOR_H
