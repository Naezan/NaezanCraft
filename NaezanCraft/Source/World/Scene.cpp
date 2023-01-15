#include "../pch.h"
#include "Scene.h"
#include "Player.h"
#include "Camera.h"

float worldTime = 0.f;
float sunIntensity = 1.f;

Scene::Scene()
{
	//TO DO player position setting
	player = std::make_unique<Player>(glm::vec3(0.f, 80.f, 0.f));
	playerCamera = player->GetCamera();
}

Scene::~Scene() = default;

void Scene::Update()
{
	player->Update();
}

void Scene::Render()
{
	player->Render();
}

glm::vec3& Scene::GetPlayerPosition()
{
	return GetPlayer()->GetPosition();
}
