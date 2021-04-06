//
// Created by korpatiy on 05.04.2021.
//

#include "../Headers/Curve.h"
#include "../Headers/MainCreator.h"

bool Curve::createCurveShaderProgram() {
    c_shaderProgram = 0;

    const GLchar vshL[] =
            "#version 330\n"
            ""
            "layout(location = 0) in vec2 a_position;"
            ""
            "void main()"
            "{"
            "    gl_Position = vec4(a_position, 0.0, 1.0);"
            "}";

    const GLchar fshL[] =
            "#version 330\n"
            ""
            "layout(location = 0) out vec4 o_color;"
            ""
            "void main()"
            "{"
            "   o_color = vec4(0.0, 0.0, 0.0, 0.0);"
            "}";

    GLuint vertexShader, fragmentShader;

    vertexShader = createShader(vshL, GL_VERTEX_SHADER);
    fragmentShader = createShader(fshL, GL_FRAGMENT_SHADER);

    c_shaderProgram = createProgram(vertexShader, fragmentShader);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return c_shaderProgram != 0;
}

bool Curve::createCurveModel() {

    size_t vertSize = bezier_points.size() * 2;
    size_t idxSize = bezier_points.size();

    auto *vertices = new GLfloat[vertSize];
    auto *indices = new GLuint[idxSize];

    int i = 0;
    for (Point2D p : bezier_points) {
        vertices[i] = p.x;
        vertices[i + 1] = p.y;
        indices[i / 2] = i / 2;
        i = i + 2;
    }

    glGenVertexArrays(1, &c_model.vao);
    glBindVertexArray(c_model.vao);

    glGenBuffers(1, &c_model.vbo);
    glBindBuffer(GL_ARRAY_BUFFER, c_model.vbo);
    glBufferData(GL_ARRAY_BUFFER, vertSize * sizeof(GLfloat), vertices, GL_STATIC_DRAW);

    glGenBuffers(1, &c_model.ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, c_model.ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, idxSize * sizeof(GLuint), indices, GL_STATIC_DRAW);

    c_model.indexCount = idxSize;

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (const GLvoid *) 0);

    delete[] vertices;
    delete[] indices;
    return c_model.vbo != 0 && c_model.ibo != 0 && c_model.vao != 0;
}

void Curve::drawCurve() const {
    glUseProgram(c_shaderProgram);
    glBindVertexArray(c_model.vao);
    glDrawElements(GL_LINE_STRIP, c_model.indexCount, GL_UNSIGNED_INT, NULL);
}

void Curve::cleanup() {
    if (c_shaderProgram != 0)
        glDeleteProgram(c_shaderProgram);
    if (c_model.vbo != 0)
        glDeleteBuffers(1, &c_model.vbo);
    if (c_model.ibo != 0)
        glDeleteBuffers(1, &c_model.ibo);
    if (c_model.vao != 0)
        glDeleteVertexArrays(1, &c_model.vao);
}
