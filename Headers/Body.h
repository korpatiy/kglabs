//
// Created by korpatiy on 02.04.2021.
//

#ifndef KGLAB2_BODY_H
#define KGLAB2_BODY_H

#include <GL/glew.h>
#include "glm/ext.hpp"
#include "glm/matrix.hpp"
#include "Model.h"
#include "iostream"

const size_t texCount = 2;

class Body {
public:
    Model g_model;
    GLuint g_shaderProgram;
    GLint g_uMVP, g_uMV;
    GLint mapLocation[texCount];
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, 0.1f, 100.f);

    GLuint textures[texCount];

    bool createShaderProgram();

    bool createModel();

    bool createTexture();

    void draw(GLfloat delta);

    void cleanup();
};

#endif //KGLAB2_BODY_H
