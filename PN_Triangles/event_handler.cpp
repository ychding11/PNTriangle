#pragma once

//#include <stdio.h>
#include <iostream>
//#include <stdlib.h>
//#include <math.h>
//#include <GL/glew.h>
#include <glfw3.h>
#include <glm/glm.hpp>
//#include <glm/gtc/matrix_transform.hpp>
//#include <glm/gtc/quaternion.hpp>
#include <string>
//#include <vector>

#include "camera.h" 
#include "event_handler.h" 




extern float tessellationLevel;
extern bool shouldTessellateModel;

extern bool shouldDisplayWireframeMode;


extern int window_width, window_height;


extern Camera camera;



void glfwindow_key_cb(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    if (action == GLFW_RELEASE)
    {
        switch (key)
        {
        case GLFW_KEY_LEFT:
            break;
        case GLFW_KEY_RIGHT:
            break;
        case GLFW_KEY_UP:
            break;
        case GLFW_KEY_DOWN:
            break;
        case GLFW_KEY_T:
            shouldTessellateModel = !shouldTessellateModel;
            tessellationLevel = 1.0f;
            break;
        case GLFW_KEY_W:
            shouldDisplayWireframeMode = !shouldDisplayWireframeMode;
            break;
        case GLFW_KEY_LEFT_BRACKET:
            if (shouldTessellateModel)
            {
                --tessellationLevel;
            }
            break;
        case GLFW_KEY_RIGHT_BRACKET:
            if (shouldTessellateModel)
            {
                ++tessellationLevel;
            }
            break;
        default:
            break;
        }
    }
}


struct ButtonState
{
    bool  isPressed{ false };
    glm::ivec2 posFirstPressed{ -1 };
    glm::ivec2 posLastSeen{ -1 };
    bool  shiftWhenPressed{ false };
    bool  ctrlWhenPressed{ false };
    bool  altWhenPressed{ false };
};

ButtonState leftButton;
ButtonState rightButton;
ButtonState centerButton;
glm::ivec2 lastMousePos{ -1,-1 };
glm::ivec2 lastMousePosition{ -1,-1 };

static inline glm::ivec2 getMousePos(GLFWwindow *window)
{
    double x, y;
    glfwGetCursorPos(window, &x, &y);
    return glm::ivec2{ (int)x, (int)y };
}

static inline glm::ivec2 getWindowSize()
{
    return glm::ivec2{ window_width, window_height };
}

static void mouseButtonLeft(const glm::ivec2 &where, bool pressed)
{

}

//< rotate
static void mouseDragLeft(const glm::ivec2  &where, const glm::ivec2 &delta)
{
    const glm::vec2 fraction = glm::vec2(delta) / glm::vec2(getWindowSize());
    camera.rotate(fraction);
}

//< pan
static void mouseDragCenter(const glm::ivec2  &where, const glm::ivec2 &delta)
{
    const glm::vec2 fraction = glm::vec2(delta) / glm::vec2(getWindowSize());
    camera.pan(-fraction); //< relative motion
}

//< zoom in / out
static void mouseDragRight(const glm::ivec2  &where, const glm::ivec2 &delta)
{
    const glm::vec2 fraction = glm::vec2(delta) / glm::vec2(getWindowSize());
    float scale = std::fabs(fraction.x) > std::fabs(fraction.y) ? fraction.x : fraction.y;
    camera.zoom(scale);
    printf("zoom in / out : %.4f\n", scale);
}

/*! callback for _moving_ the mouse to a new position */
void glfwindow_mouseMotion_cb(GLFWwindow *window, double x, double y)
{
    //OWLViewer *gw = static_cast<OWLViewer*>(glfwGetWindowUserPointer(window));
    //assert(gw);

    glm::ivec2 newMousePosition{ (int)x, (int)y };
    if (lastMousePosition != glm::ivec2(-1))
    {
        if (leftButton.isPressed)
            mouseDragLeft(newMousePosition, newMousePosition - lastMousePosition);
        if (centerButton.isPressed)
            mouseDragCenter(newMousePosition, newMousePosition - lastMousePosition);
        if (rightButton.isPressed)
            mouseDragRight(newMousePosition, newMousePosition - lastMousePosition);
    }
    lastMousePosition = newMousePosition;
}

void glfwindow_mouseButton_cb(GLFWwindow *window, int button, int action, int mods)
{
    const bool pressed = (action == GLFW_PRESS);
    lastMousePos = getMousePos(window);
    switch (button)
    {
    case GLFW_MOUSE_BUTTON_LEFT:
        leftButton.isPressed = pressed;
        leftButton.shiftWhenPressed = (mods & GLFW_MOD_SHIFT);
        leftButton.ctrlWhenPressed = (mods & GLFW_MOD_CONTROL);
        leftButton.altWhenPressed = (mods & GLFW_MOD_ALT);
        //mouseButtonLeft(lastMousePos, pressed);
        break;
    case GLFW_MOUSE_BUTTON_MIDDLE:
        centerButton.isPressed = pressed;
        centerButton.shiftWhenPressed = (mods & GLFW_MOD_SHIFT);
        centerButton.ctrlWhenPressed = (mods & GLFW_MOD_CONTROL);
        centerButton.altWhenPressed = (mods & GLFW_MOD_ALT);
        //mouseButtonCenter(lastMousePos, pressed);
        break;
    case GLFW_MOUSE_BUTTON_RIGHT:
        rightButton.isPressed = pressed;
        rightButton.shiftWhenPressed = (mods & GLFW_MOD_SHIFT);
        rightButton.ctrlWhenPressed = (mods & GLFW_MOD_CONTROL);
        rightButton.altWhenPressed = (mods & GLFW_MOD_ALT);
        //mouseButtonRight(lastMousePos, pressed);
        break;
    }
}

