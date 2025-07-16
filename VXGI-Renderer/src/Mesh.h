#pragma once

#include <iostream>
#include <vector>

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>

#include "Material.h"
#include "Shader.h"

class Mesh {
public:
	Mesh();

	void loadFromAssimp(aiMesh* assimpMesh);

	void draw();

	std::vector<glm::vec3> vertices;
	std::vector<unsigned int> indices;
	std::vector<glm::vec2> texCoords;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec3> tangents;
	std::vector<glm::vec3> bitangents;

	GLuint VAO;
	GLuint verticesBuffer;
	GLuint indicesBuffer;
	GLuint texCoordsBuffer;
	GLuint normalsBuffer;
	GLuint tangentsBuffer;
	GLuint bitangentsBuffer;

	Material* material;

	float scale;
};