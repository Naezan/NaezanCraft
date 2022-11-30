#include "../pch.h"
#include "Scene.h"
#include "Player.h"
#include "Camera.h"

const glm::mat4* Scene::ViewProjectionMatrix;

Scene::Scene()
{
	player = Actor::CreateUnique<Player>();
	playerCamera = player->GetCamera();
	ViewProjectionMatrix = &player->GetCamera()->GetViewProjectionMatrix();
}

Scene::~Scene() = default;

void Scene::Update()
{
	player->Update();
}
