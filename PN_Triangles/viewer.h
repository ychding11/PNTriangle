#include <iostream>
#include <GL/glew.h>
#include <glfw3.h>
#include <glm/glm.hpp>
#include <string>
#include <vector>


class MeshBin;
class Camera;

//<
//< It combines function of lighting, window management, rendering
//<
class Viewer
{
private:

    int m_window_width = 1280;
    int m_window_height = 720;
    GLFWwindow *m_window{nullptr};

    std::string m_window_title{ "PN Triangle" };

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
    {
        initWindow();
        initOpenGLShaders();
    }

    void SetModel(std::string modelPath)
    {
        m_objPath = modelPath;
    }

    void Run();

private:
    std::string m_objPath;
    int initWindow(void);
    void initOpenGLShaders();
    void render(const MeshBin & m_meshBin, const Camera &m_camera);
};
