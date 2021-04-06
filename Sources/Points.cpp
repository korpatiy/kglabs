//
// Created by korpatiy on 02.04.2021.
//

#include "../Headers/Points.h"
#include "../Headers/MainCreator.h"

bool Points::createPointShaderProgram() {
    p_shaderProgram = 0;

    const GLchar vshP[] =
            "#version 330\n"
            ""
            "layout(location = 0) in vec2 a_position;"
            ""
            "uniform mat4 u_MV;"
            ""
            "void main()"
            "{"
            "    gl_Position = u_MV * vec4(a_position, 0.0, 1.0);"
            "}";

    const GLchar fshP[] =
            "#version 330\n"
            ""
            "layout(location = 0) out vec4 o_color;"
            ""
            "void main()"
            "{"
            "   o_color = vec4(1.0, 0.0, 0.0, 1.0);"
            "}";

    GLuint vertexShader, fragmentShader;

    vertexShader = createShader(vshP, GL_VERTEX_SHADER);
    fragmentShader = createShader(fshP, GL_FRAGMENT_SHADER);

    this->p_shaderProgram = createProgram(vertexShader, fragmentShader);
    this->p_uMV = glGetUniformLocation(p_shaderProgram, "u_MV");

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return p_shaderProgram != 0;
}

bool Points::createPointModel() {

    const GLfloat vertices[] = {
            -0.3f, -0.5,
            0.5f, -0.5f,
            0.5f, 0.5f,
            -0.3f, 0.5f
    };

    const GLuint indices[] = {
            0, 1, 2, 2, 3, 0
    };

    glGenVertexArrays(1, &p_model.vao);
    glBindVertexArray(p_model.vao);

    glGenBuffers(1, &p_model.vbo);
    glBindBuffer(GL_ARRAY_BUFFER, p_model.vbo);
    glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(GLfloat), vertices, GL_STATIC_DRAW);

    glGenBuffers(1, &p_model.ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, p_model.ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(GLuint), indices, GL_STATIC_DRAW);

    p_model.indexCount = 6;

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (const GLvoid *) 0);

    // delete[] vertices;
    // delete[] indices;
    return p_model.vbo != 0 && p_model.ibo != 0 && p_model.vao != 0;
}

void Points::cleanup() {
    if (p_shaderProgram != 0)
        glDeleteProgram(p_shaderProgram);
    if (p_model.vbo != 0)
        glDeleteBuffers(1, &p_model.vbo);
    if (p_model.ibo != 0)
        glDeleteBuffers(1, &p_model.ibo);
    if (p_model.vao != 0)
        glDeleteVertexArrays(1, &p_model.vao);
}

void Points::drawPoints() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(p_shaderProgram);
    glBindVertexArray(p_model.vao);

    for (Point2D p: base_points) {
        auto model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(p.x, p.y, 0.0f));
        model = glm::scale(model, glm::vec3(0.02));
        glUniformMatrix4fv(this->p_uMV, 1, GL_FALSE, glm::value_ptr(model));
        glDrawElements(GL_TRIANGLES, p_model.indexCount, GL_UNSIGNED_INT, NULL);
    }
}