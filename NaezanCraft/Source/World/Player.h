#pragma once

#include "Actor.h"

class Camera;

class Player : public Actor
{
public:
	Player(glm::vec3 vel = glm::vec3(0.f, 0.f, 0.f), glm::vec3 acc = glm::vec3(0.f, 0.f, 0.f), glm::vec3 dir = glm::vec3(0.f, 0.f, -1.f));
	~Player() = default;

	virtual void Update() override;

	inline glm::vec3& GetVelocity() { return velocity; }
	inline glm::vec3& GetAcceleration() { return acceleration; }
	inline std::unique_ptr<Camera>& GetCamera() { return mainCamera; }

private:
	glm::vec3 velocity, acceleration, forwardDirection;
	std::unique_ptr<Camera> mainCamera;//single play
};