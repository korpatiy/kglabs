//
// Created by korpatiy on 02.04.2021.
//

#include "../Headers/Points.h"
#include "MainCreator.h"

bool Points::createShaderProgram() {
    g_shaderProgram = 0;

    const GLchar vshP[] =
            "#version 330\n"
            ""
            "layout(location = 0) in vec2 a_position;"
            ""
            //"uniform mat4 u_MV;"
            ""
            "void main()"
            "{"
            "    gl_Position = vec4(a_position, 0.0, 1.0);"
            "}";

    const GLchar fshP[] =
            "#version 330\n"
            ""
            "layout(location = 0) out vec4 o_color;"
            ""
            "void main()"
            "{"
            "   o_color = vec4(0.0, 0.0, 1.0, 1.0);"
            "}";

    GLuint vertexShader, fragmentShader;

    vertexShader = createShader(vshP, GL_VERTEX_SHADER);
    fragmentShader = createShader(fshP, GL_FRAGMENT_SHADER);

    g_shaderProgram = createProgram(vertexShader, fragmentShader);
    //g_uMV = glGetUniformLocation(g_shaderProgram, "u_MV");

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return g_shaderProgram != 0;
}

bool Points::createModel() {
    /* size_t vertSize = mPoints.size();
     size_t idxSize = (int) mPoints.size() / 2;*/

    size_t vertSize = (int) wPoints.size() * 6;
    size_t idxSize = (int) wPoints.size() * 6;

    auto *vertices = new GLfloat[vertSize];
    auto head = wPoints.begin();
    int triangle_size = 5;
    for (int i = 0; i < vertSize; i += 6, head++) {
        int x = head->x;
        int y = head->y;
        vertices[i] = x;
        vertices[i + 1] = y + triangle_size;
        vertices[i + 2] = x - triangle_size;
        vertices[i + 3] = y - triangle_size;
        vertices[i + 4] = x + triangle_size;
        vertices[i + 5] = y - triangle_size;
    }
    for (int i = 0; i < vertSize; i += 2) {
        vertices[i] = vertices[i] * 2 / 800 - 1;
        vertices[i + 1] = vertices[i + 1] * 2 / 600 - 1;
    }

    auto *indices = new GLuint[idxSize];
    for (int i = 0; i < idxSize; ++i) {
        indices[i] = i;
    }

    glGenVertexArrays(1, &g_model.vao);
    glBindVertexArray(g_model.vao);

    glGenBuffers(1, &g_model.vbo);
    glBindBuffer(GL_ARRAY_BUFFER, g_model.vbo);
    glBufferData(GL_ARRAY_BUFFER, vertSize * sizeof(GLfloat), vertices, GL_STATIC_DRAW);

    glGenBuffers(1, &g_model.ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_model.ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, idxSize * sizeof(GLuint), indices, GL_STATIC_DRAW);

    g_model.indexCount = idxSize;

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (const GLvoid *) 0);

    // delete[] vertices;
    // delete[] indices;
    return g_model.vbo != 0 && g_model.ibo != 0 && g_model.vao != 0;
}


void Points::cleanup() {

}

void Points::draw() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(g_shaderProgram);
    glBindVertexArray(g_model.vao);

    glDrawElements(GL_TRIANGLES, g_model.indexCount, GL_UNSIGNED_INT, NULL);
    /*for (Point2D p: wPoints) {
        auto model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(p.x, p.y, 0.0f));
        model = glm::scale(model, glm::vec3(0.01, 0.01, 0.01));
        glUniformMatrix4fv(g_uMV, 1, GL_FALSE, glm::value_ptr(model));
    }*/
}
