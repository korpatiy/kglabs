//
// Created by korpatiy on 02.04.2021.
//

#ifndef KGLAB2_POINTS_H
#define KGLAB2_POINTS_H
#include <GL/glew.h>
#include "glm/ext.hpp"
#include "glm/matrix.hpp"
#include "Model.h"
#include "Bezier.h"

class Points {
public:
    Model g_model;
    GLuint g_shaderProgram;
    GLint g_uMV;
    vector<Point2D> wPoints;
    vector<float> mPoints;

    bool createShaderProgram();

    bool createModel();

    void draw();

    void cleanup();
};

#endif //KGLAB2_POINTS_H
