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
	//TO DO setup Mesh State?
}

void Chunk::CreateChunk(std::shared_ptr<Chunk>& renderChunk, const glm::vec3& pos)
{
	renderChunk = std::make_shared<Chunk>(std::forward<const glm::vec3&>(pos));
	renderChunk->chunkMesh = std::make_unique<Mesh>(renderChunk->shared_this());
}