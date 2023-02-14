#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class VertexArray;
class VertexBuffer;
class IndexBuffer;

//struct upos3 {
//	uint8_t x : 4;
//	uint8_t z : 4;
//	uint8_t y : 8;
//};

struct VertTexCoord
{
	glm::i8vec3 pos;
	glm::u8vec2 texcoord;
	uint8_t lightlevel;
	uint8_t AO;
};

class Mesh
{
public:
	Mesh();
	virtual ~Mesh();

	inline size_t GetIndicesCount() { return indicesCount; }
	inline void SetIndicesCount(int count) { indicesCount = count; }

	void CreateVertexArray();

	void CreateVertexBuffer(int vertexStride, const void* vertexPointer,
		int texcoordStride, const void* texcoordPointer,
		int lightStride, const void* lightPointer,
		int AOStride, const void* AOPointer,
		unsigned int posType, unsigned int texType, unsigned int lightType, unsigned int AOType,
		int posSize = 3, int texSize = 2, int lightSize = 1, int AOSize = 1);

	void CreateVertexBuffer(int vertexStride, const void* vertexPointer,
		int texcoordStride, const void* texcoordPointer,
		unsigned int posType, unsigned int texType, int posSize = 3, int texSize = 2);

	void CreateVertexBuffer(int vertexStride, const void* vertexPointer,
		unsigned int posType = GL_FLOAT, int size = 4);

	virtual void CreateIndexBuffer();
	virtual void SetVertexBufferData(size_t size, const void* data);
	virtual void SetIndexBufferVector(std::vector<unsigned int>& indexData);
	void BindVertexBuffer();
	void UnBindVertexBuffer();
	void BindVertexArray();
	void UnBindVertexArray();

	void DeleteMesh();

protected:
	static const std::array<glm::i8vec3, 2> indices;

	std::unique_ptr<VertexArray> vertexArray;
	std::unique_ptr<VertexBuffer> vertexBuffer;
	std::unique_ptr<IndexBuffer> indexBuffer;

	std::vector<unsigned int> meshIndices;
	std::vector<unsigned int> rebuildIndices;
	unsigned int lastIndex;
	int indicesCount = 0;
};