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
	Chunk::CreateChunk(worldChunks[std::pair<int, int>(0, 0)], glm::vec3(0.f, 0.f, 0.f));
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
	//TO DO CraeteChunks by 20 * 20
	//Setup Neighbor chunk?
	auto chunk = GetChunkByPos(std::pair<int, int>(0, 0));
	if (chunk->chunkLoadState == ChunkLoadState::UnGenerated)
	{
		chunk->CreateChunkMesh();
	}

	//if Generated
	renderer->BeginRender(*Scene::ViewProjectionMatrix);
	renderer->Render(chunk);

	/*for (auto chunk : worldChunks)
	{
		renderer->Render();
	}*/
}

void World::Shutdown()
{
	renderer->Shutdown();
}

std::shared_ptr<Chunk> World::GetChunkByPos(const std::pair<int, int> key)
{
	auto chunkIt = worldChunks.find(key);

	if (chunkIt == worldChunks.end())
	{
		return nullptr;
	}

	return worldChunks.at(key);
}
