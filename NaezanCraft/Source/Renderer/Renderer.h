#pragma once

#include "Shader.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <map>

class Buffer;
class VertexBuffer;
class IndexBuffer;
class VertexArray;
class Texture;
class Camera;
class Chunk;

class Renderer
{
public:
	Renderer();
	~Renderer() = default;

	void BeginRender(const glm::mat4& matrix);
	void Render();
	void Shutdown();

public:
	//TO DO temp
	static std::vector<glm::vec3> cubePositions;

private:
	std::map<ShaderType, std::unique_ptr<Shader>> renderShaders;
	std::shared_ptr<VertexArray> vertexArray;
	std::shared_ptr<VertexBuffer> vertexBuffer;
	std::shared_ptr<IndexBuffer> indexBuffer;
	std::shared_ptr<Texture> texture;

	uint32_t shaderProgram;
	GLuint textureID;

	glm::mat4 ViewProjectionMatrix;

	//TO DO temp
	std::vector<std::shared_ptr<Chunk>> renderChunks;
};