#pragma once

#include "../../FastNoiseLite.h"

class Chunk;

using heightMap_2d = std::array<std::array<int, CHUNK_X>, CHUNK_Z>;

class WorldGenerator
{
public:
	WorldGenerator();
	~WorldGenerator();

	void GenerateTerrain(std::weak_ptr<Chunk> chunk);
	void SetHeightMap(std::weak_ptr<Chunk> chunk);
	int GetBlockHeight(float x, float z);
	void SetChunkBlocks(int height, std::weak_ptr<Chunk> chunk);
	void GenerateTree(std::weak_ptr<Chunk> chunk, int x, int y, int z);

private:
	heightMap_2d heightMap;
	FastNoiseLite fastNoise;

	std::random_device rd;
	std::mt19937 gen;
};