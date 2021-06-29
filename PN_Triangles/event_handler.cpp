#pragma once

#include <iostream>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <string>

#include "camera.h" 
#include "event_handler.h" 

void glfwindow_key_cb(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    if (action == GLFW_RELEASE)
    {
        glfwCallbackData *cb = static_cast<glfwCallbackData*>(glfwGetWindowUserPointer(window));
        assert(cb);

        RenderSetting &setting = cb->setting;
        DisplayOption &option  = cb->option;

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
        {
            setting.enableTess = !setting.enableTess;
            setting.innerTessLevel = { 1.f, 1.f, 1.f };
            setting.outerTessLevel = { 1.f, 1.f, 1.f, 1.f };

            break;
        }
        case GLFW_KEY_W:
        {
            option.wireframe = !option.wireframe;
            break;
        }
        case GLFW_KEY_LEFT_BRACKET:
        {
            if (setting.enableTess)
            {
                setting.innerTessLevel.x--;
                setting.innerTessLevel.y--;
                setting.innerTessLevel.z--;

                setting.outerTessLevel.x--;
                setting.outerTessLevel.y--;
                setting.outerTessLevel.z--;
                setting.outerTessLevel.w--;
            }
            break;
        }
        case GLFW_KEY_RIGHT_BRACKET:
        {
            if (setting.enableTess)
            {
                setting.innerTessLevel.x++;
                setting.innerTessLevel.y++;
                setting.innerTessLevel.z++;

                setting.outerTessLevel.x++;
                setting.outerTessLevel.y++;
                setting.outerTessLevel.z++;
                setting.outerTessLevel.w++;
            }
            break;
        }

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

static ButtonState leftButton;
static ButtonState rightButton;
static ButtonState centerButton;
static glm::ivec2 lastMousePos{ -1,-1 };
static glm::ivec2 lastMousePosition{ -1,-1 };

static inline glm::ivec2 getMousePos(GLFWwindow *window)
{
    double x, y;
    glfwGetCursorPos(window, &x, &y);
    return glm::ivec2{ (int)x, (int)y };
}

//< suppose : window size == glfw frame buffer size
static inline glm::ivec2 getWindowSize(GLFWwindow *window)
{
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    return glm::ivec2{ width, height };
}

static void mouseButtonLeft(const glm::ivec2 &where, bool pressed)
{

}

//< rotate
static void mouseDragLeft(GLFWwindow *window, const glm::ivec2  &where, const glm::ivec2 &delta)
{
    glfwCallbackData *cb = static_cast<glfwCallbackData*>(glfwGetWindowUserPointer(window));
    assert(cb);
    Camera &camera = *reinterpret_cast<Camera*>(cb->pCamera);

    const glm::vec2 fraction = glm::vec2(delta) / glm::vec2(getWindowSize(window));
    camera.rotate(fraction);
}

//< pan
static void mouseDragCenter(GLFWwindow *window, const glm::ivec2  &where, const glm::ivec2 &delta)
{
    glfwCallbackData *cb = static_cast<glfwCallbackData*>(glfwGetWindowUserPointer(window));
    assert(cb);
    Camera &camera = *reinterpret_cast<Camera*>(cb->pCamera);

    const glm::vec2 fraction = glm::vec2(delta) / glm::vec2(getWindowSize(window));
    camera.pan(-fraction); //< relative motion
}

//< zoom in / out
static void mouseDragRight(GLFWwindow *window, const glm::ivec2  &where, const glm::ivec2 &delta)
{
    glfwCallbackData *cb = static_cast<glfwCallbackData*>(glfwGetWindowUserPointer(window));
    assert(cb);
    Camera &camera = *reinterpret_cast<Camera*>(cb->pCamera);

    const glm::vec2 fraction = glm::vec2(delta) / glm::vec2(getWindowSize(window));
    float scale = std::fabs(fraction.x) > std::fabs(fraction.y) ? fraction.x : fraction.y;
    camera.zoom(scale);
    printf("zoom in / out : %.4f\n", scale);
}

/*! callback for _moving_ the mouse to a new position */
void glfwindow_mouseMotion_cb(GLFWwindow *window, double x, double y)
{
    glm::ivec2 newMousePosition{ (int)x, (int)y };
    if (lastMousePosition != glm::ivec2(-1))
    {
        if (leftButton.isPressed)
            mouseDragLeft(window, newMousePosition, newMousePosition - lastMousePosition);
        if (centerButton.isPressed)
            mouseDragCenter(window, newMousePosition, newMousePosition - lastMousePosition);
        if (rightButton.isPressed)
            mouseDragRight(window, newMousePosition, newMousePosition - lastMousePosition);
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

