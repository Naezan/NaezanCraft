#include "../../pch.h"
#include "SkyBox.h"
#include "../../TextureManager.h"
#include "../../Renderer/Environment/SkyBoxShader.h"
#include "../../Renderer/Environment/SunMoonShader.h"
#include "../../Renderer/VertexArray.h"
#include "../../Renderer/Buffer.h"
#include "../Mesh.h"
#include "Cloud.h"


#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

SkyBox::SkyBox()
{
	TextureManager::AddTexture("Sun", "../Assets/Textures/Sun.png");
	TextureManager::AddTexture("Moon", "../Assets/Textures/moon_phases.png");
	TextureManager::AddTexture("Cloud", "../Assets/Textures/clouds.png");

	//SkyShader
	{
		skyBoxShaders.emplace(ShaderType::VERTEX, std::make_unique<SkyBoxShader>("../Assets/Shaders/ProcSky.vs", ShaderType::VERTEX));
		skyBoxShaders.emplace(ShaderType::FRAGMENT, std::make_unique<SkyBoxShader>("../Assets/Shaders/ProcSky.fs", ShaderType::FRAGMENT));

		skyShaderProgram = glCreateProgram();

		for (auto& shader : skyBoxShaders)
		{
			glAttachShader(skyShaderProgram, shader.second->GetShaderID());
		}

		glLinkProgram(skyShaderProgram);

		for (auto& shader : skyBoxShaders)
		{
			shader.second->LinkComplete(skyShaderProgram);
		}

		skyBoxShaders[ShaderType::VERTEX]->GetUniform(skyShaderProgram);
	}

	//SkyVertex
		//4th value is weight value, for change specific weather color
	std::vector<GLfloat> vertexCoords{
		//Back
		SKY_SIZE, -SKY_SIZE, -SKY_SIZE, 3,
		-SKY_SIZE, -SKY_SIZE, -SKY_SIZE, 0,
		-SKY_SIZE,  SKY_SIZE, -SKY_SIZE, 1,
		SKY_SIZE,  SKY_SIZE, -SKY_SIZE, 2,

		//Front
		-SKY_SIZE, -SKY_SIZE, SKY_SIZE, 4,
		SKY_SIZE, -SKY_SIZE, SKY_SIZE, 7,
		SKY_SIZE,  SKY_SIZE, SKY_SIZE, 5,
		-SKY_SIZE,  SKY_SIZE, SKY_SIZE, 6,

		//Right
		SKY_SIZE, -SKY_SIZE,  SKY_SIZE, 7,
		SKY_SIZE, -SKY_SIZE, -SKY_SIZE, 3,
		SKY_SIZE,  SKY_SIZE, -SKY_SIZE, 2,
		SKY_SIZE,  SKY_SIZE,  SKY_SIZE, 5,

		//Left
		-SKY_SIZE, -SKY_SIZE, -SKY_SIZE, 0,
		-SKY_SIZE, -SKY_SIZE,  SKY_SIZE, 4,
		-SKY_SIZE,  SKY_SIZE,  SKY_SIZE, 6,
		-SKY_SIZE,  SKY_SIZE, -SKY_SIZE, 1,

		//Top
		-SKY_SIZE, SKY_SIZE,  SKY_SIZE, 6,
		SKY_SIZE, SKY_SIZE,  SKY_SIZE, 5,
		SKY_SIZE, SKY_SIZE, -SKY_SIZE, 2,
		-SKY_SIZE, SKY_SIZE, -SKY_SIZE, 1,

		//Bottom
		-SKY_SIZE, -SKY_SIZE, -SKY_SIZE, 0,
		SKY_SIZE, -SKY_SIZE, -SKY_SIZE, 3,
		SKY_SIZE, -SKY_SIZE,  SKY_SIZE, 7,
		-SKY_SIZE, -SKY_SIZE,  SKY_SIZE, 4,
	};

	std::vector<GLuint> indices{
		0, 1, 2,
		2, 3, 0,

		4, 5, 6,
		6, 7, 4,

		8, 9, 10,
		10, 11, 8,

		12, 13, 14,
		14, 15, 12,

		16, 17, 18,
		18, 19, 16,

		20, 21, 22,
		22, 23, 20
	};

	//Sky
	{
		vertexArray = std::make_unique<VertexArray>();

		indexBuffer = std::make_unique<IndexBuffer>(indices.size() * sizeof(GLuint), &indices.front());
		skyIndicesSize = sizeof(indices);

		vertexBuffer = std::make_unique<VertexBuffer>(0, (void*)0);
		vertexBuffer->SetBufferData(vertexCoords.size() * sizeof(GLfloat), &vertexCoords.front());

		vertexBuffer->UnBind();
		vertexArray->UnBind();
	}

	//SunMoonShader
	{
		sunMoonShaders.emplace(ShaderType::VERTEX, std::make_unique<SunMoonShader>("../Assets/Shaders/SunMoon.vs", ShaderType::VERTEX));
		sunMoonShaders.emplace(ShaderType::FRAGMENT, std::make_unique<SunMoonShader>("../Assets/Shaders/SunMoon.fs", ShaderType::FRAGMENT));

		sunMoonShaderProgram = glCreateProgram();

		for (auto& shader : sunMoonShaders)
		{
			glAttachShader(sunMoonShaderProgram, shader.second->GetShaderID());
		}

		glLinkProgram(sunMoonShaderProgram);

		for (auto& shader : sunMoonShaders)
		{
			shader.second->LinkComplete(sunMoonShaderProgram);
		}

		sunMoonShaders[ShaderType::VERTEX]->GetUniform(sunMoonShaderProgram);
		glUniform1i(glGetUniformLocation(sunMoonShaderProgram, "sunMoonTexture"), 0);
	}

	//SunMoonVertex
	struct SunMoonVertexCoord
	{
		glm::vec3 pos;
		glm::vec2 texcoord;
	};

	std::vector<SunMoonVertexCoord> sunVertexCoords
	{
		{ glm::vec3(-20,  20, 400),	glm::vec2(0, 1) },
		{ glm::vec3(20,  20, 400),	glm::vec2(1, 1) },
		{ glm::vec3(20, -20, 400),	glm::vec2(1, 0) },
		{ glm::vec3(-20, -20, 400),	glm::vec2(0, 0) }
	};

	//TO DO change random Moon texCoord
	std::vector<SunMoonVertexCoord> MoonVertexCoords
	{
		{ glm::vec3(-15,  15, -400),glm::vec2(0, .5) },
		{ glm::vec3(15,  15, -400),	glm::vec2(.25, .5) },
		{ glm::vec3(15, -15, -400),	glm::vec2(.25, 0) },
		{ glm::vec3(-15, -15, -400),glm::vec2(0, 0) }
	};

	std::vector<unsigned int> IndexCoords
	{
		0, 1, 2,
		2, 3, 0
	};

	//SunMesh
	{
		sunMesh = std::make_unique<Mesh>();
		sunMesh->CreateVertexArray();

		sunMesh->SetIndexBufferVector(IndexCoords);
		sunMesh->CreateIndexBuffer();

		sunMesh->CreateVertexBuffer(static_cast<int>(sizeof(SunMoonVertexCoord)), (void*)offsetof(SunMoonVertexCoord, pos),
			static_cast<int>(sizeof(SunMoonVertexCoord)), (void*)offsetof(SunMoonVertexCoord, texcoord),
			GL_FLOAT, GL_FLOAT);
		sunMesh->SetVertexBufferData(sunVertexCoords.size() * sizeof(SunMoonVertexCoord), &sunVertexCoords.front());

		sunMesh->UnBindVertexBuffer();
		sunMesh->UnBindVertexArray();
	}

	//MoonMesh
	{
		moonMesh = std::make_unique<Mesh>();
		moonMesh->CreateVertexArray();

		moonMesh->SetIndexBufferVector(IndexCoords);
		moonMesh->CreateIndexBuffer();

		moonMesh->CreateVertexBuffer(static_cast<int>(sizeof(SunMoonVertexCoord)), (void*)offsetof(SunMoonVertexCoord, pos),
			static_cast<int>(sizeof(SunMoonVertexCoord)), (void*)offsetof(SunMoonVertexCoord, texcoord),
			GL_FLOAT, GL_FLOAT);
		moonMesh->SetVertexBufferData(MoonVertexCoords.size() * sizeof(SunMoonVertexCoord), &MoonVertexCoords.front());

		sunMoonIndicesSize = IndexCoords.size();

		moonMesh->UnBindVertexBuffer();
		moonMesh->UnBindVertexArray();
	}

	//Cloud
	{
		cloud = std::make_unique<Cloud>();
	}
}

SkyBox::~SkyBox()
{
	if (indexBuffer != nullptr)
		indexBuffer->DeleteBuffer();
	if (vertexBuffer != nullptr)
		vertexBuffer->DeleteBuffer();
	if (vertexArray != nullptr)
		vertexArray->DeleteBuffer();

	indexBuffer.reset();
	vertexBuffer.reset();
	vertexArray.reset();

	sunMesh->DeleteMesh();
	moonMesh->DeleteMesh();

	glDeleteProgram(skyShaderProgram);
	glDeleteProgram(sunMoonShaderProgram);
}

void SkyBox::Update(glm::vec3& playerPos)
{
	TransformMatrix = glm::translate(glm::mat4(1.0f), playerPos);

	//cloud->Update();
}

void SkyBox::Render(std::weak_ptr<Camera>& camera)
{
	glDisable(GL_CULL_FACE);
	glEnable(GL_BLEND);


	//Render Sky
	glUseProgram(skyShaderProgram);
	skyBoxShaders[ShaderType::VERTEX]->Update(camera, TransformMatrix);

	vertexArray->Bind();
	glDrawElements(GL_TRIANGLES, skyIndicesSize, GL_UNSIGNED_INT, nullptr);
	vertexArray->UnBind();


	glUseProgram(sunMoonShaderProgram);
	sunMoonShaders[ShaderType::VERTEX]->Update(camera, TransformMatrix);

	//Sun
	TextureManager::BindTexture("Sun");
	sunMesh->BindVertexArray();
	glDrawElements(GL_TRIANGLES, sunMoonIndicesSize, GL_UNSIGNED_INT, nullptr);
	sunMesh->UnBindVertexArray();

	//Moon
	TextureManager::BindTexture("Moon");
	moonMesh->BindVertexArray();
	glDrawElements(GL_TRIANGLES, sunMoonIndicesSize, GL_UNSIGNED_INT, nullptr);
	moonMesh->UnBindVertexArray();

	//Cloud
	cloud->Render(camera, TransformMatrix);
}
