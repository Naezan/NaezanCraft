#include "../pch.h"
#include "World.h"
#include "Scene.h"
#include "../Renderer/Renderer.h"
#include "Chunk.h"

World::World()
{
	renderer = std::make_unique<Renderer>();
	scene = std::make_unique<Scene>();

	playerPosition = scene->GetPlayerPosition();

	//CreateChunk memory
	worldChunks.reserve(LOOK_CHUNK_SIZE * LOOK_CHUNK_SIZE);
	for (int x = static_cast<int>(playerPosition.x / CHUNK_X) - renderDistance; x <= static_cast<int>(playerPosition.x / CHUNK_X) + renderDistance; ++x)
	{
		for (int z = static_cast<int>(playerPosition.z / CHUNK_Z) - renderDistance; z <= static_cast<int>(playerPosition.z / CHUNK_Z) + renderDistance; ++z)
		{
			Chunk::CreateChunk(worldChunks[std::pair<int, int>(x, z)], glm::vec3(x, 0.f, z));
		}
	}
}

World::~World()
{
	Shutdown();
}

void World::Update()
{
	playerPosition = scene->GetPlayerPosition();

	scene->Update();

	for (int x = static_cast<int>(playerPosition.x / CHUNK_X) - renderDistance; x <= static_cast<int>(playerPosition.x / CHUNK_X) + renderDistance; ++x)
	{
		for (int z = static_cast<int>(playerPosition.z / CHUNK_Z) - renderDistance; z <= static_cast<int>(playerPosition.z / CHUNK_Z) + renderDistance; ++z)
		{
			if (!IsChunkCreatedByPos(x, z))
			{
				//TO DO memory Leak maybe circle ref?
				Chunk::CreateChunk(worldChunks[std::pair<int, int>(x, z)], glm::vec3(x, 0.f, z));
			}
		}
	}
}

void World::Render()
{
	renderer->BeginRender(*Scene::ViewProjectionMatrix);

	scene->Render();

	std::vector<decltype(worldChunks)::key_type> deletableKey;
	for (auto& chunk : worldChunks)
	{
		if (chunk.second->chunkLoadState == ChunkLoadState::UnGenerated)
		{
			chunk.second->CreateChunkMesh();
		}

		//if chunk location is out of range -> erase chunk
		if (chunk.second->position.x < static_cast<int>(playerPosition.x / CHUNK_X) - renderDistance ||
			chunk.second->position.x > static_cast<int>(playerPosition.x / CHUNK_X) + renderDistance ||
			chunk.second->position.z < static_cast<int>(playerPosition.z / CHUNK_Z) - renderDistance ||
			chunk.second->position.z > static_cast<int>(playerPosition.z / CHUNK_Z) + renderDistance)
		{
			deletableKey.push_back(std::make_pair(
				static_cast<int>(chunk.second->position.x),
				static_cast<int>(chunk.second->position.z)
			));
		}

		renderer->RenderChunk(chunk.second);
	}

	//last erase unvisible chunk
	for (auto key : deletableKey)
	{
		//TO DO memory Leak
		worldChunks.erase(key);
	}
	deletableKey.clear();
}

void World::Shutdown()
{
	renderer->Shutdown();
}

bool World::GetChunkByPos(const std::pair<int, int>& key, std::weak_ptr<Chunk>& outChunk)
{
	auto chunkIt = worldChunks.find(key);

	if (chunkIt == worldChunks.end())
	{
		return false;
	}

	outChunk = worldChunks.at(key);
	return true;
}

bool World::IsChunkCreatedByPos(int x, int y)
{
	auto iter = worldChunks.find(std::make_pair(x, y));
	if (iter == worldChunks.end())
	{
		return false;
	}

	return true;
}

bool World::IsChunkCreatedByPos(const std::pair<int, int>& pos)
{
	auto iter = worldChunks.find(pos);
	if (iter == worldChunks.end())
	{
		return false;
	}

	return true;
}
