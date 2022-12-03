#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Block.h"
#include "Mesh.h"

#define CHUNK_X 16
#define CHUNK_Y 256
#define CHUNK_Z 16

using array_3d = std::array<std::array<std::array<Block, CHUNK_Z>, CHUNK_Y>, CHUNK_X>;

class Chunk : public std::enable_shared_from_this<Chunk>
{
public:
	//You Don't new Chunk
	Chunk(const glm::vec3& pos);
	~Chunk() = default;

	void SetBlock(const glm::vec3& blockPos, BlockType type);
	Block& GetBlock(const glm::vec3& blockPos);

	void CreateChunkMesh();

	//Must called, not recommended but...
	static void CreateChunk(std::shared_ptr<Chunk>& worldChunk, const glm::vec3& pos);

	std::shared_ptr<Chunk> shared_this()
	{
		return shared_from_this();
	}

public:
	//need to use heap memory? but pointer is heavy? 65,536 * 1 byte?
	//one chunk has 65.536kbyte? chunk is heavy
	//100 chunks has 6.5536mbyte.
	array_3d chunkBlocks;
	const glm::vec3 position;

protected:
	std::unique_ptr<Mesh> chunkMesh;
};