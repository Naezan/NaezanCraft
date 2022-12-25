#include "../../pch.h"
#include "../Mesh.h"
#include "Cloud.h"
#include "../../Renderer/Environment/CloudShader.h"
#include "../../TextureManager.h"

#include <glad/glad.h>

Cloud::Cloud()
{
	//Shader
	{
		cloudShaders.emplace(ShaderType::VERTEX, std::make_unique<CloudShader>("../Assets/Shaders/Cloud.vs", ShaderType::VERTEX));
		cloudShaders.emplace(ShaderType::FRAGMENT, std::make_unique<CloudShader>("../Assets/Shaders/Cloud.fs", ShaderType::FRAGMENT));

		cloudShaderProgram = glCreateProgram();
		for (auto& shader : cloudShaders)
		{
			glAttachShader(cloudShaderProgram, shader.second->GetShaderID());
		}
		glLinkProgram(cloudShaderProgram);
		for (auto& shader : cloudShaders)
		{
			shader.second->LinkComplete(cloudShaderProgram);
		}
		cloudShaders[ShaderType::VERTEX]->GetUniform(cloudShaderProgram);
	}

	//Mesh
	{
		//cloudMesh = std::make_unique<Mesh>();
		//cloudMesh->CreateVertexBuffer(static_cast<int>(sizeof(SunMoonVertexCoord)), (void*)offsetof(SunMoonVertexCoord, pos),
		//	static_cast<int>(sizeof(SunMoonVertexCoord)), (void*)offsetof(SunMoonVertexCoord, texcoord),
		//	GL_FLOAT, GL_FLOAT);
		//cloudMesh->SetVertexBufferData(sunVertexCoords.size() * sizeof(SunMoonVertexCoord), &sunVertexCoords.front());
		//cloudMesh->BindVertexArray();
		//cloudMesh->SetIndexBufferVector(IndexCoords);
		//cloudMesh->CreateIndexBuffer();

		//cloudIndicesSize = IndexCoords.size();
	}
}

Cloud::~Cloud()
{
	glDeleteProgram(cloudShaderProgram);
}

void Cloud::Update()
{
	glUseProgram(cloudShaderProgram);
	//cloudShaders[ShaderType::VERTEX]->Update(camera, TransformMatrix);
	glUniform1i(glGetUniformLocation(cloudShaderProgram, "cloudTexture"), 0);
}

void Cloud::Render()
{
	glDisable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glUseProgram(cloudShaderProgram);

	TextureManager::BindTexture("Cloud");

	cloudMesh->BindVertexArray();
	glDrawElements(GL_TRIANGLES, cloudIndicesSize, GL_UNSIGNED_INT, nullptr);
	cloudMesh->UnBindVertexArray();
}
