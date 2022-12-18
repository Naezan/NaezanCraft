#include "../pch.h"
#include "World.h"
#include "Scene.h"
#include "Camera.h"
#include "Chunk.h"
#include "ChunkMesh.h"
#include "../Renderer/Renderer.h"
#include "../World/Generator/WorldGenerator.h"

std::unordered_map<BlockType, std::pair<int, int>> World::BlockCoordData;

World::World()
{
	renderer = std::make_unique<Renderer>();
	scene = std::make_unique<Scene>();
	worldGenerator = std::make_unique<WorldGenerator>();

	playerPosition = scene->GetPlayerPosition();

	SetBlockDatas();

	//CreateChunk memory
	//worldChunks.reserve(LOOK_CHUNK_SIZE * LOOK_CHUNK_SIZE);
	for (int x = static_cast<int>(playerPosition.x / CHUNK_X) - renderDistance; x <= static_cast<int>(playerPosition.x / CHUNK_X) + renderDistance; ++x)
	{
		for (int z = static_cast<int>(playerPosition.z / CHUNK_Z) - renderDistance; z <= static_cast<int>(playerPosition.z / CHUNK_Z) + renderDistance; ++z)
		{
			worldChunks[std::pair<int, int>(x, z)] = std::make_shared<Chunk>(glm::vec3(x, 0, z));
			worldChunks[std::pair<int, int>(x, z)]->GenerateTerrain(worldGenerator);
		}
	}
}

World::~World()
{
	Shutdown();
}

void World::SetBlockDatas()
{
	BlockCoordData[Dirt] = std::make_pair(13, 2);
	BlockCoordData[Sand] = std::make_pair(22, 13);
	BlockCoordData[Stone] = std::make_pair(23, 15);
	//BlockCoordData[Grass] = std::make_pair(10, 10);
	BlockCoordData[Water] = std::make_pair(4, 2);
	BlockCoordData[Lava] = std::make_pair(4, 0);
	BlockCoordData[Iron] = std::make_pair(6, 15);
	BlockCoordData[Gold] = std::make_pair(4, 13);
	BlockCoordData[Diamond] = std::make_pair(13, 0);
	BlockCoordData[Bedrock] = std::make_pair(7, 4);
}

void World::Update()
{
	OPTICK_EVENT();

	scene->Update();

	playerPosition = scene->GetPlayerPosition();

	for (int x = static_cast<int>(playerPosition.x / CHUNK_X) - renderDistance; x <= static_cast<int>(playerPosition.x / CHUNK_X) + renderDistance; ++x)
	{
		for (int z = static_cast<int>(playerPosition.z / CHUNK_Z) - renderDistance; z <= static_cast<int>(playerPosition.z / CHUNK_Z) + renderDistance; ++z)
		{
			if (!IsChunkCreatedByPos(x, z))
			{
				worldChunks[std::pair<int, int>(x, z)] = std::make_shared<Chunk>(glm::vec3(x, 0, z));
				worldChunks[std::pair<int, int>(x, z)]->GenerateTerrain(worldGenerator);
			}
		}
	}
}

void World::Render()
{
	OPTICK_EVENT();

	renderer->BeginRender(scene->GetCamera().lock()->GetViewProjectionMatrix());

	scene->Render();

	int ChunkCount = 0;
	std::vector<decltype(worldChunks)::key_type> deletableKey;
	for (auto& chunk : worldChunks)
	{
		if (chunk.second->chunkLoadState == ChunkLoadState::UnGenerated)
		{
			chunk.second->SetupChunkNeighbor();
			chunk.second->CreateLightMap();
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
