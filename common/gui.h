#pragma once

#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"


namespace GUI
{
    // Setup Platform/Renderer bindings
    void Setup(GLFWwindow* window, const char* glsl_version);

    // Cleanup
    void CleanUp();
    
    // Start the Dear ImGui frame
    void BeginFrame(void);
    
    // Render the GUI element 
    void EndFrame(void);

    void Dialog(const char *title, const char *msg);
    
} //< end namespace
