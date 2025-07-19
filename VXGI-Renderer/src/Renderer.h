#pragma once

#include <iostream>
#include <vector>
#include <map>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Mesh.h"
#include "Camera.h"
#include "Shader.h"

enum ShaderType {
	SHADOW_MAP_SHADER,
	VOXELIZATION_SHADER,
	VOXEL_VISUALIZETION_SHADER,
	RENDER_SHADER
};

class Renderer {
public:
	Renderer(GLFWwindow* in_window, int in_width, int in_height, Camera in_camera);

	void loadModel(std::string path, float scale);

	void loadShader(ShaderType type, const char* vertexPath, const char* fragmentPath, const char* geometryPath = nullptr);
	Shader& getShader(ShaderType type);

	void initializeShadowMap();
	void initializeVoxelTexture();

	static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

	void updateCamera(float delta = 0.1f);

	bool renderVoxelModeOn() { return renderVoxelMode; }
	void renderVoxels();

	void render();

private:
	GLFWwindow* window;
	int width, height;

	Camera camera;

	std::map<ShaderType, Shader> shaders;

	std::vector<Material> materials;
	std::vector<Mesh> meshes;

private:
	glm::vec3 toLightDirection;
	glm::mat4 lightViewProjectionMatrix;

	const int voxelDimensions = 512;
	const float voxelTotalSize = 150.0f;

	Texture2D shadowMap;
	Texture2D shadowMapDepthTexture;
	Texture3D voxelTexture;

	bool renderVoxelMode = false;

	bool showDirect = true;
	bool showIndirectDiffuse = true;
	bool showIndirectSpecular = true;
	bool showAmbientOcculision = true;
};