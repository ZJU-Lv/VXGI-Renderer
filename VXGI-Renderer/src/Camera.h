#pragma once

#include <iostream>

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>

class Camera {
public:
	Camera(glm::vec3 in_pos, glm::vec3 in_up, float in_yaw, float in_pitch, float in_fieldOfView, float in_aspectRatio, float in_near, float in_far);

	glm::mat4 getViewMatrix();
	glm::mat4 getProjectionMatrix();

	glm::vec3 getPosition() const { return position; }

	void moveForward(float delta);
	void moveBackward(float delta);
	void moveRight(float delta);
	void moveLeft(float delta);
	void moveUp(float delta);
	void moveDown(float delta);

	void turnAround(float deltaYaw, float deltaPitch);

private:
	void calculateFrontAndRight();

private:
	glm::vec3 position;
	glm::vec3 up;
	glm::vec3 front;
	glm::vec3 right;

	float yaw;
	float pitch;

	float fieldOfView;
	float aspectRatio;

	float near;
	float far;
};