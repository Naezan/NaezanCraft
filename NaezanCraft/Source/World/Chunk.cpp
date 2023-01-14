#include "../pch.h"
#include "Chunk.h"
#include "ChunkMesh.h"
#include "Mesh.h"
#include "Water.h"
#include "../Application.h"
#include "../Renderer/Renderer.h"
#include "../World/Generator/WorldGenerator.h"

#include "../Util/FileMemory.h"

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
	LightMap(CHUNK_X, std::vector<std::vector<unsigned char>>(CHUNK_Y, std::vector<unsigned char>(CHUNK_Z, 0x00))),
	emptyBlock(BlockType::Air),
	chunkBlocks(CHUNK_X, std::vector<std::vector<Block>>(CHUNK_Y, std::vector<Block>(CHUNK_Z, BlockType::Air))),
	serialStatus(ChunkSerialStatus::Loaded)
{
	//TO DO change blocktype
	//memset(&chunkBlocks[0][0][0], BlockType::Diamond, CHUNK_X * CHUNK_Y * CHUNK_Z * sizeof(Block));
	//std::fill(&chunkBlocks[0][0][0], &chunkBlocks[0][0][0] + CHUNK_SIZE, BlockType::Air);
	water = std::make_unique<Water>();
}

Chunk::~Chunk()
{
	LeftChunk.reset();
	RightChunk.reset();
	FrontChunk.reset();
	BackChunk.reset();
	water.reset();
}

void Chunk::SetBlock(const glm::vec3& blockPos, BlockType type)
{
	if (blockPos.x < 0)
	{
		if (!IsEmptyChunk(LeftChunk))
			LeftChunk.lock()->SetBlock(CHUNK_X + blockPos.x, blockPos.y, blockPos.z, type);
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

std::unique_ptr<Mesh>& Chunk::GetWaterMesh()
{
	return water->GetMesh();
}

void Chunk::SetSunLight(int x, int y, int z, int level)
{
	if (y >= CHUNK_Y || y < 0)
	{
		return;
	}

	if (x < 0)
	{
		if (!IsEmptyChunk(LeftChunk))
		{
			LeftChunk.lock()->SetSunLight(CHUNK_X + x, y, z, level);
		}
		return;
	}
	else if (x >= CHUNK_X)
	{
		if (!IsEmptyChunk(RightChunk))
			RightChunk.lock()->SetSunLight(x - CHUNK_X, y, z, level);
		return;
	}
	if (z < 0)
	{
		if (!IsEmptyChunk(BackChunk))
			BackChunk.lock()->SetSunLight(x, y, CHUNK_Z + z, level);
		return;
	}
	else if (z >= CHUNK_Z)
	{
		if (!IsEmptyChunk(FrontChunk))
			FrontChunk.lock()->SetSunLight(x, y, z - CHUNK_Z, level);
		return;
	}

	LightMap[x][y][z] = (LightMap[x][y][z] & 0x0F) | (level << 4);
}

unsigned char Chunk::GetSunLight(int x, int y, int z)
{
	if (y >= CHUNK_Y || y < 0)
	{
		return 0xF;
	}

	if (x < 0)
	{
		if (!IsEmptyChunk(LeftChunk))
			return LeftChunk.lock()->GetSunLight(CHUNK_X + x, y, z);
		else return 0xF;
	}
	else if (x >= CHUNK_X)
	{
		if (!IsEmptyChunk(RightChunk))
			return RightChunk.lock()->GetSunLight(x - CHUNK_X, y, z);
		else return 0xF;
	}
	if (z < 0)
	{
		if (!IsEmptyChunk(BackChunk))
			return BackChunk.lock()->GetSunLight(x, y, CHUNK_Z + z);
		else return 0xF;
	}
	else if (z >= CHUNK_Z)
	{
		if (!IsEmptyChunk(FrontChunk))
			return FrontChunk.lock()->GetSunLight(x, y, z - CHUNK_Z);
		else return 0xF;
	}

	return (LightMap[x][y][z] >> 4) & 0x0F;
}

void Chunk::SetTorchLight(int x, int y, int z, int level)
{
	if (y >= CHUNK_Y || y < 0)
	{
		return;
	}

	if (x < 0)
	{
		if (!IsEmptyChunk(LeftChunk))
			LeftChunk.lock()->SetTorchLight(CHUNK_X + x, y, z, level);
		return;
	}
	else if (x >= CHUNK_X)
	{
		if (!IsEmptyChunk(RightChunk))
			RightChunk.lock()->SetTorchLight(x - CHUNK_X, y, z, level);
		return;
	}
	if (z < 0)
	{
		if (!IsEmptyChunk(BackChunk))
			BackChunk.lock()->SetTorchLight(x, y, CHUNK_Z + z, level);
		return;
	}
	else if (z >= CHUNK_Z)
	{
		if (!IsEmptyChunk(FrontChunk))
			FrontChunk.lock()->SetTorchLight(x, y, z - CHUNK_Z, level);
		return;
	}

	LightMap[x][y][z] = (LightMap[x][y][z] & 0xF0) | level;
}

unsigned char Chunk::GetTorchLight(int x, int y, int z)
{
	if (y >= CHUNK_Y || y < 0)
	{
		return 0xF;
	}

	if (x < 0)
	{
		if (!IsEmptyChunk(LeftChunk))
			return LeftChunk.lock()->GetTorchLight(CHUNK_X + x, y, z);
		else return 0xF;
	}
	else if (x >= CHUNK_X)
	{
		if (!IsEmptyChunk(RightChunk))
			return RightChunk.lock()->GetTorchLight(x - CHUNK_X, y, z);
		else return 0xF;
	}
	if (z < 0)
	{
		if (!IsEmptyChunk(BackChunk))
			return BackChunk.lock()->GetTorchLight(x, y, CHUNK_Z + z);
		else return 0xF;
	}
	else if (z >= CHUNK_Z)
	{
		if (!IsEmptyChunk(FrontChunk))
			return FrontChunk.lock()->GetTorchLight(x, y, z - CHUNK_Z);
		else return 0xF;
	}

	return LightMap[x][y][z] & 0x0F;
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

void Chunk::SaveChunk(const std::string& path)
{
	OPTICK_EVENT();
	std::string filepath = GetChunkDataPath(position.x, position.z, path);
	FILE* fp = fopen(filepath.c_str(), "wb");
	if (!fp)
	{
		NC_LOG_ERROR("Failed to open chunk file {0}, {1}", position.x, position.z);
		return;
	}
	FileMemory data;
	SerializeData(data);
	fwrite(data.rawdata, data.datasize, 1, fp);
	data.reset(false);
	fclose(fp);

	serialStatus = ChunkSerialStatus::Saved;
}

void Chunk::LoadChunk(const std::string& path)
{
	OPTICK_EVENT();
	std::string filepath = GetChunkDataPath(position.x, position.z, path);
	FILE* fp = fopen(filepath.c_str(), "rb");
	if (!fp)
	{
		NC_LOG_ERROR("Failed to open chunk file {0}, {1}", position.x, position.z);
		return;
	}

	//DataSize is not Ready
	//마지막부분4바이트만 읽어옴
	int size;
	fseek(fp, 0, SEEK_END);
	size = ftell(fp);
	rewind(fp);

	FileMemory data;
	data.allocatedata(size);
	fread(data.rawdata, size, 1, fp);
	DeserializeData(data);
	data.reset(true);

	// Close file
	fclose(fp);
}

void Chunk::SerializeData(FileMemory& outFileData)
{
	auto curBlocktype = chunkBlocks[0][0][0].blockType;
	uint16_t curBlockCount = 0;
	for (int8_t y = 0; y < CHUNK_Y; ++y)
	{
		for (int8_t z = 0; z < CHUNK_Z; ++z)
		{
			for (int8_t x = 0; x < CHUNK_X; ++x)
			{
				if (curBlocktype != chunkBlocks[x][y][z].blockType)
				{
					outFileData.write<uint8_t>((uint8_t)curBlocktype);
					outFileData.write<uint16_t>(curBlockCount);

					curBlocktype = chunkBlocks[x][y][z].blockType;
					curBlockCount = 0;
				}
				++curBlockCount;
			}
		}
	}
	if (curBlockCount > 0)
	{
		outFileData.write<uint8_t>((uint8_t)curBlocktype);
		outFileData.write<uint16_t>(curBlockCount);
	}
}

void Chunk::DeserializeData(FileMemory& fileData)
{
	uint8_t* curBlocktype = new uint8_t;
	uint16_t* curBlockCount = new uint16_t;
	int x = 0, y = 0, z = 0;

	while (fileData.readoffset < fileData.datasize)
	{
		fileData.read<uint8_t>(curBlocktype);
		fileData.read<uint16_t>(curBlockCount);

		for (int i = 0; i < *curBlockCount; ++i)
		{
			chunkBlocks[x][y][z].blockType = (BlockType)*curBlocktype;
			++x;
			if (x == CHUNK_X)
			{
				x = 0;
				++z;
			}
			if (z == CHUNK_Z)
			{
				z = 0;
				++y;
			}
			if (y == CHUNK_Y)
			{
				y = 0;
				delete curBlocktype;
				delete curBlockCount;
				curBlocktype = nullptr;
				curBlockCount = nullptr;
				return;
			}
		}
		++*curBlockCount;
	}
}

std::string Chunk::GetChunkDataPath(int x, int z, const std::string& path)
{
	return path + "/" + std::to_string(x) + "_" + std::to_string(z) + ".bin";
}

void Chunk::RebuildChunkMesh()
{
	//recreate vertexbuffer and indexbuffer
	chunkMesh->RebuildBuffer(rebuildVertices);
}

void Chunk::CreateLightMap()
{
	OPTICK_EVENT();

	unsigned char lightMask[CHUNK_X + 2][CHUNK_Z + 2];

	memset(lightMask, 0xF, (CHUNK_X + 2) * (CHUNK_Z + 2));

	//맨위의 벽들을 모두 15로 만든다
	for (int iy = CHUNK_Y - 1; iy >= 0; --iy)
	{
		for (int ix = -1; ix <= CHUNK_X; ++ix)
		{
			for (int iz = -1; iz <= CHUNK_Z; ++iz)
			{
				if (GetBlock(ix, iy, iz).IsNotShadow())
				{
					//Y축 위 상태가 모두 IsNotShadow라면 15 아니면 0
					//if (GetSunLight(x, y, z) == 0)
					SetSunLight(ix, iy, iz, lightMask[ix + 1][iz + 1]);

					if (lightMask[ix + 1][iz + 1] == 0)
					{
						if ((ix >= 0 && lightMask[ix][iz + 1] == 0xF) ||
							(ix <= CHUNK_X - 1 && lightMask[ix + 2][iz + 1] == 0xF) ||
							(iz >= 0 && lightMask[ix + 1][iz] == 0xF) ||
							(iz <= CHUNK_Z - 1 && lightMask[ix + 1][iz + 2] == 0xF))
						{
							SetSunLight(ix, iy, iz, 0xF - 1);

							//-1이면0 16이면15
							CoordNode node;
							node.SetXN(ix < 0 ? 1 : (ix > CHUNK_X - 1 ? 2 : 0));
							node.SetZN(iz < 0 ? 1 : (iz > CHUNK_Z - 1 ? 2 : 0));

							int tempX = ix < 0 ? 0 : (ix > CHUNK_X - 1 ? CHUNK_X - 1 : ix);
							int tempZ = iz < 0 ? 0 : (iz > CHUNK_Z - 1 ? CHUNK_Z - 1 : iz);
							node.SetXYZ(tempX, iy, tempZ);

							sunlightBfsQueue.emplace(node);
						}
					}
				}
				else
				{
					//Solid한 블럭
					lightMask[ix + 1][iz + 1] = 0;
				}
			}
		}
	}

	while (!sunlightBfsQueue.empty())
	{
		CoordNode& node = sunlightBfsQueue.front();

		int x = node.GetX();
		if (node.GetXN() == 1)//x < 0 이였다
			x = -1;
		else if (node.GetXN() == 2)// x > CHUNK_X - 1 이였다
			x = CHUNK_X;
		int y = node.GetY();
		int z = node.GetZ();
		if (node.GetZN() == 1)
			z = -1;
		else if (node.GetZN() == 2)
			z = CHUNK_Z;
		int sunLight = GetSunLight(x, y, z);

		sunlightBfsQueue.pop();

		if (sunLight <= 0)
			continue;

		for (int i = 0; i < 6; ++i)
		{
			int dx = x + nearFaces[i].x;
			int dy = y + nearFaces[i].y;
			int dz = z + nearFaces[i].z;
			if (dy < 0 || dy >= CHUNK_Y) continue;

			Block& block = GetBlock(dx, dy, dz);
			if (block.IsNotShadow() && GetSunLight(dx, dy, dz) < sunLight - 1)
			{
				SetSunLight(dx, dy, dz, sunLight - 1);

				if (dx >= -1 && dx <= CHUNK_X && dz >= -1 && dz <= CHUNK_Z)
				{
					CoordNode node;
					node.SetXN(dx < 0 ? 1 : (dx > CHUNK_X - 1 ? 2 : 0));
					node.SetZN(dz < 0 ? 1 : (dz > CHUNK_Z - 1 ? 2 : 0));
					int tempX = dx < 0 ? 0 : (dx > CHUNK_X - 1 ? CHUNK_X - 1 : dx);
					int tempZ = dz < 0 ? 0 : (dz > CHUNK_Z - 1 ? CHUNK_Z - 1 : dz);
					node.SetXYZ(tempX, dy, tempZ);
					sunlightBfsQueue.emplace(node);
				}
			}
		}
	}
}

void Chunk::ReloadLightMap()
{
	/*while (!sunlightReloadBfsQueue.empty())
	{
		CoordNode& node = sunlightReloadBfsQueue.front();

		int x = node.GetX();
		int y = node.GetY();
		int z = node.GetZ();
		int sunLight = GetSunLight(x, y, z);

		sunlightReloadBfsQueue.pop();

		for (int i = 0; i < 6; ++i)
		{
			int dx = x + nearFaces[i].x;
			int dy = y + nearFaces[i].y;
			int dz = z + nearFaces[i].z;
			if (dy < 0 || dy >= CHUNK_Y) continue;

			Block& block = GetWorldBlock(dx, dy, dz);
			if (block.IsNotShadow() && GetSunLight(dx, dy, dz) < sunLight - 1)
			{
				SetSunLight(dx, dy, dz, sunLight - 1);

				if (dx >= -1 && dx <= CHUNK_X && dz >= -1 && dz <= CHUNK_Z)
				{
					CoordNode node;
					node.SetXYZ(dx, dy, dz);
					sunlightReloadBfsQueue.emplace(node);
				}
			}
		}
	}*/
}

int Chunk::GetBlockMaxGroundHeight(int x, int z)
{
	for (int y = CHUNK_Y - 1; y >= 0; --y) {
		if (GetBlock(x, y, z).IsGround()) return y;
	}
	return 0;
}

int Chunk::GetBlockMaxSolidHeight(int x, int z)
{
	for (int y = CHUNK_Y - 1; y >= 0; --y) {
		if (!GetBlock(x, y, z).IsTransparent()) return y;
	}
	return 0;
}

void Chunk::CreateSSAO()
{
	OPTICK_EVENT();

	for (int8_t x = 0; x < CHUNK_X; ++x)
	{
		for (int8_t z = 0; z < CHUNK_Z; ++z)
		{
			int h = GetBlockMaxGroundHeight(x, z);
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
