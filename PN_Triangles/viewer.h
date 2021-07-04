
#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "config.h"

class MeshBin;
class Camera;

class Viewer;
//void drawUI(RenderSetting &setting, DisplayOption & displayOption);
void drawUI(Viewer &viewer);

//<
//< It serve as a container for modules such as
//< 1. window management
//< 2. keyboard & mouse event handling 
//< 3. shader program management
//< 4. lighting & camera arrangement
//< 5. rendering & updating
//< 6. interact with user
//<
class Viewer
{
private:

    int m_window_width = 1280;
    int m_window_height = 720;
    GLFWwindow *m_window{nullptr};

    std::string m_window_title{ "PN Triangle" };
    bool m_capture_colorbuffer{false};

    RenderSetting &m_setting; // = GetRenderSetting();
    DisplayOption &m_option;  // = GetDisplayOption();

    friend void drawUI(Viewer &viewer);

private:

    GLuint programID;
    GLuint tessProgramID;


    GLuint lightID;
    GLuint matrixID;
    GLuint modelMatrixID;
    GLuint viewMatrixID;
    GLuint projectionMatrixID;
    GLuint mesh_color_ID;

    GLuint tessLightID;

    GLuint tess_mesh_color_ID;
    GLuint tessMatrixID;
    GLuint tessModelMatrixID;
    GLuint tessViewMatrixID;
    GLuint tessProjectionMatrixID;
    GLfloat tessellationLevelInnerID;
    GLfloat tessellationLevelOuterID;

    bool  m_wireframeMode{false};
    bool  m_tessellationEnable{false};
    float m_tessellationLevel{1.0f};

public:
    Viewer() = delete;
    ~Viewer()
    {
        glDeleteProgram(programID);
        glDeleteProgram(tessProgramID);
        glfwTerminate();
    }

    Viewer(std::string title, int width, int height)
        : m_window_title(title)
        , m_window_width(width)
        , m_window_height(height)
        , m_setting(GetRenderSetting())
        , m_option(GetDisplayOption())
    {
        initWindow();
        initOpenGLShaders();
    }

    void SetModel(std::string modelPath)
    {
        m_objPath = modelPath;
    }

    void Run();

    void SaveScreen(const std::string filename = "pic.tga");
    void SaveImageSequence(const std::string dir = "imageseq");

private:
    std::string m_objPath;
    int initWindow(void);
    void initOpenGLShaders(void);
    void render(const MeshBin & m_meshBin, const Camera &m_camera);
};

//struct RenderSetting
//{
//    // the input of current setting
//    int  onlyUseForIndentification;
//    int  width;
//    int  height;
//    int  bvhBuilderType;
//    bool enableTess;
//};
//
//struct DisplayOption
//{
//    bool wireframe;
//
//    DisplayOption()
//        : wireframe(false)
//    { }
//};
//
