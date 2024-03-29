#include <stdio.h>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "common/objloader.h"
#include "shaderUtility.h"

#include "camera.h" 
#include "viewer.h" 

#include "event_handler.h" 
#include "gui.h"

float myRandom();

void drawOverlay(const Viewer &viewer);

void Viewer::Run()
{
    MeshBin meshes{ m_objPath };
    Camera camera(meshes.Center(), meshes.LogestDim() * 1.618f); //< Set Camera by Mesh info
    glfwCallbackData cb{ &camera, m_animation_mode, GetRenderSetting(), GetDisplayOption()};
    glfwSetWindowUserPointer(m_window, &cb);

    GUI::Setup(m_window, "#version 130");
    do
    {
        animateCamera(camera);
        animateTessellation();
        render(meshes, camera);
        
    } while(glfwGetKey(m_window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(m_window) == 0);

    GUI::CleanUp();
}

void Viewer::animateTessellation()
{
    if (!m_enable_tess_anim || !m_setting.enableTess || m_frame_num % 10 != 0) return;

    const int maxInnerLevel = 4;
    const int maxOuterLevel = 6;

    m_setting.outerTessLevel.x++;
    m_setting.outerTessLevel.y++;
    m_setting.outerTessLevel.z++;
    if (m_setting.outerTessLevel.x > maxOuterLevel)
    {
        m_setting.innerTessLevel.x++;

        m_setting.outerTessLevel.x = 1;
        m_setting.outerTessLevel.y = 1;
        m_setting.outerTessLevel.z = 1;
        if (m_setting.innerTessLevel.x > maxInnerLevel)
            m_setting.innerTessLevel.x = 1;
    }
}

void Viewer::animateCamera(Camera &camera)
{
    switch (m_animation_mode)
    {
        case 0:
        {
            break;
        }
        case 1:
        {
            glm::vec2 delta{0.01, 0.0};
            camera.rotate(delta);
            break;
        }
        case 2:
        {
            float scale = -0.01;
            camera.zoom(scale); 
            break;
        }
        case 3:
        {
            float scale = 0.01;
            camera.zoom(scale); 
            break;
        }
        case 4:
        {
            glm::vec2 delta{myRandom() * 0.05, myRandom() * 0.04};
            camera.rotate(delta);
            break;
        }
        default:
        {
            break;
        }
    }
}

void Viewer::render(const MeshBin & m_meshBin, const Camera &m_camera)
{
    glm::mat4x4 modelMatrix = glm::mat4(1.0);
    glm::mat4 gViewMatrix       = m_camera.viewMatrix();
    glm::mat4 gProjectionMatrix = m_camera.projMatrix();
    glm::vec3 eye_position = m_camera.eye();
    glm::vec3 lightPos   = glm::vec3(20.0f, 20.0f, 20.0f);

#if defined(MSAA_ENABLE)
    glEnable(GL_MULTISAMPLE);
#endif

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_CULL_FACE);

    if(m_option.wireframe)
    {
        glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
    }
    else
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    for (int i = 0; i < m_meshBin.size(); ++i)
    {
        if(!m_setting.enableTess)
        {
            glUseProgram(m_linked_shader_ID);

            glUniform3f(m_light_pos_ID, lightPos.x, lightPos.y, lightPos.z);
            glUniform3f(m_mesh_color_ID, m_mesh_color.x, m_mesh_color.y, m_mesh_color.z);
            glUniformMatrix4fv(m_view_matrix_ID, 1, GL_FALSE, &gViewMatrix[0][0]);
            glUniformMatrix4fv(m_proj_matrix_ID, 1, GL_FALSE, &gProjectionMatrix[0][0]);
            glUniformMatrix4fv(m_model_matrix_ID, 1, GL_FALSE, &modelMatrix[0][0]);

            glBindVertexArray( m_meshBin.vao(i) );
            glDrawArrays( GL_TRIANGLES, 0, m_meshBin.vertex_num(i) );
        }
        else
        {
            glUseProgram(m_tes_linked_shader_ID);
            {
                glUniform3f(m_tes_light_pos_ID, lightPos.x, lightPos.y, lightPos.z);
                glUniform3f(m_tes_mesh_color_ID, m_mesh_color.x, m_mesh_color.y, m_mesh_color.z);
                glUniformMatrix4fv(m_tes_view_matrix_ID, 1, GL_FALSE, &gViewMatrix[0][0]);
                glUniformMatrix4fv(m_tes_proj_matrix_ID, 1, GL_FALSE, &gProjectionMatrix[0][0]);
                glUniformMatrix4fv(m_tes_model_matrix_ID, 1, GL_FALSE, &modelMatrix[0][0]);

                glUniform1f(m_tes_inner_level_ID, m_setting.innerTessLevel.x);
                glUniform3f(m_tes_outer_level_ID, m_setting.outerTessLevel.x, m_setting.outerTessLevel.y, m_setting.outerTessLevel.z);

                glPatchParameteri(GL_PATCH_VERTICES, 3);
                glBindVertexArray( m_meshBin.vao(i) );
                glDrawArrays( GL_PATCHES, 0, m_meshBin.vertex_num(i) );
            }
        }
    }
    glBindVertexArray(0);
    glUseProgram(0);

    //< shall capture color buffer here
    if (m_capture_colorbuffer)
    {
        SaveScreen();
        m_capture_colorbuffer = false;
    }

    if (m_save_image_sequence)
    {
        SaveImageSequence();
    }

    //< draw ui after render
    drawUI(*this);
    m_frame_num++;

    glfwSwapBuffers(m_window);
    glfwPollEvents();
}
void Viewer::initOpenGLShaders()
{
    m_linked_shader_ID     = loadStandardShaders("shaders/Standard.vert.glsl", "shaders/Standard.frag.glsl");
    m_tes_linked_shader_ID = loadTessShaders("shaders/Tessellation.vs.glsl", "shaders/Tessellation.tc.glsl", "shaders/Tessellation.te.glsl", "shaders/Tessellation.fs.glsl");

    m_model_matrix_ID = glGetUniformLocation(m_linked_shader_ID, "M");
    m_view_matrix_ID  = glGetUniformLocation(m_linked_shader_ID, "V");
    m_proj_matrix_ID  = glGetUniformLocation(m_linked_shader_ID, "P");
    m_light_pos_ID    = glGetUniformLocation(m_linked_shader_ID, "lightPosition_worldspace");
    m_mesh_color_ID   = glGetUniformLocation(m_linked_shader_ID, "mesh_color");

    m_tes_model_matrix_ID = glGetUniformLocation(m_tes_linked_shader_ID, "M");
    m_tes_view_matrix_ID  = glGetUniformLocation(m_tes_linked_shader_ID, "V");
    m_tes_proj_matrix_ID  = glGetUniformLocation(m_tes_linked_shader_ID, "P");
    m_tes_light_pos_ID    = glGetUniformLocation(m_tes_linked_shader_ID, "lightPosition_worldspace");
    m_tes_eye_pos_ID      = glGetUniformLocation(m_tes_linked_shader_ID, "eye_worldspace");
    m_tes_inner_level_ID  = glGetUniformLocation(m_tes_linked_shader_ID, "tessellationLevelInner");
    m_tes_outer_level_ID  = glGetUniformLocation(m_tes_linked_shader_ID, "tessellationLevelOuter");
    m_tes_mesh_color_ID   = glGetUniformLocation(m_tes_linked_shader_ID, "mesh_color");
}

int Viewer::initWindow()
{
    if(!glfwInit())
    {
        throw std::runtime_error("Failed to initialize GLFW !");
        return -1;
    }

#if defined(MSAA_ENABLE)
    glfwWindowHint(GLFW_SAMPLES, 4);
#endif
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

#define STB_IMAGE_WRITE_IMPLEMENTATION 
#include "stb_image_write.h" 

void Viewer::SaveScreen(const std::string filename)
{
    /**
     * https://www.khronos.org/opengl/wiki/Framebuffer#Read_color_buffer
     *
     **/

    const int kSize = m_window_height * m_window_width;
    std::vector<GLfloat> pixels((size_t)kSize * 3);
    glReadPixels(0, 0, m_window_width, m_window_height, GL_RGB, GL_UNSIGNED_BYTE, pixels.data());
    GLenum errorCheckValue = glGetError();
    if (errorCheckValue != GL_NO_ERROR)
    {
        fprintf(stderr, "Error: Could not read color buffer: %s\n", gluErrorString(errorCheckValue));
    }
    stbi_flip_vertically_on_write(1);
    stbi_write_tga(filename.c_str(), m_window_width, m_window_height, 3, pixels.data()); //< only 3 channels
    printf("save color buffer into : %s \n", filename.c_str());
}

void Viewer::SaveImageSequence(const std::string dir)
{
    std::ostringstream  iss;
    iss << std::setw(4) << std::setfill('0') << m_sequence_count;
    SaveScreen(iss.str() + ".tga");
    m_sequence_count++;
}

///////////////////////////////////////////////////////////////////////////////////////
/////
///// UI
/////
///////////////////////////////////////////////////////////////////////////////////////

#include "IconsFontAwesome4.h"
static void drawUI(Viewer &viewer)
{
    RenderSetting &setting = viewer.m_setting;
    DisplayOption &displayOption = viewer.m_option;

    if (!displayOption.showUI)
    {
        return;
    }
    GUI::BeginFrame();

    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5.0f, 5.0f));
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu(ICON_FA_CUBE " File"))
        {
            if (ImGui::MenuItem(ICON_FA_FOLDER_OPEN " Open..."))
            {
            }
            if (ImGui::MenuItem(ICON_FA_FILM " Capture..."))
            {
                viewer.m_capture_colorbuffer = true;
            }
            
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu(ICON_FA_CAMERA " Camera"))
        {
            const char *play = ICON_FA_PLAY " Start Image Sequence";
            const char *stop = ICON_FA_STOP " Stop  Image Sequence";
            if (ImGui::MenuItem(viewer.m_save_image_sequence ? stop : play))
            {
                viewer.m_save_image_sequence = !viewer.m_save_image_sequence;
                viewer.m_sequence_count = 0;
                if (viewer.m_save_image_sequence)
                {
                    Log("start Image sequence.\n");
                }
                else
                {
                    Log("stop Image sequence.\n");
                }
            }

            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu(ICON_FA_EYE " View"))
        {
            ImGui::Checkbox("ShowUI", &displayOption.showUI);
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu(ICON_FA_WINDOWS " Settings"))
        {
            bool changed = false;
            ImGui::ColorEdit3("Mesh Diffuse Color", &viewer.m_mesh_color.x);
            ImGui::Separator();
            ImGui::Checkbox("Wireframe", &displayOption.wireframe);
            if (ImGui::Checkbox("Enable Tessellation", &setting.enableTess))
            {
                if (false == setting.enableTess)
                {
                    setting.outerTessLevel = glm::vec4{1};
                    setting.innerTessLevel = glm::vec3{1};
                }
            }
            ImGui::Checkbox("Enable Tessellation Animation",  &viewer.m_enable_tess_anim);
            ImGui::Separator();
            changed |= ImGui::SliderFloat3("outer Tess Level", &setting.outerTessLevel.x, 1, 64);
            changed |= ImGui::SliderFloat("Inner Tess Level", &setting.innerTessLevel.x, 1, 64);
            if (changed)
            {
                Log("Tessellation level changed.\n");
            }
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }
    ImGui::PopStyleVar();

    drawOverlay(viewer);

    GUI::EndFrame();
}

static void drawOverlay(const Viewer &viewer)
{
    ImGui::SetNextWindowPos(ImVec2(10.0f, ImGui::GetIO().DisplaySize.y - 10.0f), ImGuiCond_Always, ImVec2(0, 1));
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 0.0f, 0.4f)); // Transparent background
    if (ImGui::Begin("StatusOverlay", nullptr,
        ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize
        | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove
        | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoBringToFrontOnFocus))
    {
        const RenderSetting &setting = viewer.m_setting;
        ImGui::Text("Toggle UI display with key [ x | X ]");
        ImGui::Text("Renderer : %s", glGetString(GL_RENDERER));
        ImGui::Text("OpenGL version : %s", glGetString(GL_VERSION));
        ImGui::Text("outer level : %.2f, %.2f, %.2f", setting.outerTessLevel.x, setting.outerTessLevel.y, setting.outerTessLevel.z);
        ImGui::Text("inner level : %.2f", setting.innerTessLevel.x);
        ImGui::Text("fps : %.2f fps", ImGui::GetIO().Framerate);
        ImGui::End();
    }
    ImGui::PopStyleColor();
}

///////////////////////////////////////////////////////////////////////////////////////
/////
/////  Random
/////
///////////////////////////////////////////////////////////////////////////////////////
#include <random>

static std::default_random_engine generator;
static std::uniform_real_distribution<float> distribution(-0.5f, 0.5f); //from -.5 to .5

static float myRandom()
{
    return distribution(generator);
}

///////////////////////////////////////////////////////////////////////////////////////
/////
///// Timing 
/////
///////////////////////////////////////////////////////////////////////////////////////
#include <chrono>
#include <thread>

auto awake_time()
{
    auto now = []() { return std::chrono::steady_clock::now(); };
    using std::chrono::operator""ms;
    return now() + 300ms;
}

//< implement a function to sleep current thread a while
//

