#include "../pch.h"
#include "ChunkMesh.h"
#include "Chunk.h"
#include "World.h"
#include "../Renderer/VertexArray.h"
#include "../Renderer/Buffer.h"

#include <glad/glad.h>

const std::array<glm::u8vec3, 4> ChunkMesh::vertices[]
{
	{ glm::u8vec3(1.f, 1.f, 0.f),	glm::u8vec3(0.f, 1.f, 0.f),	glm::u8vec3(0.f, 1.f, 1.f),	glm::u8vec3(1.f, 1.f, 1.f) },
	{ glm::u8vec3(1.f, 0.f, 1.f),	glm::u8vec3(0.f, 0.f, 1.f),	glm::u8vec3(0.f, 0.f, 0.f),	glm::u8vec3(1.f, 0.f, 0.f) },
	{ glm::u8vec3(0.f, 0.f, 1.f),	glm::u8vec3(1.f, 0.f, 1.f),	glm::u8vec3(1.f, 1.f, 1.f),	glm::u8vec3(0.f, 1.f, 1.f) },
	{ glm::u8vec3(1.f, 0.f, 0.f),	glm::u8vec3(0.f, 0.f, 0.f),	glm::u8vec3(0.f, 1.f, 0.f),	glm::u8vec3(1.f, 1.f, 0.f) },
	{ glm::u8vec3(1.f, 0.f, 1.f),	glm::u8vec3(1.f, 0.f, 0.f),	glm::u8vec3(1.f, 1.f, 0.f),	glm::u8vec3(1.f, 1.f, 1.f) },
	{ glm::u8vec3(0.f, 0.f, 0.f),	glm::u8vec3(0.f, 0.f, 1.f),	glm::u8vec3(0.f, 1.f, 1.f),	glm::u8vec3(0.f, 1.f, 0.f) }
};

const std::array<glm::u8vec3, 2> ChunkMesh::indices
{
	glm::u8vec3(0, 1, 2) , glm::u8vec3(2, 3, 0)
};

ChunkMesh::ChunkMesh(std::shared_ptr<Chunk>&& chunk)
{
	parentChunk = chunk;

	meshVertices.reserve(CHUNK_SIZE * 6);//한면당4개의점 * chunkSize 1024 -> 2304 576 (256 + 320)
}

ChunkMesh::~ChunkMesh()
{
	parentChunk.reset();

	meshIndices.clear();
	meshVertices.clear();
}

void ChunkMesh::CreateMesh()
{
	meshVertices.clear();//size to zero

	for (int x = 0; x < CHUNK_X; ++x)
	{
		for (int y = 0; y < CHUNK_Y; ++y)
		{
			for (int z = 0; z < CHUNK_Z; ++z)
			{
				Block& block = parentChunk.lock()->chunkBlocks[x][y][z];
				if (block.blockType == BlockType::Air)
					continue;

				//GetBlockTypeAndGetTexCoord
				auto texcord = GetTexCoord(block.blockType);
				AddFaces(glm::uvec3(x, y, z), block.blockType, texcord);
			}
		}
	}

	//SetVertexBuffer && texCoord
	if (!meshVertices.empty())
	{
		CreateVertexBuffer(static_cast<int>(sizeof(VertTexCoord)), (void*)offsetof(VertTexCoord, pos),
			static_cast<int>(sizeof(VertTexCoord)), (void*)offsetof(VertTexCoord, texcoord),
			static_cast<int>(sizeof(VertTexCoord)), (void*)offsetof(VertTexCoord, lightlevel),
			GL_UNSIGNED_BYTE, GL_UNSIGNED_SHORT, GL_UNSIGNED_BYTE);

		SetVertexBufferData(meshVertices.size() * sizeof(VertTexCoord), &meshVertices.front());

		float blockCount = (meshVertices.size() / 4.f);
		meshIndices.resize(blockCount * 6);
		for (int i = 0; i < blockCount; ++i)
		{
			meshIndices[i * 6 + 0] = indices[0].x + 4 * i;
			meshIndices[i * 6 + 1] = indices[0].y + 4 * i;
			meshIndices[i * 6 + 2] = indices[0].z + 4 * i;
			meshIndices[i * 6 + 3] = indices[1].x + 4 * i;
			meshIndices[i * 6 + 4] = indices[1].y + 4 * i;
			meshIndices[i * 6 + 5] = indices[1].z + 4 * i;
		}

		CreateIndexBuffer();
		UnbindAll();
	}
}

void ChunkMesh::AddFaces(const glm::u8vec3& pos, BlockType& type, const glm::u16vec2& texcoord)
{
	//Z Back
	if (pos.z > 0)
	{
		if (parentChunk.lock()->GetBlock(glm::vec3(pos.x, pos.y, pos.z - 1)).IsTransparent())
			AddFace(pos, type, FaceType::Back, texcoord);
	}
	else if (IsEmptyChunk(parentChunk.lock()->BackChunk) || parentChunk.lock()->BackChunk.lock()->GetBlock(glm::vec3(pos.x, pos.y, CHUNK_Z - 1)).IsTransparent())
	{
		AddFace(pos, type, FaceType::Back, texcoord);
	}

	//Z Front
	if (pos.z < CHUNK_Z - 1)
	{
		if (parentChunk.lock()->GetBlock(glm::vec3(pos.x, pos.y, pos.z + 1)).IsTransparent())
			AddFace(pos, type, FaceType::Front, texcoord);
	}
	else if (IsEmptyChunk(parentChunk.lock()->FrontChunk) || parentChunk.lock()->FrontChunk.lock()->GetBlock(glm::vec3(pos.x, pos.y, 0)).IsTransparent())
	{
		AddFace(pos, type, FaceType::Front, texcoord);
	}

	//Y Bottom
	if (pos.y > 0)
	{
		if (parentChunk.lock()->GetBlock(glm::vec3(pos.x, pos.y - 1, pos.z)).IsTransparent())
			AddFace(pos, type, FaceType::Bottom, texcoord);
	}
	else
	{
		//Just add Bottom Face Once
		AddFace(pos, type, FaceType::Bottom, texcoord);
	}

	//Y Top
	if (pos.y < CHUNK_Y - 1)
	{
		if (parentChunk.lock()->GetBlock(glm::vec3(pos.x, pos.y + 1, pos.z)).IsTransparent())
			AddFace(pos, type, FaceType::Top, texcoord);
	}
	else
	{
		//Just add Top Face Once
		AddFace(pos, type, FaceType::Top, texcoord);
	}


	//X Left
	if (pos.x > 0)
	{
		//만약 이전박스가 없다면 비어있으면 안되므로 현재 왼쪽면의 정보를 추가해준다
		if (parentChunk.lock()->GetBlock(glm::vec3(pos.x - 1, pos.y, pos.z)).IsTransparent())
			AddFace(pos, type, FaceType::Left, texcoord);
	}
	else if (IsEmptyChunk(parentChunk.lock()->LeftChunk) || parentChunk.lock()->LeftChunk.lock()->GetBlock(glm::vec3(CHUNK_X - 1, pos.y, pos.z)).IsTransparent())
	{
		//만약 이전 청크의 마지막이 없다면 0번째위치 왼쪽면의 정보를 추가해준다
		AddFace(pos, type, FaceType::Left, texcoord);
	}

	//X Right
	if (pos.x < CHUNK_X - 1)
	{
		//만약 다음박스가 없다면 비어있으면 안되므로 현재 오른쪽면의 정보를 추가해준다
		if (parentChunk.lock()->GetBlock(glm::vec3(pos.x + 1, pos.y, pos.z)).IsTransparent())
			AddFace(pos, type, FaceType::Right, texcoord);
	}
	else if (IsEmptyChunk(parentChunk.lock()->RightChunk) || parentChunk.lock()->RightChunk.lock()->GetBlock(glm::vec3(0, pos.y, pos.z)).IsTransparent())
	{
		//만약 다음 청크의 0번째가 없다면 CHUNK_X - 1위치 으론쪽면의 정보를 추가해준다
		AddFace(pos, type, FaceType::Right, texcoord);
	}
}

void ChunkMesh::AddFace(const glm::u8vec3& pos, const BlockType& Blocktype, const FaceType& faceType, const glm::u16vec2& texcoord)
{
	const std::array<glm::u16vec2, 4> texcoords{
		glm::u16vec2(SPRITE_WIDTH * texcoord.x, SPRITE_HEIGHT * (texcoord.y + 1.f)),
		glm::u16vec2(SPRITE_WIDTH * (texcoord.x + 1.f), SPRITE_HEIGHT * (texcoord.y + 1.f)),
		glm::u16vec2(SPRITE_WIDTH * (texcoord.x + 1.f), SPRITE_HEIGHT * texcoord.y),
		glm::u16vec2(SPRITE_WIDTH * texcoord.x, SPRITE_HEIGHT * texcoord.y)
	};

	switch (faceType)
	{
	case Top:
		meshVertices.push_back({ pos + vertices[Top][0],texcoords[0],15 });
		meshVertices.push_back({ pos + vertices[Top][1],texcoords[1],15 });
		meshVertices.push_back({ pos + vertices[Top][2],texcoords[2],15 });
		meshVertices.push_back({ pos + vertices[Top][3],texcoords[3],15 });
		break;
	case Bottom:
		meshVertices.push_back({ pos + vertices[Bottom][0],texcoords[0],15 });
		meshVertices.push_back({ pos + vertices[Bottom][1],texcoords[1],15 });
		meshVertices.push_back({ pos + vertices[Bottom][2],texcoords[2],15 });
		meshVertices.push_back({ pos + vertices[Bottom][3],texcoords[3],15 });
		break;
	case Front:
		meshVertices.push_back({ pos + vertices[Front][0],texcoords[0],15 });
		meshVertices.push_back({ pos + vertices[Front][1],texcoords[1],15 });
		meshVertices.push_back({ pos + vertices[Front][2],texcoords[2],15 });
		meshVertices.push_back({ pos + vertices[Front][3],texcoords[3],15 });
		break;
	case Back:
		meshVertices.push_back({ pos + vertices[Back][0],texcoords[0],15 });
		meshVertices.push_back({ pos + vertices[Back][1],texcoords[1],15 });
		meshVertices.push_back({ pos + vertices[Back][2],texcoords[2],15 });
		meshVertices.push_back({ pos + vertices[Back][3],texcoords[3],15 });
		break;
	case Right:
		meshVertices.push_back({ pos + vertices[Right][0],texcoords[0],15 });
		meshVertices.push_back({ pos + vertices[Right][1],texcoords[1],15 });
		meshVertices.push_back({ pos + vertices[Right][2],texcoords[2],15 });
		meshVertices.push_back({ pos + vertices[Right][3],texcoords[3],15 });
		break;
	case Left:
		meshVertices.push_back({ pos + vertices[Left][0],texcoords[0],15 });
		meshVertices.push_back({ pos + vertices[Left][1],texcoords[1],15 });
		meshVertices.push_back({ pos + vertices[Left][2],texcoords[2],15 });
		meshVertices.push_back({ pos + vertices[Left][3],texcoords[3],15 });
		break;
	}

	int a = 5;
}

glm::u16vec2 ChunkMesh::GetTexCoord(BlockType& type)
{
	std::pair<int, int> coord = World::BlockCoordData[type];
	return glm::u16vec2(coord.first, coord.second);
}

bool ChunkMesh::IsEmptyChunk(std::weak_ptr<Chunk> const& chunk)
{
	return !chunk.owner_before(std::weak_ptr<Chunk>()) && !std::weak_ptr<Chunk>().owner_before(chunk);
}
