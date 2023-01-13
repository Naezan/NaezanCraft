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


std::unordered_map<BlockType, std::pair<int, int>> World::BlockCoordData;
std::mutex World::worldMutex;
int World::drawCall;
const std::array <glm::vec2, 32> World::animOffsets
{
	glm::vec2(0, 0),glm::vec2(1, 0),glm::vec2(2, 0), glm::vec2(3, 0), glm::vec2(4, 0), glm::vec2(5, 0), glm::vec2(6, 0), glm::vec2(7, 0),
	glm::vec2(8, 0),glm::vec2(9, 0),glm::vec2(10,0), glm::vec2(11,0), glm::vec2(12,0), glm::vec2(13,0), glm::vec2(14,0), glm::vec2(15,0),
	glm::vec2(0, 1),glm::vec2(1, 1),glm::vec2(2, 1), glm::vec2(3, 1), glm::vec2(4, 1), glm::vec2(5, 1), glm::vec2(6, 1), glm::vec2(7, 1),
	glm::vec2(8, 1),glm::vec2(9, 1),glm::vec2(10,1), glm::vec2(11,1), glm::vec2(12,1), glm::vec2(13,1), glm::vec2(14,1), glm::vec2(15,1)
};
std::string World::worldPath = "";

static const GLfloat cubeVertices[] = {
	-1.0f,-1.0f,-1.0f, // triangle 1 : begin
	-1.0f,-1.0f, 1.0f,
	-1.0f, 1.0f, 1.0f, // triangle 1 : end
	1.0f, 1.0f,-1.0f, // triangle 2 : begin
	-1.0f,-1.0f,-1.0f,
	-1.0f, 1.0f,-1.0f, // triangle 2 : end
	1.0f,-1.0f, 1.0f,
	-1.0f,-1.0f,-1.0f,
	1.0f,-1.0f,-1.0f,
	1.0f, 1.0f,-1.0f,
	1.0f,-1.0f,-1.0f,
	-1.0f,-1.0f,-1.0f,
	-1.0f,-1.0f,-1.0f,
	-1.0f, 1.0f, 1.0f,
	-1.0f, 1.0f,-1.0f,
	1.0f,-1.0f, 1.0f,
	-1.0f,-1.0f, 1.0f,
	-1.0f,-1.0f,-1.0f,
	-1.0f, 1.0f, 1.0f,
	-1.0f,-1.0f, 1.0f,
	1.0f,-1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,
	1.0f,-1.0f,-1.0f,
	1.0f, 1.0f,-1.0f,
	1.0f,-1.0f,-1.0f,
	1.0f, 1.0f, 1.0f,
	1.0f,-1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,
	1.0f, 1.0f,-1.0f,
	-1.0f, 1.0f,-1.0f,
	1.0f, 1.0f, 1.0f,
	-1.0f, 1.0f,-1.0f,
	-1.0f, 1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,
	-1.0f, 1.0f, 1.0f,
	1.0f,-1.0f, 1.0f
};


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

	//Load HiZ Shader
	//LoadCullingShader();
	LoadSimpleCubeShader();

	//CreateChunk memory
	//worldChunks.reserve(LOOK_CHUNK_SIZE * LOOK_CHUNK_SIZE);
	updateFutures.push_back(std::future<void>(std::async(std::launch::async, &World::AsyncLoadChunk, this)));
}

World::~World()
{
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
	BlockCoordData[Dirt] = std::make_pair(13, 2);
	BlockCoordData[Sand] = std::make_pair(22, 13);
	BlockCoordData[Stone] = std::make_pair(23, 15);
	//BlockCoordData[Grass] = std::make_pair(7, 24);
	BlockCoordData[Grass] = std::make_pair(9, 24);
	BlockCoordData[GrassSide] = std::make_pair(8, 13);
	BlockCoordData[OakLog] = std::make_pair(16, 15);
	BlockCoordData[OakLogTB] = std::make_pair(17, 0);
	BlockCoordData[OakLeaves] = std::make_pair(16, 0);
	BlockCoordData[BirchLog] = std::make_pair(16, 11);
	BlockCoordData[BirchLogTB] = std::make_pair(16, 12);
	BlockCoordData[BirchLeaves] = std::make_pair(16, 1);
	BlockCoordData[WaterT] = std::make_pair(0, 0);
	BlockCoordData[Lava] = std::make_pair(4, 0);
	BlockCoordData[Iron] = std::make_pair(6, 15);
	BlockCoordData[Gold] = std::make_pair(4, 13);
	BlockCoordData[Diamond] = std::make_pair(13, 0);
	BlockCoordData[Bedrock] = std::make_pair(7, 4);
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

				glGenQueries(1, &chunk.second->queryID);

				chunk.second->chunkBoxMesh = std::make_unique<Mesh>();
				chunk.second->chunkBoxMesh->CreateVertexArray();
				chunk.second->chunkBoxMesh->CreateVertexBuffer(0, (GLvoid*)0, GL_FLOAT, 3);
				chunk.second->chunkBoxMesh->SetVertexBufferData(sizeof(cubeVertices), cubeVertices);
				chunk.second->chunkBoxMesh->UnBindVertexBuffer();
				chunk.second->chunkBoxMesh->UnBindVertexArray();

				chunk.second->SetLoadState(ChunkLoadState::Builted);
			}
			if (chunk.second->chunkLoadState == ChunkLoadState::Builted)
			{
				/*
				if (occlusionCull)
				{
					//BindFrameBuffer
					glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
					glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

					glUseProgram(hiZmapShaderProgram);
					glUniform1i(glGetUniformLocation(this->hiZmapShaderProgram, "LastMip"), 0);

					// 뎁스 이미지만을 렌더링 하기 위해서 컬러 버퍼를 모두 끈다
					glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
					glActiveTexture(GL_TEXTURE0);
					glBindTexture(GL_TEXTURE_2D, depthTexID);
					// 뎁스 검사를 끄고 뎁스를 쓸 수 있도록 변경
					glDepthFunc(GL_ALWAYS);

					// calculate the number of mipmap levels for NPOT texture
					int numLevels = 1 + (int)floorf(log2f(fmaxf(SCREEN_WIDTH, SCREEN_HEIGHT)));
					int currentWidth = SCREEN_WIDTH;
					int currentHeight = SCREEN_HEIGHT;
					for (int i = 1; i < numLevels; i++)
					{
						glUniform2i(glGetUniformLocation(hiZmapShaderProgram, "LastMipSize"), currentWidth, currentHeight);

						// 다음 뷰포트 사이즈 계산
						currentWidth /= 2;
						currentHeight /= 2;
						// ensure that the viewport size is always at least 1x1
						currentWidth = currentWidth > 0 ? currentWidth : 1;
						currentHeight = currentHeight > 0 ? currentHeight : 1;

						glViewport(0, 0, currentWidth, currentHeight);
						// bind next level for rendering but first restrict fetches only to previous level
						glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, i - 1);
						glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, i - 1);

						glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexID, i);
						// dummy draw command as the full screen quad is generated completely by a geometry shader
						glDrawArrays(GL_POINTS, 0, 1);
						drawCall++;
					}

					// reset mipmap level range for the depth image
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, numLevels - 1);
					// reset the framebuffer configuration
					glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTexID, 0);
					glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexID, 0);
					// reenable color buffer writes, reset viewport and reenable depth test
					glDepthFunc(GL_LEQUAL);
					glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
				}
				*/

				/*
				//render objects and apply culling
				glUseProgram(cullingShaderProgram);
				glUniformSubroutinesuiv(GL_VERTEX_SHADER, 1, &this->subIndexVS[this->cullMode]);
				glUniformSubroutinesuiv(GL_GEOMETRY_SHADER, 1, &this->subIndexGS[this->LODMode ? 1 : 0]);

				glGenQueries(1, );

				glGenBuffers(1, &TFBO);
				glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, TFBO);
				glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, TFBO);
				glBufferData(GL_TRANSFORM_FEEDBACK_BUFFER, size, newData, GL_DYNAMIC_COPY);
				//TODO TransformFeedback->SetOutVisible();

				//SetColor
				//SetClear

				glEnable(GL_RASTERIZER_DISCARD);

				glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, TFBO);
				glBeginTransformFeedback(GL_POINTS);

				glBeginQuery(GL_PRIMITIVES_GENERATED, QueryId);

				for (int i = -3; i <= 3; i++)
					for (int j = -3; j <= 3; j++)
						if (visible[i + 3][j + 3])
						{
							glUniform2f(glGetUniformLocation(this->cullPO, "Offset"), TERRAIN_OBJECT_SIZE * (i - x), TERRAIN_OBJECT_SIZE * (j - z));
							glDrawArrays(GL_POINTS, 0, this->instanceCount);
						}

				glEndQuery(GL_PRIMITIVES_GENERATED);
				glEndTransformFeedback();

				glDisable(GL_RASTERIZER_DISCARD);

				// draw trees
				glUseProgram(this->treePO);
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D_ARRAY, this->treeTex);
				glActiveTexture(GL_TEXTURE1);
				glBindTexture(GL_TEXTURE_2D, this->terrainTex);

				glBindVertexArray(this->treeVA);
				glGetQueryObjectiv(this->cullQuery, GL_QUERY_RESULT, &visible);
				if (visible > 0)
				{
					// draw
				}

				//UnBindFrameBuffer
				glBindFramebuffer(GL_FRAMEBUFFER, 0);
				glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
				*/

				//glShadeModel(GL_FLAT);
				//glDisable(GL_LIGHTING);
				//glDisable(GL_COLOR_MATERIAL);
				//glDisable(GL_NORMALIZE);
				//glColorMask(false, false, false, false);
				//glDisable(GL_TEXTURE);
				//glDepthMask(GL_FALSE);

				////DrawBoundingBox
				//RenderBoundingBox(chunk.second->chunkBox, chunk.second->chunkBoxMesh);
				//glBeginQuery(GL_SAMPLES_PASSED, chunk.second->queryID);
				////DrawBoundingBox
				//glEnable(GL_DEPTH_TEST);
				//RenderBoundingBox(chunk.second->chunkBox, chunk.second->chunkBoxMesh);
				//glEndQuery(GL_SAMPLES_PASSED);

				//glColorMask(true, true, true, true);
				//glEnable(GL_TEXTURE);
				//glDepthMask(GL_TRUE);

				//glClear(GL_DEPTH_BUFFER_BIT);


				//GLint iPassingSamples = 0;
				//glGetQueryObjectiv(chunk.second->queryID, GL_QUERY_RESULT, &iPassingSamples);

				//if (iPassingSamples > 0)
				//{
				//	//RenderBoundingBox(chunk.second->chunkBox, chunk.second->chunkBoxMesh);
				//	renderer->RenderChunk(chunk.second);//이미 렌더링 된 녀석은 제외 시켜야함
				//	//++chunkCount;
				//	++drawCall;
				//}

				renderer->RenderChunk(chunk.second);//이미 렌더링 된 녀석은 제외 시켜야함
				//RenderBoundingBox(chunk.second->chunkBox, chunk.second->chunkBoxMesh);
			}
		}
	}

	for (auto& waterchunk : worldChunks)
	{
		if (waterchunk.second->chunkLoadState == ChunkLoadState::Builted)
		{
			renderer->RenderWater(waterchunk.second, animOffsets[waterAnimIndex]);
		}
	}
	//change water offset
	//x first, y last, (width 16, height 2)
	waterAnimIndex += 0.1f;
	waterAnimIndex = waterAnimIndex >= 32 ? 0 : waterAnimIndex;

	scene->Render();
	sky->Render(scene->GetCamera());

	RemoveChunk();

	//std::cout << ChunkCount << std::endl;
	//NC_LOG_DEBUG("drawCall : {0}", drawCall);
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
				if (!IsChunkGenerated(std::make_pair<int, int>(static_cast<int>(chunk.second->position.x), static_cast<int>(chunk.second->position.z))))
					GenerateChunkTerrain(chunk.second, chunk.second->position.x, chunk.second->position.z);
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

void World::GenerateChunkTerrain(std::weak_ptr<Chunk> chunk, int x, int z)
{
	chunk.lock()->GenerateTerrain(worldGenerator);
	std::pair<int, int> key(x, z);
	SetWorldChunkLoadStatus(key, true);
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
				if (glIsQuery(worldChunks[*iter]->queryID) == TRUE)
				{
					glDeleteQueries(1, &worldChunks[*iter]->queryID);
				}
				worldChunks[*iter].reset();
				worldChunks.erase(*iter);

				iter = _deletableKey.erase(iter);
			}
		}
		else
		{
			if (worldChunks[*iter]->IsLoaded())
			{
				ChunkThread::saveChunks.push_back(worldChunks[*iter]);
				worldChunks[*iter]->SetSerialStatus(ChunkSerialStatus::Saving);
			}
			++iter;
		}
	}
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

bool World::IsChunkGenerated(std::pair<int, int> key)
{
	//찾으면 return true or false 일반적으로 찾으면 true return
	if (worldChunkLoadStatus.find(key) != worldChunkLoadStatus.end())
		return worldChunkLoadStatus[key];

	//아직 생성되지 않은경우
	return false;
}

void World::SetWorldChunkLoadStatus(std::pair<int, int> key, bool status)
{
	worldChunkLoadStatus[key] = status;
}
