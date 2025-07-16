#include "Renderer.h"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

Renderer::Renderer(GLFWwindow* in_window, int in_width, int in_height, Camera in_camera)
	: window(in_window), width(in_width), height(in_height), camera(in_camera)
{
	shadowMap.width = 4096;
	shadowMap.height = 4096;

	voxelTexture.size = voxelDimensions;

	toLightDirection = glm::vec3(-0.3, 0.9, -0.25);
	glm::mat4 lightViewMatrix = glm::lookAt(toLightDirection, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
	glm::mat4 lightProjectionMatrix = glm::ortho<float>(-120, 120, -120, 120, -500, 500);
	lightViewProjectionMatrix = lightProjectionMatrix * lightViewMatrix;

	glfwSetKeyCallback(in_window, keyCallback);
}

void Renderer::loadModel(std::string path, float scale)
{
	Assimp::Importer importer;

	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_CalcTangentSpace | aiProcess_JoinIdenticalVertices);

	if (!scene)
	{
		std::cout << "Error loading model: " << importer.GetErrorString() << std::endl;
		return;
	}

	std::replace(path.begin(), path.end(), '\\', '/');
	std::string directory = path.substr(0, path.find_last_of('/') + 1);
	for (int i = 0; i < scene->mNumMaterials; ++i)
	{
		Material material;
		material.loadFromAssimp(scene->mMaterials[i], directory);
		materials.push_back(material);
	}

	for (int i = 0; i < scene->mNumMeshes; ++i)
	{
		Mesh mesh;
		mesh.loadFromAssimp(scene->mMeshes[i]);
		mesh.material = &materials[scene->mMeshes[i]->mMaterialIndex];
		mesh.scale = scale;
		meshes.push_back(mesh);
	}
}

void Renderer::loadShader(ShaderType type, const char* vertexPath, const char* fragmentPath, const char* geometryPath)
{
	shaders[type] = Shader(vertexPath, fragmentPath, geometryPath);
}

Shader& Renderer::getShader(ShaderType type)
{
	return shaders[type];
}

void Renderer::initializeShadowMap()
{
	GLuint shadowMapFBO;
	glGenFramebuffers(1, &shadowMapFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);

	glGenTextures(1, &shadowMap.textureID);
	glBindTexture(GL_TEXTURE_2D, shadowMap.textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, shadowMap.width, shadowMap.height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);

	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, shadowMap.textureID, 0);
	glDrawBuffer(GL_NONE);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cout << "Error creating shadow map framebuffer" << std::endl;
		return;
	}

	/*-----------------draw to shadow map-------------------*/
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);

	glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);

	glViewport(0, 0, shadowMap.width, shadowMap.height);

	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	shaders[SHADOW_MAP_SHADER].bind();

	for (Mesh& mesh : meshes)
	{
		glm::mat4 modelMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(mesh.scale));
		glm::mat4 lightModelViewProjectionMatrix = lightViewProjectionMatrix * modelMatrix;
		shaders[SHADOW_MAP_SHADER].setUniformMatrix4fv("LightModelViewProjectionMatrix", lightModelViewProjectionMatrix);
		mesh.draw();
	}

	shaders[SHADOW_MAP_SHADER].unbind();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, width, height);
	/*-----------------draw to shadow map-------------------*/
}

void Renderer::initializeVoxelTexture()
{
	glEnable(GL_TEXTURE_3D);

	glGenTextures(1, &voxelTexture.textureID);
	glBindTexture(GL_TEXTURE_3D, voxelTexture.textureID);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int numVoxels = voxelTexture.size * voxelTexture.size * voxelTexture.size;
	GLubyte* data = new GLubyte[numVoxels * 4];
	for (int i = 0; i < numVoxels * 4; i++)
		data[i] = 0;
	glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA8, voxelTexture.size, voxelTexture.size, voxelTexture.size, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	delete[] data;

	glGenerateMipmap(GL_TEXTURE_3D);

	float size = voxelTotalSize;
	glm::mat4 projectionMatrix = glm::ortho(-size * 0.5f, size * 0.5f, -size * 0.5f, size * 0.5f, size * 0.5f, size * 1.5f);
	glm::mat4 projectiomFromXAxis = projectionMatrix * glm::lookAt(glm::vec3(size, 0, 0), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
	glm::mat4 projectiomFromYAxis = projectionMatrix * glm::lookAt(glm::vec3(0, size, 0), glm::vec3(0, 0, 0), glm::vec3(0, 0, -1));
	glm::mat4 projectiomFromZAxis = projectionMatrix * glm::lookAt(glm::vec3(0, 0, size), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));

	/*-----------------draw to voxel texture-------------------*/
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);

	glViewport(0, 0, voxelTexture.size, voxelTexture.size);
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	shaders[VOXELIZATION_SHADER].bind();

	// uniforms in geometry shader
	shaders[VOXELIZATION_SHADER].setUniformMatrix4fv("ProjectiomFromXAxis", projectiomFromXAxis);
	shaders[VOXELIZATION_SHADER].setUniformMatrix4fv("ProjectiomFromYAxis", projectiomFromYAxis);
	shaders[VOXELIZATION_SHADER].setUniformMatrix4fv("ProjectiomFromZAxis", projectiomFromZAxis);

	// uniforms in fragment shader
	shaders[VOXELIZATION_SHADER].setUniform1i("VoxelDimensions", voxelDimensions);
	shaders[VOXELIZATION_SHADER].setUniform3f("ToLightDirection", toLightDirection.x, toLightDirection.y, toLightDirection.z);

	glActiveTexture(GL_TEXTURE0 + 5);
	glBindTexture(GL_TEXTURE_2D, shadowMap.textureID);
	shaders[VOXELIZATION_SHADER].setUniform1i("ShadowMap", 5);

	glBindImageTexture(6, voxelTexture.textureID, 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA8);
	shaders[VOXELIZATION_SHADER].setUniform1i("VoxelTexture", 6);

	for (Mesh& mesh : meshes)
	{
		mesh.material->bind(shaders[VOXELIZATION_SHADER]);

		glm::mat4 modelMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(mesh.scale));
		glm::mat4 lightModelViewProjectionMatrix = lightViewProjectionMatrix * modelMatrix;

		shaders[VOXELIZATION_SHADER].setUniformMatrix4fv("ModelMatrix", modelMatrix);
		shaders[VOXELIZATION_SHADER].setUniformMatrix4fv("LightModelViewProjectionMatrix", lightModelViewProjectionMatrix);

		mesh.draw();
	}

	shaders[VOXELIZATION_SHADER].unbind();

	glActiveTexture(GL_TEXTURE6);
	glBindTexture(GL_TEXTURE_3D, voxelTexture.textureID);
	glGenerateMipmap(GL_TEXTURE_3D);

	glViewport(0, 0, width, height);
	/*-----------------draw to voxel texture-------------------*/
}

void Renderer::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	Renderer* rendererPtr = static_cast<Renderer*>(glfwGetWindowUserPointer(window));

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	if (key == GLFW_KEY_1 && action == GLFW_PRESS)
		rendererPtr->showDirect = !rendererPtr->showDirect;
	if (key == GLFW_KEY_2 && action == GLFW_PRESS)
		rendererPtr->showIndirectDiffuse = !rendererPtr->showIndirectDiffuse;
	if (key == GLFW_KEY_3 && action == GLFW_PRESS)
		rendererPtr->showIndirectSpecular = !rendererPtr->showIndirectSpecular;
	if (key == GLFW_KEY_4 && action == GLFW_PRESS)
		rendererPtr->showAmbientOcculision = !rendererPtr->showAmbientOcculision;
}

void Renderer::updateCamera(float delta)
{
	static bool first = true;
	static double lastX = 0, lastY = 0;
	if (first)
	{
		glfwGetCursorPos(window, &lastX, &lastY);
		first = false;
	}

	double currentX, currentY;
	glfwGetCursorPos(window, &currentX, &currentY);
	float deltaX = static_cast<float>(currentX - lastX);
	float deltaY = static_cast<float>(currentY - lastY);
	lastX = currentX;
	lastY = currentY;
	camera.turnAround(deltaX * 0.001f, deltaY * -0.001f);

	const float speed = 2.0f;
	float translationDelta = delta * speed;

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.moveForward(translationDelta);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.moveBackward(translationDelta);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.moveRight(translationDelta);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.moveLeft(translationDelta);
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		camera.moveUp(translationDelta);
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		camera.moveDown(translationDelta);
}

void Renderer::render()
{
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, width, height);

	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	shaders[RENDER_SHADER].bind();

	// uniforms in vertex shader
	shaders[RENDER_SHADER].setUniform3f("CameraPosition", camera.getPosition().x, camera.getPosition().y, camera.getPosition().z);
	shaders[RENDER_SHADER].setUniform3f("ToLightDirection", toLightDirection.x, toLightDirection.y, toLightDirection.z);
	shaders[RENDER_SHADER].setUniformMatrix4fv("ViewMatrix", camera.getViewMatrix());
	shaders[RENDER_SHADER].setUniformMatrix4fv("ProjectionMatrix", camera.getProjectionMatrix());

	// uniforms in fragment shader
	shaders[RENDER_SHADER].setUniform1i("VoxelDimensions", voxelDimensions);
	shaders[RENDER_SHADER].setUniform1f("VoxelTotalSize", voxelTotalSize);

	shaders[RENDER_SHADER].setUniform1i("ShowDirect", showDirect);
	shaders[RENDER_SHADER].setUniform1i("ShowIndirectDiffuse", showIndirectDiffuse);
	shaders[RENDER_SHADER].setUniform1i("ShowIndirectSpecular", showIndirectSpecular);
	shaders[RENDER_SHADER].setUniform1i("ShowAmbientOcculision", showAmbientOcculision);

	glActiveTexture(GL_TEXTURE0 + 3);
	glBindTexture(GL_TEXTURE_2D, shadowMap.textureID);
	shaders[RENDER_SHADER].setUniform1i("ShadowMap", 3);

	glActiveTexture(GL_TEXTURE0 + 4);
	glBindTexture(GL_TEXTURE_3D, voxelTexture.textureID);
	shaders[RENDER_SHADER].setUniform1i("VoxelTexture", 4);

	for (Mesh& mesh : meshes)
	{
		mesh.material->bind(shaders[RENDER_SHADER]);

		glm::mat4 modelMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(mesh.scale));
		glm::mat4 lightModelViewProjectionMatrix = lightViewProjectionMatrix * modelMatrix;

		shaders[RENDER_SHADER].setUniformMatrix4fv("ModelMatrix", modelMatrix);
		shaders[RENDER_SHADER].setUniformMatrix4fv("LightModelViewProjectionMatrix", lightModelViewProjectionMatrix);

		mesh.draw();
	}

	shaders[RENDER_SHADER].unbind();
}
