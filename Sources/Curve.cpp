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
            "layout(location = 0) in vec3 a_position;"
            ""
            "void main()"
            "{"
            "    gl_Position = vec4(a_position, 1.0);"
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
    float curveWidth = 1.0f;
    vector<glm::vec3> vertices;

    vertices.emplace_back(bezier_points[0].x, bezier_points[0].y, 0.0f);
    vertices.emplace_back(bezier_points[0].x, bezier_points[0].y, 0.0f);

    for (size_t i = 1; i < bezier_points.size() - 1; i++) {
        auto vec1 = bezier_points[i] - bezier_points[i - 1];
        auto vec2 = bezier_points[i + 1] - bezier_points[i];
        vec1.normalize();
        vec2.normalize();
        auto h = vec1 + vec2;
        h.normalize();
        swap(h.x, h.y);
        h.x *= -1.0f;
        auto point1 = bezier_points[i] - h * curveWidth;
        auto point2 = bezier_points[i] + h * curveWidth;
        vertices.emplace_back(point1.x, point1.y, -1.0f);
        vertices.emplace_back(point2.x, point2.y, -1.0f);
    }

    vertices.emplace_back(bezier_points[bezier_points.size() - 1].x, bezier_points[bezier_points.size() - 1].y, 0.0f);

    vector<unsigned int> indices;
    for (size_t i = 0; i < bezier_points.size() - 1; i++) {
        indices.push_back(0 + i * 2);
        indices.push_back(1 + i * 2);
        indices.push_back(3 + i * 2);
        indices.push_back(3 + i * 2);
        indices.push_back(2 + i * 2);
        indices.push_back(0 + i * 2);
    }

    glGenVertexArrays(1, &c_model.vao);
    glBindVertexArray(c_model.vao);

    glGenBuffers(1, &c_model.vbo);
    glBindBuffer(GL_ARRAY_BUFFER, c_model.vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), &vertices[0], GL_STATIC_DRAW);

    glGenBuffers(1, &c_model.ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, c_model.ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &indices[0], GL_STATIC_DRAW);

    c_model.indexCount = indices.size();

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (const GLvoid *) 0);

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
