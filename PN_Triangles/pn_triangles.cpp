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
#include <common/objloader.hpp>

using namespace glm;
using namespace std;

const int window_width = 1000, window_height = 1000;
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

GLuint matrixID;
GLuint modelMatrixID;
GLuint viewMatrixID;
GLuint projectionMatrixID;
GLuint lightID;
GLuint mesh_color_ID;
GLuint tess_mesh_color_ID;

GLuint tessMatrixID;
GLuint tessModelMatrixID;
GLuint tessViewMatrixID;
GLuint tessProjectionMatrixID;
GLuint tessLightID;
GLfloat tessellationLevelInnerID;
GLfloat tessellationLevelOuterID;

GLfloat cameraAngleTheta = 3.142 / 4;
GLfloat cameraAnglePhi = asin(1 / sqrt(3));
GLfloat cameraSphereRadius = sqrt(675);
bool moveCameraLeft = false;
bool moveCameraRight = false;
bool moveCameraUp = false;
bool moveCameraDown = false;
float tessellationLevel = 1.0f;
bool shouldTessellateModel = false;
bool shouldDisplayWireframeMode = false;

int initWindow(void);
void initOpenGL(void);
void createObjects(void);

static void keyCallback(GLFWwindow* , int, int, int, int);
static void mouseCallback(GLFWwindow*, int, int, int);

GLuint loadStandardShaders(const char*, const char*);
GLuint loadTessShaders(const char*, const char*, const char*, const char*);

void cleanup(void);


void create_vaos(std::vector<Mesh> &meshes);
void render_scene();

void initOpenGL()
{
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_CULL_FACE);

    gProjectionMatrix = glm::perspective(45.0f, 4.0f / 3.0f, 0.1f, 100.0f);

    gViewMatrix = glm::lookAt(glm::vec3(15.0f, 15.0f, 15.0f),
                              glm::vec3(0.0f, 10.0f, 0.0f),
                              glm::vec3(0.0f, 1.0f, 0.0f));

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

void render_scene()
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_CULL_FACE);

    if (moveCameraLeft)
    {
        cameraAngleTheta -= 0.01f;
    }

    if (moveCameraRight)
    {
        cameraAngleTheta += 0.01f;
    }

    if (moveCameraUp)
    {
        cameraAnglePhi -= 0.01f;
    }

    if (moveCameraDown)
    {
        cameraAnglePhi += 0.01f;
    }

    if (moveCameraLeft || moveCameraRight || moveCameraDown || moveCameraUp)
    {
        float camX = cameraSphereRadius * cos(cameraAnglePhi) * sin(cameraAngleTheta);
        float camY = cameraSphereRadius * sin(cameraAnglePhi);
        float camZ = cameraSphereRadius * cos(cameraAnglePhi) * cos(cameraAngleTheta);
        gViewMatrix = glm::lookAt(glm::vec3(camX, camY, camZ),	// eye
            glm::vec3(0.0, 10.0, 0.0),	// center
            glm::vec3(0.0, 1.0, 0.0));	// up
    }

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

GLuint loadStandardShaders(const char *vert_file_path, const char *frag_file_path) {
    GLuint vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

    string vertexShaderCode;
    ifstream vertexShaderStream(vert_file_path, ios::in);
    if(vertexShaderStream.is_open()) {
        string line = "";
        while(std::getline(vertexShaderStream, line)) {
            vertexShaderCode += "\n" + line;
        }
        vertexShaderStream.close();
    } else {
        cout << "Couldn't open " << vert_file_path << endl;
        getchar();
        return 0;
    }

    string fragmentShaderCode;
    ifstream fragmentShaderStream(frag_file_path, ios::in);
    if(fragmentShaderStream.is_open()) {
        string line = "";
        while(std::getline(fragmentShaderStream, line)) {
            fragmentShaderCode += "\n" + line;
        }
        fragmentShaderStream.close();
    } else {
        cout << "Couldn't open " << frag_file_path << endl;
        getchar();
        return 0;
    }

    GLint result = GL_FALSE;
    int infoLogLength;

    cout << "Compiling shader " <<  vert_file_path << endl;
    char const *vertexSource = vertexShaderCode.c_str();
    glShaderSource(vertexShaderID, 1, &vertexSource, NULL);
    glCompileShader(vertexShaderID);

    glGetShaderiv(vertexShaderID, GL_COMPILE_STATUS, &result);
    glGetShaderiv(vertexShaderID, GL_INFO_LOG_LENGTH, &infoLogLength);
    if(infoLogLength > 0) {
        vector<char> vertexShaderErrorMessage(infoLogLength + 1);
        glGetShaderInfoLog(vertexShaderID, infoLogLength, NULL, &vertexShaderErrorMessage[0]);
        cout << &vertexShaderErrorMessage[0] << endl;
    }

    cout << "Compiling shader " <<  frag_file_path << endl;
    char const *fragmentSource = fragmentShaderCode.c_str();
    glShaderSource(fragmentShaderID, 1, &fragmentSource, NULL);
    glCompileShader(fragmentShaderID);

    glGetShaderiv(fragmentShaderID, GL_COMPILE_STATUS, &result);
    glGetShaderiv(fragmentShaderID, GL_INFO_LOG_LENGTH, &infoLogLength);
    if(infoLogLength > 0) {
        vector<char> fragmentShaderErrorMessage(infoLogLength + 1);
        glGetShaderInfoLog(fragmentShaderID, infoLogLength, NULL, &fragmentShaderErrorMessage[0]);
        cout << &fragmentShaderErrorMessage[0] << endl;
    }

    cout << "Linking shader" << endl;
    GLuint programID = glCreateProgram();
    glAttachShader(programID, vertexShaderID);
    glAttachShader(programID, fragmentShaderID);
    glLinkProgram(programID);

    glGetShaderiv(programID, GL_LINK_STATUS, &result);
    glGetShaderiv(programID, GL_INFO_LOG_LENGTH, &infoLogLength);
    if(infoLogLength > 0) {
        vector<char> programErrorMessage(infoLogLength + 1);
        glGetShaderInfoLog(programID, infoLogLength, NULL, &programErrorMessage[0]);
        cout << programErrorMessage[0] << endl;
    }

    glDetachShader(programID, vertexShaderID);
    glDetachShader(programID, fragmentShaderID);

    glDeleteShader(vertexShaderID);
    glDeleteShader(fragmentShaderID);

    return programID;
}

GLuint loadTessShaders(const char *tess_vert_file_path, const char *tess_ctrl_file_path, const char *tess_eval_file_path,
	const char *tess_frag_file_path) {
	GLuint tessVertShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint tessCtrlShaderID = glCreateShader(GL_TESS_CONTROL_SHADER);
	GLuint tessEvalShaderID = glCreateShader(GL_TESS_EVALUATION_SHADER);
	GLuint tessFragShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	string tessVertexShaderCode;
	ifstream tessVertexShaderStream(tess_vert_file_path, std::ios::in);
	if(tessVertexShaderStream.is_open()) {
		string line = "";
		while(std::getline(tessVertexShaderStream, line)) {
			tessVertexShaderCode += "\n" + line;
		}
        tessVertexShaderStream.close();
	} else {
		printf("Impossible to open %s.\n", tess_vert_file_path);
		getchar();
		return 0;
	}

	string tessCtrlShaderCode;
	ifstream tessCtrlShaderStream(tess_ctrl_file_path, std::ios::in);
	if(tessCtrlShaderStream.is_open()) {
		string line = "";
		while(std::getline(tessCtrlShaderStream, line)) {
			tessCtrlShaderCode += "\n" + line;
		}
		tessCtrlShaderStream.close();
	} else {
		printf("Impossible to open %s\n", tess_ctrl_file_path);
		getchar();
		return 0;
	}

	string tessEvalShaderCode;
	ifstream tessEvalShaderStream(tess_eval_file_path, std::ios::in);
	if(tessEvalShaderStream.is_open()) {
		string line = "";
		while(std::getline(tessEvalShaderStream, line)) {
			tessEvalShaderCode += "\n" + line;
		}
		tessEvalShaderStream.close();
	} else {
        printf("Impossible to open %s.\n", tess_eval_file_path);
		getchar();
		return 0;
	}

	string tessFragShaderCode;
	ifstream tessFragShaderStream(tess_frag_file_path, std::ios::in);
	if(tessFragShaderStream.is_open()) {
        string line = "";
		while(std::getline(tessFragShaderStream, line)) {
			tessFragShaderCode += "\n" + line;
		}
		tessFragShaderStream.close();
	} else {
		printf("Impossible to open %s.\n", tess_frag_file_path);
		getchar();
		return 0;
	}

	GLint result = false;
	int infoLogLength;

	printf("Compiling shader: %s\n", tess_vert_file_path);
	char const* tessVertSourcePointer = tessVertexShaderCode.c_str();
	glShaderSource(tessVertShaderID, 1, &tessVertSourcePointer, NULL);
	glCompileShader(tessVertShaderID);
	glGetShaderiv(tessVertShaderID, GL_COMPILE_STATUS, &result);
	glGetShaderiv(tessVertShaderID, GL_INFO_LOG_LENGTH, &infoLogLength);
	if(infoLogLength > 0) {
		std::vector<char> tessVertShaderErrMsg(infoLogLength + 1);
		glGetShaderInfoLog(tessVertShaderID, infoLogLength, NULL, &tessVertShaderErrMsg[0]);
		printf("%s\n", &tessVertShaderErrMsg[0]);
	}

	printf("Compiling shader: %s\n", tess_ctrl_file_path);
	char const* tessCtrlSourcePointer = tessCtrlShaderCode.c_str();
	glShaderSource(tessCtrlShaderID, 1, &tessCtrlSourcePointer, NULL);
	glCompileShader(tessCtrlShaderID);
	glGetShaderiv(tessCtrlShaderID, GL_COMPILE_STATUS, &result);
	glGetShaderiv(tessCtrlShaderID, GL_INFO_LOG_LENGTH, &infoLogLength);
	if(infoLogLength > 0) {
		std::vector<char> tessCtrlShaderErrMsg(infoLogLength + 1);
		glGetShaderInfoLog(tessCtrlShaderID, infoLogLength, NULL, &tessCtrlShaderErrMsg[0]);
		printf("%s\n", &tessCtrlShaderErrMsg[0]);
	}

	printf("Compiling shader: %s\n", tess_eval_file_path);
	char const* tessEvalSourcePointer = tessEvalShaderCode.c_str();
	glShaderSource(tessEvalShaderID, 1, &tessEvalSourcePointer, NULL);
	glCompileShader(tessEvalShaderID);
	glGetShaderiv(tessEvalShaderID, GL_COMPILE_STATUS, &result);
	glGetShaderiv(tessEvalShaderID, GL_INFO_LOG_LENGTH, &infoLogLength);
	if(infoLogLength > 0) {
		std::vector<char> tessEvalShaderErrMsg(infoLogLength + 1);
		glGetShaderInfoLog(tessEvalShaderID, infoLogLength, NULL, &tessEvalShaderErrMsg[0]);
		printf("%s\n", &tessEvalShaderErrMsg[0]);
	}

	printf("Compiling shader: %s\n", tess_frag_file_path);
	char const* tessFragSourcePointer = tessFragShaderCode.c_str();
	glShaderSource(tessFragShaderID, 1, &tessFragSourcePointer, NULL);
	glCompileShader(tessFragShaderID);
	glGetShaderiv(tessFragShaderID, GL_COMPILE_STATUS, &result);
	glGetShaderiv(tessFragShaderID, GL_INFO_LOG_LENGTH, &infoLogLength);
	if(infoLogLength > 0) {
		std::vector<char> tessFragShaderErrMsg(infoLogLength + 1);
		glGetShaderInfoLog(tessFragShaderID, infoLogLength, NULL, &tessFragShaderErrMsg[0]);
		printf("%s\n", &tessFragShaderErrMsg[0]);
	}

    printf("Linking Shader\n");
	GLuint tessProgramID = glCreateProgram();
	glAttachShader(tessProgramID, tessVertShaderID);
	glAttachShader(tessProgramID, tessCtrlShaderID);
	glAttachShader(tessProgramID, tessEvalShaderID);
	glAttachShader(tessProgramID, tessFragShaderID);
	glLinkProgram(tessProgramID);

	glGetProgramiv(tessProgramID, GL_LINK_STATUS, &result);
	glGetProgramiv(tessProgramID, GL_INFO_LOG_LENGTH, &infoLogLength);
	if(infoLogLength > 0) {
		std::vector<char> tessProgramErrMsg(infoLogLength + 1);
		glGetProgramInfoLog(tessProgramID, infoLogLength, NULL, &tessProgramErrMsg[0]);
		printf("%s\n", &tessProgramErrMsg[0]);
	}

	glDetachShader(tessProgramID, tessVertShaderID);
	glDetachShader(tessProgramID, tessCtrlShaderID);
	glDetachShader(tessProgramID, tessEvalShaderID);
	glDetachShader(tessProgramID, tessFragShaderID);

	glDeleteShader(tessVertShaderID);
	glDeleteShader(tessCtrlShaderID);
	glDeleteShader(tessEvalShaderID);
	glDeleteShader(tessFragShaderID);

	return tessProgramID;
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

    glfwSetCursorPos(window, window_width / 2, window_height / 2);
    glfwSetKeyCallback(window, keyCallback);
    glfwSetMouseButtonCallback(window, mouseCallback);

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

static void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    if(action == GLFW_PRESS) {
        switch (key) {
        case GLFW_KEY_LEFT:
            moveCameraLeft = true;
            break;
        case GLFW_KEY_RIGHT:
            moveCameraRight = true;
            break;
        case GLFW_KEY_UP:
            moveCameraUp = true;
            break;
        case GLFW_KEY_DOWN:
            moveCameraDown = true;
            break;
        default:
            break;
        }
    } else if(action == GLFW_RELEASE) {
        switch (key) {
        case GLFW_KEY_LEFT:
            moveCameraLeft = false;
            break;
        case GLFW_KEY_RIGHT:
            moveCameraRight = false;
            break;
        case GLFW_KEY_UP:
            moveCameraUp = false;
            break;
        case GLFW_KEY_DOWN:
            moveCameraDown = false;
            break;
        case GLFW_KEY_T:
            shouldTessellateModel = !shouldTessellateModel;
            tessellationLevel = 1.0f;
            break;
        case GLFW_KEY_W:
            shouldDisplayWireframeMode = ! shouldDisplayWireframeMode;
            break;
        case GLFW_KEY_LEFT_BRACKET:
            if(shouldTessellateModel) {
                --tessellationLevel;
            }
            break;
        case GLFW_KEY_RIGHT_BRACKET:
            if(shouldTessellateModel) {
                ++tessellationLevel;
            }
            break;
        default:
            break;
        }
    }
}

static void mouseCallback(GLFWwindow *window, int button, int action, int mods)
{

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
    createObjects("Model/Suzanne.obj");

    do
    {
        render_scene();

    } while(glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(window) == 0);

    cleanup();

    return 0;
}

