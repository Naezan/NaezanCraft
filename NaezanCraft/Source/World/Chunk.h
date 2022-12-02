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

class Chunk
{
public:
	Chunk(const glm::vec3& pos);
	~Chunk() = default;

	void SetBlock(const glm::vec3& blockPos, BlockType type);
	Block& GetBlock(const glm::vec3& blockPos);

public:
	//need to use heap memory? but pointer is heavy? 65,536 * 2 byte = 131,072byte?
	//one chunk has 131.072kbyte? chunk is heavy
	array_3d chunkBlocks;
	const glm::vec3 position;
	
private:
	Mesh chunkMesh;
};