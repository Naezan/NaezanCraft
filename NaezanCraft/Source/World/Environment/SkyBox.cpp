#include "../../pch.h"
#include "SkyBox.h"
#include "../Mesh.h"
#include "../../Renderer/Environment/SkyBoxShader.h"
#include "../../Renderer/Environment/SunMoonShader.h"
#include "../../Renderer/VertexArray.h"
#include "../../Renderer/Buffer.h"

#include "../../TextureManager.h"

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

SkyBox::SkyBox()
{
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
	{
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

		vertexArray = VertexArray::CreateArray();
		vertexArray->Bind();

		vertexBuffer = Buffer::CreateBuffer<VertexBuffer>(0, (void*)0);
		vertexBuffer->SetBufferData(vertexCoords.size() * sizeof(GLfloat), &vertexCoords.front());

		vertexArray->Bind();
		indexBuffer = Buffer::CreateBuffer<IndexBuffer>(indices.size() * sizeof(GLuint), &indices.front());
		skyIndicesSize = indices.size();
		indices.clear();
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

		TextureManager::AddTexture("Sun", "../Assets/Textures/sun.png");
		TextureManager::AddTexture("Moon", "../Assets/Textures/moon_phases.png");
		sunMoonShaders[ShaderType::VERTEX]->GetUniform(sunMoonShaderProgram);
		glUniform1i(glGetUniformLocation(sunMoonShaderProgram, "sunMoonTexture"), 0);

	}

	//SunMoonVertex
	{
		std::vector<VertTexCoord> sunVertexCoords{
			{ glm::i8vec3(-25,  25, -400),	glm::vec2(0, 1) },
			{ glm::i8vec3(25,  25, -400),	glm::vec2(1, 1) },
			{ glm::i8vec3(25, -25, -400),	glm::vec2(1, 0) },
			{ glm::i8vec3(-25, -25, -400),	glm::vec2(0, 0) }
		};

		//TO DO change random Moon texCoord
		std::vector<VertTexCoord> MoonVertexCoords{
			{ glm::i8vec3(-15,  15, -400),	glm::vec2(0, 1) },
			{ glm::i8vec3(15,  15, -400),	glm::vec2(.25f, 1) },
			{ glm::i8vec3(15, -15, -400),	glm::vec2(.25f, 0) },
			{ glm::i8vec3(-15, -15, -400),	glm::vec2(0, 0) }
		};

		std::vector<unsigned int> IndexCoords{
			0, 1, 2,
			2, 3, 0
		};

		//Create Sun Moon
		sunMesh = std::make_unique<Mesh>();
		sunMesh->CreateVertexBuffer();
		sunMesh->SetVertexBufferData(sunVertexCoords);
		sunMesh->BindVertexArray();
		sunMesh->SetIndexBufferVector(IndexCoords);
		sunMesh->CreateIndexBuffer();


		moonMesh = std::make_unique<Mesh>();
		moonMesh->CreateVertexBuffer();
		moonMesh->SetVertexBufferData(MoonVertexCoords);
		moonMesh->BindVertexArray();
		moonMesh->SetIndexBufferVector(IndexCoords);
		moonMesh->CreateIndexBuffer();

		sunMoonIndicesSize = IndexCoords.size();
	}
}

SkyBox::~SkyBox() = default;

void SkyBox::Update(glm::vec3& playerPos)
{
	sunMoonTransformMatrix = glm::translate(glm::mat4(1.0f), playerPos);
}

void SkyBox::Render(std::shared_ptr<Camera>& camera)
{
	//Render Sky
	{
		glDisable(GL_CULL_FACE);
		glEnable(GL_BLEND);
		glUseProgram(skyShaderProgram);

		vertexArray->Bind();
		skyBoxShaders[ShaderType::VERTEX]->Update(camera);
		glDrawElements(GL_TRIANGLES, skyIndicesSize, GL_UNSIGNED_INT, nullptr);
		vertexArray->UnBind();
	}

	glDisable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glUseProgram(sunMoonShaderProgram);

	//UpdateTime this is bad..
	sunMoonShaders[ShaderType::VERTEX]->Update(camera, sunMoonTransformMatrix);

	//Sun
	sunMesh->BindVertexArray();
	TextureManager::BindTexture("Sun");
	glDrawElements(GL_TRIANGLES, sunMoonIndicesSize, GL_UNSIGNED_INT, nullptr);
	sunMesh->UnBindVertexArray();

	//Moon
	/*moonMesh->BindVertexArray();
	TextureManager::BindTexture("Moon");
	glDrawElements(GL_TRIANGLES, sunMoonIndicesSize, GL_UNSIGNED_INT, nullptr);
	moonMesh->UnBindVertexArray();*/

	//TO DO Cloud
}
