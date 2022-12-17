#pragma once

#include "Block.h"
#include "../Renderer/FrustomCulling.h"

using array_3d = std::array<std::array<std::array<Block, CHUNK_Z>, CHUNK_Y>, CHUNK_X>;
using vector_3d = std::vector<std::vector<std::vector<unsigned int>>>;

class ChunkMesh;
class WorldGenerator;

enum class ChunkLoadState
{
	Generated, UnGenerated
};

class Chunk : public std::enable_shared_from_this<Chunk>
{
public:
	//You Don't new Chunk
	Chunk(const glm::vec3& pos);
	~Chunk();

	void SetBlock(const glm::vec3& blockPos, BlockType type);
	void SetBlock(int x, int y, int z, BlockType type);
	Block& GetBlock(const glm::vec3& blockPos);
	Block& GetBlock(int x, int y, int z);
	Block& GetWorldBlock(const glm::vec3& blockPos);
	Block& GetWorldBlock(int x, int y, int z);

	int GetWorldLightLevel(int x, int y, int z);
	void SetWorldLightLevel(int x, int y, int z, int level);

	void SetupChunkNeighbor();
	void CreateChunkMesh();
	void GenerateTerrain(std::unique_ptr<WorldGenerator>& worldGenerator);

	void CreateLightMap();
	int GetBlockMaxHeight(int x, int z);

public:
	//need to use heap memory? but pointer is heavy? 65,536 * 1 byte?
	//one chunk has 65.536kbyte? chunk is heavy
	//100 chunks has 6.5536mbyte.
	array_3d chunkBlocks;
	glm::vec3 position;
	ChunkLoadState chunkLoadState;
	std::unique_ptr<ChunkMesh> chunkMesh;

	AABox chunkBox;

	std::weak_ptr<Chunk> LeftChunk;
	std::weak_ptr<Chunk> RightChunk;
	std::weak_ptr<Chunk> FrontChunk;
	std::weak_ptr<Chunk> BackChunk;

	//Lighting
	vector_3d LightMap;
	static const std::array <glm::vec3, 6> nearFaces;
};