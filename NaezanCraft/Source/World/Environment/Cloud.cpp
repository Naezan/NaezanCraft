#include "../../pch.h"
#include "../Mesh.h"
#include "Cloud.h"
#include "../../Renderer/Environment/CloudShader.h"
#include "../../TextureManager.h"
#include "../Scene.h"
#include "../Camera.h"

Cloud::Cloud(glm::vec3 _position) : renderPosition(_position), updatePosition(_position), isStartRender(false), deltaPos(0.f)
{
	//Shader
	{
		cloudShaders.emplace(ShaderType::VERTEX, std::make_unique<CloudShader>("../Assets/Shaders/CloudVert.vs", ShaderType::VERTEX));
		cloudShaders.emplace(ShaderType::FRAGMENT, std::make_unique<CloudShader>("../Assets/Shaders/CloudFrag.fs", ShaderType::FRAGMENT));

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
		//cloudShaders[ShaderType::VERTEX]->GetUniform(cloudShaderProgram);
		glUniform1i(glGetUniformLocation(cloudShaderProgram, "cloudTexture"), 0);
	}

	struct CloudVertexCoord
	{
		glm::vec3 pos;
		glm::vec2 texcoord;
	};

	std::vector<CloudVertexCoord> cloudVertexCoords
	{
		{ glm::vec3(-2048,  256, 2048),	glm::vec2(0, 1) },
		{ glm::vec3(2048,  256, 2048),	glm::vec2(1, 1) },
		{ glm::vec3(2048, 256, -2048),	glm::vec2(1, 0) },
		{ glm::vec3(-2048, 256, -2048),	glm::vec2(0, 0) }
	};

	std::vector<unsigned int> IndexCoords
	{
		0, 1, 2,
		2, 3, 0
	};

	//Mesh
	{
		cloudMesh = std::make_unique<Mesh>();
		cloudMesh->CreateVertexArray();

		cloudMesh->SetIndexBufferVector(IndexCoords);
		cloudMesh->CreateIndexBuffer();

		cloudMesh->CreateVertexBuffer(static_cast<int>(sizeof(CloudVertexCoord)), (void*)offsetof(CloudVertexCoord, pos),
			static_cast<int>(sizeof(CloudVertexCoord)), (void*)offsetof(CloudVertexCoord, texcoord),
			GL_FLOAT, GL_FLOAT);
		cloudMesh->SetVertexBufferData(cloudVertexCoords.size() * sizeof(CloudVertexCoord), &cloudVertexCoords.front());
		cloudMesh->SetIndicesCount(IndexCoords.size());

		cloudMesh->UnBindVertexBuffer();
		cloudMesh->UnBindVertexArray();
	}
}

Cloud::~Cloud()
{
	cloudMesh->DeleteMesh();

	glDeleteProgram(cloudShaderProgram);
}

void Cloud::Update(glm::vec3 playerPos)
{
	if (!isStartRender)
	{
		isStartRender = true;
		renderPosition = playerPos;
	}
	updatePosition = playerPos;
}

void Cloud::Render(std::weak_ptr<Camera>& camera, glm::mat4& transformMatrix)
{
	glDisable(GL_CULL_FACE);
	glEnable(GL_BLEND);

	glUseProgram(cloudShaderProgram);

	deltaPos += CLOUD_SPEED;

	if (deltaPos > 4096) {
		deltaPos = 0;
	}

	if (abs(updatePosition.x - renderPosition.x) > 4096) {
		renderPosition.x = updatePosition.x;
	}

	if (abs(updatePosition.z - renderPosition.z) > 4096) {
		renderPosition.z = updatePosition.z;
	}

	TextureManager::BindTexture("Cloud");

	glUniform1f(glGetUniformLocation(cloudShaderProgram, "lightIntensity"), sunIntensity);
	glUniformMatrix4fv(glGetUniformLocation(cloudShaderProgram, "projectionview"), 1, GL_FALSE, glm::value_ptr(camera.lock()->GetViewProjectionMatrix()));
	glm::mat4 backmodel = glm::translate(glm::mat4(1.0f), glm::vec3(renderPosition.x, 0, renderPosition.z + deltaPos - 4096));
	glUniformMatrix4fv(glGetUniformLocation(cloudShaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(backmodel));

	cloudMesh->BindVertexArray();
	glDrawElements(GL_TRIANGLES, cloudMesh->GetIndicesCount(), GL_UNSIGNED_INT, nullptr);
	cloudMesh->UnBindVertexArray();

	glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(renderPosition.x, 0, renderPosition.z + deltaPos));
	glUniformMatrix4fv(glGetUniformLocation(cloudShaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

	cloudMesh->BindVertexArray();
	glDrawElements(GL_TRIANGLES, cloudMesh->GetIndicesCount(), GL_UNSIGNED_INT, nullptr);
	cloudMesh->UnBindVertexArray();

	glm::mat4 frontmodel = glm::translate(glm::mat4(1.0f), glm::vec3(renderPosition.x, 0, renderPosition.z + deltaPos + 4096));
	glUniformMatrix4fv(glGetUniformLocation(cloudShaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(frontmodel));

	cloudMesh->BindVertexArray();
	glDrawElements(GL_TRIANGLES, cloudMesh->GetIndicesCount(), GL_UNSIGNED_INT, nullptr);
	cloudMesh->UnBindVertexArray();
}
