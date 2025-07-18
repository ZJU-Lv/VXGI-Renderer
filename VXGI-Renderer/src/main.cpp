#include <iostream>
#include <filesystem>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Renderer.h"

const int WindowWidth = 1280;
const int WindowHeight = 720;

int main()
{
	/*--------------------initialize--------------------*/
	if (!glfwInit()) {
		std::cerr << "Failed to initialize GLFW" << std::endl;
		return -1;
	}
	
	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	GLFWwindow* window = glfwCreateWindow(1280, 720, "VXGI Renderer", nullptr, nullptr);
	if (!window) {
		std::cerr << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) {
		std::cerr << "Failed to initialize GLEW" << std::endl;
		glfwDestroyWindow(window);
		glfwTerminate();
		return -1;
	}
	/*--------------------initialize--------------------*/

	/*------------------create renderer------------------*/
	glm::vec3 pos = glm::vec3(0.0, 10.0, 0.0);
	glm::vec3 up = glm::vec3(0.0, 1.0, 0.0);
	float yaw = 0.0f;
	float pitch = 0.0f;

	Camera camera(pos, up, yaw, pitch, 45.0f, (float)WindowWidth / WindowHeight, 0.1f, 1000.0f);
	Renderer renderer(window, WindowWidth, WindowHeight, camera);

	glfwSetWindowUserPointer(window, &renderer);

	std::filesystem::path currentFile = __FILE__;
	std::filesystem::path shaderDir = currentFile.parent_path().parent_path() / "shader";
	
	std::filesystem::path shadowMapVert = shaderDir / "shadowMap.vert";
	std::filesystem::path shadowMapFrag = shaderDir / "shadowMap.frag";
	renderer.loadShader(SHADOW_MAP_SHADER, shadowMapVert.string().c_str(), shadowMapFrag.string().c_str());

	std::filesystem::path voxelizationVert = shaderDir / "voxelization.vert";
	std::filesystem::path voxelizationFrag = shaderDir / "voxelization.frag";
	std::filesystem::path voxelizationGeom = shaderDir / "voxelization.geom";
	renderer.loadShader(VOXELIZATION_SHADER, voxelizationVert.string().c_str(), voxelizationFrag.string().c_str(), voxelizationGeom.string().c_str());

	std::filesystem::path voxelVisualizationVert = shaderDir / "voxelVisualization.vert";
	std::filesystem::path voxelVisualizationFrag = shaderDir / "voxelVisualization.frag";
	std::filesystem::path voxelVisualizationGeom = shaderDir / "voxelVisualization.geom";
	renderer.loadShader(VOXEL_VISUALIZETION_SHADER, voxelVisualizationVert.string().c_str(), voxelVisualizationFrag.string().c_str(), voxelVisualizationGeom.string().c_str());

	std::filesystem::path renderVert = shaderDir / "render.vert";
	std::filesystem::path renderFrag = shaderDir / "render.frag";
	renderer.loadShader(RENDER_SHADER, renderVert.string().c_str(), renderFrag.string().c_str());

	std::filesystem::path modelDir = currentFile.parent_path().parent_path() / "model";
	std::filesystem::path sponzaPath = modelDir / "sponza" / "sponza.obj";
	renderer.loadModel(sponzaPath.string().c_str(), 0.05f);

	renderer.initializeShadowMap();
	renderer.initializeVoxelTexture();
	/*------------------create renderer------------------*/

	/*--------------------main loop---------------------*/
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		renderer.updateCamera();

		renderer.renderVoxels();
		/*if(renderer.renderVoxelModeOn())
			renderer.renderVoxels();
		else
			renderer.render();*/

		glfwSwapBuffers(window);
	}
	/*--------------------main loop---------------------*/
	
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}