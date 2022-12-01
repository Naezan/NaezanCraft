#include "../pch.h"
#include "Renderer.h"
#include "Buffer.h"
#include "VertexArray.h"
#include "../World/Camera.h"
#include "Texture.h"

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

	//float vertices[] = {
	//-0.5f,  0.5f, -0.5f, 0.0f, 0.0f,//Top ㄱ
	// 0.5f,  0.5f, -0.5f, 1.0f, 0.0f,
	// 0.5f,  0.5f,  0.5f, 1.0f, 0.0f,
	// 0.5f,  0.5f,  0.5f, 1.0f, 1.0f,//Top ㄴ
	//-0.5f,  0.5f,  0.5f, 0.0f, 1.0f,
	//-0.5f,  0.5f, -0.5f, 0.0f, 0.0f,

	//-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,//Bottom
	// 0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
	// 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
	// 0.5f, -0.5f,  0.5f,  1.0f, 1.0f,
	//-0.5f, -0.5f,  0.5f,  0.0f, 1.0f,
	//-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

	//-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,//Front
	// 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
	// 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	// 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
	//-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
	//-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

	//-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,//Back
	// 0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
	// 0.5f,  0.5f, -0.5f,  1.0f, 0.0f,
	// 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	//-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
	//-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

	// 0.5f,  0.5f,  0.5f,  0.0f, 0.0f,//Right
	// 0.5f,  0.5f, -0.5f,  1.0f, 0.0f,
	// 0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
	// 0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
	// 0.5f, -0.5f,  0.5f,  0.0f, 1.0f,
	// 0.5f,  0.5f,  0.5f,  0.0f, 0.0f,

	//-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,//Left
	//-0.5f,  0.5f, -0.5f,  1.0f, 0.0f,
	//-0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
	//-0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
	//-0.5f, -0.5f,  0.5f,  0.0f, 1.0f,
	//-0.5f,  0.5f,  0.5f,  0.0f, 0.0f
	//};

	unsigned int indices[] = {
		0, 1, 3, // first triangle
		1, 2, 3  // second triangle
	};

	//버텍스 버퍼
	vertexBuffer = Buffer::CreateBuffer<VertexBuffer>(sizeof(vertices), vertices);

	//인덱스 버퍼
	//indexBuffer = Buffer::CreateBuffer<IndexBuffer>();

	//텍스쳐
	texture = Texture::CreateTexture("../Assets/Textures/diamond_block.png");
	glUniform1i(glGetUniformLocation(shaderProgram, "cubeTexture"), 0);
}

glm::vec3 cubePositions[] = {
	glm::vec3(0.0f,  0.0f,  0.0f),
	glm::vec3(2.0f,  5.0f, -15.0f),
	glm::vec3(-1.5f, -2.2f, -2.5f),
	glm::vec3(-3.8f, -2.0f, -12.3f),
	glm::vec3(2.4f, -0.4f, -3.5f),
	glm::vec3(-1.7f,  3.0f, -7.5f),
	glm::vec3(1.3f, -2.0f, -2.5f),
	glm::vec3(1.5f,  2.0f, -2.5f),
	glm::vec3(1.5f,  0.2f, -1.5f),
	glm::vec3(-1.3f,  1.0f, -1.5f)
};

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

	for (int i = 0; i < 10; ++i)
	{
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, cubePositions[i]);
		float angle = 20.0f * i;
		model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
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
