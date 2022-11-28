#include "../pch.h"
#include "World.h"
#include "../Renderer/Renderer.h"
#include "Player.h"
#include "Camera.h"

World::World()
{
	renderer = Renderer::CreateRenderer();
	player = Actor::Create<Player>();
}

World::~World()
{
	Shutdown();
}

void World::Update()
{
	renderer->BeginRender(player->GetCamera());
	player->Update();
	renderer->Render();
}

void World::Shutdown()
{
	renderer->Shutdown();
}
