#pragma once

#include "Shader.h"

class Buffer;
class VertexBuffer;
class IndexBuffer;
class VertexArray;

class Renderer
{
public:
	Renderer();
	~Renderer() = default;

	void Render();
	void Shutdown();

	static inline std::unique_ptr<Renderer> CreateRenderer()
	{
		return std::make_unique<Renderer>();
	}

private:
	std::vector<std::unique_ptr<Shader>> renderShaders;
	std::shared_ptr<VertexArray> vertexArray;
	std::shared_ptr<VertexBuffer> vertexBuffer;
	std::shared_ptr<IndexBuffer> indexBuffer;

	uint32_t shaderProgram;
};