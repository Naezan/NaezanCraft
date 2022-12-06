#include "../pch.h"
#include "Scene.h"
#include "Player.h"
#include "Camera.h"

const glm::mat4* Scene::ViewProjectionMatrix;

Scene::Scene()
{
	//TO DO player position setting
	player = Actor::CreateUnique<Player>(glm::vec3(0.f, 70.f, 50.f), glm::vec3(10.f, 10.f, 10.f));
	playerCamera = player->GetCamera();
	ViewProjectionMatrix = &player->GetCamera()->GetViewProjectionMatrix();
}

Scene::~Scene() = default;

void Scene::Update()
{
	player->Update();
}
