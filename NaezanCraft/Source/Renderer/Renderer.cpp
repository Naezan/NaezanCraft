#include "../pch.h"
#include "Renderer.h"
#include "../World/Camera.h"
#include "../World/Chunk.h"
#include "../World/ChunkMesh.h"

#include "../TextureManager.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

Renderer::Renderer()
{
	//�ؽ��� Atlas
	TextureManager::AddTexture("CubeAtlas", "../Assets/Textures/Atlas.png");

	//���̴�
	renderShaders.emplace(ShaderType::VERTEX, Shader::CreateShader<ShaderType::VERTEX>("../Assets/Shaders/CubeVert.vs"));
	renderShaders.emplace(ShaderType::FRAGMENT, Shader::CreateShader<ShaderType::FRAGMENT>("../Assets/Shaders/CubeFrag.fs"));

	shaderProgram = glCreateProgram();

	for (auto& shader : renderShaders)
	{
		glAttachShader(shaderProgram, shader.second->GetShaderID());
	}

	glLinkProgram(shaderProgram);

	for (auto& shader : renderShaders)
	{
		shader.second->LinkComplete(shaderProgram);
	}
}

void Renderer::BeginRender(const glm::mat4& matrix)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	ViewProjectionMatrix = matrix;
}

void Renderer::RenderChunk(std::weak_ptr<Chunk> chunk)
{
	glDisable(GL_BLEND);
	glDisable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glCullFace(GL_FRONT);
	glFrontFace(GL_CCW);
	glUseProgram(shaderProgram);

	TextureManager::BindTexture("CubeAtlas");
	glUniform1i(glGetUniformLocation(shaderProgram, "cubeTexture"), 0);

	glUniform4f(glGetUniformLocation(shaderProgram, "ambientLight"), 0.4f, 0.4f, 0.4f, 1.0f);
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projectionview"), 1, GL_FALSE, glm::value_ptr(ViewProjectionMatrix));
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(chunk.lock()->position.x * (CHUNK_X), chunk.lock()->position.y * (CHUNK_Y), chunk.lock()->position.z * (CHUNK_Z)));
	model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
	//model = glm::rotate(model, glm::radians(0.f), glm::vec3(0.0f, 0.0f, 0.0f));
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

	chunk.lock()->chunkMesh->BindVertexArray();
	glDrawElements(GL_TRIANGLES, chunk.lock()->chunkMesh->GetIndicesCount(), GL_UNSIGNED_INT, 0);
	chunk.lock()->chunkMesh->UnBindVertexArray();
}

void Renderer::Shutdown()
{
	glDeleteProgram(shaderProgram);
}
