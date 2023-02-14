#include "../pch.h"
#include "ChunkMesh.h"
#include "Chunk.h"
#include "World.h"
#include "Water.h"

#include "../Renderer/VertexArray.h"
#include "../Renderer/Buffer.h"

const std::array<glm::i8vec3, 4> ChunkMesh::vertices[]
{
	//trb, tlb, tlf, trf (Top)
	{ glm::i8vec3(1, 1, 0),	glm::i8vec3(0, 1, 0),	glm::i8vec3(0, 1, 1),	glm::i8vec3(1, 1, 1) },

	//Brf, Blf, Blb, Brb (Bottom)
	{ glm::i8vec3(1, 0, 1),	glm::i8vec3(0, 0, 1),	glm::i8vec3(0, 0, 0),	glm::i8vec3(1, 0, 0) },

	//flB, frB, frt, flt (Front)
	{ glm::i8vec3(0, 0, 1),	glm::i8vec3(1, 0, 1),	glm::i8vec3(1, 1, 1),	glm::i8vec3(0, 1, 1) },

	//brB, blB, blt, brt (Back)
	{ glm::i8vec3(1, 0, 0),	glm::i8vec3(0, 0, 0),	glm::i8vec3(0, 1, 0),	glm::i8vec3(1, 1, 0) },

	//rBf, rBb, rtb, rtf (Right)
	{ glm::i8vec3(1, 0, 1),	glm::i8vec3(1, 0, 0),	glm::i8vec3(1, 1, 0),	glm::i8vec3(1, 1, 1) },

	//lBb, lBf, ltf, ltb (Left)
	{ glm::i8vec3(0, 0, 0),	glm::i8vec3(0, 0, 1),	glm::i8vec3(0, 1, 1),	glm::i8vec3(0, 1, 0) }
};

//struct upos3 operator+(const struct upos3& a, const glm::i8vec3& b) {
//	struct upos3 result = {};
//	result.x = a.x + b.x;
//	result.y = a.y + b.y;
//	result.z = a.z + b.z;
//	return result;
//}

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
	std::vector<glm::i8vec3> waterPositions;

	for (int x = 0; x < CHUNK_X; ++x)
	{
		for (int z = 0; z < CHUNK_Z; ++z)
		{
			bool isWater = false;
			for (int y = CHUNK_Y - 1; y >= 0; --y)
			{
				Block& block = parentChunk.lock()->chunkBlocks[x][y][z];
				if (block.blockType == BlockType::Air)
					continue;

				if (block.blockType == BlockType::WaterT)
				{
					if (!isWater)
					{
						isWater = true;
						waterPositions.push_back(glm::i8vec3(x, y, z));
					}
					continue;
				}

				AddFaces(glm::i8vec3(x, y, z), block);
			}
		}
	}

	for (auto& pos : waterPositions)
	{
		auto& b = parentChunk.lock()->GetBlock(pos);
		AddFace(pos, parentChunk.lock()->GetBlock(pos), FaceType::Top, waterVertices);
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
			GL_UNSIGNED_BYTE, GL_UNSIGNED_BYTE, GL_UNSIGNED_BYTE, GL_UNSIGNED_BYTE);

		SetVertexBufferData(meshVertices.size() * sizeof(VertTexCoord), &meshVertices.front());

		vertexBuffer->UnBind();
		vertexArray->UnBind();

		if (!waterVertices.empty())
			parentChunk.lock()->water->CreateBuffer(waterVertices);
	}
}

void ChunkMesh::RebuildBuffer(std::vector<VertTexCoord>& _rebuildVertices)
{
	vertexArray->Bind();
	indexBuffer->Bind();

	float rebuildBlockCount = (_rebuildVertices.size() / 4.f);
	rebuildIndices.resize(rebuildBlockCount * 6);//추가 생성 개수
	for (int i = 0; i < rebuildBlockCount; ++i)
	{
		rebuildIndices[i * 6 + 0] = indices[0].x + 4 * (i + 1) + lastIndex;
		rebuildIndices[i * 6 + 1] = indices[0].y + 4 * (i + 1) + lastIndex;
		rebuildIndices[i * 6 + 2] = indices[0].z + 4 * (i + 1) + lastIndex;
		rebuildIndices[i * 6 + 3] = indices[1].x + 4 * (i + 1) + lastIndex;
		rebuildIndices[i * 6 + 4] = indices[1].y + 4 * (i + 1) + lastIndex;
		rebuildIndices[i * 6 + 5] = indices[1].z + 4 * (i + 1) + lastIndex;
	}

	meshIndices.insert(meshIndices.end(), rebuildIndices.begin(), rebuildIndices.end());
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, meshIndices.size() * sizeof(unsigned int), meshIndices.data(), GL_STATIC_DRAW);
	indicesCount = meshIndices.size();
	lastIndex = rebuildIndices.back();
	rebuildIndices.clear();
	rebuildIndices.shrink_to_fit();

	vertexBuffer->Bind();

	meshVertices.insert(meshVertices.end(), _rebuildVertices.begin(), _rebuildVertices.end());
	glBufferData(GL_ARRAY_BUFFER, meshVertices.size() * sizeof(VertTexCoord), meshVertices.data(), GL_STATIC_DRAW);
	_rebuildVertices.clear();
	_rebuildVertices.shrink_to_fit();

	vertexBuffer->UnBind();
	vertexArray->UnBind();
}

void ChunkMesh::AddFaces(const glm::i8vec3& pos, Block& block)
{
	//Z Back
	if (pos.z > 0)
	{
		Block& backBlock = parentChunk.lock()->GetBlock(glm::vec3(pos.x, pos.y, pos.z - 1));
		if (backBlock.IsTransparent() && !block.IsSameFluid(backBlock.blockType))
			AddFace(pos, block, FaceType::Back, meshVertices);
	}
	else if (!Chunk::IsEmptyChunk(parentChunk.lock()->BackChunk))
	{
		auto& backChunk = parentChunk.lock()->BackChunk.lock();
		Block& backBlock = backChunk->GetBlock(glm::vec3(pos.x, pos.y, CHUNK_Z - 1));
		if (backBlock.IsTransparent() && !block.IsSameFluid(backBlock.blockType))
			AddFace(pos, block, FaceType::Back, meshVertices);
	}

	//Z Front
	if (pos.z < CHUNK_Z - 1)
	{
		Block& frontBlock = parentChunk.lock()->GetBlock(glm::vec3(pos.x, pos.y, pos.z + 1));
		if (frontBlock.IsTransparent() && !block.IsSameFluid(frontBlock.blockType))
			AddFace(pos, block, FaceType::Front, meshVertices);
	}
	else if (!Chunk::IsEmptyChunk(parentChunk.lock()->FrontChunk))
	{
		auto& frontChunk = parentChunk.lock()->FrontChunk.lock();
		Block& frontBlock = frontChunk->GetBlock(glm::vec3(pos.x, pos.y, 0));
		if (frontBlock.IsTransparent() && !block.IsSameFluid(frontBlock.blockType))
			AddFace(pos, block, FaceType::Front, meshVertices);
	}

	//Y Bottom
	if (pos.y > 0)
	{
		Block& bottomBlock = parentChunk.lock()->GetBlock(glm::vec3(pos.x, pos.y - 1, pos.z));
		if (bottomBlock.IsTransparent() && !block.IsSameFluid(bottomBlock.blockType))
			AddFace(pos, block, FaceType::Bottom, meshVertices);
	}
	else
	{
		//Y == 0, Just add Bottom Face Once
		AddFace(pos, block, FaceType::Bottom, meshVertices);
	}

	//Y Top
	if (pos.y < CHUNK_Y - 1)
	{
		Block& topBlock = parentChunk.lock()->GetBlock(glm::vec3(pos.x, pos.y + 1, pos.z));
		if (topBlock.IsTransparent() && !block.IsSameFluid(topBlock.blockType))
			AddFace(pos, block, FaceType::Top, meshVertices);
	}
	else
	{
		//Y == 127, Just add Top Face Once
		AddFace(pos, block, FaceType::Top, meshVertices);
	}


	//X Left
	if (pos.x > 0)
	{
		//만약 이전박스가 없다면 비어있으면 안되므로 현재 왼쪽면의 정보를 추가해준다
		Block& leftBlock = parentChunk.lock()->GetBlock(glm::vec3(pos.x - 1, pos.y, pos.z));
		if (leftBlock.IsTransparent() && !block.IsSameFluid(leftBlock.blockType))
			AddFace(pos, block, FaceType::Left, meshVertices);
	}
	else if (!Chunk::IsEmptyChunk(parentChunk.lock()->LeftChunk))
	{
		//만약 이전 청크의 마지막이 없다면 0번째위치 왼쪽면의 정보를 추가해준다
		auto& leftChunk = parentChunk.lock()->LeftChunk.lock();
		Block& leftBlock = leftChunk->GetBlock(glm::vec3(CHUNK_X - 1, pos.y, pos.z));
		if (leftBlock.IsTransparent() && !block.IsSameFluid(leftBlock.blockType))
			AddFace(pos, block, FaceType::Left, meshVertices);
	}

	//X Right
	if (pos.x < CHUNK_X - 1)
	{
		//만약 다음박스가 없다면 비어있으면 안되므로 현재 오른쪽면의 정보를 추가해준다
		Block& rightBlock = parentChunk.lock()->GetBlock(glm::vec3(pos.x + 1, pos.y, pos.z));
		if (rightBlock.IsTransparent() && !block.IsSameFluid(rightBlock.blockType))
			AddFace(pos, block, FaceType::Right, meshVertices);
	}
	else if (!Chunk::IsEmptyChunk(parentChunk.lock()->RightChunk))
	{
		//만약 다음 청크의 0번째가 없다면 CHUNK_X - 1위치 으론쪽면의 정보를 추가해준다
		auto& rightChunk = parentChunk.lock()->RightChunk.lock();
		Block& rightBlock = rightChunk->GetBlock(glm::vec3(0, pos.y, pos.z));
		if (rightBlock.IsTransparent() && !block.IsSameFluid(rightBlock.blockType))
			AddFace(pos, block, FaceType::Right, meshVertices);
	}
}

void ChunkMesh::AddFace(const glm::i8vec3& pos, Block& block, const FaceType& faceType, std::vector<VertTexCoord>& _meshVertices)
{
	glm::u8vec2 texcoord = GetTexCoord(block.blockType);
	if ((block.blockType == OakLog || block.blockType == BirchLog) && (faceType == FaceType::Top || faceType == FaceType::Bottom))
	{
		texcoord = GetTexCoord(BlockType(block.blockType + 1));
	}
	else if (block.blockType == Grass)
	{
		if (faceType == FaceType::Bottom)
		{
			texcoord = GetTexCoord(Dirt);
		}
		else if (faceType != FaceType::Top)
		{
			texcoord = GetTexCoord(GrassSide);
		}
	}

	const std::array<glm::u8vec2, 4> texcoords{
		glm::u8vec2(SPRITE_WIDTH * texcoord.x, SPRITE_HEIGHT * (texcoord.y + 1)),
		glm::u8vec2(SPRITE_WIDTH * (texcoord.x + 1), SPRITE_HEIGHT * (texcoord.y + 1)),
		glm::u8vec2(SPRITE_WIDTH * (texcoord.x + 1), SPRITE_HEIGHT * texcoord.y),
		glm::u8vec2(SPRITE_WIDTH * texcoord.x, SPRITE_HEIGHT * texcoord.y)
	};

	const std::array<unsigned char, 6> lightLevels{
		parentChunk.lock()->GetSunLight(pos.x, pos.y + 1, pos.z),
		parentChunk.lock()->GetSunLight(pos.x, pos.y - 1, pos.z),
		parentChunk.lock()->GetSunLight(pos.x, pos.y, pos.z + 1),
		parentChunk.lock()->GetSunLight(pos.x, pos.y, pos.z - 1),
		parentChunk.lock()->GetSunLight(pos.x + 1, pos.y, pos.z),
		parentChunk.lock()->GetSunLight(pos.x - 1, pos.y, pos.z)
	};

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
		//0 + 2 < 1 + 3
		/*
		1 -- 0
		| \  |
		2 -- 3
		*/
		if (block.Get_trb_AO() + block.Get_tlf_AO() < block.Get_tlb_AO() + block.Get_trf_AO())
		{
			_meshVertices.push_back({ pos + vertices[Top][1],texcoords[1],lightLevels[Top],(uint8_t)block.Get_tlb_AO() });
			_meshVertices.push_back({ pos + vertices[Top][2],texcoords[2],lightLevels[Top],(uint8_t)block.Get_tlf_AO() });
			_meshVertices.push_back({ pos + vertices[Top][3],texcoords[3],lightLevels[Top],(uint8_t)block.Get_trf_AO() });
			_meshVertices.push_back({ pos + vertices[Top][0],texcoords[0],lightLevels[Top],(uint8_t)block.Get_trb_AO() });
		}
		else
		{
			_meshVertices.push_back({ pos + vertices[Top][0],texcoords[0],lightLevels[Top],(uint8_t)block.Get_trb_AO() });
			_meshVertices.push_back({ pos + vertices[Top][1],texcoords[1],lightLevels[Top],(uint8_t)block.Get_tlb_AO() });
			_meshVertices.push_back({ pos + vertices[Top][2],texcoords[2],lightLevels[Top],(uint8_t)block.Get_tlf_AO() });
			_meshVertices.push_back({ pos + vertices[Top][3],texcoords[3],lightLevels[Top],(uint8_t)block.Get_trf_AO() });
		}
		break;
	case Bottom:
		if (block.Get_Brf_AO() + block.Get_Blb_AO() < block.Get_Blf_AO() + block.Get_Brb_AO())
		{
			_meshVertices.push_back({ pos + vertices[Bottom][1],texcoords[1],lightLevels[Bottom],(uint8_t)block.Get_Blf_AO() });
			_meshVertices.push_back({ pos + vertices[Bottom][2],texcoords[2],lightLevels[Bottom],(uint8_t)block.Get_Blb_AO() });
			_meshVertices.push_back({ pos + vertices[Bottom][3],texcoords[3],lightLevels[Bottom],(uint8_t)block.Get_Brb_AO() });
			_meshVertices.push_back({ pos + vertices[Bottom][0],texcoords[0],lightLevels[Bottom],(uint8_t)block.Get_Brf_AO() });
		}
		else
		{
			_meshVertices.push_back({ pos + vertices[Bottom][0],texcoords[0],lightLevels[Bottom],(uint8_t)block.Get_Brf_AO() });
			_meshVertices.push_back({ pos + vertices[Bottom][1],texcoords[1],lightLevels[Bottom],(uint8_t)block.Get_Blf_AO() });
			_meshVertices.push_back({ pos + vertices[Bottom][2],texcoords[2],lightLevels[Bottom],(uint8_t)block.Get_Blb_AO() });
			_meshVertices.push_back({ pos + vertices[Bottom][3],texcoords[3],lightLevels[Bottom],(uint8_t)block.Get_Brb_AO() });
		}
		break;
	case Front:
		if (block.Get_flB_AO() + block.Get_frt_AO() < block.Get_frB_AO() + block.Get_flt_AO())
		{
			_meshVertices.push_back({ pos + vertices[Front][1],texcoords[1],lightLevels[Front],(uint8_t)block.Get_frB_AO() });
			_meshVertices.push_back({ pos + vertices[Front][2],texcoords[2],lightLevels[Front],(uint8_t)block.Get_frt_AO() });
			_meshVertices.push_back({ pos + vertices[Front][3],texcoords[3],lightLevels[Front],(uint8_t)block.Get_flt_AO() });
			_meshVertices.push_back({ pos + vertices[Front][0],texcoords[0],lightLevels[Front],(uint8_t)block.Get_flB_AO() });
		}
		else
		{
			_meshVertices.push_back({ pos + vertices[Front][0],texcoords[0],lightLevels[Front],(uint8_t)block.Get_flB_AO() });
			_meshVertices.push_back({ pos + vertices[Front][1],texcoords[1],lightLevels[Front],(uint8_t)block.Get_frB_AO() });
			_meshVertices.push_back({ pos + vertices[Front][2],texcoords[2],lightLevels[Front],(uint8_t)block.Get_frt_AO() });
			_meshVertices.push_back({ pos + vertices[Front][3],texcoords[3],lightLevels[Front],(uint8_t)block.Get_flt_AO() });
		}
		break;
	case Back:
		if (block.Get_brB_AO() + block.Get_blt_AO() < block.Get_blB_AO() + block.Get_brt_AO())
		{
			_meshVertices.push_back({ pos + vertices[Back][1],texcoords[1],lightLevels[Back],(uint8_t)block.Get_blB_AO() });
			_meshVertices.push_back({ pos + vertices[Back][2],texcoords[2],lightLevels[Back],(uint8_t)block.Get_blt_AO() });
			_meshVertices.push_back({ pos + vertices[Back][3],texcoords[3],lightLevels[Back],(uint8_t)block.Get_brt_AO() });
			_meshVertices.push_back({ pos + vertices[Back][0],texcoords[0],lightLevels[Back],(uint8_t)block.Get_brB_AO() });
		}
		else
		{
			_meshVertices.push_back({ pos + vertices[Back][0],texcoords[0],lightLevels[Back],(uint8_t)block.Get_brB_AO() });
			_meshVertices.push_back({ pos + vertices[Back][1],texcoords[1],lightLevels[Back],(uint8_t)block.Get_blB_AO() });
			_meshVertices.push_back({ pos + vertices[Back][2],texcoords[2],lightLevels[Back],(uint8_t)block.Get_blt_AO() });
			_meshVertices.push_back({ pos + vertices[Back][3],texcoords[3],lightLevels[Back],(uint8_t)block.Get_brt_AO() });
		}
		break;
	case Right:
		if (block.Get_rBf_AO() + block.Get_rtb_AO() < block.Get_rBb_AO() + block.Get_rtf_AO())
		{
			_meshVertices.push_back({ pos + vertices[Right][1],texcoords[1],lightLevels[Right],(uint8_t)block.Get_rBb_AO() });
			_meshVertices.push_back({ pos + vertices[Right][2],texcoords[2],lightLevels[Right],(uint8_t)block.Get_rtb_AO() });
			_meshVertices.push_back({ pos + vertices[Right][3],texcoords[3],lightLevels[Right],(uint8_t)block.Get_rtf_AO() });
			_meshVertices.push_back({ pos + vertices[Right][0],texcoords[0],lightLevels[Right],(uint8_t)block.Get_rBf_AO() });
		}
		else
		{
			_meshVertices.push_back({ pos + vertices[Right][0],texcoords[0],lightLevels[Right],(uint8_t)block.Get_rBf_AO() });
			_meshVertices.push_back({ pos + vertices[Right][1],texcoords[1],lightLevels[Right],(uint8_t)block.Get_rBb_AO() });
			_meshVertices.push_back({ pos + vertices[Right][2],texcoords[2],lightLevels[Right],(uint8_t)block.Get_rtb_AO() });
			_meshVertices.push_back({ pos + vertices[Right][3],texcoords[3],lightLevels[Right],(uint8_t)block.Get_rtf_AO() });
		}
		break;
	case Left:
		if (block.Get_lBb_AO() + block.Get_ltf_AO() < block.Get_lBf_AO() + block.Get_ltb_AO())
		{
			_meshVertices.push_back({ pos + vertices[Left][1],texcoords[1],lightLevels[Left],(uint8_t)block.Get_lBf_AO() });
			_meshVertices.push_back({ pos + vertices[Left][2],texcoords[2],lightLevels[Left],(uint8_t)block.Get_ltf_AO() });
			_meshVertices.push_back({ pos + vertices[Left][3],texcoords[3],lightLevels[Left],(uint8_t)block.Get_ltb_AO() });
			_meshVertices.push_back({ pos + vertices[Left][0],texcoords[0],lightLevels[Left],(uint8_t)block.Get_lBb_AO() });
		}
		else
		{
			_meshVertices.push_back({ pos + vertices[Left][0],texcoords[0],lightLevels[Left],(uint8_t)block.Get_lBb_AO() });
			_meshVertices.push_back({ pos + vertices[Left][1],texcoords[1],lightLevels[Left],(uint8_t)block.Get_lBf_AO() });
			_meshVertices.push_back({ pos + vertices[Left][2],texcoords[2],lightLevels[Left],(uint8_t)block.Get_ltf_AO() });
			_meshVertices.push_back({ pos + vertices[Left][3],texcoords[3],lightLevels[Left],(uint8_t)block.Get_ltb_AO() });
		}
		break;
	}
}

void ChunkMesh::DeleteChunkMesh()
{
	DeleteMesh();

	parentChunk.reset();

	meshIndices.clear();
	meshVertices.clear();
}

glm::u8vec2 ChunkMesh::GetTexCoord(const BlockType& type)
{
	std::pair<int, int> coord = World::BlockCoordData[type];
	return glm::u8vec2(coord.first, coord.second);
}

bool ChunkMesh::IsValidWaterMesh()
{
	return !waterVertices.empty();
}
