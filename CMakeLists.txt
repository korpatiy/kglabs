cmake_minimum_required(VERSION 3.17)
project(kglab2)

set(CMAKE_CXX_STANDARD 14)
find_package(OpenGL REQUIRED)
if(NOT OPENGL_FOUND)
    message("ERROR: OpenGL not found")
endif(NOT OPENGL_FOUND)
set(GL_LIBRARY GL GLU X11)

add_executable(kglab2 main.cpp Sources/Body.cpp Headers/Body.h Sources/MainCreator.cpp Headers/MainCreator.h Headers/Bezier.h Sources/Points.cpp Headers/Points.h Sources/Bezier.cpp Sources/Curve.cpp Headers/Curve.h)
target_link_libraries(kglab2 glfw GLEW libGLEW.so libGLU.so libGL.so)