#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Block.h"

using array_3d = std::array<std::array<std::array<Block, CHUNK_Z>, CHUNK_Y>, CHUNK_X>;
class ChunkMesh;

enum class ChunkLoadState
{
	Generated, UnGenerated
};

class Chunk
{
public:
	//You Don't new Chunk
	Chunk(const glm::vec3& pos);
	~Chunk();

	void SetBlock(const glm::vec3& blockPos, BlockType type);
	Block& GetBlock(const glm::vec3& blockPos);

	void CreateChunkMesh();

	//Must called, not recommended but...
	static void CreateChunk(std::shared_ptr<Chunk>& worldChunk, const glm::vec3& pos);

public:
	//need to use heap memory? but pointer is heavy? 65,536 * 1 byte?
	//one chunk has 65.536kbyte? chunk is heavy
	//100 chunks has 6.5536mbyte.
	array_3d chunkBlocks;
	const glm::vec3 position;
	ChunkLoadState chunkLoadState;
	std::unique_ptr<ChunkMesh> chunkMesh;

protected:
};