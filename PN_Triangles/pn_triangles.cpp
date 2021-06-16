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

using namespace glm;
using namespace std;

const int window_width = 1280, window_height = 720;
GLFWwindow *window;


glm::mat4 gProjectionMatrix;
glm::mat4 gViewMatrix;
GLuint programID;
GLuint tessProgramID;


GLuint m_object_num = 0;
const GLuint m_max_object_num = 256;
GLuint m_vao_id[m_max_object_num] = {0};
GLuint m_vbo_id[m_max_object_num] = {0};
size_t m_vb_size[m_max_object_num] = {0};
size_t m_vertex_num[m_max_object_num] = {0};

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

float tessellationLevel = 1.0f;
bool shouldTessellateModel = false;

bool shouldDisplayWireframeMode = false;

int initWindow(void);
void initOpenGL(void);

static void glfwindow_key_cb(GLFWwindow* , int, int, int, int);

static void glfwindow_mouseMotion_cb(GLFWwindow *window, double x, double y);
static void glfwindow_mouseButton_cb(GLFWwindow *window, int button, int action, int mods);


void cleanup(void);

void create_vaos(std::vector<Mesh> &meshes);
void render_scene();

void initOpenGL()
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

void createObjects(const std::string &filename)
{
    std::vector<Mesh> meshes;
    load_obj(filename, "Model/", meshes);
    create_vaos(meshes);
}


void create_vaos(std::vector<Mesh> &meshes)
{
    for (int i = 0; i < meshes.size(); ++i)
    {
        GLenum errorCheckValue = glGetError();

        m_vb_size[m_object_num] = meshes[i].vertices.size() * sizeof(SimpleVertex);
        m_vertex_num[m_object_num] = meshes[i].vertices.size();

        const size_t vertexStride = sizeof(SimpleVertex);
        const size_t normalOffset =  sizeof(meshes[i].vertices[0].position);

        glGenVertexArrays(1, &m_vao_id[m_object_num]);
        glBindVertexArray(m_vao_id[m_object_num]);

        glGenBuffers(1, &m_vbo_id[m_object_num]);
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo_id[m_object_num]);
        glBufferData(GL_ARRAY_BUFFER, m_vb_size[m_object_num], meshes[i].vertices.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, vertexStride, 0);
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, vertexStride, (GLvoid*)normalOffset);

        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);

        glBindVertexArray(0);

        errorCheckValue = glGetError();
        if(errorCheckValue != GL_NO_ERROR)
        {
            fprintf(stderr, "Error: Could not create a VBO: %s\n", gluErrorString(errorCheckValue));
        }
        m_object_num++;
    }
}

Camera camera(glm::vec3{0.f}, 5.f);

void render_scene()
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_CULL_FACE);

    gViewMatrix = camera.viewMatrix();

    if(shouldDisplayWireframeMode)
    {
        glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
    }
    else
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
    glm::vec3 lightPos = glm::vec3(20.0f, 20.0f, 20.0f);
    glm::vec3 mesh_color = glm::vec3(0.9f, 0.5f, 3.0f);
    glm::mat4x4 modelMatrix = glm::mat4(1.0);

    for (int i = 0; i < m_object_num; ++i)
    {
        if(!shouldTessellateModel)
        {
            glUseProgram(programID);

            glUniform3f(lightID, lightPos.x, lightPos.y, lightPos.z);
            glUniform3f(mesh_color_ID, mesh_color.x, mesh_color.y, mesh_color.z);
            glUniformMatrix4fv(viewMatrixID, 1, GL_FALSE, &gViewMatrix[0][0]);
            glUniformMatrix4fv(projectionMatrixID, 1, GL_FALSE, &gProjectionMatrix[0][0]);
            glUniformMatrix4fv(modelMatrixID, 1, GL_FALSE, &modelMatrix[0][0]);

            glBindVertexArray(m_vao_id[i]);
            glDrawArrays( GL_TRIANGLES, 0, m_vertex_num[i] );
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
                glBindVertexArray(m_vao_id[i]);
                glDrawArrays( GL_PATCHES, 0, m_vertex_num[i] );
            }
        }
    }
    glBindVertexArray(0);
    glUseProgram(0);

    glfwSwapBuffers(window);
    glfwPollEvents();
}

const char * g_window_title = "PN Triangle";
int initWindow()
{
    if(!glfwInit())
    {
        throw runtime_error("Failed to initialize GLFW !");
        return -1;
    }

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    window = glfwCreateWindow(window_width, window_height, g_window_title, NULL, NULL);
    if(window == NULL)
    {
        throw runtime_error("Failed to open GLFW window!");
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window); //< pick context of window into current thread

    glewExperimental = true;
    if(glewInit() != GLEW_OK)
    {
        throw runtime_error("Failed to initialize GLEW !");
        return -1;
    }

    glfwSetCursorPos(window, window_width / 2.f, window_height / 2.f);
    glfwSetKeyCallback(window, glfwindow_key_cb);
    glfwSetMouseButtonCallback(window, glfwindow_mouseButton_cb);
    glfwSetCursorPosCallback(window, glfwindow_mouseMotion_cb);

    return 0;
}

void cleanup()
{
    for(int i = 0; i < m_object_num; i++)
    {
        glDeleteBuffers(1, &m_vbo_id[i]);
        glDeleteVertexArrays(1, &m_vao_id[i]);
    }
    glDeleteProgram(programID);
    glDeleteProgram(tessProgramID);
    glfwTerminate();
}

static void glfwindow_key_cb(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    if(action == GLFW_RELEASE)
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
            shouldDisplayWireframeMode = ! shouldDisplayWireframeMode;
            break;
        case GLFW_KEY_LEFT_BRACKET:
            if(shouldTessellateModel)
            {
                --tessellationLevel;
            }
            break;
        case GLFW_KEY_RIGHT_BRACKET:
            if(shouldTessellateModel)
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

inline glm::ivec2 getMousePos() 
{
    double x, y;
    glfwGetCursorPos(window, &x, &y);
    return glm::ivec2{(int)x, (int)y};
}

inline glm::ivec2 getWindowSize() 
{
    return glm::ivec2{ window_width, window_height};
}

void mouseButtonLeft(const glm::ivec2 &where, bool pressed)
{

}

//< rotate
void mouseDragLeft(const glm::ivec2  &where, const glm::ivec2 &delta)
{
    const glm::vec2 fraction = glm::vec2(delta) / glm::vec2(getWindowSize());
    camera.rotate(fraction);
}

//< pan
void mouseDragCenter(const glm::ivec2  &where, const glm::ivec2 &delta)
{
    const glm::vec2 fraction = glm::vec2(delta) / glm::vec2(getWindowSize());
    camera.pan(-fraction ); //< relative motion
}

//< zoom in / out
void mouseDragRight(const glm::ivec2  &where, const glm::ivec2 &delta)
{
    const glm::vec2 fraction = glm::vec2(delta) / glm::vec2(getWindowSize());
    float scale = std::fabs(fraction.x) > std::fabs(fraction.y) ? fraction.x : fraction.y;
    camera.zoom(scale);
    printf("zoom in / out : %.4f\n", scale);
}

/*! callback for _moving_ the mouse to a new position */
static void glfwindow_mouseMotion_cb(GLFWwindow *window, double x, double y)
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

static void glfwindow_mouseButton_cb(GLFWwindow *window, int button, int action, int mods)
{
    const bool pressed = (action == GLFW_PRESS);
    lastMousePos = getMousePos();
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

int main(int argc, char **argv)
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



    initOpenGL();
    createObjects(modelPath);

    gProjectionMatrix = camera.projMatrix();

    gViewMatrix = camera.viewMatrix();

    do
    {
        render_scene();

    } while(glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(window) == 0);

    cleanup();

    return 0;
}

