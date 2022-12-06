#pragma once

#include "Shader.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Texture;
class Camera;
class Chunk;

class Renderer
{
public:
	Renderer();
	~Renderer() = default;

	void BeginRender(const glm::mat4& matrix);
	void Render(std::shared_ptr<Chunk>& chunk);
	void Shutdown();

private:
	std::map<ShaderType, std::unique_ptr<Shader>> renderShaders;
	std::shared_ptr<Texture> texture;

	uint32_t shaderProgram;
	unsigned int textureID;

	glm::mat4 ViewProjectionMatrix;
};