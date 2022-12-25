#include "../pch.h"
#include "Player.h"
#include "Camera.h"
#include "../Input/InputSystem.h"
#include "../Window.h"
#include "../Event/EventSystem.h"

Player::Player(glm::vec3 pos, glm::vec3 vel, glm::vec3 acc, glm::vec3 dir)
	: velocity(vel), acceleration(acc), forwardDirection(dir)
{
	position = pos;
	mainCamera = std::make_shared<Camera>(position);
	mainCamera->SetOwner(this);
	//SetupEventCallback
	Window::GetEventDispatcher().AddCallbackFunction(EventType::CursorPos, std::bind(&Player::OnCursorPos, this, std::placeholders::_1));

	//TO DO SetCameraToChild?
}

Player::~Player() = default;

void Player::Update()
{
	const static float cameraSpeed = 0.05f; // adjust accordingly
	if (Input::GetIsKeyPressed(GLFW_KEY_W))
	{
		position += cameraSpeed * mainCamera->GetForwadDir() * velocity;
		mainCamera->GetPosition() = position;
	}
	if (Input::GetIsKeyPressed(GLFW_KEY_S))
	{
		position -= cameraSpeed * mainCamera->GetForwadDir() * velocity;
		mainCamera->GetPosition() = position;
	}
	if (Input::GetIsKeyPressed(GLFW_KEY_A))
	{
		position -= glm::normalize(glm::cross(mainCamera->GetForwadDir(), Actor::UpVector)) * cameraSpeed * velocity;
		mainCamera->GetPosition() = position;
	}
	if (Input::GetIsKeyPressed(GLFW_KEY_D))
	{
		position += glm::normalize(glm::cross(mainCamera->GetForwadDir(), Actor::UpVector)) * cameraSpeed * velocity;
		mainCamera->GetPosition() = position;
	}

	mainCamera->Update();
}

void Player::OnCursorPos(const Event& event)
{
	static const CursorPosEvent& e = EventTypeCast<CursorPosEvent>(event);
	mainCamera->UpdateRotation(e.Getxpos(), e.Getypos());
}
