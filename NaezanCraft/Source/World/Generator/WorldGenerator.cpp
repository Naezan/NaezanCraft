#include "../../pch.h"
#include "WorldGenerator.h"
#include "../Chunk.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

WorldGenerator::WorldGenerator()
{
	//setting noise
}

WorldGenerator::~WorldGenerator() = default;

void WorldGenerator::GenerateTerrain(std::weak_ptr<Chunk> chunk)
{
	const glm::vec3& chunkPos = chunk.lock()->position;

	SetHeightMap(chunk);

	//전체 블럭 최대 높이 찾기
	int maxHeight = 0;
	for (int i = 0; i < CHUNK_X; ++i)
	{
		maxHeight = std::max(maxHeight, *std::max_element(heightMap[i].begin(), heightMap[i].end()));
	}
	SetChunkBlocks(maxHeight, chunk);
}

void WorldGenerator::SetHeightMap(std::weak_ptr<Chunk> chunk)
{
	for (int x = 0; x < CHUNK_X; ++x)
	{
		for (int z = 0; z < CHUNK_Z; ++z)
		{
			int h = GetBlockHeight(x, z);
			heightMap[x][z] = h;
		}
	}
}

int WorldGenerator::GetBlockHeight(int x, int z)
{
	//TO DO
	//GetBiome->GetNoiseArea();
	return 0.0f;
}

void WorldGenerator::SetChunkBlocks(int maxHeight, std::weak_ptr<Chunk> chunk)
{
	for (int y = 0; y < maxHeight; ++y)
	{
		for (int x = 0; x < CHUNK_X; ++x)
		{
			for (int z = 0; z < CHUNK_Z; ++z)
			{
				//SetBlockType
				int h = heightMap[x][z];
				//현재 블록 위치위의 빈자리
				if (y > h)
				{
					if (y <= WATER_HEIGHT)
					{
						//Water
					}
				}
				//경계면
				else if (y == h)
				{
					if (y >= WATER_HEIGHT)
					{
						//Defualt set block by biome, stone(mountain) or dirt(forest) or sand(desert)

						//TODO Flower, Tree, Catus, Grass
					}
					else
					{
						//RandomBaseMakeUnderWaterCluster(Mountain, Forest, Desert, Ocean) -> Circle?
						//Defualt set block by biome, stone(mountain) or dirt(forest) or sand(desert)
					}
				}
				//블록최대 높이 아래3칸
				else if (y >= h - 3)
				{
					//set block by biome, stone(mountain) or dirt(forest) or sand(desert)
				}
				//맨바닥
				else if (y == 0)
				{
					//bedrock
				}
				//맨바닥3칸위
				else if (y <= 3)
				{
					//Stone or bedrock
					if (y == 1)
					{
						//bedrock70? rock30?
					}
					else if (y == 2)
					{
						//bedrock30? stone70?
					}
				}
				//나머지
				else
				{
					//stone
				}
			}
		}
	}
}