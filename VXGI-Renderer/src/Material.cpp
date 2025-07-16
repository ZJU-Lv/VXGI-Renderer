#include "Material.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

Texture2D loadTexture2D(std::string path)
{
	Texture2D texture;

	const char* filename = path.c_str();
	GLubyte* textureData = stbi_load(filename, &texture.width, &texture.height, &texture.componentsPerPixel, 0);

	if (!textureData)
	{
		std::cout << "Couldn't load image: " << filename << std::endl;
		return texture;
	}

	glGenTextures(1, &texture.textureID);
	glBindTexture(GL_TEXTURE_2D, texture.textureID);

	if (texture.componentsPerPixel == 4)
	{
		glTexImage2D(GL_TEXTURE_2D,
			0,
			GL_RGBA,
			texture.width,
			texture.height,
			0,
			GL_RGBA,
			GL_UNSIGNED_BYTE,
			textureData);
	}
	else if (texture.componentsPerPixel == 3)
	{
		glTexImage2D(GL_TEXTURE_2D,
			0,
			GL_RGB,
			texture.width,
			texture.height,
			0,
			GL_RGB,
			GL_UNSIGNED_BYTE,
			textureData);
	}
	else if (texture.componentsPerPixel == 1)
	{
		glTexImage2D(GL_TEXTURE_2D,
			0,
			GL_RED,
			texture.width,
			texture.height,
			0,
			GL_RED,
			GL_UNSIGNED_BYTE,
			textureData);
	}

	if (texture.componentsPerPixel == 4 || texture.componentsPerPixel == 3 || texture.componentsPerPixel == 1)
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glGenerateMipmap(GL_TEXTURE_2D);
	}

	GLenum glError = glGetError();
	if (glError)
		std::cout << "Error loading texture: " << path << std::endl;

	stbi_image_free(textureData);

	return texture;
}


Material::Material()
{
}

void Material::loadFromAssimp(aiMaterial* assimpMaterial, std::string directory)
{
	aiString matName;
	assimpMaterial->Get(AI_MATKEY_NAME,matName);
	name = matName.data;

	aiColor3D color;
	assimpMaterial->Get(AI_MATKEY_COLOR_AMBIENT, color);
	ambientColor = glm::vec3(color.r, color.g, color.b);

	assimpMaterial->Get(AI_MATKEY_COLOR_SPECULAR, color);
	specularColor = glm::vec3(color.r, color.g, color.b);

	assimpMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, color);
	diffuseColor = glm::vec3(color.r, color.g, color.b);

	assimpMaterial->Get(AI_MATKEY_SHININESS, shininess);
	assimpMaterial->Get(AI_MATKEY_OPACITY, opacity);

	if (assimpMaterial->GetTextureCount(aiTextureType_DIFFUSE) > 0)
	{
		aiString texturePath;
		if (assimpMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &texturePath) == AI_SUCCESS) {
			std::string fullPath = directory + texturePath.data;
			diffuseTexture = loadTexture2D(fullPath);
		}
	}

	if (assimpMaterial->GetTextureCount(aiTextureType_AMBIENT) > 0)
	{
		aiString texturePath;
		if (assimpMaterial->GetTexture(aiTextureType_AMBIENT, 0, &texturePath) == AI_SUCCESS) {
			std::string fullPath = directory + texturePath.data;
			specularTexture = loadTexture2D(fullPath);
		}
	}

	if (assimpMaterial->GetTextureCount(aiTextureType_HEIGHT) > 0)
	{
		aiString texturePath;
		if (assimpMaterial->GetTexture(aiTextureType_HEIGHT, 0, &texturePath) == AI_SUCCESS) {
			std::string fullPath = directory + texturePath.data;
			heightTexture = loadTexture2D(fullPath);
		}
	}

	if (assimpMaterial->GetTextureCount(aiTextureType_OPACITY) > 0)
	{
		aiString texturePath;
		if (assimpMaterial->GetTexture(aiTextureType_OPACITY, 0, &texturePath) == AI_SUCCESS) {
			std::string fullPath = directory + texturePath.data;
			maskTexture = loadTexture2D(fullPath);
		}
	}
}

void Material::bind(Shader& shader)
{
	shader.bind();

	shader.setUniform1i("Shininess", shininess);
	shader.setUniform1i("Opacity", opacity);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, diffuseTexture.textureID);
	shader.setUniform1i("DiffuseTexture", 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, specularTexture.textureID);
	shader.setUniform1i("SpecularTexture", 1);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, heightTexture.textureID);
	shader.setUniform1i("HeightTexture", 2);
	shader.setUniform2f("HeightTextureSize", heightTexture.width, heightTexture.height);
}
