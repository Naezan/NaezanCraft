#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Block.h"
#include "../Renderer/Shader.h"

class Renderer;
class Scene;
class SkyBox;
class Chunk;
class WorldGenerator;
class Mesh;
class ChunkThread;
struct AABox;
enum class ChunkLoadState;

struct Pair_IntHash
{
	std::size_t operator () (const std::pair<int, int>& p) const {
		std::size_t hash1 = std::hash<int>{}(p.first);
		std::size_t hash2 = std::hash<int>{}(p.second);

		if (hash1 != hash2) {
			return hash1 ^ hash2;
		}

		return hash1;
	}
};

class World
{
public:
	World();
	~World();

	void SetSavePath(std::string& path);
	void SetBlockDatas();
	void SetBlockSoundData();
	void LoadCullingShader();
	void LoadSimpleCubeShader();
	void Update();
	void Render();
	void RenderBoundingBox(AABox& boundbox, std::unique_ptr<Mesh>& boxMesh);
	void Shutdown();

	void AsyncLoadChunk();
	void RemoveChunk();
	void GenerateChunkTerrain(std::weak_ptr<Chunk> chunk);
	void CreateChunk(std::weak_ptr<Chunk> chunk);
	void UpdateChunk();
	void RemoveWorldChunk(std::vector<std::pair<int, int>>& _deletableKey);

	void SaveWorldChunks();

	static inline std::unique_ptr<World> CreateCraftWorld()
	{
		return std::make_unique<World>();
	}

	bool GetChunkByPos(const std::pair<int, int>& key, std::weak_ptr<Chunk>& outChunk);
	bool IsChunkCreatedByPos(int x, int y);
	bool IsChunkCreatedByPos(const std::pair<int, int>& pos);
	bool GetBlockByWorldPos(int x, int y, int z, Block& block);
	bool SetBlockByWorldPos(int x, int y, int z, BlockType blocktype);
	bool CanEmplaceBlockByWorldPos(int blockX, int blockY, int blockZ, int faceblockX, int faceblockY, int faceblockZ);
	void RegisterReloadChunk(std::pair<int, int> key, const glm::vec3& blockPos);
	bool ExistChunkFilePath(const std::string& path);

public:
	static std::unordered_map<BlockType, std::pair<int, int>> BlockCoordData;
	static std::unordered_map<BlockType, unsigned char> BlockLightScale;
	static std::mutex worldMutex;
	static int drawCall;
	static const std::array <glm::vec2, 32> waterAnimOffsets;

	//Save
	static std::string worldPath;

private:
	std::unique_ptr<Renderer> renderer;
	std::unique_ptr<Scene> scene;
	std::unique_ptr<SkyBox> sky;

	std::unordered_map<std::pair<int, int>, std::shared_ptr<Chunk>, Pair_IntHash> worldChunks;
	std::unordered_map<std::pair<int, int>, std::shared_ptr<Chunk>, Pair_IntHash> loadChunks;
	std::map<std::pair<int, int>, glm::vec3> reloadChunks;

	const int renderDistance = 5;
	glm::vec3 playerPosition;

	std::unique_ptr<WorldGenerator> worldGenerator;
	std::vector<std::future<void>> updateFutures;

	//culling
	uint32_t hiZmapShaderProgram;
	uint32_t cullingShaderProgram;
	uint32_t colorTexID;
	uint32_t depthTexID;
	uint32_t frameBuffer;
	std::map<ShaderType, std::unique_ptr<Shader>> hiZmapShaders;
	std::map<ShaderType, std::unique_ptr<Shader>> cullingShaders;
	bool occlusionCull;

	uint32_t simpleCubeShaderProgram;
	std::map<ShaderType, std::unique_ptr<Shader>> simpleCubeShaders;

	//water
	float waterAnimIndex;

	//save
	std::unique_ptr<ChunkThread> chunkThread;

	//sound 블록타입에 맞는 사운드key값과 사운드 개수
	std::map<BlockType, std::pair<std::string, int>> blockSounds;
};