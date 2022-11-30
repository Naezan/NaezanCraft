#include "../pch.h"
#include "World.h"
#include "Scene.h"
#include "../Renderer/Renderer.h"

World::World()
{
	renderer = std::make_unique<Renderer>();
	scene = std::make_unique<Scene>();
}

World::~World()
{
	Shutdown();
}

void World::Update()
{
	scene->Update();

	renderer->BeginRender(*Scene::ViewProjectionMatrix);
	renderer->Render();
}

void World::Shutdown()
{
	renderer->Shutdown();
}
