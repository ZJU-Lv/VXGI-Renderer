#include "Camera.h"

Camera::Camera(glm::vec3 in_pos, glm::vec3 in_up, float in_yaw, float in_pitch, float in_fieldOfView, float in_aspectRatio, float in_near, float in_far)
	: position(in_pos), up(in_up), yaw(in_yaw), pitch(in_pitch), fieldOfView(in_fieldOfView), aspectRatio(in_aspectRatio), near(in_near), far(in_far)
{
	calculateFrontAndRight();
}

void Camera::calculateFrontAndRight()
{
	front.x = cos(yaw) * cos(pitch);
	front.y = sin(pitch);
	front.z = sin(yaw) * cos(pitch);
	front = glm::normalize(front);

	right = glm::normalize(glm::cross(front, up));
}

glm::mat4 Camera::getViewMatrix()
{
	return glm::lookAt(position, position + front, up);
}

glm::mat4 Camera::getProjectionMatrix()
{
	return glm::perspective(fieldOfView, aspectRatio, near, far);
}

void Camera::moveForward(float delta) {
	position += front * delta;
}

void Camera::moveBackward(float delta) {
	position -= front * delta;
}

void Camera::moveRight(float delta) {
	position += right * delta;
}

void Camera::moveLeft(float delta) {
	position -= right * delta;
}

void Camera::moveUp(float delta) {
	position += up * delta;
}

void Camera::moveDown(float delta) {
	position -= up * delta;
}

void Camera::turnAround(float deltaYaw, float deltaPitch)
{
	yaw += deltaYaw;
	pitch += deltaPitch;

	if (pitch > glm::half_pi<float>())
		pitch = glm::half_pi<float>();
	if (pitch < -glm::half_pi<float>())
		pitch = -glm::half_pi<float>();

	calculateFrontAndRight();
}
