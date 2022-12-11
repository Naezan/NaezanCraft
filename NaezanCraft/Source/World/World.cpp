#include "../pch.h"
#include "World.h"
#include "Scene.h"
#include "Camera.h"
#include "Chunk.h"
#include "../Renderer/Renderer.h"

World::World()
{
	renderer = std::make_unique<Renderer>();
	scene = std::make_unique<Scene>();

	playerPosition = scene->GetPlayerPosition();

	//CreateChunk memory
	//TO DO 16*16청크가 15*15청크로 바뀌면서 메모리 릭이 생기는 느낌
	worldChunks.reserve(LOOK_CHUNK_SIZE * LOOK_CHUNK_SIZE);
	for (int x = static_cast<int>(playerPosition.x / CHUNK_X) - renderDistance; x <= static_cast<int>(playerPosition.x / CHUNK_X) + renderDistance; ++x)
	{
		for (int z = static_cast<int>(playerPosition.z / CHUNK_Z) - renderDistance; z <= static_cast<int>(playerPosition.z / CHUNK_Z) + renderDistance; ++z)
		{
			worldChunks[std::pair<int, int>(x, z)] = std::make_shared<Chunk>(glm::vec3(x, 0.f, z));
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

	playerPosition = scene->GetPlayerPosition();
	for (int x = static_cast<int>(playerPosition.x / CHUNK_X) - renderDistance; x <= static_cast<int>(playerPosition.x / CHUNK_X) + renderDistance; ++x)
	{
		for (int z = static_cast<int>(playerPosition.z / CHUNK_Z) - renderDistance; z <= static_cast<int>(playerPosition.z / CHUNK_Z) + renderDistance; ++z)
		{
			if (!IsChunkCreatedByPos(x, z))
			{
				worldChunks[std::pair<int, int>(x, z)] = std::make_shared<Chunk>(glm::vec3(x, 0.f, z));
			}
		}
	}
}

void World::Render()
{
	renderer->BeginRender(scene->GetCamera().lock()->GetViewProjectionMatrix());

	scene->Render();

	int ChunkCount = 0;
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

		if (scene->GetCamera().lock()->GetFrustum().AABB(chunk.second->chunkBox) != CullingState::OUTSIDE)
		{
			renderer->RenderChunk(chunk.second);
			ChunkCount++;
		}
	}

	//std::cout << "ChunkCount : " << ChunkCount << std::endl;

	//last erase unvisible chunk
	for (auto key : deletableKey)
	{
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
