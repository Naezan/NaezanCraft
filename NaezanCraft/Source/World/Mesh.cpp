#include "../pch.h"
#include "Mesh.h"
#include "Chunk.h"
#include "Block.h"

const std::array<glm::vec3, 4> Mesh::vertices[]
{
	{ glm::vec3(0.5f, 0.5f, -0.5f) ,glm::vec3(-0.5f, 0.5f, -0.5f), glm::vec3(-0.5f, 0.5f, 0.5f),glm::vec3(0.5f, 0.5f, 0.5f) },
	{ glm::vec3(0.5f, -0.5f,  0.5f) ,glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec3(-0.5f, -0.5f, -0.5f),glm::vec3(0.5f, -0.5f, -0.5f) },
	{ glm::vec3(-0.5f, -0.5f,  0.5f) ,glm::vec3(0.5f, -0.5f,  0.5f), glm::vec3(0.5f,  0.5f,  0.5f),glm::vec3(-0.5f,  0.5f,  0.5f) },
	{ glm::vec3(0.5f, -0.5f, -0.5f) ,glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(-0.5f,  0.5f, -0.5f),glm::vec3(0.5f,  0.5f, -0.5f) },
	{ glm::vec3(0.5f, -0.5f,  0.5f) ,glm::vec3(0.5f, -0.5f, -0.5f), glm::vec3(0.5f,  0.5f, -0.5f),glm::vec3(0.5f,  0.5f,  0.5f) },
	{ glm::vec3(-0.5f, -0.5f, -0.5f) ,glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec3(-0.5f,  0.5f,  0.5f),glm::vec3(-0.5f,  0.5f, -0.5f) }
};

const std::array<glm::u8vec3, 2> Mesh::indices
{
	glm::u8vec3(0, 1, 2) , glm::u8vec3(2, 3, 1)
};

const std::array<glm::vec2, 4> Mesh::texcoords
{
	glm::vec2(0.0f, 1.0f) ,glm::vec2(1.0f, 1.0f), glm::vec2(1.0f, 0.0f), glm::vec2(0.0f, 0.0f)
};

void Mesh::CreateChunkMesh(Chunk& chunk)
{
	for (int x = 0; x < CHUNK_X; ++x)
	{
		for (int y = 0; y < CHUNK_Y; ++y)
		{
			for (int z = 0; z < CHUNK_Z; ++z)
			{
				Block& block = chunk.chunkBlocks[x][y][z];

				glm::vec3 tempPos = glm::vec3(x, y, z);
				glm::vec3 worldPosition = chunk.position + tempPos;
				worldPosition /= 2;
				AddFaces(tempPos, block.blockType);
			}
		}
	}
}

void Mesh::AddFaces(glm::vec3& pos, BlockType& type)
{
	//AddFace();
}

void Mesh::AddFace(const glm::vec3& pos, const BlockType& Blocktype, const FaceType& faceType)
{
	
}
