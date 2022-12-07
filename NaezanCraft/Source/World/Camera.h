#pragma once

#include "Actor.h"

class Camera : public Actor
{
public:
	Camera(glm::vec3 pos = glm::vec3(0.f, 0.f, 0.f), glm::vec3 front = glm::vec3(0.f, 0.f, -1.f),
		float fovy = 45.f, float aspect = 1280.0f / 720.0f, float zNear = 0.1f, float zFar = 1000.0f);
	~Camera() = default;

	virtual void Update() override;

	void UpdateRotation(double xpos, double ypos);
	void UpdateViewMatrix();
	void UpdateProjectionMatrix();

	inline const float& GetYaw() { return yaw; }
	inline const float& GetPitch() { return pitch; }
	inline const float& GetFov() { return fov; }
	inline const glm::vec3& GetForwadDir() { return cameraFront; }
	inline const glm::mat4& GetViewMatrix() { return viewMatrix; }
	inline const glm::mat4& GetProjectionMatrix() { return projectionMatrix; }
	inline const glm::mat4& GetViewProjectionMatrix() { return projectionXviewMatrix; }

	void SetFov(float _fov);
	void SetAspect(float _aspect);
	void SetNearFar(float _near, float _far);

private:
	float yaw = -90.f, pitch = 0.f;
	float fov, aspect, zNear, zFar;
	glm::vec3 cameraFront, cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::mat4 viewMatrix, projectionMatrix, projectionXviewMatrix;
};