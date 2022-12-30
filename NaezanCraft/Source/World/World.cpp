#include "../pch.h"
#include "World.h"
#include "Scene.h"
#include "Camera.h"
#include "Chunk.h"
#include "ChunkMesh.h"
#include "../Application.h"
#include "../Renderer/Renderer.h"
#include "../World/Generator/WorldGenerator.h"
#include "../World/Environment/SkyBox.h"

std::unordered_map<BlockType, std::pair<int, int>> World::BlockCoordData;
std::mutex World::worldMutex;

World::World()
{
	renderer = std::make_unique<Renderer>();
	scene = std::make_unique<Scene>();
	sky = std::make_unique<SkyBox>();
	worldGenerator = std::make_unique<WorldGenerator>();

	playerPosition = scene->GetPlayerPosition();

	SetBlockDatas();

	//CreateChunk memory
	//worldChunks.reserve(LOOK_CHUNK_SIZE * LOOK_CHUNK_SIZE);
	updateFutures.push_back(std::future<void>(std::async(std::launch::async, &World::AsyncLoadChunk, this, ChunkLoadState::Unloaded)));
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
	sky->Update(scene->GetPlayerPosition());

	playerPosition = scene->GetPlayerPosition();

	//블록 설치 또는 삭제할때 이 함수가 실행된다
	UpdateChunk();
}

void World::Render()
{
	OPTICK_EVENT();

	renderer->BeginRender(scene->GetCamera().lock()->GetViewProjectionMatrix());

	int ChunkCount = 0;
	std::unique_lock<std::mutex> lock(worldMutex);
	for (auto& chunk : worldChunks)
	{
		if (scene->GetCamera().lock()->GetFrustum().AABB(chunk.second->chunkBox) != CullingState::OUTSIDE)
		{
			if (chunk.second->chunkLoadState == ChunkLoadState::MeshLoaded)
			{
				chunk.second->CreateMeshBuffer();
				chunk.second->SetLoadState(ChunkLoadState::Builted);
			}
			if (chunk.second->chunkLoadState == ChunkLoadState::Builted)
			{
				renderer->RenderChunk(chunk.second);//이미 렌더링 된 녀석은 제외 시켜야함
				++ChunkCount;
			}
		}
	}

	scene->Render();
	sky->Render(scene->GetCamera());

	RemoveChunk();
	//std::cout << ChunkCount << std::endl;
}

void World::Shutdown()
{
	renderer->Shutdown();
	updateFutures.clear();
	worldChunks.clear();
}

void World::AsyncLoadChunk(const ChunkLoadState& loadState)
{
	while (Application::IsRunning())
	{
		for (int x = static_cast<int>(playerPosition.x / CHUNK_X) - renderDistance; x <= static_cast<int>(playerPosition.x / CHUNK_X) + renderDistance; ++x)
		{
			for (int z = static_cast<int>(playerPosition.z / CHUNK_Z) - renderDistance; z <= static_cast<int>(playerPosition.z / CHUNK_Z) + renderDistance; ++z)
			{
				std::unique_lock<std::mutex> lock(worldMutex);
				if (!IsChunkCreatedByPos(x, z))
				{
					worldChunks[std::pair<int, int>(x, z)] = std::make_shared<Chunk>(glm::vec3(x, 0, z));
					worldChunks[std::pair<int, int>(x, z)]->GenerateTerrain(worldGenerator);
				}
				loadChunks.emplace(std::pair<int, int>(x, z), worldChunks[std::pair<int, int>(x, z)]);
			}
		}

		//LoadChunkList에 있는 녀석들만 렌더링하고 삭제하고 등 처리
		for (const auto chunk : loadChunks)
		{
			if (chunk.second->chunkLoadState == loadState)
			{
				CreateChunk(chunk.second);
			}
		}

		loadChunks.clear();
	}
}

void World::RemoveChunk()
{
	std::vector<decltype(worldChunks)::key_type> deletableKey;
	for (auto& chunk : worldChunks)
	{
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
	}

	if (!deletableKey.empty())
	{
		RemoveWorldChunk(deletableKey);
	}
}

void World::CreateChunk(std::weak_ptr<Chunk> chunk)
{
	//chunk.lock()->CreateLightMap();
	chunk.lock()->CreateSSAO();

	std::unique_lock<std::mutex> lock(worldMutex);
	chunk.lock()->CreateChunkMesh(false);

	chunk.lock()->SetupChunkNeighbor();
}

void World::UpdateChunk()
{
	std::unique_lock<std::mutex> lock(worldMutex);
	for (auto& loadinfo : reloadChunks)
	{
		auto chunk = loadChunks.find(loadinfo.first);
		if (chunk != loadChunks.end())
		{
			if (chunk->second->chunkLoadState == ChunkLoadState::Builted)
			{
				chunk->second->CreateChunkMesh(true);
				std::weak_ptr<Chunk> outChunk;
				GetChunkByPos(loadinfo.first, outChunk);
				outChunk.lock()->ReloadSSAO(loadinfo.second);
			}
		}
	}
	reloadChunks.clear();
}

void World::RemoveWorldChunk(std::vector<decltype(worldChunks)::key_type>& _deletableKey)
{
	for (auto key : _deletableKey)
	{
		if (worldChunks[key]->chunkMesh != nullptr)
			worldChunks[key]->chunkMesh->DeleteChunkMesh();
		worldChunks[key].reset();
		worldChunks.erase(key);
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

bool World::GetBlockByWorldPos(int x, int y, int z, Block& block)
{
	std::pair<int, int> key(x, z);
	if (key.first < 0)
	{
		key.first -= (CHUNK_X - 1);
	}
	key.first /= CHUNK_X;

	if (key.second < 0)
	{
		key.second -= (CHUNK_Z - 1);
	}
	key.second /= CHUNK_Z;

	std::weak_ptr<Chunk> outChunk;
	if (GetChunkByPos(key, outChunk))
	{
		x %= CHUNK_X;
		z %= CHUNK_Z;

		if (x < 0)
		{
			x += CHUNK_X;
			x %= CHUNK_X;
		}
		if (z < 0)
		{
			z += CHUNK_Z;
			z %= CHUNK_Z;
		}
		block = outChunk.lock()->GetBlock(x, y, z);
		return true;
	}

	return false;
}

bool World::SetBlockByWorldPos(int x, int y, int z, BlockType blocktype)
{
	std::pair<int, int> key(x, z);
	if (key.first < 0)
	{
		key.first -= (CHUNK_X - 1);
	}
	key.first /= CHUNK_X;

	if (key.second < 0)
	{
		key.second -= (CHUNK_Z - 1);
	}
	key.second /= CHUNK_Z;

	std::weak_ptr<Chunk> outChunk;
	if (GetChunkByPos(key, outChunk))
	{
		x %= CHUNK_X;
		z %= CHUNK_Z;

		if (x < 0)
		{
			x += CHUNK_X;
			x %= CHUNK_X;
		}
		if (z < 0)
		{
			z += CHUNK_Z;
			z %= CHUNK_Z;
		}
		outChunk.lock()->SetBlock(x, y, z, blocktype);

		//리로드할 청크 추가
		RegisterReloadChunk(key, glm::vec3(x, y, z));
		return true;
	}
	return false;
}

bool World::CanEmplaceBlockByWorldPos(int blockX, int blockY, int blockZ, int faceblockX, int faceblockY, int faceblockZ)
{
	Block outBlock;
	if (GetBlockByWorldPos(blockX, blockY, blockZ, outBlock) && !outBlock.IsFluid())
	{
		Block faceBlock;
		if (GetBlockByWorldPos(faceblockX, faceblockY, faceblockZ, faceBlock) && faceBlock.IsFluid())
		{
			return true;
		}
	}

	return false;
}

void World::RegisterReloadChunk(std::pair<int, int> key, const glm::vec3& blockPos)
{
	reloadChunks.emplace(key, blockPos);
}
