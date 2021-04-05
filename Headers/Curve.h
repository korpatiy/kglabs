//
// Created by korpatiy on 05.04.2021.
//

#ifndef KGLAB2_CURVE_H
#define KGLAB2_CURVE_H

#include "Model.h"
#include "Bezier.h"

class Curve {
public:
    Model c_model;
    GLuint c_shaderProgram;
    vector<Point2D> bezier_points;

    bool createCurveShaderProgram();

    bool createCurveModel();

    void drawCurve() const;

    void cleanup();
};


#endif //KGLAB2_CURVE_H
