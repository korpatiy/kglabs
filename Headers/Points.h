//
// Created by korpatiy on 02.04.2021.
//

#ifndef KGLAB2_POINTS_H
#define KGLAB2_POINTS_H

#include "Model.h"
#include "Bezier.h"

class Points {
public:
    Model p_model;
    GLuint p_shaderProgram;
    GLint p_uMV;
    vector<Point2D> base_points;

    bool createPointShaderProgram();

    bool createPointModel();

    void drawPoints();

    void cleanup();
};

#endif //KGLAB2_POINTS_H
