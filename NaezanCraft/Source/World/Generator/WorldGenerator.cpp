#include "../../pch.h"
#include "WorldGenerator.h"
#include "../Chunk.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

WorldGenerator::WorldGenerator() : gen(rd())
{
	fastNoise.SetFrequency(.02f);
	fastNoise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2S);
	fastNoise.SetFractalType(FastNoiseLite::FractalType_FBm);
	fastNoise.SetFractalOctaves(4);
}

WorldGenerator::~WorldGenerator() = default;

void WorldGenerator::GenerateTerrain(std::weak_ptr<Chunk> chunk)
{
	const glm::vec3& chunkPos = chunk.lock()->position;

	SetHeightMap(chunk);
	//전체 블럭 최대 높이 찾기
	int maxHeight = WATER_HEIGHT;
	for (int i = 0; i < CHUNK_X; ++i)
	{
		maxHeight = std::max(maxHeight, *std::max_element(heightMap[i].begin(), heightMap[i].end()));
	}
	SetChunkBlocks(maxHeight, chunk);
}

void WorldGenerator::SetHeightMap(std::weak_ptr<Chunk> chunk)
{
	//TODO위치를 좀 더 확실하게 다른걸로
	auto pos = chunk.lock()->position;
	pos.x *= CHUNK_X;
	pos.z *= CHUNK_Z;

	for (int x = 0; x < CHUNK_X; ++x)
	{
		for (int z = 0; z < CHUNK_Z; ++z)
		{
			//GetBiome->GetNoiseArea();
			int h = GetBlockHeight(pos.x + static_cast<float>(x), pos.z + static_cast<float>(z));
			if (h > 128)
				h = 128;
			heightMap[x][z] = h;
		}
	}
}

int WorldGenerator::GetBlockHeight(float x, float z)
{
	float noise = (fastNoise.GetNoise(x, z) + 1.f) / 2.f;
	return static_cast<int>(noise * 35.f + 43.f);
}

void WorldGenerator::SetChunkBlocks(int maxHeight, std::weak_ptr<Chunk> chunk)
{
	for (int y = 0; y <= maxHeight; ++y)
	{
		for (int x = 0; x < CHUNK_X; ++x)
		{
			for (int z = 0; z < CHUNK_Z; ++z)
			{
				//SetBlockType
				int h = heightMap[x][z];
				//현재 블록 위치위의 빈자리
				if (chunk.lock()->GetBlock(x, y, z) == Block())
				{
					continue;
				}
				if (y > h)
				{
					if (y <= WATER_HEIGHT)
					{
						//Water
						chunk.lock()->SetBlock(x, y, z, BlockType::Water);
					}
				}
				//경계면
				else if (y == h)
				{
					if (y >= WATER_HEIGHT)
					{
						//Defualt set block by biome, stone(mountain) or dirt(forest) or sand(desert)
						//TODO GetBiome(x, z).GetSurfaceBlockType();
						chunk.lock()->SetBlock(x, y, z, BlockType::Dirt);

						//TODO Flower, Tree, Catus, Grass
					}
					else
					{
						//RandomBaseMakeUnderWaterCluster(Mountain, Forest, Desert, Ocean) -> Circle?
						//Defualt set block by biome, stone(mountain) or dirt(forest) or sand(desert)
						//TODO GetBiome(x, z).GetInWaterBlockType();
						chunk.lock()->SetBlock(x, y, z, BlockType::Sand);
					}
				}
				//블록최대 높이 아래3칸
				else if (y >= h - 3)
				{
					//set block by biome, stone(mountain) or dirt(forest) or sand(desert)
					//TODO GetBiome(x, z).GetUnderGroundBlockType();
					chunk.lock()->SetBlock(x, y, z, BlockType::Stone);
				}
				//맨바닥
				else if (y == 0)
				{
					chunk.lock()->SetBlock(x, y, z, BlockType::Bedrock);
				}
				//맨바닥3칸위
				else if (y <= 3)
				{
					//Stone or bedrock
					if (y == 1)
					{
						std::uniform_int_distribution<int> dis(0, 3);
						if (dis(gen) < 3)
							chunk.lock()->SetBlock(x, y, z, BlockType::Bedrock);
						else
							chunk.lock()->SetBlock(x, y, z, BlockType::Stone);
					}
					else if (y == 2)
					{
						std::uniform_int_distribution<int> dis(0, 2);
						if (dis(gen) < 2)
							chunk.lock()->SetBlock(x, y, z, BlockType::Bedrock);
						else
							chunk.lock()->SetBlock(x, y, z, BlockType::Stone);
					}
					else
					{
						std::uniform_int_distribution<int> dis(0, 1);
						if (dis(gen) < 1)
							chunk.lock()->SetBlock(x, y, z, BlockType::Bedrock);
						else
							chunk.lock()->SetBlock(x, y, z, BlockType::Stone);
					}
				}
				//나머지
				else
				{
					//stone
					chunk.lock()->SetBlock(x, y, z, BlockType::Stone);
				}
			}
		}
	}
}
