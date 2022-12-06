#include "../pch.h"
#include "Renderer.h"
#include "Texture.h"
#include "../World/Camera.h"
#include "../World/Chunk.h"
#include "../World/Mesh.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

Renderer::Renderer()
{
	//쉐이더
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

	//TO DO change Vertex Info
	//텍스쳐
	texture = Texture::CreateTexture("../Assets/Textures/diamond_block.png");
	glUniform1i(glGetUniformLocation(shaderProgram, "cubeTexture"), 0);
}

void Renderer::BeginRender(const glm::mat4& matrix)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	ViewProjectionMatrix = matrix;
}

void Renderer::Render(std::shared_ptr<Chunk>& chunk)
{
	glUseProgram(shaderProgram);

	//TO DO RenderChunkBegin && RenderChunk
	uint32_t viewProjectionLoc = glGetUniformLocation(shaderProgram, "projectionview");
	glUniformMatrix4fv(viewProjectionLoc, 1, GL_FALSE, glm::value_ptr(ViewProjectionMatrix));

	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(chunk->position.x * (CHUNK_X - 1), chunk->position.y * (CHUNK_Y - 1), chunk->position.z * (CHUNK_Z - 1)));
	model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
	//model = glm::rotate(model, glm::radians(0.f), glm::vec3(0.0f, 0.0f, 0.0f));
	uint32_t modelLoc = glGetUniformLocation(shaderProgram, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	chunk->chunkMesh->BindVertexArray();
	glDrawElements(GL_TRIANGLES, chunk->chunkMesh->GetIndicesCount(), GL_UNSIGNED_INT, 0);
	chunk->chunkMesh->UnBindVertexArray();
}

void Renderer::Shutdown()
{
	glDeleteProgram(shaderProgram);
}
