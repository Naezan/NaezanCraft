#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class VertexArray;
class VertexBuffer;
class IndexBuffer;

class Mesh
{
public:
	Mesh();
	virtual ~Mesh();

	inline size_t GetIndicesCount() { return indicesCount; }

	void CreateVertexArray();
	void CreateVertexBuffer(int vertexStride, const void* vertexPointer,
		int texcoordStride, const void* texcoordPointer,
		int lightStride, const void* lightPointer,
		int AOStride, const void* AOPointer,
		unsigned int posType, unsigned int texType, unsigned int lightType, unsigned int AOType);
	void CreateVertexBuffer(int vertexStride, const void* vertexPointer,
		int texcoordStride, const void* texcoordPointer,
		unsigned int posType, unsigned int texType);
	virtual void CreateIndexBuffer();
	virtual void SetVertexBufferData(size_t size, const void* data);
	virtual void SetIndexBufferVector(std::vector<unsigned int>& indexData);
	void BindVertexBuffer();
	void UnBindVertexBuffer();
	void BindVertexArray();
	void UnBindVertexArray();

	void DeleteMesh();

protected:
	std::unique_ptr<VertexArray> vertexArray;
	std::unique_ptr<VertexBuffer> vertexBuffer;
	std::unique_ptr<IndexBuffer> indexBuffer;

	std::vector<unsigned int> meshIndices;
	std::vector<unsigned int> rebuildIndices;
	unsigned int lastIndex;
	int indicesCount = 0;
};