#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Block.h"

class Renderer;
class Scene;
class SkyBox;
class Chunk;
class WorldGenerator;
enum class ChunkLoadState;

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

	void AsyncLoadChunk();
	void RemoveChunk();
	void CreateChunk(std::weak_ptr<Chunk> chunk);
	void UpdateChunk();
	void RemoveWorldChunk(std::vector<std::pair<int, int>>& _deletableKey);

	static inline std::unique_ptr<World> CreateCraftWorld()
	{
		return std::make_unique<World>();
	}

	bool GetChunkByPos(const std::pair<int, int>& key, std::weak_ptr<Chunk>& outChunk);
	bool IsChunkCreatedByPos(int x, int y);
	bool IsChunkCreatedByPos(const std::pair<int, int>& pos);
	bool GetBlockByWorldPos(int x, int y, int z, Block& block);
	bool SetBlockByWorldPos(int x, int y, int z, BlockType blocktype);
	bool CanEmplaceBlockByWorldPos(int blockX, int blockY, int blockZ, int faceblockX, int faceblockY, int faceblockZ);
	void RegisterReloadChunk(std::pair<int, int> key, const glm::vec3& blockPos);

public:
	static std::unordered_map<BlockType, std::pair<int, int>> BlockCoordData;
	static std::mutex worldMutex;

private:
	std::unique_ptr<Renderer> renderer;
	std::unique_ptr<Scene> scene;
	std::unique_ptr<SkyBox> sky;

	std::unordered_map<std::pair<int, int>, std::shared_ptr<Chunk>, Pair_IntHash> worldChunks;
	std::unordered_map<std::pair<int, int>, std::shared_ptr<Chunk>, Pair_IntHash> loadChunks;
	std::map<std::pair<int, int>, glm::vec3> reloadChunks;

	const int renderDistance = 5;
	glm::vec3 playerPosition;

	std::unique_ptr<WorldGenerator> worldGenerator;
	std::vector<std::future<void>> updateFutures;
};