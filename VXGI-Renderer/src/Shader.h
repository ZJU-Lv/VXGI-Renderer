#pragma once

#include <iostream>

#include <GL/glew.h>

#include <glm.hpp>

class Shader {
public:
	Shader();
	Shader(const char* vertexPath, const char* fragmentPath, const char* geometryPath = nullptr);

	void bind();
	void unbind();

	void setUniform1i(std::string name, int value);
	void setUniform1f(std::string name, float value);
	void setUniform2f(std::string name, float value1, float value2);
	void setUniform3f(std::string name, float value1, float value2, float value3);
	void setUniformMatrix4fv(std::string name, glm::mat4 matrix);

private:
	GLuint program;
};