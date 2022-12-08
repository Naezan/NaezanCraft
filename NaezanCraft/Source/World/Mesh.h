#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class VertexArray;
class VertexBuffer;
class IndexBuffer;

struct VertTexCoord
{
	glm::u8vec3 pos;
	glm::vec2 texcoord;
};

class Mesh
{
public:
	Mesh();
	virtual ~Mesh();

	inline size_t GetIndicesCount() { return indicesCount; }

	virtual void CreateVertexBuffer();
	virtual void CreateIndexBuffer();
	void BindVertexArray();
	void UnBindVertexArray();

protected:
	std::shared_ptr<VertexArray> vertexArray;
	std::shared_ptr<VertexBuffer> vertexBuffer;
	std::shared_ptr<IndexBuffer> indexBuffer;

	std::vector<VertTexCoord> meshVertices;
	std::vector<unsigned int> meshIndices;
	int indicesCount;
};