#include "../pch.h"
#include "Chunk.h"
#include "ChunkMesh.h"
#include "../Renderer/Renderer.h"

Chunk::Chunk(const glm::vec3& pos) :
	position(pos), chunkLoadState(ChunkLoadState::UnGenerated)
{
	//TO DO change blocktype
	//memset(&chunkBlocks[0][0][0], BlockType::Diamond, CHUNK_X * CHUNK_Y * CHUNK_Z * sizeof(Block));
	std::fill(&chunkBlocks[0][0][0], &chunkBlocks[0][0][0] + CHUNK_X* CHUNK_Y * CHUNK_Z, BlockType::Diamond);
}

Chunk::~Chunk() = default;

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
	if(chunkLoadState == ChunkLoadState::UnGenerated)
		chunkLoadState = ChunkLoadState::Generated;

	chunkMesh = std::make_unique<ChunkMesh>(shared_from_this());
	chunkMesh->CreateMesh();
}