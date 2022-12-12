#pragma once

class Chunk;

using heightMap_2d = std::array<std::array<int, CHUNK_X>, CHUNK_Z>;

class WorldGenerator
{
public:
	WorldGenerator();
	~WorldGenerator();

	void GenerateTerrain(std::weak_ptr<Chunk> chunk);
	void SetHeightMap(std::weak_ptr<Chunk> chunk);
	int GetBlockHeight(int x, int z);
	void SetChunkBlocks(int height, std::weak_ptr<Chunk> chunk);

private:
	heightMap_2d heightMap;
};