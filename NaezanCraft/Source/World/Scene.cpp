#include "../pch.h"
#include "Scene.h"
#include "Player.h"
#include "Camera.h"

float worldTime = 0.f;
float sunIntensity = 1.f;

Scene::Scene()
{
	//TO DO player position setting
	player = Actor::CreateUnique<Player>(glm::vec3(50.f, 80.f, 50.f), glm::vec3(20.f, 20.f, 20.f));
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
