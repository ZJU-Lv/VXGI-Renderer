#pragma once

#include <iostream>

#include <GL/glew.h>
#include <glm.hpp>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include "Shader.h"

struct Texture2D {
	GLuint textureID;
	int width;
	int height;
	int componentsPerPixel;
};

struct Texture3D {
	GLuint textureID;
	int size;
	int componentsPerPixel;
};

Texture2D loadTexture2D(std::string path);

class Material {
public:
	Material();

	void loadFromAssimp(aiMaterial* assimpMaterial, std::string directory);

	void bind(Shader& shader);

	std::string name;

	glm::vec3 ambientColor;
	glm::vec3 diffuseColor;
	glm::vec3 specularColor;
	float shininess;
	float opacity;

	Texture2D diffuseTexture;
	Texture2D specularTexture;
	Texture2D maskTexture;
	Texture2D heightTexture;
};