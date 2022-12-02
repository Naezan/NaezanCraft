#include "../pch.h"
#include "Renderer.h"
#include "Buffer.h"
#include "VertexArray.h"
#include "../World/Camera.h"
#include "Texture.h"
#include "../World/Chunk.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

std::vector<glm::vec3> Renderer::cubePositions;

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

	//버텍스 어레이
	vertexArray = VertexArray::CreateArray();

	float vertices[] = {
	 0.5f,  0.5f, -0.5f,  0.0f, 1.0f,//Top
	-0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
	 0.5f,  0.5f, -0.5f,  0.0f, 1.0f,

	 0.5f, -0.5f,  0.5f,  0.0f, 1.0f,//Bottom
	-0.5f, -0.5f,  0.5f,  1.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
	-0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
	 0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, 1.0f,

	-0.5f, -0.5f,  0.5f,  0.0f, 1.0f,//Front
	 0.5f, -0.5f,  0.5f,  1.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 1.0f,

	 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,//Back
	-0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
	-0.5f,  0.5f, -0.5f,  1.0f, 0.0f,
	-0.5f,  0.5f, -0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f, -0.5f,  0.0f, 0.0f,
	 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

	 0.5f, -0.5f,  0.5f,  0.0f, 1.0f,//Right
	 0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, 1.0f,

	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,//Left
	-0.5f, -0.5f,  0.5f,  1.0f, 1.0f,
	-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	-0.5f,  0.5f, -0.5f,  0.0f, 0.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f
	};

	unsigned int indices[] = {
		0, 1, 2,
		2, 3, 1
	};

	//버텍스 버퍼
	vertexBuffer = Buffer::CreateBuffer<VertexBuffer>(sizeof(vertices), vertices);

	//인덱스 버퍼
	indexBuffer = Buffer::CreateBuffer<IndexBuffer>(sizeof(indices), indices);

	//텍스쳐
	texture = Texture::CreateTexture("../Assets/Textures/diamond_block.png");
	glUniform1i(glGetUniformLocation(shaderProgram, "cubeTexture"), 0);

	//CreateChunk
	renderChunks.push_back(std::make_shared<Chunk>(glm::vec3(0.f, 0.f, 0.f)));
	//renderChunks[0]->CreateChunk();
}

void Renderer::BeginRender(const glm::mat4& matrix)
{
	ViewProjectionMatrix = matrix;
}

void Renderer::Render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(shaderProgram);

	//NC_LOG_DEBUG("Yaw : {0}", yaw);
	//NC_LOG_DEBUG("Pitch : {0}", pitch);

	//TO DO 월드의 플레이어 -> 카메라 -> 뷰프로젝션?
	uint32_t viewProjectionLoc = glGetUniformLocation(shaderProgram, "projectionview");
	glUniformMatrix4fv(viewProjectionLoc, 1, GL_FALSE, glm::value_ptr(ViewProjectionMatrix));

	for (int i = 0; i < cubePositions.size(); ++i)
	{
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, cubePositions[i]);
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
		//model = glm::rotate(model, glm::radians(0.f), glm::vec3(0.0f, 0.0f, 0.0f));
		uint32_t modelLoc = glGetUniformLocation(shaderProgram, "model");
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

		glDrawArrays(GL_TRIANGLES, 0, 36);
	}

	//glDrawArrays(GL_TRIANGLES, 0, 36);
	//Render VertexBuffer, IndexBuffer, FrameBuffer
}

void Renderer::Shutdown()
{
	glDeleteProgram(shaderProgram);
}
