#include "../pch.h"
#include "Scene.h"
#include "Player.h"
#include "Camera.h"

float worldTime = 0.f;
float sunIntensity = 1.f;

Scene::Scene()
{
	//TO DO player position setting
	player = Actor::CreateUnique<Player>(glm::vec3(0.f, 80.f, 0.f));
	playerCamera = player->GetCamera();
}

Scene::~Scene() = default;

void Scene::Update()
{
	player->Update();
}

void Scene::Render()
{
}

glm::vec3& Scene::GetPlayerPosition()
{
	return GetPlayer()->GetPosition();
}
