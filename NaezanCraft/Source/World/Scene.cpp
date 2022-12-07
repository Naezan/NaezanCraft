#include "../pch.h"
#include "Scene.h"
#include "Player.h"
#include "Camera.h"

const glm::mat4* Scene::ViewProjectionMatrix;

Scene::Scene()
{
	//TO DO player position setting
	player = Actor::CreateUnique<Player>(glm::vec3(-40.f, 150.f, 100.f), glm::vec3(20.f, 20.f, 20.f));
	playerCamera = player->GetCamera();
	ViewProjectionMatrix = &player->GetCamera()->GetViewProjectionMatrix();
}

Scene::~Scene() = default;

void Scene::Update()
{
	player->Update();
}
