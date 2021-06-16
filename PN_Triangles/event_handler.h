#pragma once

#include <glfw3.h>

//<
//<
//<

void glfwindow_key_cb(GLFWwindow *window, int key, int scancode, int action, int mods);

void glfwindow_mouseMotion_cb(GLFWwindow *window, double x, double y);

void glfwindow_mouseButton_cb(GLFWwindow *window, int button, int action, int mods);
