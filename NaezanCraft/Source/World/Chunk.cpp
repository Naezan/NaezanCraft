#include "../pch.h"
#include "Chunk.h"
#include "../Renderer/Renderer.h"

Chunk::Chunk(const glm::vec3& pos) :
	position(pos), chunkMesh(shared_from_this())
{
	memset(&chunkBlocks, BlockType::Air, CHUNK_X * CHUNK_Y * CHUNK_Z);
}

void Chunk::SetBlock(const glm::vec3& blockPos, BlockType type)
{
	chunkBlocks[blockPos.x][blockPos.y][blockPos.z].blockType = type;
}

Block& Chunk::GetBlock(const glm::vec3& blockPos)
{
	return chunkBlocks[blockPos.x][blockPos.y][blockPos.z];
}

void Chunk::CreateChunkMesh()
{
	chunkMesh.CreateMesh();
	//TO DO setup Mesh State?
}

std::shared_ptr<Chunk> Chunk::CreateChunk(const glm::vec3& pos)
{
	struct ChunkMakeShared : public Chunk
	{
		ChunkMakeShared(const glm::vec3& _pos) : Chunk(_pos)
		{}
	};

	return std::make_shared<ChunkMakeShared>(std::forward<const glm::vec3&>(pos));
}