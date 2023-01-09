#pragma once

#include "Shader.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Camera;
class Chunk;
class Mesh;

class Renderer
{
public:
	Renderer();
	~Renderer() = default;

	void CreateRenderShader();
	void BeginRender(const glm::mat4& matrix);
	void RenderChunk(std::weak_ptr<Chunk> chunk);
	void RenderWater(std::weak_ptr<Chunk> chunk, const glm::vec2& animOffset);
	void Shutdown();

	inline uint32_t GetRenderShader() { return shaderProgram; }

private:
	std::map<ShaderType, std::unique_ptr<Shader>> renderShaders;

	uint32_t shaderProgram;

	glm::mat4 ViewProjectionMatrix;
};