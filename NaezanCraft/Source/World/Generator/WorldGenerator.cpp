#include "../../pch.h"
#include "WorldGenerator.h"
#include "../Chunk.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <FastNoise/FastNoise.h>

WorldGenerator::WorldGenerator()
{
	//setting noise
}

WorldGenerator::~WorldGenerator() = default;

void WorldGenerator::GenerateTerrain(std::weak_ptr<Chunk> chunk)
{
	const glm::vec3& chunkPos = chunk.lock()->position;

	SetHeightMap(chunk);

	//��ü ���� �ִ� ���� ã��
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
			//GetBiome->GetNoiseArea();
			int h = GetBlockHeight(x, z);
			heightMap[x][z] = h;
		}
	}
}

int WorldGenerator::GetBlockHeight(int x, int z)
{
	auto perlin = FastNoise::New<FastNoise::Perlin>();
	auto fractal = FastNoise::New<FastNoise::FractalFBm>();

	fractal->SetSource(perlin);
	fractal->SetOctaveCount(8);

	std::vector<float> noise(16 * 16);

	fractal->GenUniformGrid2D(noise.data(), 0, 0, 16, 16, 0.02f, 1337);

	int index = 0;

	for (int y = 0; y < 16; y++)
	{
		for (int x = 0; x < 16; x++)
		{
			std::cout << "x " << x << "\ty " << y << "\t: " << noise[index++] << std::endl;
		}
	}
	return 0;
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
				//���� ���� ��ġ���� ���ڸ�
				if (y > h)
				{
					if (y <= WATER_HEIGHT)
					{
						//Water
					}
				}
				//����
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
				//�����ִ� ���� �Ʒ�3ĭ
				else if (y >= h - 3)
				{
					//set block by biome, stone(mountain) or dirt(forest) or sand(desert)
				}
				//�ǹٴ�
				else if (y == 0)
				{
					//bedrock
				}
				//�ǹٴ�3ĭ��
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
				//������
				else
				{
					//stone
				}
			}
		}
	}
}