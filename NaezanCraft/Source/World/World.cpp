#include "../pch.h"
#include "World.h"
#include "Scene.h"
#include "Camera.h"
#include "Chunk.h"
#include "ChunkMesh.h"
#include "Water.h"

#include "../Application.h"
#include "../Renderer/Renderer.h"
#include "../World/Generator/WorldGenerator.h"
#include "../World/Environment/SkyBox.h"

#include "../ChunkThread.h"
#include "../Sound/SoundManager.h"


std::unordered_map<BlockType, std::pair<int, int>> World::BlockCoordData;
std::mutex World::worldMutex;
int World::drawCall;
const std::array <glm::vec2, 32> World::waterAnimOffsets
{
	glm::vec2(0, 0),glm::vec2(1, 0),glm::vec2(2, 0), glm::vec2(3, 0), glm::vec2(4, 0), glm::vec2(5, 0), glm::vec2(6, 0), glm::vec2(7, 0),
	glm::vec2(8, 0),glm::vec2(9, 0),glm::vec2(10,0), glm::vec2(11,0), glm::vec2(12,0), glm::vec2(13,0), glm::vec2(14,0), glm::vec2(15,0),
	glm::vec2(0, 1),glm::vec2(1, 1),glm::vec2(2, 1), glm::vec2(3, 1), glm::vec2(4, 1), glm::vec2(5, 1), glm::vec2(6, 1), glm::vec2(7, 1),
	glm::vec2(8, 1),glm::vec2(9, 1),glm::vec2(10,1), glm::vec2(11,1), glm::vec2(12,1), glm::vec2(13,1), glm::vec2(14,1), glm::vec2(15,1)
};
std::string World::worldPath = "";

World::World() : occlusionCull(true)
{
	waterAnimIndex = 0;

	renderer = std::make_unique<Renderer>();
	scene = std::make_unique<Scene>();
	sky = std::make_unique<SkyBox>();
	worldGenerator = std::make_unique<WorldGenerator>();

	playerPosition = scene->GetPlayerPosition();

	SetSavePath(worldPath);
	SetBlockDatas();
	SetBlockSoundData();

	//Load HiZ Shader
	//LoadCullingShader();
	//LoadSimpleCubeShader();

	//CreateChunk memory
	chunkThread = std::make_unique<ChunkThread>();

	updateFutures.push_back(std::future<void>(std::async(std::launch::async, &World::AsyncLoadChunk, this)));
}

World::~World()
{
	SaveWorldChunks();

	Shutdown();
}

void World::SetSavePath(std::string& path)
{
	std::string str = std::filesystem::current_path().string();
	str = str.substr(0, str.find_last_of("/\\"));
	str = (str / std::filesystem::path("WorldData")).string();

	DWORD fileAttrib = GetFileAttributesA(str.c_str());
	if (fileAttrib == INVALID_FILE_ATTRIBUTES)
	{
		if (!CreateDirectoryA(str.c_str(), NULL))
		{
			path = "";
			NC_LOG_ERROR("Failed to find save folder");
			return;
		}
	}

	path = str;
}

void World::SetBlockDatas()
{
	BlockCoordData[Dirt] = std::make_pair(13, 4);
	BlockCoordData[Sand] = std::make_pair(4, 2);
	BlockCoordData[Stone] = std::make_pair(5, 2);
	//BlockCoordData[Grass] = std::make_pair(1, 3);
	BlockCoordData[Grass] = std::make_pair(3, 3);
	BlockCoordData[GrassSide] = std::make_pair(8, 13);
	BlockCoordData[OakLog] = std::make_pair(6, 0);
	BlockCoordData[BirchLog] = std::make_pair(6, 2);
	BlockCoordData[OakLogTB] = std::make_pair(6, 1);
	BlockCoordData[BirchLogTB] = std::make_pair(6, 3);
	BlockCoordData[OakLeaves] = std::make_pair(14, 0);
	BlockCoordData[BirchLeaves] = std::make_pair(14, 1);
	BlockCoordData[WaterT] = std::make_pair(0, 0);
	BlockCoordData[Lava] = std::make_pair(4, 0);
	BlockCoordData[Iron] = std::make_pair(13, 2);
	BlockCoordData[Gold] = std::make_pair(4, 13);
	BlockCoordData[Diamond] = std::make_pair(13, 0);
	BlockCoordData[Bedrock] = std::make_pair(7, 4);
	BlockCoordData[GrowStone] = std::make_pair(3, 13);
}

void World::SetBlockSoundData()
{
	//default sound
	blockSounds.emplace(Dirt, std::make_pair("dirt", 1));

	blockSounds.emplace(Sand, std::make_pair("sand", 2));
	blockSounds.emplace(Stone, std::make_pair("stone", 2));
	blockSounds.emplace(Grass, std::make_pair("grass", 2));
	blockSounds.emplace(OakLog, std::make_pair("wood", 2));
	blockSounds.emplace(BirchLog, std::make_pair("wood", 2));

	//not have base sound
	blockSounds.emplace(Diamond, std::make_pair("cloth", 2));
	blockSounds.emplace(Bedrock, std::make_pair("cloth", 2));
}

void World::LoadCullingShader()
{
	hiZmapShaders.emplace(ShaderType::VERTEX, std::make_unique<Shader>("../Assets/Shaders/HiZmapVert.vs", ShaderType::VERTEX));
	hiZmapShaders.emplace(ShaderType::FRAGMENT, std::make_unique<Shader>("../Assets/Shaders/HiZmapFrag.fs", ShaderType::FRAGMENT));
	hiZmapShaders.emplace(ShaderType::GEOMETRY, std::make_unique<Shader>("../Assets/Shaders/HiZmapGeo.gs", ShaderType::GEOMETRY));

	cullingShaders.emplace(ShaderType::VERTEX, std::make_unique<Shader>("../Assets/Shaders/HiZOcclusionVert.vs", ShaderType::VERTEX));
	cullingShaders.emplace(ShaderType::GEOMETRY, std::make_unique<Shader>("../Assets/Shaders/HiZOcclusionGeo.gs", ShaderType::GEOMETRY));

	hiZmapShaderProgram = glCreateProgram();
	for (auto& shader : hiZmapShaders)
	{
		glAttachShader(hiZmapShaderProgram, shader.second->GetShaderID());
	}
	glLinkProgram(hiZmapShaderProgram);
	for (auto& shader : hiZmapShaders)
	{
		shader.second->LinkComplete(hiZmapShaderProgram);
	}
	//SetUniform
	glUniform1i(glGetUniformLocation(this->hiZmapShaderProgram, "LastMip"), 0);

	cullingShaderProgram = glCreateProgram();
	for (auto& shader : cullingShaders)
	{
		glAttachShader(cullingShaderProgram, shader.second->GetShaderID());
	}
	glLinkProgram(cullingShaderProgram);
	for (auto& shader : cullingShaders)
	{
		shader.second->LinkComplete(cullingShaderProgram);
	}
	//SetUniform
	glUniform1i(glGetUniformLocation(cullingShaderProgram, "HiZBuffer"), 0);

	// create color buffer texture
	glGenTextures(1, &colorTexID);
	glBindTexture(GL_TEXTURE_2D, colorTexID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, SCREEN_WIDTH, SCREEN_HEIGHT, 0,
		GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	// create depth buffer texture
	glGenTextures(1, &depthTexID);
	glBindTexture(GL_TEXTURE_2D, depthTexID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, SCREEN_WIDTH, SCREEN_HEIGHT, 0,
		GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
	// depth texture is gonna be a mipmap so we have to establish the mipmap chain
	glGenerateMipmap(GL_TEXTURE_2D);

	// create framebuffer object
	glGenFramebuffers(1, &frameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
	// setup color and depth buffer texture
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTexID, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexID, 0);

	GLenum status;
	status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	switch (status) {
		\
	case GL_FRAMEBUFFER_COMPLETE:
		break;
	case GL_FRAMEBUFFER_UNSUPPORTED:
		/* choose different formats */
		std::cout << "Unsupported framebuffer format!" << std::endl;
		break;
	default:
		/* programming error; will fail on all hardware */
		std::cout << "Invalid framebuffer format!" << std::endl;
		exit(0);
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void World::LoadSimpleCubeShader()
{
	simpleCubeShaders.emplace(ShaderType::VERTEX, std::make_unique<Shader>("../Assets/Shaders/SimpleCube.vs", ShaderType::VERTEX));
	simpleCubeShaders.emplace(ShaderType::FRAGMENT, std::make_unique<Shader>("../Assets/Shaders/SimpleCube.fs", ShaderType::FRAGMENT));

	simpleCubeShaderProgram = glCreateProgram();
	for (auto& shader : simpleCubeShaders)
	{
		glAttachShader(simpleCubeShaderProgram, shader.second->GetShaderID());
	}
	glLinkProgram(simpleCubeShaderProgram);
	for (auto& shader : simpleCubeShaders)
	{
		shader.second->LinkComplete(simpleCubeShaderProgram);
	}
}

void World::Update()
{
	OPTICK_EVENT();

	scene->Update();
	sky->Update(scene->GetPlayerPosition());

	playerPosition = scene->GetPlayerPosition();

	//블록 설치 또는 삭제할때 이 함수가 실행된다
	UpdateChunk();
}

void World::Render()
{
	OPTICK_EVENT();

	renderer->BeginRender(scene->GetCamera().lock()->GetViewProjectionMatrix());

	int chunkCount = 0;
	drawCall = 8;
	std::unique_lock<std::mutex> lock(worldMutex);
	for (auto& chunk : worldChunks)
	{
		if (scene->GetCamera().lock()->GetFrustum().AABB(chunk.second->chunkBox) != CullingState::OUTSIDE)
		{
			if (chunk.second->chunkLoadState == ChunkLoadState::MeshLoaded)
			{
				chunk.second->CreateMeshBuffer();

				chunk.second->SetLoadState(ChunkLoadState::Builted);
			}
			if (chunk.second->chunkLoadState == ChunkLoadState::Builted)
			{
				renderer->RenderChunk(chunk.second);//이미 렌더링 된 녀석은 제외 시켜야함
				++chunkCount;
				//RenderBoundingBox(chunk.second->chunkBox, chunk.second->chunkBoxMesh);
			}
		}
	}

	for (auto& waterchunk : worldChunks)
	{
		if (waterchunk.second->chunkLoadState == ChunkLoadState::Builted)
		{
			renderer->RenderWater(waterchunk.second, waterAnimOffsets[waterAnimIndex]);
		}
	}
	//water offset
	//x first, y last, (width 16, height 2)
	waterAnimIndex += 0.1f;
	waterAnimIndex = waterAnimIndex >= 32 ? 0 : waterAnimIndex;

	sky->Render(scene->GetCamera());
	scene->Render();

	RemoveChunk();

	NC_LOG_DEBUG("Chunk DrawCall : {0}", chunkCount);
}

void World::RenderBoundingBox(AABox& boundbox, std::unique_ptr<Mesh>& boxMesh)
{
	static const glm::vec3 color = glm::vec3(1.f, 1.f, 1.f);
	const glm::vec3 halfDiagonal = (boundbox.maxcorner - boundbox.corner) * 0.5f;
	const glm::vec3 position = boundbox.corner + halfDiagonal;
	glm::mat4 transform = glm::mat4(1.f);
	transform = glm::translate(transform, position);
	transform = glm::scale(transform, halfDiagonal);

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glUseProgram(simpleCubeShaderProgram);
	glUniformMatrix4fv(glGetUniformLocation(simpleCubeShaderProgram, "projectionview"), 1, GL_FALSE, glm::value_ptr(scene->GetCamera().lock()->GetViewProjectionMatrix()));
	glUniformMatrix4fv(glGetUniformLocation(simpleCubeShaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(transform));
	glUniformMatrix4fv(glGetUniformLocation(simpleCubeShaderProgram, "color"), 1, GL_FALSE, glm::value_ptr(color));
	boxMesh->BindVertexArray();
	glDrawArrays(GL_TRIANGLES, 0, 36);
	boxMesh->UnBindVertexArray();

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void World::Shutdown()
{
	renderer->Shutdown();
	updateFutures.clear();
	chunkThread.reset();

	worldChunks.clear();
}

void World::AsyncLoadChunk()
{
	while (Application::IsRunning())
	{
		for (int x = static_cast<int>(playerPosition.x / CHUNK_X) - renderDistance; x <= static_cast<int>(playerPosition.x / CHUNK_X) + renderDistance; ++x)
		{
			for (int z = static_cast<int>(playerPosition.z / CHUNK_Z) - renderDistance; z <= static_cast<int>(playerPosition.z / CHUNK_Z) + renderDistance; ++z)
			{
				std::unique_lock<std::mutex> lock(worldMutex);
				if (!IsChunkCreatedByPos(x, z))
				{
					worldChunks[std::pair<int, int>(x, z)] = std::make_shared<Chunk>(glm::vec3(x, 0, z));
				}
				loadChunks.emplace(std::pair<int, int>(x, z), worldChunks[std::pair<int, int>(x, z)]);
			}
		}

		for (const auto& chunk : loadChunks)
		{
			if (chunk.second->chunkLoadState == ChunkLoadState::Unloaded)
			{
				chunk.second->SetupChunkNeighbor();
				if (!ExistChunkFilePath(Chunk::GetChunkDataPath(chunk.second->position.x, chunk.second->position.z, worldPath)))
					GenerateChunkTerrain(chunk.second);
				else
					chunk.second->LoadChunk(worldPath);
				chunk.second->SetLoadState(ChunkLoadState::TerrainGenerated);
			}
		}

		for (const auto chunk : loadChunks)
		{
			if (chunk.second->chunkLoadState == ChunkLoadState::TerrainGenerated)
			{
				if (chunk.second->AllDirectionChunkIsReady())
					CreateChunk(chunk.second);
			}
		}

		loadChunks.clear();
	}
}

void World::RemoveChunk()
{
	std::vector<decltype(worldChunks)::key_type> deletableKey;
	for (auto& chunk : worldChunks)
	{
		//if chunk location is out of range -> erase chunk
		if (chunk.second->position.x < static_cast<int>(playerPosition.x / CHUNK_X) - renderDistance ||
			chunk.second->position.x > static_cast<int>(playerPosition.x / CHUNK_X) + renderDistance ||
			chunk.second->position.z < static_cast<int>(playerPosition.z / CHUNK_Z) - renderDistance ||
			chunk.second->position.z > static_cast<int>(playerPosition.z / CHUNK_Z) + renderDistance)
		{
			deletableKey.push_back(std::make_pair(
				static_cast<int>(chunk.second->position.x),
				static_cast<int>(chunk.second->position.z)
			));
		}
	}

	if (!deletableKey.empty())
	{
		RemoveWorldChunk(deletableKey);
	}
}

void World::GenerateChunkTerrain(std::weak_ptr<Chunk> chunk)
{
	chunk.lock()->GenerateTerrain(worldGenerator);
}

void World::CreateChunk(std::weak_ptr<Chunk> chunk)
{
	chunk.lock()->CreateLightMap();
	chunk.lock()->CreateSSAO();

	std::unique_lock<std::mutex> lock(worldMutex);
	chunk.lock()->CreateChunkMesh(false);
}

void World::UpdateChunk()
{
	std::unique_lock<std::mutex> lock(worldMutex);
	for (auto& loadinfo : reloadChunks)
	{
		auto chunk = worldChunks.find(loadinfo.first);
		if (chunk != worldChunks.end())
		{
			if (chunk->second->chunkLoadState == ChunkLoadState::Builted)
			{
				chunk->second->CreateLightMap();
				chunk->second->ReloadSSAO(loadinfo.second);
				chunk->second->CreateChunkMesh(true);
			}
		}
	}
	reloadChunks.clear();
}

void World::RemoveWorldChunk(std::vector<decltype(worldChunks)::key_type>& _deletableKey)
{
	auto iter = _deletableKey.begin();
	for (; iter != _deletableKey.end();)
	{
		//세이브 완료가 되지 않았다면 건너뛰고 세이브가 완료되었다면 딜리트함수 수행
		if (worldChunks[*iter]->IsSaved())
		{
			if (worldChunks[*iter]->chunkMesh != nullptr)
			{
				worldChunks[*iter]->chunkMesh->DeleteChunkMesh();
			}
			worldChunks[*iter].reset();
			worldChunks.erase(*iter);

			iter = _deletableKey.erase(iter);
		}
		else
		{
			if (worldChunks[*iter]->IsLoaded())
			{
				chunkThread->saveChunks.push_back(worldChunks[*iter]);
				worldChunks[*iter]->SetSerialStatus(ChunkSerialStatus::Saving);
			}
			++iter;
		}
	}
	chunkThread->BeginThread();
}

void World::SaveWorldChunks()
{
	for (auto& chunk : worldChunks)
	{
		if (!chunk.second->IsSaved())
		{
			if (chunk.second->IsLoaded())
			{
				chunkThread->saveChunks.push_back(chunk.second);
				chunk.second->SetSerialStatus(ChunkSerialStatus::Saving);
			}
		}
	}
	chunkThread->BeginThread();
}


bool World::GetChunkByPos(const std::pair<int, int>& key, std::weak_ptr<Chunk>& outChunk)
{
	auto chunkIt = worldChunks.find(key);

	if (chunkIt == worldChunks.end())
	{
		return false;
	}

	outChunk = worldChunks.at(key);

	return true;
}

bool World::IsChunkCreatedByPos(int x, int y)
{
	auto iter = worldChunks.find(std::make_pair(x, y));
	if (iter == worldChunks.end())
	{
		return false;
	}

	return true;
}

bool World::IsChunkCreatedByPos(const std::pair<int, int>& pos)
{
	auto iter = worldChunks.find(pos);
	if (iter == worldChunks.end())
	{
		return false;
	}

	return true;
}

bool World::GetBlockByWorldPos(int x, int y, int z, Block& block)
{
	std::pair<int, int> key(x, z);
	if (key.first < 0)
	{
		key.first -= (CHUNK_X - 1);
	}
	key.first /= CHUNK_X;

	if (key.second < 0)
	{
		key.second -= (CHUNK_Z - 1);
	}
	key.second /= CHUNK_Z;

	std::weak_ptr<Chunk> outChunk;
	if (GetChunkByPos(key, outChunk))
	{
		x %= CHUNK_X;
		z %= CHUNK_Z;

		if (x < 0)
		{
			x += CHUNK_X;
			x %= CHUNK_X;
		}
		if (z < 0)
		{
			z += CHUNK_Z;
			z %= CHUNK_Z;
		}
		block = outChunk.lock()->GetBlock(x, y, z);
		return true;
	}

	return false;
}

bool World::SetBlockByWorldPos(int x, int y, int z, BlockType blocktype)
{
	std::pair<int, int> key(x, z);
	if (key.first < 0)
	{
		key.first -= (CHUNK_X - 1);
	}
	key.first /= CHUNK_X;

	if (key.second < 0)
	{
		key.second -= (CHUNK_Z - 1);
	}
	key.second /= CHUNK_Z;

	std::weak_ptr<Chunk> outChunk;
	if (GetChunkByPos(key, outChunk))
	{
		x %= CHUNK_X;
		z %= CHUNK_Z;

		if (x < 0)
		{
			x += CHUNK_X;
			x %= CHUNK_X;
		}
		if (z < 0)
		{
			z += CHUNK_Z;
			z %= CHUNK_Z;
		}

		//사이드 청크 리로드
		std::pair<int, int> sidekey = key;
		if (x == 0)
		{
			sidekey.first -= 1;
			RegisterReloadChunk(sidekey, glm::vec3(15, y, z));
		}
		else if (x == 15)
		{
			sidekey.first += 1;
			RegisterReloadChunk(sidekey, glm::vec3(0, y, z));
		}
		sidekey = key;
		if (z == 0)
		{
			sidekey.second -= 1;
			RegisterReloadChunk(sidekey, glm::vec3(x, y, 15));
		}
		else if (z == 15)
		{
			sidekey.second += 1;
			RegisterReloadChunk(sidekey, glm::vec3(x, y, 0));
		}

		std::random_device rd;
		std::mt19937 gen(rd());
		if (outChunk.lock()->GetBlock(x, y, z).blockType == BlockType::Air && blocktype != BlockType::Air)
		{
			//Emplace Sound
			std::pair<std::string, int> soundinfo;
			if (blockSounds.find(blocktype) == blockSounds.end())
			{
				soundinfo.first = "dirt";
				soundinfo.second = 1;
			}
			else
			{
				soundinfo = blockSounds[blocktype];
			}

			if (soundinfo.second != 1)
			{
				std::uniform_int_distribution<int> dis(1, soundinfo.second);
				soundinfo.first += std::to_string(dis(gen));
			}

			SoundManager::Play2D(soundinfo.first, false);
		}
		else if (outChunk.lock()->GetBlock(x, y, z).blockType != BlockType::Air && blocktype == BlockType::Air)
		{
			//Delete Sound
			auto& soundBlockType = outChunk.lock()->GetBlock(x, y, z).blockType;
			std::pair<std::string, int> soundinfo;
			if (blockSounds.find(soundBlockType) == blockSounds.end())
			{
				soundinfo.first = "dirt";
				soundinfo.second = 1;
			}
			else
			{
				soundinfo = blockSounds[soundBlockType];
			}

			if (soundinfo.second != 1)
			{
				std::uniform_int_distribution<int> dis(1, soundinfo.second);
				soundinfo.first += std::to_string(dis(gen));
			}

			SoundManager::Play2D(soundinfo.first, false);
		}

		outChunk.lock()->SetBlock(x, y, z, blocktype);

		outChunk.lock()->SetSunLight(x, y, z, 0);

		//리로드할 청크 추가
		RegisterReloadChunk(key, glm::vec3(x, y, z));
		return true;
	}
	return false;
}

bool World::CanEmplaceBlockByWorldPos(int blockX, int blockY, int blockZ, int faceblockX, int faceblockY, int faceblockZ)
{
	Block outBlock;
	if (GetBlockByWorldPos(blockX, blockY, blockZ, outBlock) && !outBlock.IsFluid())
	{
		Block faceBlock;
		if (GetBlockByWorldPos(faceblockX, faceblockY, faceblockZ, faceBlock) && faceBlock.IsFluid())
		{
			return true;
		}
	}

	return false;
}

void World::RegisterReloadChunk(std::pair<int, int> key, const glm::vec3& blockPos)
{
	reloadChunks.emplace(key, blockPos);
}

bool World::ExistChunkFilePath(const std::string& path)
{
	struct stat buffer;
	return (stat(path.c_str(), &buffer) == 0);
}