//
// Created by korpatiy on 02.04.2021.
//

#include <stb_image.h>
#include "../Headers/Body.h"
#include "MainCreator.h"

using namespace std;

bool Body::createShaderProgram() {
    g_shaderProgram = 0;

    const GLchar vsh[] =
            "#version 330\n"
            ""
            "layout(location = 0) in vec2 a_pos;"
            ""
            "uniform mat4 u_mvp;"
            "uniform mat4 u_mv;"
            ""
            "out vec3 v_normal;"
            "out vec3 v_pos;"
            "out vec2 v_texCoord;"
            ""
            "float a = -0.7;"
            ""
            "float f(vec2 p) { return a * length(p) * sin(length(p)); }"
            "vec3 grad(vec2 p)"
            "{"
            "float len = length(p);"
            "if (len == 0) return vec3(0,1,0);"
            "else"
            "{"
            "float coefficient = a * (sin(len) / len + cos(len));"
            "float dx = -p.x * coefficient;"
            "float dy = -p.y * coefficient;"
            "return vec3(dx, 1.0, dy);"
            "}"
            "}"
            ""
            "void main()"
            "{"
            "    float y = f(a_pos);"
            "    vec4 p0 = vec4(a_pos[0], y, a_pos[1], 1.0);"
            "    v_normal = transpose(inverse(mat3(u_mv))) * normalize(grad(a_pos));"
            "    v_pos = vec3(u_mv * p0);"
            "    gl_Position = u_mvp * p0;"
            "    v_texCoord = a_pos / 5;"
            "}";

    const GLchar fsh[] =
            "#version 330\n"
            ""
            "in vec3 v_normal;"
            "in vec3 v_pos;"
            "in vec2 v_texCoord;"
            ""
            "layout(location = 0) out vec4 o_color;"
            ""
            "uniform sampler2D u_map0;"
            "uniform sampler2D u_map1;"
            ""
            "void main()"
            "{"
            "   float S = 20;"
            "   vec3 color = vec3(1, 0, 0);"
            "   vec3 n = normalize(v_normal);"
            "   vec3 E = vec3(0, 0, 0);"
            "   vec3 L = vec3(5,5,0);"
            "   vec3 l = normalize(v_pos - L);"
            "   float d = max(dot(n, -l), 0.3);"
            "   vec3 e = normalize(E - v_pos);"
            "   vec3 h = normalize(-l + e);"
            "   float s = pow(max(dot(n, h), 0.0), S);"
            "   vec4 texColor = mix(texture(u_map0, v_texCoord), texture(u_map1, v_texCoord), 0.5);"
            "   o_color = vec4(texColor * d + s * vec4(1.0));"
            //"    o_color = texture(u_map0, v_texCoord);"
            //"   o_color = vec4(color * d +s * vec3(1.0, 1.0, 1.0),1.0);"
            //"   o_color = vec4(abs(n),1);"
            "}";

    GLuint vertexShader, fragmentShader;

    vertexShader = createShader(vsh, GL_VERTEX_SHADER);
    fragmentShader = createShader(fsh, GL_FRAGMENT_SHADER);

    g_shaderProgram = createProgram(vertexShader, fragmentShader);

    g_uMVP = glGetUniformLocation(g_shaderProgram, "u_mvp");
    g_uVM = glGetUniformLocation(g_shaderProgram, "u_mv");

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return g_shaderProgram != 0;
}

bool Body::createModel() {
    size_t n = 100;
    size_t vertSize = 2 * n * n;
    size_t idxSize = (n - 1) * (n - 1) * 6;

    auto *vertices = new GLfloat[vertSize];
    for (size_t i = 0; i < n; i++) {
        for (size_t j = 0; j < n; j++) {
            size_t idx = 2 * (i * n + j);
            vertices[idx] = 0.1f * (j - n / 2.0f);
            vertices[idx + 1] = 0.1f * (i - n / 2.0f);
        }
    }

    auto *indices = new GLuint[idxSize];
    for (size_t i = 0; i < n - 1; i++) {
        for (size_t j = 0; j < n - 1; j++) {
            size_t idx = 6 * (i * (n - 1) + j);
            indices[idx] = n * i + j;
            indices[idx + 1] = n * i + j + 1;
            indices[idx + 2] = n * (i + 1) + j + 1;
            indices[idx + 3] = n * (i + 1) + j + 1;
            indices[idx + 4] = n * (i + 1) + j;
            indices[idx + 5] = n * i + j;
        }
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
    //glEnableVertexAttribArray(1);
    //glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (const GLvoid *) (3 * sizeof(GLfloat)));

    delete[] vertices;
    delete[] indices;
    return g_model.vbo != 0 && g_model.ibo != 0 && g_model.vao != 0;
}

bool Body::createTexture() {
    for (size_t i = 0; i < texCount; i++) {
        GLsizei texW, texH, nrChannels;
        GLvoid *image;
        glGenTextures(1, &textures[i]);
        glBindTexture(GL_TEXTURE_2D, textures[i]);

        string way = "../textures/texture" + to_string(i) + ".jpg";
        image = stbi_load(way.c_str(), &texW, &texH, &nrChannels, 0);
        if (image) {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texW, texH, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
            glGenerateMipmap(GL_TEXTURE_2D);
        } else {
            cout << "Failed to load texture" << std::endl;
            return false;
        }
        stbi_image_free(image);
        // Установка параметров наложения текстуры
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        // Установка параметров фильтрации текстуры
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        mapLocation[i] = glGetUniformLocation(g_shaderProgram, ("u_map" + to_string(i)).c_str());
    }
    return true;
}

void Body::draw(GLfloat delta) {
    // Clear color buffer.
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    for (int i = 0; i < texCount; ++i) {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, textures[i]);
        glUniform1i(mapLocation[i], i);
    }

    glUseProgram(g_shaderProgram);
    glBindVertexArray(g_model.vao);

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
    model = glm::rotate(model, glm::radians(delta), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::scale(model, glm::vec3(2.0f));


    glm::mat4 view = lookAt(glm::vec3(10.0f, 40.0f, 30.0f),
                            glm::vec3(0.0f, 0.0f, 0.0f),
                            glm::vec3(0.0f, 1.0f, 0.0f));

    glUniformMatrix4fv(g_uMVP, 1, GL_FALSE, glm::value_ptr(projection * view * model));
    glUniformMatrix4fv(g_uVM, 1, GL_FALSE, glm::value_ptr(view * model));

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
    for (unsigned int &texture : textures) {
        glDeleteTextures(1, &texture);
    }
}
