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
			LoadChunks[std::pair<int, int>(x, z)] = worldChunks[std::pair<int, int>(x, z)];
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

	UpdateChunk();
}

void World::Render()
{
	OPTICK_EVENT();

	renderer->BeginRender(scene->GetCamera().lock()->GetViewProjectionMatrix());

	scene->Render();

	//LoadChunkList에 있는 녀석들만 렌더링하고 삭제하고 등 처리
	int ChunkCount = 0;
	std::vector<decltype(worldChunks)::key_type> deletableKey;
	for (auto& chunk : LoadChunks)
	{
		if (chunk.second->chunkLoadState == ChunkLoadState::Unloaded ||
			chunk.second->chunkLoadState == ChunkLoadState::Loaded)
		{
			//TO DO 쓰레드가 여기서 시행되기때문에 렌더링 될때 매쉬가 생성안되어 있을 경우의 수가 존재한다
			renderFutures.push_back(std::make_shared<std::future<void>>(std::async(std::launch::async, &World::CreateChunk, this, std::ref(chunk.second))));
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
			//wait to render
			renderer->RenderChunk(chunk.second);
			ChunkCount++;
		}
	}

	//std::cout << "ChunkCount : " << ChunkCount << std::endl;

	if (!deletableKey.empty())
	{
		RemoveWorldChunk(deletableKey);
	}
}

void World::Shutdown()
{
	renderer->Shutdown();
	updateFutures.clear();
	renderFutures.clear();
	worldChunks.clear();
	LoadChunks.clear();
}

void World::CreateChunk(std::shared_ptr<Chunk>& chunk)
{
	std::lock_guard<std::mutex> lock(worldMutex);
	chunk->SetupChunkNeighbor();
	//TODO 최적화 후 처리
	//chunk.second->CreateLightMap();
	chunk->CreateSSAO();

	chunk->CreateChunkMesh(false);
}

void World::UpdateChunk()
{
	std::lock_guard<std::mutex> lock(worldMutex);
	for (int x = static_cast<int>(playerPosition.x / CHUNK_X) - renderDistance; x <= static_cast<int>(playerPosition.x / CHUNK_X) + renderDistance; ++x)
	{
		for (int z = static_cast<int>(playerPosition.z / CHUNK_Z) - renderDistance; z <= static_cast<int>(playerPosition.z / CHUNK_Z) + renderDistance; ++z)
		{
			if (!IsChunkCreatedByPos(x, z))
			{
				updateFutures.push_back(std::make_shared<std::future<void>>(std::async(std::launch::async, &World::GenerateChunkTerrain, this, x, z)));
				//Add to LoadChunkList;
				LoadChunks[std::pair<int, int>(x, z)] = worldChunks[std::pair<int, int>(x, z)];
			}
		}
	}
}

void World::GenerateChunkTerrain(int x, int z)
{
	worldChunks[std::pair<int, int>(x, z)] = std::make_shared<Chunk>(glm::vec3(x, 0, z));
	worldChunks[std::pair<int, int>(x, z)]->GenerateTerrain(worldGenerator);
	worldChunks[std::pair<int, int>(x, z)]->chunkLoadState = ChunkLoadState::Loaded;
}

void World::RemoveWorldChunk(std::vector<decltype(worldChunks)::key_type>& _deletableKey)
{
	std::lock_guard<std::mutex> lock(worldMutex);
	//last erase unvisible chunk
	for (auto key : _deletableKey)
	{
		//worldChunks[key].reset();
		worldChunks.erase(key);
		LoadChunks.erase(key);
	}
	_deletableKey.clear();
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