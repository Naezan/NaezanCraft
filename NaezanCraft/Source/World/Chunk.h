#pragma once

#include "Block.h"
#include "../Renderer/FrustomCulling.h"

using array_3d = std::array<std::array<std::array<Block, CHUNK_Z>, CHUNK_Y>, CHUNK_X>;
using vector_3d = std::vector<std::vector<std::vector<unsigned char>>>;

class Chunk;
class Mesh;
class Water;
class ChunkMesh;
class WorldGenerator;
struct VertTexCoord;

enum class ChunkLoadState
{
	Unloaded, TerrainGenerated, MeshLoaded, Builted
};

struct LightNode {

	LightNode(short _index = 0, signed char _in = 0) : index(_index), indexnegative(_in) {}

	//최상위 1바이트는 y 다음 4비트는 x, 마지막 4비트는 z
	short index; //this is the x y z coordinate!
	signed char indexnegative;//0x00x-xy-yz-z
	
	inline int GetX() { return (index >> 4) & 0xF; }
	inline int GetY() { return (index >> 8); }
	inline int GetZ() { return index & 0xF; }
	inline int GetXN() { return (indexnegative >> 4) & 0b00000011; }
	inline int GetYN() { return (indexnegative >> 2) & 0b00000011; }
	inline int GetZN() { return indexnegative & 0b00000011; }

	inline void SetX(int val) { index = (index & 0xFF0F) | (val << 4); }
	inline void SetY(int val) { index = (index & 0xFF) | (val << 8); }
	inline void SetZ(int val) { index = (index & 0xFFF0) | val; }
	inline void SetXN(int val) { indexnegative = (indexnegative & 0b00001111) | (val << 4); }
	inline void SetYN(int val) { indexnegative = (indexnegative & 0b00110011) | (val << 2); }
	inline void SetZN(int val) { indexnegative = (indexnegative & 0b00111100) | (val); }

	inline void SetXYZ(int x, int y, int z) { SetX(x); SetY(y); SetZ(z); }
};

class Chunk : public std::enable_shared_from_this<Chunk>
{
public:
	//You Don't new Chunk
	Chunk(const glm::vec3& pos);
	~Chunk();

	void SetBlock(const glm::vec3& blockPos, BlockType type);
	void SetBlock(int x, int y, int z, BlockType type);
	Block& GetBlock(const glm::vec3& blockPos);
	Block& GetBlock(int x, int y, int z);
	Block& GetWorldBlock(const glm::vec3& blockPos);
	Block& GetWorldBlock(int wx, int wy, int wz);
	void SetLoadState(const ChunkLoadState& state);

	std::unique_ptr<Mesh>& GetWaterMesh();

	void SetSunLight(int x, int y, int z, int level);
	unsigned char GetSunLight(int x, int y, int z);
	void SetTorchLight(int x, int y, int z, int level);
	unsigned char GetTorchLight(int x, int y, int z);

	void SetupChunkNeighbor();
	void CreateChunkMesh(bool isReload);
	void CreateMeshBuffer();
	void GenerateTerrain(std::unique_ptr<WorldGenerator>& worldGenerator);

	//rebuild
	void RebuildChunkMesh();

	inline bool IsRebuild() { return !rebuildVertices.empty(); }

	//Lighting
	void CreateLightMap();
	void ReloadLightMap();//TODO change Reload Code
	int GetBlockMaxGroundHeight(int x, int z);
	int GetBlockMaxSolidHeight(int x, int z);

	//AO
	void CreateSSAO();
	void ReloadSSAO(int x, int y, int z);
	void ReloadSSAO(const glm::vec3& loadPos);
	void CaculateAO(int x, int y, int z, const glm::ivec3& dir);
	uint8_t CacluateVertexAO(bool side1, bool side2, bool corner);

	static bool IsEmptyChunk(std::weak_ptr<Chunk> const& chunk);
	bool AllDirectionChunkIsReady();

public:
	//need to use heap memory? but pointer is heavy? 65,536 * 1 byte?
	//one chunk has 65.536kbyte? chunk is heavy
	//100 chunks has 6.5536mbyte.
	std::vector<std::vector<std::vector<Block>>> chunkBlocks;
	glm::vec3 position;
	ChunkLoadState chunkLoadState;
	std::unique_ptr<ChunkMesh> chunkMesh;
	std::vector<VertTexCoord> rebuildVertices;

	AABox chunkBox;

	std::weak_ptr<Chunk> LeftChunk;
	std::weak_ptr<Chunk> RightChunk;
	std::weak_ptr<Chunk> FrontChunk;
	std::weak_ptr<Chunk> BackChunk;

	//Lighting
	vector_3d LightMap;//0xF(torch), 0xF0(sun)
	std::queue <LightNode> sunlightBfsQueue;
	std::queue <LightNode> sunlightReloadBfsQueue;
	std::queue <LightNode> lightBfsQueue;

	static const std::array <glm::ivec3, 6> nearFaces;
	Block emptyBlock;

	//Water
	std::unique_ptr<Water> water;

	//Query
	unsigned int queryID;
	std::unique_ptr<Mesh> chunkBoxMesh;
};