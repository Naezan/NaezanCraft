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

Mesh::Mesh(std::shared_ptr<Chunk> chunk)
{
	parentChunk = chunk;
}

void Mesh::CreateMesh()
{
	for (int x = 0; x < CHUNK_X; ++x)
	{
		for (int y = 0; y < CHUNK_Y; ++y)
		{
			for (int z = 0; z < CHUNK_Z; ++z)
			{
				Block& block = parentChunk->chunkBlocks[x][y][z];
				if (block.blockType == BlockType::Air)
					continue;

				glm::vec3 tempPos = glm::vec3(x, y, z);
				glm::vec3 worldPosition;
				worldPosition.x = parentChunk->position.x * CHUNK_X + x;
				worldPosition.y = y;
				worldPosition.x = parentChunk->position.z * CHUNK_Z + z;
				worldPosition /= 2;
				AddFaces(tempPos, block.blockType);
			}
		}
	}
}

void Mesh::AddFaces(glm::vec3& pos, BlockType& type)
{
	//X Left
	if (pos.x > 0)
	{
		//만약 이전박스가 없다면 비어있으면 안되므로 현재 왼쪽면의 정보를 추가해준다
		if (parentChunk->GetBlock(glm::vec3(pos.x - 1, pos.y, pos.z)).IsTransparent())
			AddFace(pos, type, FaceType::Left);
	}
	else
	{
		//만약 이전 청크의 마지막이 없다면 0번째위치 왼쪽면의 정보를 추가해준다
		AddFace(pos, type, FaceType::Left);
	}

	//X Right
	if (pos.x < CHUNK_X - 1)
	{
		//만약 다음박스가 없다면 비어있으면 안되므로 현재 오른쪽면의 정보를 추가해준다
		if (parentChunk->GetBlock(glm::vec3(pos.x + 1, pos.y, pos.z)).IsTransparent())
			AddFace(pos, type, FaceType::Right);
	}
	else
	{
		//만약 다음 청크의 0번째가 없다면 CHUNK_X - 1위치 으론쪽면의 정보를 추가해준다
		AddFace(pos, type, FaceType::Right);
	}

	//Y Bottom
	if (pos.y > 0)
	{
		if (parentChunk->GetBlock(glm::vec3(pos.x, pos.y - 1, pos.z)).IsTransparent())
			AddFace(pos, type, FaceType::Bottom);
	}
	else
	{
		AddFace(pos, type, FaceType::Bottom);
	}

	//Y Top
	if (pos.y < CHUNK_Y - 1)
	{
		if (parentChunk->GetBlock(glm::vec3(pos.x, pos.y + 1, pos.z)).IsTransparent())
			AddFace(pos, type, FaceType::Top);
	}
	else
	{
		AddFace(pos, type, FaceType::Top);
	}

	//Z Back
	if (pos.z > 0)
	{
		if (parentChunk->GetBlock(glm::vec3(pos.x, pos.y, pos.z - 1)).IsTransparent())
			AddFace(pos, type, FaceType::Back);
	}
	else
	{
		if (parentChunk->GetBlock(glm::vec3(pos.x, pos.y, pos.z + 1)).IsTransparent())
			AddFace(pos, type, FaceType::Back);
	}

	//Z Front
	if (pos.z < CHUNK_Z - 1)
	{
		AddFace(pos, type, FaceType::Front);
	}
	else
	{
		AddFace(pos, type, FaceType::Front);
	}
}

void Mesh::AddFace(const glm::vec3& pos, const BlockType& Blocktype, const FaceType& faceType)
{

}
