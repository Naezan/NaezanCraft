#include "../pch.h"
#include "Renderer.h"
#include "Shader.h"
#include "Buffer.h"

Renderer::Renderer()
{
	renderShaders.push_back(Shader::CreateShader<ShaderType::VERTEX>("path/to/shaders/shader.vs"));
	renderShaders.push_back(Shader::CreateShader<ShaderType::FRAGMENT>("path/to/shaders/shader.vs"));

	float vertices[] = {
		// positions         // colors
		 0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,   // bottom right
		-0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,   // bottom left
		 0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f    // top 
	};
	Buffer::CreateBuffer<VertexBuffer>(18, vertices);

	shaderProgram = glCreateProgram();

	for (auto& shader : renderShaders)
	{
		glAttachShader(shaderProgram, shader->GetShaderID());
	}

	glLinkProgram(shaderProgram);

	for (auto& shader : renderShaders)
	{
		shader->LinkComplete();
	}
}

Renderer::~Renderer()
{
}

void Renderer::Render()
{
	glUseProgram(shaderProgram);
}

void Renderer::Shutdown()
{
	glDeleteProgram(shaderProgram);
}
