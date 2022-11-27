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

	inline float& GetYaw() { return yaw; }
	inline float& GetPitch() { return pitch; }

private:
	std::vector<std::unique_ptr<Shader>> renderShaders;
	std::shared_ptr<VertexArray> vertexArray;
	std::shared_ptr<VertexBuffer> vertexBuffer;
	std::shared_ptr<IndexBuffer> indexBuffer;

	float yaw, pitch;

	uint32_t shaderProgram;
};