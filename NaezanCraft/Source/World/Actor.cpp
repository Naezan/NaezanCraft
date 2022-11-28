#include "../pch.h"
#include "Actor.h"

const glm::vec3 Actor::UpVector(glm::vec3(0.0f, 1.0f, 0.0f));

void Actor::SetOwner(Actor* owner)
{
	Owner = owner;
}
