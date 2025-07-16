#include "Shader.h"

#include <vector>
#include <string>
#include <fstream>

std::string readFile(const char* path)
{
    std::string shaderCode;
    std::ifstream stream(path, std::ios::in);
    if (stream.is_open())
    {
        std::string line = "";
        while (getline(stream, line))
            shaderCode += "\n" + line;
        stream.close();
    }
    else
    {
        std::cout << "Fail to open shader " << path << "!" << std::endl;
        return "";
    }

    return shaderCode;
}

void compileShaderCode(GLuint shaderID, const char* shaderCode)
{
    GLint result = GL_FALSE;
    int infoLogLength;

    glShaderSource(shaderID, 1, &shaderCode, NULL);
    glCompileShader(shaderID);

    glGetShaderiv(shaderID, GL_COMPILE_STATUS, &result);
    glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &infoLogLength);
    if (result == GL_FALSE)
    {
        std::vector<char> errorMessage(infoLogLength + 1);
        glGetShaderInfoLog(shaderID, infoLogLength, NULL, &errorMessage[0]);
		std::cout << "Shader compile error: " << &errorMessage[0] << std::endl;
    }
}

Shader::Shader()
{
}

Shader::Shader(const char* vertexPath, const char* fragmentPath, const char* geometryPath)
{
    std::string vertexShaderCode = readFile(vertexPath);
    std::string fragmentShaderCode = readFile(fragmentPath);
    std::string geometryShaderCode = geometryPath != nullptr ? readFile(geometryPath) : "";

    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    GLuint geometryShader;
    if (geometryPath)
        geometryShader = glCreateShader(GL_GEOMETRY_SHADER);

    compileShaderCode(vertexShader, vertexShaderCode.c_str());
    compileShaderCode(fragmentShader, fragmentShaderCode.c_str());
    if(geometryPath)
		compileShaderCode(geometryShader, geometryShaderCode.c_str());

    program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    if (geometryPath)
        glAttachShader(program, geometryShader);
    glLinkProgram(program);

    GLint result;
    int infoLogLength;
    glGetProgramiv(program, GL_LINK_STATUS, &result);
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLength);
    if (result == GL_FALSE)
    {
        std::vector<char> ProgramErrorMessage(infoLogLength + 1);
        glGetProgramInfoLog(program, infoLogLength, NULL, &ProgramErrorMessage[0]);
        std::cout << "Shader link error: " << &ProgramErrorMessage[0] << std::endl;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    if (geometryPath)
        glDeleteShader(geometryShader);
}

void Shader::bind()
{
	glUseProgram(program);
}

void Shader::unbind()
{
	glUseProgram(0);
}

void Shader::setUniform1i(std::string name, int value)
{
    glUniform1i(glGetUniformLocation(program, name.c_str()), value);
}

void Shader::setUniform1f(std::string name, float value)
{
    glUniform1f(glGetUniformLocation(program, name.c_str()), value);
}

void Shader::setUniform2f(std::string name, float value1, float value2)
{
    glUniform2f(glGetUniformLocation(program, name.c_str()), value1, value2);
}

void Shader::setUniform3f(std::string name, float value1, float value2, float value3)
{
    glUniform3f(glGetUniformLocation(program, name.c_str()), value1, value2, value3);
}

void Shader::setUniformMatrix4fv(std::string name, glm::mat4 matrix)
{
    glUniformMatrix4fv(glGetUniformLocation(program, name.c_str()), 1, GL_FALSE, &matrix[0][0]);
}
