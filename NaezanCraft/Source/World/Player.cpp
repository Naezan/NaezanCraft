#include "../pch.h"
#include "Player.h"

#include "Camera.h"
#include "../Input/InputSystem.h"

Player::Player(glm::vec3 vel, glm::vec3 acc, glm::vec3 dir)
	: velocity(vel), acceleration(acc), forwardDirection(dir)
{
	mainCamera = Actor::Create<Camera>();
	mainCamera->SetOwner(this);//TO DO is this working?
}

void Player::Update()
{
	const static float cameraSpeed = 0.05f; // adjust accordingly
	if (Input::GetIsKeyPressed(GLFW_KEY_W))
		velocity = cameraSpeed * mainCamera->GetForwadDir();
	if (Input::GetIsKeyPressed(GLFW_KEY_S))
		velocity = -cameraSpeed * mainCamera->GetForwadDir();
	if (Input::GetIsKeyPressed(GLFW_KEY_A))
		velocity = -glm::normalize(glm::cross(mainCamera->GetForwadDir(), Actor::UpVector)) * cameraSpeed;
	if (Input::GetIsKeyPressed(GLFW_KEY_D))
		velocity = glm::normalize(glm::cross(mainCamera->GetForwadDir(), Actor::UpVector)) * cameraSpeed;

	position += velocity;

	mainCamera->Update();
}
