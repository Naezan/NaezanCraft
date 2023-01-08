#include "../pch.h"
#include "Chunk.h"
#include "ChunkMesh.h"
#include "Mesh.h"
#include "../Application.h"
#include "../Renderer/Renderer.h"
#include "../World/Generator/WorldGenerator.h"

const std::array<glm::ivec3, 6> Chunk::nearFaces
{
	glm::ivec3(-1, 0, 0),
	glm::ivec3(1 , 0, 0),
	glm::ivec3(0 , 0,-1),
	glm::ivec3(0 , 0, 1),
	glm::ivec3(0 ,-1, 0),
	glm::ivec3(0 , 1, 0)
};

Chunk::Chunk(const glm::vec3& pos) :
	position(pos), chunkLoadState(ChunkLoadState::Unloaded),
	chunkBox(glm::vec3(pos.x* CHUNK_X, pos.y* CHUNK_Y, pos.z* CHUNK_Z), CHUNK_X, CHUNK_Y, CHUNK_Z),
	LightMap(CHUNK_X, std::vector<std::vector<unsigned char>>(CHUNK_Y, std::vector<unsigned char>(CHUNK_Z))),
	emptyBlock(BlockType::Air),
	chunkBlocks(CHUNK_X, std::vector<std::vector<Block>>(CHUNK_Y, std::vector<Block>(CHUNK_Z, BlockType::Air)))
{
	//TO DO change blocktype
	//memset(&chunkBlocks[0][0][0], BlockType::Diamond, CHUNK_X * CHUNK_Y * CHUNK_Z * sizeof(Block));
	//std::fill(&chunkBlocks[0][0][0], &chunkBlocks[0][0][0] + CHUNK_SIZE, BlockType::Air);
}

Chunk::~Chunk()
{
	LeftChunk.reset();
	RightChunk.reset();
	FrontChunk.reset();
	BackChunk.reset();
}

void Chunk::SetBlock(const glm::vec3& blockPos, BlockType type)
{
	if (blockPos.x < 0)
	{
		if (!IsEmptyChunk(LeftChunk))
			LeftChunk.lock()->SetBlock(CHUNK_X  + blockPos.x, blockPos.y, blockPos.z, type);
		return;
	}
	else if (blockPos.x >= CHUNK_X)
	{
		if (!IsEmptyChunk(RightChunk))
			RightChunk.lock()->SetBlock(blockPos.x - CHUNK_X, blockPos.y, blockPos.z, type);
		return;
	}
	if (blockPos.z < 0)
	{
		if (!IsEmptyChunk(BackChunk))
			BackChunk.lock()->SetBlock(blockPos.x, blockPos.y, CHUNK_Z + blockPos.z, type);
		return;
	}
	else if (blockPos.z >= CHUNK_Z)
	{
		if (!IsEmptyChunk(FrontChunk))
			FrontChunk.lock()->SetBlock(blockPos.x, blockPos.y, blockPos.z - CHUNK_Z, type);
		return;
	}

	chunkBlocks[blockPos.x][blockPos.y][blockPos.z].blockType = type;
}

void Chunk::SetBlock(int x, int y, int z, BlockType type)
{
	if (x < 0)
	{
		if (!IsEmptyChunk(LeftChunk))
			LeftChunk.lock()->SetBlock(CHUNK_X + x, y, z, type);
		return;
	}
	else if (x >= CHUNK_X)
	{
		if (!IsEmptyChunk(RightChunk))
			RightChunk.lock()->SetBlock(x - CHUNK_X, y, z, type);
		return;
	}
	if (z < 0)
	{
		if (!IsEmptyChunk(BackChunk))
			BackChunk.lock()->SetBlock(x, y, CHUNK_Z + z, type);
		return;
	}
	else if (z >= CHUNK_Z)
	{
		if (!IsEmptyChunk(FrontChunk))
			FrontChunk.lock()->SetBlock(x, y, z - CHUNK_Z, type);
		return;
	}

	chunkBlocks[x][y][z].blockType = type;
}

Block& Chunk::GetBlock(const glm::vec3& blockPos)
{
	int x = blockPos.x;
	int y = blockPos.y;
	int z = blockPos.z;

	if (y >= CHUNK_Y || y < 0)
	{
		return emptyBlock;
	}

	if (x < 0)
	{
		if (!IsEmptyChunk(LeftChunk))
			return LeftChunk.lock()->GetBlock(CHUNK_X + x, y, z);
		else
			return emptyBlock;
	}
	else if (x >= CHUNK_X)
	{
		if (!IsEmptyChunk(RightChunk))
			return RightChunk.lock()->GetBlock(x - CHUNK_X, y, z);
		else
			return emptyBlock;
	}
	if (z < 0)
	{
		if (!IsEmptyChunk(BackChunk))
			return BackChunk.lock()->GetBlock(x, y, CHUNK_Z + z);
		else
			return emptyBlock;
	}
	else if (z >= CHUNK_Z)
	{
		if (!IsEmptyChunk(FrontChunk))
			return FrontChunk.lock()->GetBlock(x, y, z - CHUNK_Z);
		else
			return emptyBlock;
	}

	return chunkBlocks[blockPos.x][blockPos.y][blockPos.z];
}

Block& Chunk::GetBlock(int x, int y, int z)
{
	if (y >= CHUNK_Y || y < 0)
	{
		return emptyBlock;
	}

	if (x < 0)
	{
		if (!IsEmptyChunk(LeftChunk))
			return LeftChunk.lock()->GetBlock(CHUNK_X + x, y, z);
		else
			return emptyBlock;
	}
	else if (x >= CHUNK_X)
	{
		if (!IsEmptyChunk(RightChunk))
			return RightChunk.lock()->GetBlock(x - CHUNK_X, y, z);
		else
			return emptyBlock;
	}
	if (z < 0)
	{
		if (!IsEmptyChunk(BackChunk))
			return BackChunk.lock()->GetBlock(x, y, CHUNK_Z + z);
		else
			return emptyBlock;
	}
	else if (z >= CHUNK_Z)
	{
		if (!IsEmptyChunk(FrontChunk))
			return FrontChunk.lock()->GetBlock(x, y, z - CHUNK_Z);
		else
			return emptyBlock;
	}

	return chunkBlocks[x][y][z];
}

Block& Chunk::GetWorldBlock(const glm::vec3& blockPos)
{
	int dx = blockPos.x - position.x;
	int dy = blockPos.y;
	int dz = blockPos.z - position.z;

	if (dy >= CHUNK_Y)
	{
		return emptyBlock;
	}

	if (dx < 0)
	{
		if (!IsEmptyChunk(LeftChunk))
			return LeftChunk.lock()->GetBlock(CHUNK_X - 1, dy, dz);
		else return emptyBlock;
	}
	else if (dx >= CHUNK_X)
	{
		if (!IsEmptyChunk(RightChunk))
			return RightChunk.lock()->GetBlock(0, dy, dz);
		else return emptyBlock;
	}
	if (dz < 0)
	{
		if (!IsEmptyChunk(BackChunk))
			return BackChunk.lock()->GetBlock(dx, dy, CHUNK_Z - 1);
		else return emptyBlock;
	}
	else if (dz >= CHUNK_Z)
	{
		if (!IsEmptyChunk(FrontChunk))
			return FrontChunk.lock()->GetBlock(dx, dy, 0);
		else return emptyBlock;
	}

	return GetBlock(dx, dy, dz);
}

Block& Chunk::GetWorldBlock(int wx, int wy, int wz)
{
	// 범위밖이면 다른방향의 청크블록을 찾는다 만약에 그것도 없다면 디폴트 생성자 리턴
	int dx = wx - position.x;
	int dy = wy;
	int dz = wz - position.z;

	if (dy >= CHUNK_Y)
	{
		return emptyBlock;
	}

	if (dx < 0)
	{
		if (!IsEmptyChunk(LeftChunk))
			return LeftChunk.lock()->GetBlock(CHUNK_X - 1, dy, dz);
		else return emptyBlock;
	}
	else if (dx >= CHUNK_X)
	{
		if (!IsEmptyChunk(RightChunk))
			return RightChunk.lock()->GetBlock(0, dy, dz);
		else return emptyBlock;
	}
	if (dz < 0)
	{
		if (!IsEmptyChunk(BackChunk))
			return BackChunk.lock()->GetBlock(dx, dy, CHUNK_Z - 1);
		else return emptyBlock;
	}
	else if (dz >= CHUNK_Z)
	{
		if (!IsEmptyChunk(FrontChunk))
			return FrontChunk.lock()->GetBlock(dx, dy, 0);
		else return emptyBlock;
	}

	return GetBlock(dx, dy, dz);
}

void Chunk::SetLoadState(const ChunkLoadState& state)
{
	chunkLoadState = state;
}

void Chunk::SetLightLevel(int x, int y, int z, int level)
{
	LightMap[x][y][z] = level;
}

unsigned char Chunk::GetLightLevel(int x, int y, int z)
{
	//TODO 범위를 넘어가게 되면 다른방향 청크 검사
	if (x < 0 || x >= CHUNK_X || y < 0 || y >= CHUNK_Y || z < 0 || z >= CHUNK_Z)
	{
		return 15;
	}
	/*if (y >= CHUNK_Y || y < 0)
	{
		return 15;
	}

	if (x < 0)
	{
		if (!IsEmptyChunk(LeftChunk))
			return LeftChunk.lock()->GetLightLevel(CHUNK_X - 1, y, z);
		else return 15;
	}
	else if (x >= CHUNK_X)
	{
		if (!IsEmptyChunk(RightChunk))
			return RightChunk.lock()->GetLightLevel(0, y, z);
		else return 15;
	}
	if (z < 0)
	{
		if (!IsEmptyChunk(BackChunk))
			return BackChunk.lock()->GetLightLevel(x, y, CHUNK_Z - 1);
		else return 15;
	}
	else if (z >= CHUNK_Z)
	{
		if (!IsEmptyChunk(FrontChunk))
			return FrontChunk.lock()->GetLightLevel(x, y, 0);
		else return 15;
	}*/

	return LightMap[x][y][z];
}

unsigned char Chunk::GetWorldLightLevel(int wx, int wy, int wz)
{
	// 범위밖이면 15를 리턴 그외의 경우엔 특정 청크 블록의 라이트맵의 값을 가져옴
	int dx = wx - position.x;
	int dy = wy;
	int dz = wz - position.z;

	if (dy >= CHUNK_Y)
	{
		return 15;
	}

	if (dx < 0)
	{
		if (!IsEmptyChunk(LeftChunk))
			return LeftChunk.lock()->GetLightLevel(CHUNK_X - 1, dy, dz);
		else return 15;
	}
	else if (dx >= CHUNK_X)
	{
		if (!IsEmptyChunk(RightChunk))
			return RightChunk.lock()->GetLightLevel(0, dy, dz);
		else return 15;
	}
	if (dz < 0)
	{
		if (!IsEmptyChunk(BackChunk))
			return BackChunk.lock()->GetLightLevel(dx, dy, CHUNK_Z - 1);
		else return 15;
	}
	else if (dz >= CHUNK_Z)
	{
		if (!IsEmptyChunk(FrontChunk))
			return FrontChunk.lock()->GetLightLevel(dx, dy, 0);
		else return 15;
	}

	return GetLightLevel(dx, dy, dz);
}

void Chunk::SetWorldLightLevel(int wx, int wy, int wz, int level)
{
	int dx = wx - position.x;
	int dy = wy;
	int dz = wz - position.z;

	if (dy >= CHUNK_Y)
	{
		return;
	}

	if (dx < 0)
	{
		if (!IsEmptyChunk(LeftChunk))
			LeftChunk.lock()->SetLightLevel(CHUNK_X - 1, dy, dz, level);
		return;
	}
	else if (dx >= CHUNK_X)
	{
		if (!IsEmptyChunk(RightChunk))
			RightChunk.lock()->SetLightLevel(0, dy, dz, level);
		return;
	}
	if (dz < 0)
	{
		if (!IsEmptyChunk(BackChunk))
			BackChunk.lock()->SetLightLevel(dx, dy, CHUNK_Z - 1, level);
		return;
	}
	else if (dz >= CHUNK_Z)
	{
		if (!IsEmptyChunk(FrontChunk))
			FrontChunk.lock()->SetLightLevel(dx, dy, 0, level);
		return;
	}

	SetLightLevel(dx, dy, dz, level);
}

void Chunk::SetupChunkNeighbor()
{
	if (GET_World()->GetChunkByPos(std::pair<int, int>(static_cast<int>(position.x - 1), static_cast<int>(position.z)), LeftChunk))
	{
		LeftChunk.lock()->RightChunk = shared_from_this();
	}
	else
		LeftChunk.reset();
	if (GET_World()->GetChunkByPos(std::pair<int, int>(static_cast<int>(position.x + 1), static_cast<int>(position.z)), RightChunk))
	{
		RightChunk.lock()->LeftChunk = shared_from_this();
	}
	else
		RightChunk.reset();
	if (GET_World()->GetChunkByPos(std::pair<int, int>(static_cast<int>(position.x), static_cast<int>(position.z - 1)), BackChunk))
	{
		BackChunk.lock()->FrontChunk = shared_from_this();
	}
	else
		BackChunk.reset();
	if (GET_World()->GetChunkByPos(std::pair<int, int>(static_cast<int>(position.x), static_cast<int>(position.z + 1)), FrontChunk))
	{
		FrontChunk.lock()->BackChunk = shared_from_this();
	}
	else
		FrontChunk.reset();
}

void Chunk::CreateChunkMesh(bool isReload)
{
	OPTICK_EVENT();

	if (isReload)
	{
		chunkMesh.reset();
	}

	//TODO 메쉬 생성 자체를 쓰레드에 넣지않고 외부에서 따로 처리해줘야하고 여기선 그 메쉬를 복사해서 가져와줘야한다
	chunkMesh = std::make_unique<ChunkMesh>(shared_from_this());
	chunkMesh->CreateMesh();
	SetLoadState(ChunkLoadState::MeshLoaded);
}

void Chunk::CreateMeshBuffer()
{
	chunkMesh->CreateBuffer();
}

void Chunk::GenerateTerrain(std::unique_ptr<WorldGenerator>& worldGenerator)
{
	worldGenerator->GenerateTerrain(shared_from_this());
}

void Chunk::RebuildChunkMesh()
{
	//recreate vertexbuffer and indexbuffer
	chunkMesh->RebuildBuffer(rebuildVertices);
}

void Chunk::CreateLightMap()
{
	OPTICK_EVENT();
	std::queue<glm::ivec3> blockPosQue;
	//맨위의 벽들을 모두 15로 만든다
	for (int x = 0; x < CHUNK_X; ++x)
	{
		for (int z = 0; z < CHUNK_Z; ++z)
		{
			glm::ivec3 pos(x, CHUNK_Y - 1, z);
			blockPosQue.push(pos);
			LightMap[x][CHUNK_Y - 1][z] = 15;
		}
	}

	//맨위에서 아래부분들을 검사하면서 표면부분의 라이팅을 셋팅
	while (!blockPosQue.empty())
	{
		glm::ivec3 localPos = blockPosQue.front();
		unsigned char lightLevel = LightMap[localPos.x][localPos.y][localPos.z];
		glm::ivec3 worldPos =
		{
			localPos.x + position.x,
			localPos.y,
			localPos.z + position.z
		};

		blockPosQue.pop();

		//TODO setting world chunk lighting
		for (int i = 0; i < 6; ++i)
		{
			int dx = worldPos.x + nearFaces[i].x;
			int dy = worldPos.y + nearFaces[i].y;
			int dz = worldPos.z + nearFaces[i].z;
			if (dy < 0) continue;

			Block& block = GetWorldBlock(dx, dy, dz);
			if (block.IsSmooth())
			{
				bool isChangeLight = false;
				int maxHeight = GetBlockMaxHeight(localPos.x, localPos.z);
				//상하 좌우 위아래가 투명인 경우
				if (nearFaces[i].y == 0 || dy < maxHeight)
				{
					//현재의 라이팅보다 한단계 낮은 빛 적용
					if (GetWorldLightLevel(dx, dy, dz) < lightLevel - 1)
					{
						isChangeLight = true;
						SetWorldLightLevel(dx, dy, dz, lightLevel - 1);
					}
				}
				//최대높이 이상의 블럭들
				else
				{
					//셋팅이 안되어 있다면 웬만해선 15로 설정
					if (GetWorldLightLevel(dx, dy, dz) < lightLevel)
					{
						isChangeLight = true;
						SetWorldLightLevel(dx, dy, dz, lightLevel);
					}
				}

				if (isChangeLight)
				{
					int localdx = dx - position.x;
					int localdz = dz - position.z;
					if (localdx >= 0 && localdx < CHUNK_X && localdz >= 0 && localdz < CHUNK_Z)
						blockPosQue.emplace(dx - position.x, dy, dz - position.z);
				}
			}
		}
	}
}

int Chunk::GetBlockMaxHeight(int x, int z)
{
	OPTICK_EVENT();
	for (int y = CHUNK_Y - 1; y >= 0; --y) {
		if (GetBlock(x, y, z).IsGround()) return y;//TO DO
	}
	return 0;
}

void Chunk::CreateSSAO()
{
	for (int8_t x = 0; x < CHUNK_X; ++x)
	{
		for (int8_t z = 0; z < CHUNK_Z; ++z)
		{
			int h = GetBlockMaxHeight(x, z);
			for (int8_t y = 0; y <= h; ++y)
			{
				Block& block = GetBlock(x, h, z);
				if (!block.IsFluid())
				{
					for (auto& dir : nearFaces)
					{
						CaculateAO(x, y, z, dir);
					}
				}
			}
		}
	}
}

void Chunk::ReloadSSAO(int x, int y, int z)
{
	for (int8_t dx = x - 1; dx <= x + 1; ++dx)
	{
		for (int8_t dz = z - 1; dz <= z + 1; ++dz)
		{
			for (int8_t dy = y - 1; dy <= y + 1; ++dy)
			{
				Block& block = GetBlock(dx, dy, dz);
				if (!block.IsFluid())
				{
					for (auto& dir : nearFaces)
					{
						CaculateAO(dx, dy, dz, dir);
					}
				}
			}
		}
	}
}

void Chunk::ReloadSSAO(const glm::vec3& loadPos)
{
	int x = loadPos.x;
	int y = loadPos.y;
	int z = loadPos.z;
	for (int8_t dx = x - 1; dx <= x + 1; ++dx)
	{
		for (int8_t dz = z - 1; dz <= z + 1; ++dz)
		{
			for (int8_t dy = y - 1; dy <= y + 1; ++dy)
			{
				Block& block = GetBlock(dx, dy, dz);
				if (!block.IsFluid())
				{
					for (auto& dir : nearFaces)
					{
						CaculateAO(dx, dy, dz, dir);
					}
				}
			}
		}
	}
}

void Chunk::CaculateAO(int x, int y, int z, const glm::ivec3& dir)
{
	//Right
	if (dir.x > 0)
	{
		bool topfront = !GetBlock(x + 1, y + 1, z + 1).IsSmooth();
		bool topback = !GetBlock(x + 1, y + 1, z - 1).IsSmooth();
		bool bottomfront = !GetBlock(x + 1, y - 1, z + 1).IsSmooth();
		bool bottomback = !GetBlock(x + 1, y - 1, z - 1).IsSmooth();

		bool top = !GetBlock(x + 1, y + 1, z).IsSmooth();
		bool bottom = !GetBlock(x + 1, y - 1, z).IsSmooth();
		bool front = !GetBlock(x + 1, y, z + 1).IsSmooth();
		bool back = !GetBlock(x + 1, y, z - 1).IsSmooth();

		//2비트로 하나의 점 표현가능
		//1바이트면 한면 표현가능, 6바이트면 6면 다 표현가능
		auto& block = GetBlock(x, y, z);

		block.Set_rtf_AO(CacluateVertexAO(top, front, topfront));
		block.Set_rtb_AO(CacluateVertexAO(top, back, topback));
		block.Set_rBf_AO(CacluateVertexAO(bottom, front, bottomfront));
		block.Set_rBb_AO(CacluateVertexAO(bottom, back, bottomback));
	}
	//Left
	else if (dir.x < 0)
	{
		bool topfront = !GetBlock(x - 1, y + 1, z + 1).IsSmooth();
		bool topback = !GetBlock(x - 1, y + 1, z - 1).IsSmooth();
		bool bottomfront = !GetBlock(x - 1, y - 1, z + 1).IsSmooth();
		bool bottomback = !GetBlock(x - 1, y - 1, z - 1).IsSmooth();

		bool top = !GetBlock(x - 1, y + 1, z).IsSmooth();
		bool bottom = !GetBlock(x - 1, y - 1, z).IsSmooth();
		bool front = !GetBlock(x - 1, y, z + 1).IsSmooth();
		bool back = !GetBlock(x - 1, y, z - 1).IsSmooth();

		auto& block = GetBlock(x, y, z);

		block.Set_ltf_AO(CacluateVertexAO(top, front, topfront));
		block.Set_ltb_AO(CacluateVertexAO(top, back, topback));
		block.Set_lBf_AO(CacluateVertexAO(bottom, front, bottomfront));
		block.Set_lBb_AO(CacluateVertexAO(bottom, back, bottomback));
	}
	//Top
	else if (dir.y > 0)
	{
		bool rightfront = !GetBlock(x + 1, y + 1, z + 1).IsSmooth();
		bool rightback = !GetBlock(x + 1, y + 1, z - 1).IsSmooth();
		bool leftfront = !GetBlock(x - 1, y + 1, z + 1).IsSmooth();
		bool leftback = !GetBlock(x - 1, y + 1, z - 1).IsSmooth();

		bool right = !GetBlock(x + 1, y + 1, z).IsSmooth();
		bool left = !GetBlock(x - 1, y + 1, z).IsSmooth();
		bool front = !GetBlock(x, y + 1, z + 1).IsSmooth();
		bool back = !GetBlock(x, y + 1, z - 1).IsSmooth();

		auto& block = GetBlock(x, y, z);

		block.Set_trf_AO(CacluateVertexAO(right, front, rightfront));
		block.Set_trb_AO(CacluateVertexAO(right, back, rightback));
		block.Set_tlf_AO(CacluateVertexAO(left, front, leftfront));
		block.Set_tlb_AO(CacluateVertexAO(left, back, leftback));
	}
	//Bottom
	else if (dir.y < 0)
	{
		bool rightfront = !GetBlock(x + 1, y - 1, z + 1).IsSmooth();
		bool rightback = !GetBlock(x + 1, y - 1, z - 1).IsSmooth();
		bool leftfront = !GetBlock(x - 1, y - 1, z + 1).IsSmooth();
		bool leftback = !GetBlock(x - 1, y - 1, z - 1).IsSmooth();

		bool right = !GetBlock(x + 1, y - 1, z).IsSmooth();
		bool left = !GetBlock(x - 1, y - 1, z).IsSmooth();
		bool front = !GetBlock(x, y - 1, z + 1).IsSmooth();
		bool back = !GetBlock(x, y - 1, z - 1).IsSmooth();

		auto& block = GetBlock(x, y, z);

		block.Set_Brf_AO(CacluateVertexAO(right, front, rightfront));
		block.Set_Brb_AO(CacluateVertexAO(right, back, rightback));
		block.Set_Blf_AO(CacluateVertexAO(left, front, leftfront));
		block.Set_Blb_AO(CacluateVertexAO(left, back, leftback));
	}
	//Front
	else if (dir.z > 0)
	{
		bool righttop = !GetBlock(x + 1, y + 1, z + 1).IsSmooth();
		bool rightbottom = !GetBlock(x + 1, y - 1, z + 1).IsSmooth();
		bool lefttop = !GetBlock(x - 1, y + 1, z + 1).IsSmooth();
		bool leftbottom = !GetBlock(x - 1, y - 1, z + 1).IsSmooth();

		bool right = !GetBlock(x + 1, y, z + 1).IsSmooth();
		bool left = !GetBlock(x - 1, y, z + 1).IsSmooth();
		bool top = !GetBlock(x, y + 1, z + 1).IsSmooth();
		bool bottom = !GetBlock(x, y - 1, z + 1).IsSmooth();

		auto& block = GetBlock(x, y, z);

		block.Set_frt_AO(CacluateVertexAO(right, top, righttop));
		block.Set_frB_AO(CacluateVertexAO(right, bottom, rightbottom));
		block.Set_flt_AO(CacluateVertexAO(left, top, lefttop));
		block.Set_flB_AO(CacluateVertexAO(left, bottom, leftbottom));
	}
	//Back
	else if (dir.z < 0)
	{
		bool righttop = !GetBlock(x + 1, y + 1, z - 1).IsSmooth();
		bool rightbottom = !GetBlock(x + 1, y - 1, z - 1).IsSmooth();
		bool lefttop = !GetBlock(x - 1, y + 1, z - 1).IsSmooth();
		bool leftbottom = !GetBlock(x - 1, y - 1, z - 1).IsSmooth();

		bool right = !GetBlock(x + 1, y, z - 1).IsSmooth();
		bool left = !GetBlock(x - 1, y, z - 1).IsSmooth();
		bool top = !GetBlock(x, y + 1, z - 1).IsSmooth();
		bool bottom = !GetBlock(x, y - 1, z - 1).IsSmooth();

		auto& block = GetBlock(x, y, z);

		block.Set_brt_AO(CacluateVertexAO(right, top, righttop));
		block.Set_brB_AO(CacluateVertexAO(right, bottom, rightbottom));
		block.Set_blt_AO(CacluateVertexAO(left, top, lefttop));
		block.Set_blB_AO(CacluateVertexAO(left, bottom, leftbottom));
	}
}

uint8_t Chunk::CacluateVertexAO(bool side1, bool side2, bool corner)
{
	if (side1 && side2)
	{
		return 0;
	}
	return 3 - (side1 + side2 + corner);
}

bool Chunk::IsEmptyChunk(std::weak_ptr<Chunk> const& chunk)
{
	if (chunk.expired())
	{
		return true;
	}
	return !chunk.owner_before(std::weak_ptr<Chunk>()) && !std::weak_ptr<Chunk>().owner_before(chunk);
}

bool Chunk::AllDirectionChunkIsReady()
{
	return !IsEmptyChunk(LeftChunk) && !IsEmptyChunk(RightChunk) && !IsEmptyChunk(FrontChunk) && !IsEmptyChunk(BackChunk);
}
