#include "../pch.h"
#include "Player.h"
#include "Camera.h"
#include "../Input/InputSystem.h"
#include "../Window.h"
#include "../Event/EventSystem.h"
#include "../Application.h"
#include "Chunk.h"
#include "../Math/Ray.h"
#include "../Util/OutLine.h"
#include "../Util/Line.h"

Player::Player(glm::vec3 pos, glm::vec3 vel, glm::vec3 acc, glm::vec3 dir)
	: velocity(vel), acceleration(acc), forwardDirection(dir), playerBox(glm::vec3(-.3f, -1.5f, -.3f), .6f, 1.5f, .6f),
	outBlockPosition(0, 0, 0), outFaceBlockPosition(0, 0, 0), handBlockType(BlockType::Diamond)
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
	bool isMoving = false;
	const static float cameraSpeed = 0.2f; // adjust accordingly
	velocity = glm::vec3();
	if (Input::GetIsKeyPressed(GLFW_KEY_W))
	{
		velocity += mainCamera->GetForwadDir();
		isMoving = true;
	}
	if (Input::GetIsKeyPressed(GLFW_KEY_S))
	{
		velocity -= mainCamera->GetForwadDir();
		isMoving = true;
	}
	if (Input::GetIsKeyPressed(GLFW_KEY_A))
	{
		velocity -= glm::normalize(glm::cross(mainCamera->GetForwadDir(), Actor::UpVector));
		isMoving = true;
	}
	if (Input::GetIsKeyPressed(GLFW_KEY_D))
	{
		velocity += glm::normalize(glm::cross(mainCamera->GetForwadDir(), Actor::UpVector));
		isMoving = true;
	}
	if (Input::GetIsKeyPressed(GLFW_KEY_Q))
	{
		velocity.y = 1;
		isMoving = true;
	}
	if (Input::GetIsKeyPressed(GLFW_KEY_E))
	{
		velocity.y = -1;
		isMoving = true;
	}
	if (Input::GetIsMouseButtonPressed(GLFW_MOUSE_BUTTON_LEFT))
	{
		//DeleteBlock
		if (!Input::isLeftMousePressed)
		{
			Input::isLeftMousePressed = true;
			GET_World()->SetBlockByWorldPos(outBlockPosition.x, outBlockPosition.y, outBlockPosition.z, BlockType::Air);
		}
	}
	if (Input::GetIsMouseButtonReleased(GLFW_MOUSE_BUTTON_LEFT))
	{
		//삭제
		if (Input::isLeftMousePressed)
		{
			Input::isLeftMousePressed = false;
		}
	}
	if (Input::GetIsMouseButtonPressed(GLFW_MOUSE_BUTTON_RIGHT))
	{
		//EmplaceBlock
		if (!Input::isRightMousePressed)
		{
			Input::isRightMousePressed = true;
			if (GET_World()->CanEmplaceBlockByWorldPos(
				outBlockPosition.x, outBlockPosition.y, outBlockPosition.z,
				outFaceBlockPosition.x, outFaceBlockPosition.y, outFaceBlockPosition.z)
				)
			{
				GET_World()->SetBlockByWorldPos(outFaceBlockPosition.x, outFaceBlockPosition.y, outFaceBlockPosition.z, handBlockType);
			}
		}
	}
	if (Input::GetIsMouseButtonReleased(GLFW_MOUSE_BUTTON_RIGHT))
	{
		//설치
		if (Input::isRightMousePressed)
		{
			Input::isRightMousePressed = false;
		}
	}

	//월드 기준 벡터
	glm::normalize(velocity);
	std::array<glm::vec3, 3> directions = { glm::vec3(velocity.x, 0, 0), glm::vec3(0, velocity.y, 0), glm::vec3(0, 0, velocity.z) };
	if (isMoving)
	{
		for (int i = 0; i < DIR_END; ++i)
		{
			switch (i)
			{
			case DIR_X:
				position.x += cameraSpeed * velocity.x;
				break;
			case DIR_Y:
				position.y += cameraSpeed * velocity.y;
				break;
			case DIR_Z:
				position.z += cameraSpeed * velocity.z;
				break;
			}
			Collision(directions[i]);
		}
	}

	mainCamera->GetPosition() = position;
	mainCamera->Update();

	//raycast 0.5칸정도 에러가 보인다
	rayBlock = Ray::BlockTraversal(position, mainCamera->GetForwadDir(), outBlockPosition, outFaceBlockPosition);
}

void Player::Render()
{
	if (rayBlock.blockType != Air)
	{
		OutLine outlineblock(outBlockPosition);
		outlineblock.SetPV(mainCamera->GetViewProjectionMatrix());
		outlineblock.SetColor(glm::vec3(1, 0, 0));
		outlineblock.Render();

		OutLine outlinefaceblock(outFaceBlockPosition);
		outlinefaceblock.SetPV(mainCamera->GetViewProjectionMatrix());
		outlinefaceblock.SetColor(glm::vec3(0, 1, 0));
		outlinefaceblock.Render();
	}

	/*Line line(position, position + mainCamera->GetForwadDir() * 10.f);
	line.setMVP(mainCamera->GetViewProjectionMatrix());
	line.Render();*/
}

void Player::Collision(const glm::vec3& dir)
{
	glm::vec3 cornerWorldPos = position + playerBox.corner;

	//y = 0에서 1.5까지

	float dx = cornerWorldPos.x;
	float dz = cornerWorldPos.z;
	if (dx < 0)
	{
		dx = std::floor(dx);
	}
	if (dz < 0)
	{
		dz = std::floor(dz);
	}

	for (int y = cornerWorldPos.y; y < cornerWorldPos.y + playerBox.y; ++y)
	{
		for (int x = dx; x < cornerWorldPos.x + playerBox.x; ++x)
		{
			for (int z = dz; z < cornerWorldPos.z + playerBox.z; ++z)
			{
				Block block;
				GET_World()->GetBlockByWorldPos(x, y, z, block);
				if (!block.IsFluid())
				{
					if (dir.y > 0)
					{
						position.y = y;
					}
					else if (dir.y < 0)
					{
						position.y = (y + 1) + playerBox.y;
					}

					if (dir.x > 0)
					{
						position.x = x - (playerBox.x / 2.f) - 0.001f;//Add adjustment value(need to decimal type)
					}
					else if (dir.x < 0)
					{
						position.x = (x + 1) + (playerBox.x / 2.f);
					}

					if (dir.z > 0)
					{
						position.z = z - (playerBox.z / 2.f) - 0.001f;//Add adjustment value(need to decimal type)
					}
					else if (dir.z < 0)
					{
						position.z = (z + 1) + (playerBox.z / 2.f);
					}
				}
			}
		}
	}
}

void Player::OnCursorPos(const Event& event)
{
	static const CursorPosEvent& e = EventTypeCast<CursorPosEvent>(event);
	mainCamera->UpdateRotation(e.Getxpos(), e.Getypos());
}
