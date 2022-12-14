#include "../pch.h"
#include "Chunk.h"
#include "ChunkMesh.h"
#include "../Renderer/Renderer.h"
#include "../World/Generator/WorldGenerator.h"

Chunk::Chunk(const glm::vec3& pos) :
	position(pos), chunkLoadState(ChunkLoadState::UnGenerated),
	chunkBox(glm::vec3(pos.x * CHUNK_X, pos.y * CHUNK_Y, pos.z * CHUNK_Z), CHUNK_X, CHUNK_Y, CHUNK_Z)
{
	//TO DO change blocktype
	//memset(&chunkBlocks[0][0][0], BlockType::Diamond, CHUNK_X * CHUNK_Y * CHUNK_Z * sizeof(Block));
	std::fill(&chunkBlocks[0][0][0], &chunkBlocks[0][0][0] + CHUNK_X * CHUNK_Y * CHUNK_Z, BlockType::Air);
}

Chunk::~Chunk() = default;

void Chunk::SetBlock(const glm::vec3& blockPos, BlockType type)
{
	chunkBlocks[blockPos.x][blockPos.y][blockPos.z].blockType = type;
}

void Chunk::SetBlock(int x, int y, int z, BlockType type)
{
	chunkBlocks[x][y][z].blockType = type;
}

Block& Chunk::GetBlock(const glm::vec3& blockPos)
{
	return chunkBlocks[blockPos.x][blockPos.y][blockPos.z];
}

Block& Chunk::GetBlock(int x, int y, int z)
{
	return chunkBlocks[x][y][z];
}

void Chunk::SetupChunkMeshNeighbor()
{
	chunkMesh->SetupChunkNeighbor();
}

void Chunk::CreateChunkMesh()
{
	if (chunkLoadState == ChunkLoadState::UnGenerated)
		chunkLoadState = ChunkLoadState::Generated;

	chunkMesh->CreateMesh();
}

void Chunk::GenerateTerrain(std::unique_ptr<WorldGenerator>& worldGenerator)
{
	worldGenerator->GenerateTerrain(shared_from_this());
}