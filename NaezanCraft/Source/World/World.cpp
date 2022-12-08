#include "../pch.h"
#include "World.h"
#include "Scene.h"
#include "../Renderer/Renderer.h"
#include "Chunk.h"

World::World()
{
	renderer = std::make_unique<Renderer>();
	scene = std::make_unique<Scene>();

	//CreateChunk memory
	worldChunks.reserve(LOOK_CHUNK_SIZE * LOOK_CHUNK_SIZE);
	for (int i = 0; i < 10; ++i)
	{
		for (int j = 0; j < 10; ++j)
		{
			Chunk::CreateChunk(worldChunks[std::pair<int, int>(i, j)], glm::vec3(i, 0.f, j));
		}
	}
}

World::~World()
{
	Shutdown();
}

void World::Update()
{
	scene->Update();
}

void World::Render()
{
	renderer->BeginRender(*Scene::ViewProjectionMatrix);

	scene->Render();

	//TO DO CraeteChunks by 20 * 20
	//Setup Neighbor chunk?
	//if Generated
	for (auto chunk : worldChunks)
	{
		if (chunk.second->chunkLoadState == ChunkLoadState::UnGenerated)
		{
			chunk.second->CreateChunkMesh();
		}
		renderer->Render(chunk.second);
	}
}

void World::Shutdown()
{
	renderer->Shutdown();
}

bool World::GetChunkByPos(const std::pair<int, int>& key, std::shared_ptr<Chunk>& outChunk)
{
	auto chunkIt = worldChunks.find(key);

	if (chunkIt == worldChunks.end())
	{
		return false;
	}

	outChunk = worldChunks.at(key);
	return true;
}
