#include "../pch.h"
#include "Chunk.h"
#include "../Renderer/Renderer.h"

Chunk::Chunk(const glm::vec3& pos) :
	position(pos)
{
	chunkBlocks.fill({});
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
	chunkMesh->CreateMesh();
	//TO DO Setup Chunk GenerateState
}

void Chunk::CreateChunk(std::shared_ptr<Chunk>& worldChunk, const glm::vec3& pos)
{
	worldChunk = std::make_shared<Chunk>(std::forward<const glm::vec3&>(pos));
	worldChunk->chunkMesh = std::make_unique<Mesh>(worldChunk->shared_this());
}