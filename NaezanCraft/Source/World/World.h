#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Block.h"

class Renderer;
class Scene;
class Chunk;
class WorldGenerator;

struct Pair_IntHash
{
	std::size_t operator () (const std::pair<int, int>& p) const {
		std::size_t hash1 = std::hash<int>{}(p.first);
		std::size_t hash2 = std::hash<int>{}(p.second);

		if (hash1 != hash2) {
			return hash1 ^ hash2;
		}

		return hash1;
	}
};

class World
{
public:
	World();
	~World();

	void SetBlockDatas();
	void Update();
	void Render();
	void Shutdown();

	void AsyncCreateChunk();
	void RemoveChunk();
	void CreateChunk(std::shared_ptr<Chunk>& chunk);
	void UpdateChunk();
	void GenerateChunkTerrain(int x, int z);
	void RemoveWorldChunk(std::vector<std::pair<int, int>>& _deletableKey);

	static inline std::unique_ptr<World> CreateCraftWorld()
	{
		return std::make_unique<World>();
	}

	bool GetChunkByPos(const std::pair<int, int>& key, std::weak_ptr<Chunk>& outChunk);
	bool IsChunkCreatedByPos(int x, int y);
	bool IsChunkCreatedByPos(const std::pair<int, int>& pos);

public:
	static std::unordered_map<BlockType, std::pair<int, int>> BlockCoordData;
	static std::unordered_map<std::pair<int, int>, std::shared_ptr<Chunk>, Pair_IntHash> RenderChunks;

private:
	std::unique_ptr<Renderer> renderer;
	std::unique_ptr<Scene> scene;

	std::unordered_map<std::pair<int, int>, std::shared_ptr<Chunk>, Pair_IntHash> worldChunks;
	std::unordered_map<std::pair<int, int>, std::shared_ptr<Chunk>, Pair_IntHash> LoadChunks;

	const int renderDistance = 5;
	glm::vec3 playerPosition;

	std::unique_ptr<WorldGenerator> worldGenerator;
	std::mutex worldMutex;
	std::vector<std::shared_ptr<std::future<void>>> updateFutures;
	std::vector<std::shared_ptr<std::future<void>>> renderFutures;
};