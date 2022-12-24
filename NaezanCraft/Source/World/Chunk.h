#pragma once

#include "Block.h"
#include "../Renderer/FrustomCulling.h"

using array_3d = std::array<std::array<std::array<Block, CHUNK_Z>, CHUNK_Y>, CHUNK_X>;
using vector_3d = std::vector<std::vector<std::vector<unsigned char>>>;

class ChunkMesh;
class WorldGenerator;

enum class ChunkLoadState
{
	Unloaded, MeshLoaded, Builted
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
	Block& GetWorldBlock(int wx, int wy, int wz);
	void SetLoadState(const ChunkLoadState& state);

	void SetLightLevel(int x, int y, int z, int level);
	unsigned char GetLightLevel(int x, int y, int z);
	unsigned char GetWorldLightLevel(int wx, int wy, int wz);
	void SetWorldLightLevel(int wx, int wy, int wz, int level);

	void SetupChunkNeighbor();
	void CreateChunkMesh(bool _isRebuild);
	void CreateMeshBuffer();
	void GenerateTerrain(std::unique_ptr<WorldGenerator>& worldGenerator);

	//Lighting
	void CreateLightMap();
	int GetBlockMaxHeight(int x, int z);

	//AO
	void CreateSSAO();
	void CaculateAO(int x, int y, int z, const glm::ivec3& dir);
	uint8_t CacluateVertexAO(bool side1, bool side2, bool corner);

	static bool IsEmptyChunk(std::weak_ptr<Chunk> const& chunk);

public:
	//need to use heap memory? but pointer is heavy? 65,536 * 1 byte?
	//one chunk has 65.536kbyte? chunk is heavy
	//100 chunks has 6.5536mbyte.
	std::vector<std::vector<std::vector<Block>>> chunkBlocks;
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
	static const std::array <glm::ivec3, 6> nearFaces;
	Block emptyBlock;
};