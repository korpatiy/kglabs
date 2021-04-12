//
// Created by korpatiy on 02.04.2021.
//

#include <stb_image.h>
#include "../Headers/Body.h"
#include "../Headers/MainCreator.h"

using namespace std;

bool Body::createShaderProgram() {
    g_shaderProgram = 0;

    const GLchar vsh[] =
            "#version 330\n"
            ""
            "layout(location = 0) in vec3 a_pos;"
            "layout(location = 1) in vec3 a_normal;"
            ""
            "uniform mat4 u_mvp;"
            "uniform mat4 u_mv;"
            "uniform mat3 u_normal;"
            ""
            "out vec3 v_normal;"
            "out vec3 v_pos;"
            ""
            "void main()"
            "{"
            "    vec4 p0 = vec4(a_pos, 1.0);"
            "    v_normal = u_normal * a_normal;"
            //"    v_normal = u_normal * a_pos;"
            "    v_pos = vec3(u_mv * p0);"
            "    gl_Position = u_mvp * p0;"
            "}";

    const GLchar fsh[] =
            "#version 330\n"
            ""
            "in vec3 v_normal;"
            "in vec3 v_pos;"
            ""
            "layout(location = 0) out vec4 o_color;"
            ""
            ""
            "void main()"
            "{"
            "   float S = 60;"
            "   vec3 color = vec3(1, 0, 0);"
            "   vec3 n = normalize(v_normal);"
            "   vec3 E = vec3(0, 0, 0);"
            "   vec3 L = vec3(5,5,0);"
            "   vec3 l = normalize(v_pos - L);"
            "   float d = max(dot(n, -l), 0.3);"
            "   vec3 e = normalize(E - v_pos);"
            "   vec3 h = normalize(-l + e);"
            "   float s = pow(max(dot(n, h), 0.0), S) * 0.5;"
            "   vec3 f_color = color * d +s * vec3(1.0, 1.0, 1.0);"
            //"   o_color = vec4(f_color, 1.0);"
            "   o_color = vec4(pow(f_color, vec3(1.0 / 2.2)), 1.0);"
            "}";

    GLuint vertexShader, fragmentShader;

    vertexShader = createShader(vsh, GL_VERTEX_SHADER);
    fragmentShader = createShader(fsh, GL_FRAGMENT_SHADER);

    g_shaderProgram = createProgram(vertexShader, fragmentShader);

    g_uNorm = glGetUniformLocation(g_shaderProgram, "u_normal");
    g_uMVP = glGetUniformLocation(g_shaderProgram, "u_mvp");
    g_uMV = glGetUniformLocation(g_shaderProgram, "u_mv");

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return g_shaderProgram != 0;
}

bool Body::createModel(const std::vector<Point2D> &points) {
    const int rev = 128;

    const int n = points.size();
    const int vertSize = 6 * n * rev;
    const int idxSize = 6 * (n - 1) * rev;
    auto *vertices = new GLfloat[vertSize];
    auto *indices = new GLuint[idxSize];

    auto v = points[1] - points[0];
    v.normalize();
    vertices[0] = points[0].x;
    vertices[1] = points[0].y;
    vertices[2] = 0.0f;
    vertices[3] = -v.y;
    vertices[4] = v.x;
    vertices[5] = 0.0f;

    v = points[n - 2] - points[n - 1];
    v.normalize();
    int step = 6 * n;
    vertices[step - 6] = points[n - 1].x;
    vertices[step - 5] = points[n - 1].y;
    vertices[step - 4] = 0.0f;
    vertices[step - 3] = v.y;
    vertices[step - 2] = -v.x;
    vertices[step - 1] = 0.0f;

    for (int i = 6, k = 1; i < (n - 1) * 6; i += 6, k++) {
        auto v1 = points[k] - points[k - 1];
        v1.normalize();
        auto v2 = points[k + 1] - points[k];
        v2.normalize();
        auto normal = v1 + v2;
        normal.normalize();
        vertices[i] = points[k].x;
        vertices[i + 1] = points[k].y;
        vertices[i + 2] = 0.0f;
        vertices[i + 3] = -normal.y;
        vertices[i + 4] = normal.x;
        vertices[i + 5] = 0.0f;
    }

    auto model = glm::mat4(1.0f);
    auto rad = 360.0f / rev;
    auto rotated = glm::rotate(model, glm::radians(rad), glm::vec3(1.0, 0.0, 0.0));
    for (int i = step; i < vertSize; i += step) {
        for (int j = 0; j < step; j += 6) {
            size_t idx = i + j - step;
            auto resultPos = rotated * glm::vec4(vertices[idx], vertices[idx + 1], vertices[idx + 2], 1.0f);
            auto resultNormal = rotated * glm::vec4(vertices[idx + 3], vertices[idx + 4], vertices[idx + 5], 1.0f);
            vertices[i + j] = resultPos.x;
            vertices[i + j + 1] = resultPos.y;
            vertices[i + j + 2] = resultPos.z;
            vertices[i + j + 3] = resultNormal.x;
            vertices[i + j + 4] = resultNormal.y;
            vertices[i + j + 5] = resultNormal.z;
        }
    }

    int i, k;
    for (i = 0, k = 0; i < (n - 1) * (rev - 1) * 6; k++) {
        for (int j = 0; j < n - 1; j++, i += 6, k++) {
            indices[i] = k;
            indices[i + 1] = k + n;
            indices[i + 2] = k + 1;
            indices[i + 3] = k + n;
            indices[i + 4] = k + 1 + n;
            indices[i + 5] = k + 1;
        }
    }

    for (int c = 0, k = n * (rev - 1); c < n - 1; c++, i += 6) {
        indices[i] = c;
        indices[i + 1] = c + k + 1;
        indices[i + 2] = c + k;
        indices[i + 3] = c;
        indices[i + 4] = c + 1;
        indices[i + 5] = c + k + 1;
    }
    /*for (int i = 1; i < points.size() - 1; i++) {
        auto vec1 = points[i] - points[i - 1];
        auto vec2 = points[i + 1] - points[i];
        vec1.normalize();
        vec2.normalize();
        auto normal = vec1 + vec2;
        swap(normal.x, normal.y);
        normal.x *= -1.0f;
        normals.emplace_back(normal.x, normal.y);
    }
    normals.push_back(normals[normals.size() - 1]);
    auto v = points[n - 2] - points[n - 1];
    v.normalize();
    v.x *= -1.0f;
    normals.emplace_back(v.x, v.y);
    int x = normals.size();

    auto *vertices = new GLfloat[vertSize];
    for (size_t i = 0; i <= rev; i++) {
        auto model = glm::mat4(1.0f);
        auto rad = 360.0f * i / rev;
        auto rotated = glm::rotate(model, glm::radians(rad), glm::vec3(1.0, 0.0, 0.0));
        for (size_t j = 0; j < n; j++) {
            size_t idx = 6 * (i * n + j);
            auto resultPos = rotated * glm::vec4(points[j].x, points[j].y, 0.0f, 1.0f);
            auto resultNormal = rotated * glm::vec4(normals[i].x, normals[i].y, 0.0f, 1.0f);
            vertices[idx] = resultPos.x;
            vertices[idx + 1] = resultPos.y;
            vertices[idx + 2] = resultPos.z;
            vertices[idx + 3] = resultNormal.x;
            vertices[idx + 4] = resultNormal.y;
            vertices[idx + 5] = resultNormal.z;
        }
    }

    auto *indices = new GLuint[idxSize];
    for (size_t i = 0; i < rev; i++) {
        for (size_t j = 0; j < n - 1; j++) {
            size_t idx = 6 * (i * n + j);
            indices[idx] = n * i + j;
            indices[idx + 1] = n * i + (j + 1);
            indices[idx + 2] = n * (i + 1) + j;
            indices[idx + 3] = n * (i + 1) + j;
            indices[idx + 4] = n * (i + 1) + (j + 1);
            indices[idx + 5] = n * i + (j + 1);
        }
    }*/

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
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (const GLvoid *) 0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (const GLvoid *) (3 * sizeof(GLfloat)));

    delete[] vertices;
    delete[] indices;
    return g_model.vbo != 0 && g_model.ibo != 0 && g_model.vao != 0;
}

void Body::draw(GLfloat delta) {
    // Clear color buffer.
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(g_shaderProgram);
    glBindVertexArray(g_model.vao);

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
    model = glm::rotate(model, glm::radians(delta), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::scale(model, glm::vec3(5.0f));

    glm::mat4 view = lookAt(glm::vec3(10.0f, 0.0f, 30.0f),
                            glm::vec3(0.0f, 0.0f, 0.0f),
                            glm::vec3(0.0f, 1.0f, 0.0f));

    glUniformMatrix3fv(g_uNorm, 1, GL_FALSE, glm::value_ptr(glm::transpose(glm::inverse(glm::mat3(view * model)))));
    glUniformMatrix4fv(g_uMVP, 1, GL_FALSE, glm::value_ptr(projection * view * model));
    glUniformMatrix4fv(g_uMV, 1, GL_FALSE, glm::value_ptr(view * model));

    glDrawElements(GL_TRIANGLES, g_model.indexCount, GL_UNSIGNED_INT, NULL);
}

void Body::cleanup() {
    if (g_shaderProgram != 0)
        glDeleteProgram(g_shaderProgram);
    if (g_model.vbo != 0)
        glDeleteBuffers(1, &g_model.vbo);
    if (g_model.ibo != 0)
        glDeleteBuffers(1, &g_model.ibo);
    if (g_model.vao != 0)
        glDeleteVertexArrays(1, &g_model.vao);
}
