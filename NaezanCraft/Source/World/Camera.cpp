#include "../pch.h"
#include "Camera.h"
#include "Player.h"
#include "../Renderer/FrustomCulling.h"

Camera::Camera(glm::vec3 pos, glm::vec3 front, float fovy, float aspect, float zNear, float zFar)
{
	position = pos; cameraFront = front;
	fov = fovy; this->aspect = aspect; this->zNear = zNear; this->zFar = zFar;

	viewMatrix = glm::lookAt(position,
		position + cameraFront,
		cameraUp);

	projectionMatrix = glm::perspective(glm::radians(fovy), aspect, zNear, zFar);
	projectionXviewMatrix = projectionMatrix * viewMatrix;//view -> projection 수행 행렬에선 반대로 계산
}

void Camera::Update()
{
	UpdateViewMatrix();
	frustom.UpdateFrustum(projectionXviewMatrix);
}

void Camera::UpdateRotation(double xpos, double ypos)
{
	static float lastX = 640, lastY = 360;

	static bool firstMouse = true;
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates range from bottom to top
	lastX = xpos;
	lastY = ypos;

	const float sensitivity = 0.1f;
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	yaw += xoffset;
	pitch += yoffset;

	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;

	glm::vec3 direction;
	direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	direction.y = sin(glm::radians(pitch));
	direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	cameraFront = glm::normalize(direction);
}

void Camera::UpdateViewMatrix()
{
	viewMatrix = glm::lookAt(position,
		position + cameraFront,
		cameraUp);
	projectionXviewMatrix = projectionMatrix * viewMatrix;
}

void Camera::UpdateProjectionMatrix()
{
	projectionMatrix = glm::perspective(glm::radians(fov), aspect, zNear, zFar);
	projectionXviewMatrix = projectionMatrix * viewMatrix;
}

void Camera::SetFov(float _fov)
{
	fov = _fov;
	UpdateProjectionMatrix();
}

void Camera::SetAspect(float _aspect)
{
	aspect = _aspect;
	UpdateProjectionMatrix();
}

void Camera::SetNearFar(float _near, float _far)
{
	zNear = _near; zFar = _far;
	UpdateProjectionMatrix();
}
