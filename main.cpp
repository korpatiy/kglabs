#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "Headers/Body.h"
#include "Headers/Bezier.h"
#include "Headers/Points.h"
#include "Headers/Curve.h"

#define STB_IMAGE_IMPLEMENTATION

#include <stb_image.h>
#include <algorithm>

using namespace std;

GLFWwindow *g_window;

Body body;
Points points;
Curve curve;
bool drawBody = false;

void calculatePoints();

void reshape(GLFWwindow *window, int width, int height) {
    // body.projection = glm::perspective(glm::radians(45.0f), GLfloat(width) / GLfloat(height), 0.1f, 100.f);
    glViewport(0, 0, width, height);
}

bool initOpenGL() {
    // Initialize GLFW functions.
    if (!glfwInit()) {
        cout << "Failed to initialize GLFW" << endl;
        return false;
    }

    // Request OpenGL 3.3 without obsoleted functions.
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create window.
    g_window = glfwCreateWindow(800, 600, "OpenGL Test", NULL, NULL);
    if (g_window == NULL) {
        cout << "Failed to open GLFW window" << endl;
        glfwTerminate();
        return false;
    }

    // Initialize OpenGL context with.
    glfwMakeContextCurrent(g_window);

    // Set internal GLEW variable to activate OpenGL core profile.
    glewExperimental = true;

    // Initialize GLEW functions.
    if (glewInit() != GLEW_OK) {
        cout << "Failed to initialize GLEW" << endl;
        return false;
    }

    // Ensure we can capture the escape key being pressed.
    glfwSetInputMode(g_window, GLFW_STICKY_KEYS, GL_TRUE);

    // Set callback for framebuffer resizing event.
    glfwSetFramebufferSizeCallback(g_window, reshape);

    return true;
}

void tearDownOpenGL() {
    // Terminate GLFW.
    glfwTerminate();
}

void mouse_button_callback(GLFWwindow *window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        int width = 0, height = 0;
        double x_pos, y_pos;
        glfwGetCursorPos(window, &x_pos, &y_pos);
        glfwGetWindowSize(window, &width, &height);

        x_pos = x_pos / width * 2. - 1.;
        y_pos = 1. - y_pos / height * 2.;

        points.base_points.emplace_back(x_pos, y_pos);

        if (points.base_points.size() < 2)
            return;
        if (points.base_points.size() == 2) {
            curve.bezier_points = points.base_points;
            curve.createCurveModel();
            return;
        }
        calculatePoints();
        curve.createCurveModel();
    }
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
        if (points.base_points.size() < 2)
            return;
        drawBody = true;
        if (points.base_points.size() == 2) {
            auto vec1 = curve.bezier_points[0] - curve.bezier_points[1];
            auto vec2 = curve.bezier_points[1] - curve.bezier_points[0];
            vec1.normalize();
            vec2.normalize();
            auto normal = vec1 + vec2;
            normal.normalize();
            body.normals.emplace_back(normal.x, normal.y);
            body.createModel(curve.bezier_points);
            return;
        }
        auto vec1 = curve.bezier_points[0] - curve.bezier_points[2];
        auto vec2 = curve.bezier_points[2] - curve.bezier_points[0];
        vec1.normalize();
        vec2.normalize();
        auto normal = vec1 + vec2;
        normal.normalize();
        body.normals.emplace_back(normal.x, normal.y);

        for (int i = 0; i < curve.bezier_points.size() - 2; i++) {
            vec1 = curve.bezier_points[i] - curve.bezier_points[i + 2];
            vec2 = curve.bezier_points[i + 2] - curve.bezier_points[i];
            vec1.normalize();
            vec2.normalize();
            normal = vec1 + vec2;
            body.normals.emplace_back(normal.x, normal.y);
        }
        body.normals.push_back(body.normals[body.normals.size() - 1]);
        body.createModel(curve.bezier_points);
    }
}

void calculatePoints() {
    vector<Segment> segment;
    tbezierSO0(points.base_points, segment);
    curve.bezier_points.clear();

    for (auto s : segment) {
        for (int i = 0; i < RESOLUTION; ++i) {
            Point2D point = s.calc((double) i / (double) RESOLUTION);
            curve.bezier_points.push_back(point);
        }
    }
}

bool init() {
    // Set initial color of color buffer to white.
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

    glEnable(GL_DEPTH_TEST);

    return points.createPointShaderProgram() && points.createPointModel()
           && curve.createCurveShaderProgram() && body.createShaderProgram();
}


int main() {
    // Initialize OpenGL
    if (!initOpenGL())
        return -1;

    // Initialize graphical resources.
    bool isOk = init();

    glfwSetMouseButtonCallback(g_window, mouse_button_callback);

    if (isOk) {

        // Main loop until window closed or escape pressed.
        int delta = 0;
        while (glfwGetKey(g_window, GLFW_KEY_ESCAPE) != GLFW_PRESS
               && glfwWindowShouldClose(g_window) == 0) {

            // Draw scene.
            points.drawPoints();
            curve.drawCurve();

            if (delta > 360) delta = 0;
            if (drawBody)
                body.draw(delta);
            delta++;

            // Swap buffers.
            glfwSwapBuffers(g_window);
            // Poll window events.
            glfwPollEvents();
        }
    }

    // Cleanup graphical resources.
    points.cleanup();
    curve.cleanup();
    body.cleanup();
    // Tear down OpenGL.
    tearDownOpenGL();

    return isOk ? 0 : -1;
}
