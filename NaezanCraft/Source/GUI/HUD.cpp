#include "../pch.h"
#include "HUD.h"
#include "Inventory.h"
#include "../../World/Mesh.h"
#include "../TextureManager.h"

HUD::HUD()
{
	rectShaders.emplace(ShaderType::VERTEX, std::make_unique<Shader>("../Assets/Shaders/QuadVert.vs", ShaderType::VERTEX));
	rectShaders.emplace(ShaderType::FRAGMENT, std::make_unique<Shader>("../Assets/Shaders/QuadFrag.fs", ShaderType::FRAGMENT));

	shaderProgram = glCreateProgram();
	for (auto& shader : rectShaders)
	{
		glAttachShader(shaderProgram, shader.second->GetShaderID());
	}
	glLinkProgram(shaderProgram);
	for (auto& shader : rectShaders)
	{
		shader.second->LinkComplete(shaderProgram);
	}
	glUniform1i(glGetUniformLocation(shaderProgram, "quadTexture"), 0);
	
	//Vertex
	struct QuadCoord
	{
		glm::vec3 pos;
		glm::vec2 texcoord;
	};

	std::vector<QuadCoord> QuadCoords
	{
		{ glm::vec3(SCREEN_WIDTH / 2.f - CROSSHAIR_HALFSIZE, SCREEN_HEIGHT / 2.f + CROSSHAIR_HALFSIZE, 1.f),	glm::vec2(0, 1) },
		{ glm::vec3(SCREEN_WIDTH / 2.f + CROSSHAIR_HALFSIZE, SCREEN_HEIGHT / 2.f + CROSSHAIR_HALFSIZE, 1.f),	glm::vec2(1, 1) },
		{ glm::vec3(SCREEN_WIDTH / 2.f + CROSSHAIR_HALFSIZE, SCREEN_HEIGHT / 2.f - CROSSHAIR_HALFSIZE, 1.f),	glm::vec2(1, 0) },
		{ glm::vec3(SCREEN_WIDTH / 2.f - CROSSHAIR_HALFSIZE, SCREEN_HEIGHT / 2.f - CROSSHAIR_HALFSIZE, 1.f),	glm::vec2(0, 0) }
	};

	std::vector<unsigned int> IndexCoords
	{
		0, 1, 2,
		2, 3, 0
	};

	//CrosshairMesh
	{
		crosshairMesh = std::make_unique<Mesh>();
		crosshairMesh->CreateVertexArray();

		crosshairMesh->SetIndexBufferVector(IndexCoords);
		crosshairMesh->CreateIndexBuffer();

		crosshairMesh->CreateVertexBuffer(static_cast<int>(sizeof(QuadCoord)), (void*)offsetof(QuadCoord, pos),
			static_cast<int>(sizeof(QuadCoord)), (void*)offsetof(QuadCoord, texcoord),
			GL_FLOAT, GL_FLOAT, 3, 2);
		crosshairMesh->SetVertexBufferData(QuadCoords.size() * sizeof(QuadCoord), &QuadCoords.front());

		crosshairMesh->UnBindVertexBuffer();
		crosshairMesh->UnBindVertexArray();
	}

	inventory = std::make_unique<Inventory>(shaderProgram);
	inventory->SetOwner(this);
}

HUD::~HUD()
{
	crosshairMesh->DeleteMesh();

	glDeleteProgram(shaderProgram);

	inventory.reset();
}

void HUD::Update()
{
	inventory->Update();
}

void HUD::Render()
{
	glEnable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);

	glUseProgram(shaderProgram);
	glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(SCREEN_WIDTH),
		static_cast<float>(SCREEN_HEIGHT), 0.0f);
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
	crosshairModel = glm::mat4(1.0f);
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(crosshairModel));

	TextureManager::BindTexture("Crosshair");

	crosshairMesh->BindVertexArray();
	glDrawElements(GL_TRIANGLES, crosshairMesh->GetIndicesCount(), GL_UNSIGNED_INT, nullptr);
	crosshairMesh->UnBindVertexArray();

	inventory->Render();
}
