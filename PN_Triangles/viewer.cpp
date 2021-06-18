#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <math.h>
#include <GL/glew.h>
#include <glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <string>
#include <fstream>
#include <vector>

#include "common/objloader.h"
#include "shaderUtility.h"

#include "camera.h" 
#include "event_handler.h" 



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

    //glm::mat4 gProjectionMatrix;
    //glm::mat4 gViewMatrix;
    
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

    bool  shouldTessellateModel = false;
    bool  shouldDisplayWireframeMode = false;
    float tessellationLevel = 1.0f;

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

private:
    int initWindow(void);
    void initOpenGLShaders();
    void render(const MeshBin & m_meshBin, const Camera &m_camera);
};

void Viewer::render(const MeshBin & m_meshBin, const Camera &m_camera)
{
    glm::mat4 gViewMatrix = m_camera.viewMatrix();
    glm::mat4 gProjectionMatrix = m_camera.projMatrix();
    glm::vec3 lightPos = glm::vec3(20.0f, 20.0f, 20.0f);
    glm::vec3 mesh_color = glm::vec3(0.9f, 0.5f, 3.0f);
    glm::mat4x4 modelMatrix = glm::mat4(1.0);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_CULL_FACE);

    if(shouldDisplayWireframeMode)
    {
        glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
    }
    else
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    for (int i = 0; i < m_meshBin.size(); ++i)
    {
        if(!shouldTessellateModel)
        {
            glUseProgram(programID);

            glUniform3f(lightID, lightPos.x, lightPos.y, lightPos.z);
            glUniform3f(mesh_color_ID, mesh_color.x, mesh_color.y, mesh_color.z);
            glUniformMatrix4fv(viewMatrixID, 1, GL_FALSE, &gViewMatrix[0][0]);
            glUniformMatrix4fv(projectionMatrixID, 1, GL_FALSE, &gProjectionMatrix[0][0]);
            glUniformMatrix4fv(modelMatrixID, 1, GL_FALSE, &modelMatrix[0][0]);

            glBindVertexArray( m_meshBin.vao(i) );
            glDrawArrays( GL_TRIANGLES, 0, m_meshBin.vertex_num(i) );
        }
        else
        {
            glUseProgram(tessProgramID);
            {
                glUniform3f(tessLightID, lightPos.x, lightPos.y, lightPos.z);
                glUniform3f(tess_mesh_color_ID, mesh_color.x, mesh_color.y, mesh_color.z);
                glUniformMatrix4fv(tessViewMatrixID, 1, GL_FALSE, &gViewMatrix[0][0]);
                glUniformMatrix4fv(tessProjectionMatrixID, 1, GL_FALSE, &gProjectionMatrix[0][0]);
                glUniformMatrix4fv(tessModelMatrixID, 1, GL_FALSE, &modelMatrix[0][0]);

                glUniform1f(tessellationLevelInnerID, tessellationLevel);
                glUniform1f(tessellationLevelOuterID, tessellationLevel);

                glPatchParameteri(GL_PATCH_VERTICES, 3);
                glBindVertexArray( m_meshBin.vao(i) );
                glDrawArrays( GL_PATCHES, 0, m_meshBin.vertex_num(i) );
            }
        }
    }
    glBindVertexArray(0);
    glUseProgram(0);

    glfwSwapBuffers(m_window);
    glfwPollEvents();
}
void Viewer::initOpenGLShaders()
{
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_CULL_FACE);

    programID = loadStandardShaders("shaders/Standard.vert", "shaders/Standard.frag");
    tessProgramID = loadTessShaders("shaders/Tessellation.vs.glsl", "shaders/Tessellation.tc.glsl", "shaders/Tessellation.te.glsl", "shaders/Tessellation.fs.glsl");

    matrixID = glGetUniformLocation(programID, "MVP");
    modelMatrixID = glGetUniformLocation(programID, "M");
    viewMatrixID = glGetUniformLocation(programID, "V");
    projectionMatrixID = glGetUniformLocation(programID, "P");
    lightID = glGetUniformLocation(programID, "lightPosition_worldspace");
    mesh_color_ID = glGetUniformLocation(programID, "mesh_color");

    tessModelMatrixID = glGetUniformLocation(tessProgramID, "M");
    tessViewMatrixID = glGetUniformLocation(tessProgramID, "V");
    tessProjectionMatrixID = glGetUniformLocation(tessProgramID, "P");
    tessLightID = glGetUniformLocation(tessProgramID, "lightPosition_worldspace");
    tessellationLevelInnerID = glGetUniformLocation(tessProgramID, "tessellationLevelInner");
    tessellationLevelOuterID = glGetUniformLocation(tessProgramID, "tessellationLevelOuter");

    tess_mesh_color_ID = glGetUniformLocation(tessProgramID, "mesh_color");

}

int Viewer::initWindow()
{
    if(!glfwInit())
    {
        throw std::runtime_error("Failed to initialize GLFW !");
        return -1;
    }

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    m_window = glfwCreateWindow(m_window_width, m_window_height, m_window_title.c_str(), NULL, NULL);
    if(m_window == NULL)
    {
        throw std::runtime_error("Failed to open GLFW window!");
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(m_window); //< pick context of window into current thread

    glfwSetCursorPos(m_window, m_window_width / 2.f, m_window_height / 2.f);
    glfwSetKeyCallback(m_window, glfwindow_key_cb);
    glfwSetMouseButtonCallback(m_window, glfwindow_mouseButton_cb);
    glfwSetCursorPosCallback(m_window, glfwindow_mouseMotion_cb);

    glewExperimental = true;
    if(glewInit() != GLEW_OK)
    {
        throw std::runtime_error("Failed to initialize GLEW !");
        return -1;
    }
    return 0;
}

#if 0
int test(int argc, char **argv)
{
    std::string modelPath;
    char **filename = nullptr; 
    for (int i = 0; i < argc; ++i)
    {
        if (i + 1 < argc && strcmp(argv[i], "-obj") == 0)
            filename = argv + i + 1, ++i;
        else
        {
        }
    }

    if ( filename != nullptr )
    {
        modelPath = *filename;
    }
    else
    {
        modelPath = "Model/Suzanne.obj";
    }

    int errorCode = initWindow();
    if(errorCode != 0)
    {
        return errorCode;
    }

    Camera camera(glm::vec3{0.f}, 5.f);
    glfwCallbackData cb{ &camera };
    glfwSetWindowUserPointer(window, &cb);

    initOpenGLShaders();

    MeshBin meshes{modelPath};

    gProjectionMatrix = camera.projMatrix();

    gViewMatrix = camera.viewMatrix();

    do
    {
        render_scene(meshes, camera);

    } while(glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(window) == 0);

    cleanup();

    return 0;
}
#endif
