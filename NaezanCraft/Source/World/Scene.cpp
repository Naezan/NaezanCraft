#include "../pch.h"
#include "Scene.h"
#include "Player.h"
#include "Camera.h"
#include "../World/Environment/SkyBox.h"

const glm::mat4* Scene::ViewProjectionMatrix;

Scene::Scene()
{
	//TO DO player position setting
	player = Actor::CreateUnique<Player>(glm::vec3(100.f, 25.f, 50.f), glm::vec3(20.f, 20.f, 20.f));
	playerCamera = player->GetCamera();
	ViewProjectionMatrix = &player->GetCamera()->GetViewProjectionMatrix();
	sky = std::make_unique<SkyBox>();
}

Scene::~Scene() = default;

void Scene::Update()
{
	player->Update();
	sky->Update(player->GetPosition());
}

void Scene::Render()
{
	sky->Render(playerCamera);
}
