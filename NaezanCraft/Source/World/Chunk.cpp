#include "../pch.h"
#include "Chunk.h"
#include "ChunkMesh.h"
#include "../Application.h"
#include "../Renderer/Renderer.h"
#include "../World/Generator/WorldGenerator.h"

const std::array<glm::vec3, 6> Chunk::nearFaces
{
	glm::vec3(-1, 0, 0),
	glm::vec3(1 , 0, 0),
	glm::vec3(0 , 0,-1),
	glm::vec3(0 , 0, 1),
	glm::vec3(0 ,-1, 0),
	glm::vec3(0 , 1, 0)
};

Chunk::Chunk(const glm::vec3& pos) :
	position(pos), chunkLoadState(ChunkLoadState::UnGenerated),
	chunkBox(glm::vec3(pos.x* CHUNK_X, pos.y* CHUNK_Y, pos.z* CHUNK_Z), CHUNK_X, CHUNK_Y, CHUNK_Z),
	LightMap(CHUNK_X, std::vector<std::vector<unsigned char>>(CHUNK_Y, std::vector<unsigned char>(CHUNK_Z))),
	emptyBlock(BlockType::Air)
{
	//TO DO change blocktype
	//memset(&chunkBlocks[0][0][0], BlockType::Diamond, CHUNK_X * CHUNK_Y * CHUNK_Z * sizeof(Block));
	std::fill(&chunkBlocks[0][0][0], &chunkBlocks[0][0][0] + CHUNK_SIZE, BlockType::Air);
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
	if (GET_World()->GetChunkByPos(std::pair<int, int>(static_cast<int>(position.x + 1), static_cast<int>(position.z)), RightChunk))
	{
		RightChunk.lock()->LeftChunk = shared_from_this();
	}
	if (GET_World()->GetChunkByPos(std::pair<int, int>(static_cast<int>(position.x), static_cast<int>(position.z - 1)), BackChunk))
	{
		BackChunk.lock()->FrontChunk = shared_from_this();
	}
	if (GET_World()->GetChunkByPos(std::pair<int, int>(static_cast<int>(position.x), static_cast<int>(position.z + 1)), FrontChunk))
	{
		FrontChunk.lock()->BackChunk = shared_from_this();
	}
}

void Chunk::CreateChunkMesh()
{
	OPTICK_EVENT();
	if (chunkLoadState == ChunkLoadState::UnGenerated)
		chunkLoadState = ChunkLoadState::Generated;

	chunkMesh = std::make_unique<ChunkMesh>(shared_from_this());
	chunkMesh->CreateMesh();
}

void Chunk::GenerateTerrain(std::unique_ptr<WorldGenerator>& worldGenerator)
{
	worldGenerator->GenerateTerrain(shared_from_this());
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
			if (block.IsTransparent())
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
	for (int y = CHUNK_Y - 1; y >= 0; --y) {
		if (!GetBlock(x, y, z).IsTransparent()) return y;
	}
	return 0;
}

bool Chunk::IsEmptyChunk(std::weak_ptr<Chunk> const& chunk)
{
	return !chunk.owner_before(std::weak_ptr<Chunk>()) && !std::weak_ptr<Chunk>().owner_before(chunk);
}
