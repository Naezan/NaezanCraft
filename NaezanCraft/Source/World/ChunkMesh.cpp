#include "../pch.h"
#include "ChunkMesh.h"
#include "Chunk.h"
#include "World.h"
#include "../Renderer/VertexArray.h"
#include "../Renderer/Buffer.h"

#include <glad/glad.h>

const std::array<glm::u8vec3, 4> ChunkMesh::vertices[]
{
	//trb, tlb, tlf, trf
	{ glm::u8vec3(1.f, 1.f, 0.f),	glm::u8vec3(0.f, 1.f, 0.f),	glm::u8vec3(0.f, 1.f, 1.f),	glm::u8vec3(1.f, 1.f, 1.f) },
	//Brf, Blf, Blb, Brb
	{ glm::u8vec3(1.f, 0.f, 1.f),	glm::u8vec3(0.f, 0.f, 1.f),	glm::u8vec3(0.f, 0.f, 0.f),	glm::u8vec3(1.f, 0.f, 0.f) },
	//flB, frB, frt, flt
	{ glm::u8vec3(0.f, 0.f, 1.f),	glm::u8vec3(1.f, 0.f, 1.f),	glm::u8vec3(1.f, 1.f, 1.f),	glm::u8vec3(0.f, 1.f, 1.f) },
	//brB, blB, blt, brt
	{ glm::u8vec3(1.f, 0.f, 0.f),	glm::u8vec3(0.f, 0.f, 0.f),	glm::u8vec3(0.f, 1.f, 0.f),	glm::u8vec3(1.f, 1.f, 0.f) },
	//rBf, rBb, rtb, rtf
	{ glm::u8vec3(1.f, 0.f, 1.f),	glm::u8vec3(1.f, 0.f, 0.f),	glm::u8vec3(1.f, 1.f, 0.f),	glm::u8vec3(1.f, 1.f, 1.f) },
	//lBb, lBf, ltf, ltb
	{ glm::u8vec3(0.f, 0.f, 0.f),	glm::u8vec3(0.f, 0.f, 1.f),	glm::u8vec3(0.f, 1.f, 1.f),	glm::u8vec3(0.f, 1.f, 0.f) }
};

const std::array<glm::u8vec3, 2> ChunkMesh::indices
{
	glm::u8vec3(0, 1, 2) , glm::u8vec3(2, 3, 0)
};

ChunkMesh::ChunkMesh(std::shared_ptr<Chunk>&& chunk)
{
	parentChunk = chunk;

	meshVertices.clear();
	meshVertices.reserve(CHUNK_SIZE * 6);//한면당4개의점 * chunkSize 1024 -> 2304 576 (256 + 320)
}

ChunkMesh::~ChunkMesh()
{
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
}

void ChunkMesh::CreateBuffer()
{
	//SetVertexBuffer && texCoord
	if (!meshVertices.empty())
	{
		vertexArray = std::make_unique<VertexArray>();

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

		CreateVertexBuffer(static_cast<int>(sizeof(VertTexCoord)), (void*)offsetof(VertTexCoord, pos),
			static_cast<int>(sizeof(VertTexCoord)), (void*)offsetof(VertTexCoord, texcoord),
			static_cast<int>(sizeof(VertTexCoord)), (void*)offsetof(VertTexCoord, lightlevel),
			static_cast<int>(sizeof(VertTexCoord)), (void*)offsetof(VertTexCoord, AO),
			GL_UNSIGNED_BYTE, GL_UNSIGNED_SHORT, GL_UNSIGNED_BYTE, GL_UNSIGNED_BYTE);

		SetVertexBufferData(meshVertices.size() * sizeof(VertTexCoord), &meshVertices.front());

		vertexBuffer->UnBind();
		vertexArray->UnBind();
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
	else if (Chunk::IsEmptyChunk(parentChunk.lock()->BackChunk) || parentChunk.lock()->BackChunk.lock()->GetBlock(glm::vec3(pos.x, pos.y, CHUNK_Z - 1)).IsTransparent())
	{
		AddFace(pos, type, FaceType::Back, texcoord);
	}

	//Z Front
	if (pos.z < CHUNK_Z - 1)
	{
		if (parentChunk.lock()->GetBlock(glm::vec3(pos.x, pos.y, pos.z + 1)).IsTransparent())
			AddFace(pos, type, FaceType::Front, texcoord);
	}
	else if (Chunk::IsEmptyChunk(parentChunk.lock()->FrontChunk) || parentChunk.lock()->FrontChunk.lock()->GetBlock(glm::vec3(pos.x, pos.y, 0)).IsTransparent())
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
	else if (Chunk::IsEmptyChunk(parentChunk.lock()->LeftChunk) || parentChunk.lock()->LeftChunk.lock()->GetBlock(glm::vec3(CHUNK_X - 1, pos.y, pos.z)).IsTransparent())
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
	else if (Chunk::IsEmptyChunk(parentChunk.lock()->RightChunk) || parentChunk.lock()->RightChunk.lock()->GetBlock(glm::vec3(0, pos.y, pos.z)).IsTransparent())
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

	Block& block = parentChunk.lock()->GetBlock(pos);

	/*
	//trb, tlb, tlf, trf
	//Brf, Blf, Blb, Brb
	//flB, frB, frt, flt
	//brB, blB, blt, brt
	//rBf, rBb, rtb, rtf
	//lBb, lBf, ltf, ltb
	*/
	switch (faceType)
	{
	case Top:
		meshVertices.push_back({ pos + vertices[Top][0],texcoords[0],15,(uint8_t)block.Get_trb_AO() });
		meshVertices.push_back({ pos + vertices[Top][1],texcoords[1],15,(uint8_t)block.Get_tlb_AO() });
		meshVertices.push_back({ pos + vertices[Top][2],texcoords[2],15,(uint8_t)block.Get_tlf_AO() });
		meshVertices.push_back({ pos + vertices[Top][3],texcoords[3],15,(uint8_t)block.Get_trf_AO() });
		break;
	case Bottom:
		meshVertices.push_back({ pos + vertices[Bottom][0],texcoords[0],15,(uint8_t)block.Get_Brf_AO() });
		meshVertices.push_back({ pos + vertices[Bottom][1],texcoords[1],15,(uint8_t)block.Get_Blf_AO() });
		meshVertices.push_back({ pos + vertices[Bottom][2],texcoords[2],15,(uint8_t)block.Get_Blb_AO() });
		meshVertices.push_back({ pos + vertices[Bottom][3],texcoords[3],15,(uint8_t)block.Get_Brb_AO() });
		break;
	case Front:
		meshVertices.push_back({ pos + vertices[Front][0],texcoords[0],15,(uint8_t)block.Get_flB_AO() });
		meshVertices.push_back({ pos + vertices[Front][1],texcoords[1],15,(uint8_t)block.Get_frB_AO() });
		meshVertices.push_back({ pos + vertices[Front][2],texcoords[2],15,(uint8_t)block.Get_frt_AO() });
		meshVertices.push_back({ pos + vertices[Front][3],texcoords[3],15,(uint8_t)block.Get_flt_AO() });
		break;
	case Back:
		meshVertices.push_back({ pos + vertices[Back][0],texcoords[0],15,(uint8_t)block.Get_brB_AO() });
		meshVertices.push_back({ pos + vertices[Back][1],texcoords[1],15,(uint8_t)block.Get_blB_AO() });
		meshVertices.push_back({ pos + vertices[Back][2],texcoords[2],15,(uint8_t)block.Get_blt_AO() });
		meshVertices.push_back({ pos + vertices[Back][3],texcoords[3],15,(uint8_t)block.Get_brt_AO() });
		break;
	case Right:
		meshVertices.push_back({ pos + vertices[Right][0],texcoords[0],15,(uint8_t)block.Get_rBf_AO() });
		meshVertices.push_back({ pos + vertices[Right][1],texcoords[1],15,(uint8_t)block.Get_rBb_AO() });
		meshVertices.push_back({ pos + vertices[Right][2],texcoords[2],15,(uint8_t)block.Get_rtb_AO() });
		meshVertices.push_back({ pos + vertices[Right][3],texcoords[3],15,(uint8_t)block.Get_rtf_AO() });
		break;
	case Left:
		meshVertices.push_back({ pos + vertices[Left][0],texcoords[0],15,(uint8_t)block.Get_lBb_AO() });
		meshVertices.push_back({ pos + vertices[Left][1],texcoords[1],15,(uint8_t)block.Get_lBf_AO() });
		meshVertices.push_back({ pos + vertices[Left][2],texcoords[2],15,(uint8_t)block.Get_ltf_AO() });
		meshVertices.push_back({ pos + vertices[Left][3],texcoords[3],15,(uint8_t)block.Get_ltb_AO() });
		break;
	}

	//TODO 최적화 후 처리
	/*const std::array<unsigned char, 6> lightLevels{
		parentChunk.lock()->GetLightLevel(pos.x, pos.y + 1, pos.z),
		parentChunk.lock()->GetLightLevel(pos.x, pos.y - 1, pos.z),
		parentChunk.lock()->GetLightLevel(pos.x, pos.y, pos.z + 1),
		parentChunk.lock()->GetLightLevel(pos.x, pos.y, pos.z - 1),
		parentChunk.lock()->GetLightLevel(pos.x + 1, pos.y, pos.z),
		parentChunk.lock()->GetLightLevel(pos.x - 1, pos.y, pos.z)
	};

	switch (faceType)
	{
	case Top:
		meshVertices.push_back({ pos + vertices[Top][0],texcoords[0],lightLevels[Top] });
		meshVertices.push_back({ pos + vertices[Top][1],texcoords[1],lightLevels[Top] });
		meshVertices.push_back({ pos + vertices[Top][2],texcoords[2],lightLevels[Top] });
		meshVertices.push_back({ pos + vertices[Top][3],texcoords[3],lightLevels[Top] });
		break;
	case Bottom:
		meshVertices.push_back({ pos + vertices[Bottom][0],texcoords[0],lightLevels[Bottom] });
		meshVertices.push_back({ pos + vertices[Bottom][1],texcoords[1],lightLevels[Bottom] });
		meshVertices.push_back({ pos + vertices[Bottom][2],texcoords[2],lightLevels[Bottom] });
		meshVertices.push_back({ pos + vertices[Bottom][3],texcoords[3],lightLevels[Bottom] });
		break;
	case Front:
		meshVertices.push_back({ pos + vertices[Front][0],texcoords[0],lightLevels[Front] });
		meshVertices.push_back({ pos + vertices[Front][1],texcoords[1],lightLevels[Front] });
		meshVertices.push_back({ pos + vertices[Front][2],texcoords[2],lightLevels[Front] });
		meshVertices.push_back({ pos + vertices[Front][3],texcoords[3],lightLevels[Front] });
		break;
	case Back:
		meshVertices.push_back({ pos + vertices[Back][0],texcoords[0],lightLevels[Back] });
		meshVertices.push_back({ pos + vertices[Back][1],texcoords[1],lightLevels[Back] });
		meshVertices.push_back({ pos + vertices[Back][2],texcoords[2],lightLevels[Back] });
		meshVertices.push_back({ pos + vertices[Back][3],texcoords[3],lightLevels[Back] });
		break;
	case Right:
		meshVertices.push_back({ pos + vertices[Right][0],texcoords[0],lightLevels[Right] });
		meshVertices.push_back({ pos + vertices[Right][1],texcoords[1],lightLevels[Right] });
		meshVertices.push_back({ pos + vertices[Right][2],texcoords[2],lightLevels[Right] });
		meshVertices.push_back({ pos + vertices[Right][3],texcoords[3],lightLevels[Right] });
		break;
	case Left:
		meshVertices.push_back({ pos + vertices[Left][0],texcoords[0],lightLevels[Left] });
		meshVertices.push_back({ pos + vertices[Left][1],texcoords[1],lightLevels[Left] });
		meshVertices.push_back({ pos + vertices[Left][2],texcoords[2],lightLevels[Left] });
		meshVertices.push_back({ pos + vertices[Left][3],texcoords[3],lightLevels[Left] });
		break;
	}*/
}

void ChunkMesh::DeleteChunkMesh()
{
	DeleteMesh();

	parentChunk.reset();

	meshIndices.clear();
	meshVertices.clear();
}

glm::u16vec2 ChunkMesh::GetTexCoord(BlockType& type)
{
	std::pair<int, int> coord = World::BlockCoordData[type];
	return glm::u16vec2(coord.first, coord.second);
}
