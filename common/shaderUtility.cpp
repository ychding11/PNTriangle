
#include <string>
#include <iostream>
#include <fstream>
#include <vector>

#include "shaderUtility.h"
#include "Log.h"

GLuint loadStandardShaders(const char *vert_file_path, const char *frag_file_path)
{
    GLuint vertexShaderID   = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

    std::string vertexShaderCode;
    std::ifstream vertexShaderStream(vert_file_path, std::ios::in);
    if(vertexShaderStream.is_open())
    {
        std::string line = "";
        while(std::getline(vertexShaderStream, line))
        {
            vertexShaderCode += "\n" + line;
        }
        vertexShaderStream.close();
    }
    else
    {
        Err(std::string("Couldn't open ") + vert_file_path );
        exit(1);
    }

    std::string fragmentShaderCode;
    std::ifstream fragmentShaderStream(frag_file_path, std::ios::in);
    if(fragmentShaderStream.is_open())
    {
        std::string line = "";
        while(std::getline(fragmentShaderStream, line))
        {
            fragmentShaderCode += "\n" + line;
        }
        fragmentShaderStream.close();
    }
    else
    {
        Err(std::string("Couldn't open ") + frag_file_path);
        exit(1);
    }

    GLint result = GL_FALSE;
    int infoLogLength;

    Log(std::string("Compiling shader ") + vert_file_path);
    char const *vertexSource = vertexShaderCode.c_str();
    glShaderSource(vertexShaderID, 1, &vertexSource, NULL);
    glCompileShader(vertexShaderID);

    glGetShaderiv(vertexShaderID, GL_COMPILE_STATUS, &result);
    glGetShaderiv(vertexShaderID, GL_INFO_LOG_LENGTH, &infoLogLength);
    if(infoLogLength > 0)
    {
        std::vector<char> vertexShaderErrorMessage(infoLogLength + 1);
        glGetShaderInfoLog(vertexShaderID, infoLogLength, NULL, &vertexShaderErrorMessage[0]);
        std::cout << &vertexShaderErrorMessage[0] << std::endl;
    }

    Log(std::string("Compiling shader ") + frag_file_path);
    char const *fragmentSource = fragmentShaderCode.c_str();
    glShaderSource(fragmentShaderID, 1, &fragmentSource, NULL);
    glCompileShader(fragmentShaderID);

    glGetShaderiv(fragmentShaderID, GL_COMPILE_STATUS, &result);
    glGetShaderiv(fragmentShaderID, GL_INFO_LOG_LENGTH, &infoLogLength);
    if(infoLogLength > 0)
    {
        std::vector<char> fragmentShaderErrorMessage(infoLogLength + 1);
        glGetShaderInfoLog(fragmentShaderID, infoLogLength, NULL, &fragmentShaderErrorMessage[0]);
        std::cout << &fragmentShaderErrorMessage[0] << std::endl;
    }

    Log(std::string("Linking shader "));
    GLuint programID = glCreateProgram();
    glAttachShader(programID, vertexShaderID);
    glAttachShader(programID, fragmentShaderID);
    glLinkProgram(programID);

    glGetShaderiv(programID, GL_LINK_STATUS, &result);
    glGetShaderiv(programID, GL_INFO_LOG_LENGTH, &infoLogLength);
    if(infoLogLength > 0)
    {
        std::vector<char> programErrorMessage(infoLogLength + 1);
        glGetShaderInfoLog(programID, infoLogLength, NULL, &programErrorMessage[0]);
        std::cout << programErrorMessage[0] << std::endl;
    }

    glDetachShader(programID, vertexShaderID);
    glDetachShader(programID, fragmentShaderID);

    glDeleteShader(vertexShaderID);
    glDeleteShader(fragmentShaderID);

    return programID;
}

GLuint loadTessShaders(const char *tess_vert_file_path, const char *tess_ctrl_file_path, const char *tess_eval_file_path, const char *tess_frag_file_path)
{
	GLuint tessVertShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint tessCtrlShaderID = glCreateShader(GL_TESS_CONTROL_SHADER);
	GLuint tessEvalShaderID = glCreateShader(GL_TESS_EVALUATION_SHADER);
	GLuint tessFragShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	std::string tessVertexShaderCode;
	std::ifstream tessVertexShaderStream(tess_vert_file_path, std::ios::in);
	if(tessVertexShaderStream.is_open())
    {
		std::string line;
		while(std::getline(tessVertexShaderStream, line))
        {
			tessVertexShaderCode += "\n" + line;
		}
        tessVertexShaderStream.close();
	}
    else
    {
        Err(std::string("Couldn't open ") + tess_vert_file_path);
        exit(1);
	}

	std::string tessCtrlShaderCode;
	std::ifstream tessCtrlShaderStream(tess_ctrl_file_path, std::ios::in);
	if(tessCtrlShaderStream.is_open())
    {
		std::string line;
		while(std::getline(tessCtrlShaderStream, line))
        {
			tessCtrlShaderCode += "\n" + line;
		}
		tessCtrlShaderStream.close();
	}
    else
    {
        Err(std::string("Couldn't open ") + tess_ctrl_file_path);
        exit(1);
	}

	std::string tessEvalShaderCode;
	std::ifstream tessEvalShaderStream(tess_eval_file_path, std::ios::in);
	if(tessEvalShaderStream.is_open())
    {
        std::string line;
		while(std::getline(tessEvalShaderStream, line))
        {
			tessEvalShaderCode += "\n" + line;
		}
		tessEvalShaderStream.close();
	}
    else
    {
        Err(std::string("Couldn't open ") + tess_eval_file_path);
        exit(1);
	}

	std::string tessFragShaderCode;
	std::ifstream tessFragShaderStream(tess_frag_file_path, std::ios::in);
	if(tessFragShaderStream.is_open())
    {
        std::string line;
		while(std::getline(tessFragShaderStream, line))
        {
			tessFragShaderCode += "\n" + line;
		}
		tessFragShaderStream.close();
	}
    else
    {
        Err(std::string("Couldn't open ") + tess_frag_file_path);
        exit(1);
	}

	GLint result = false;
	int infoLogLength;

	printf("Compiling shader: %s\n", tess_vert_file_path);
	char const* tessVertSourcePointer = tessVertexShaderCode.c_str();
	glShaderSource(tessVertShaderID, 1, &tessVertSourcePointer, NULL);
	glCompileShader(tessVertShaderID);
	glGetShaderiv(tessVertShaderID, GL_COMPILE_STATUS, &result);
	glGetShaderiv(tessVertShaderID, GL_INFO_LOG_LENGTH, &infoLogLength);
	if(infoLogLength > 0)
    {
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
	if(infoLogLength > 0)
    {
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
	if(infoLogLength > 0)
    {
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
	if(infoLogLength > 0)
    {
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

