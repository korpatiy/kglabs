#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "Headers/Body.h"
#include "Headers/Bezier.h"
#include "Headers/Points.h"

#define STB_IMAGE_IMPLEMENTATION

#include <stb_image.h>

using namespace std;

GLFWwindow *g_window;

Body body;
Points points;


void reshape(GLFWwindow *window, int width, int height) {
    body.projection = glm::perspective(glm::radians(45.0f), GLfloat(width) / GLfloat(height), 0.1f, 100.f);
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

vector<float> buildModelPts(vector<Segment> splines, vector<Point2D> windowPts) {
    assert(splines.size() + 1 == windowPts.size());
    vector<float> anspts;
    int width, height = 0;
    glfwGetWindowSize(g_window, &width, &height);

    for (auto s : splines) {
        for (int i = 0; i < RESOLUTION; ++i) {
            Point2D p = s.calc((double) i / (double) RESOLUTION);
            anspts.push_back(2 * p.x / width - 1);
            anspts.push_back(2 * p.y / height - 1);
        }
    }

    return anspts;
}

void mouse_button_callback(GLFWwindow *window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        int width = 0, height = 0;
        double x_pos, y_pos;
        glfwGetCursorPos(window, &x_pos, &y_pos);
        glfwGetWindowSize(g_window, &width, &height);

        points.wPoints.emplace_back(x_pos,  height - y_pos);

/*
       if (points.wPoints.size() >= 2) {
            tbezierSO0(points.wPoints, curve);
            points.mPoints = buildModelPts(curve, points.wPoints);
            points.createModel();
        }*/
        if (points.wPoints.size() >= 1) {
            points.createModel();
            points.draw();
            glfwSwapBuffers(g_window);
            glfwPollEvents();
        }
    }
}

bool init() {
    // Set initial color of color buffer to white.
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

    glEnable(GL_DEPTH_TEST);

    glfwSetMouseButtonCallback(g_window, mouse_button_callback);
    return points.createShaderProgram() && points.createModel();
}

int main() {
    // Initialize OpenGL
    if (!initOpenGL())
        return -1;

    // Initialize graphical resources.
    bool isOk = init();

    if (isOk) {

        // Main loop until window closed or escape pressed.
        int delta = 0;
        while (glfwGetKey(g_window, GLFW_KEY_ESCAPE) != GLFW_PRESS
               && glfwWindowShouldClose(g_window) == 0
               && glfwGetKey(g_window, GLFW_KEY_ENTER) != GLFW_PRESS) {

            // Draw scene.

            /*if (delta > 360) delta = 0;
            body.draw(delta);
            delta++;

            // Swap buffers.
            glfwSwapBuffers(g_window);*/
            // Poll window events.
            glfwPollEvents();
        }
    }

    // Cleanup graphical resources.
    body.cleanup();

    // Tear down OpenGL.
    tearDownOpenGL();

    return isOk ? 0 : -1;
}
