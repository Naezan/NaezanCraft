#pragma once

#include "Shader.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Buffer;
class VertexBuffer;
class IndexBuffer;
class VertexArray;
class Camera;

class Renderer
{
public:
	Renderer();
	~Renderer() = default;

	void BeginRender(std::unique_ptr<Camera>& camera);
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

	glm::mat4 ViewProjectionMatrix;
};