#pragma once

#include <GLFW/glfw3.h>
#include "config.h"

//<
//<
//<

struct glfwCallbackData
{
    void *pCamera;
    int animation_mode;
    RenderSetting &setting;
    DisplayOption &option;
};

void glfwindow_key_cb(GLFWwindow *window, int key, int scancode, int action, int mods);

void glfwindow_mouseMotion_cb(GLFWwindow *window, double x, double y);

void glfwindow_mouseButton_cb(GLFWwindow *window, int button, int action, int mods);
