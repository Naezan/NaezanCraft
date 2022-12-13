#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Block.h"

class Renderer;
class Scene;
class Chunk;
class WorldGenerator;

class World
{
public:
	World();
	~World();

	void SetBlockDatas();
	void Update();
	void Render();
	void Shutdown();

	static inline std::unique_ptr<World> CreateCraftWorld()
	{
		return std::make_unique<World>();
	}

	bool GetChunkByPos(const std::pair<int, int>& key, std::weak_ptr<Chunk>& outChunk);
	bool IsChunkCreatedByPos(int x, int y);
	bool IsChunkCreatedByPos(const std::pair<int, int>& pos);

public:
	static std::unordered_map<BlockType, std::pair<int, int>> BlockCoordData;

private:
	std::unique_ptr<Renderer> renderer;
	std::unique_ptr<Scene> scene;

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
	//need to chage map?
	std::unordered_map<std::pair<int, int>, std::shared_ptr<Chunk>, Pair_IntHash> worldChunks;

	const int renderDistance = 5;
	glm::vec3 playerPosition;

	std::unique_ptr<WorldGenerator> worldGenerator;
};