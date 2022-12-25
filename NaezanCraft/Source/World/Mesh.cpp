#include "../pch.h"
#include "Mesh.h"

#include "../Renderer/VertexArray.h"
#include "../Renderer/Buffer.h"

Mesh::Mesh()
{
}

Mesh::~Mesh()
{
}

void Mesh::CreateVertexArray()
{
	vertexArray = std::make_unique<VertexArray>();
}

void Mesh::CreateVertexBuffer(int vertexStride, const void* vertexPointer,
	int texcoordStride, const void* texcoordPointer,
	int lightStride, const void* lightPointer,
	int AOStride, const void* AOPointer,
	unsigned int posType, unsigned int texType, unsigned int lightType, unsigned int AOType)
{
	vertexBuffer = std::make_unique<VertexBuffer>(
		static_cast<int>(vertexStride), vertexPointer,
		static_cast<int>(texcoordStride), texcoordPointer,
		static_cast<int>(lightStride), lightPointer,
		static_cast<int>(AOStride), AOPointer,
		posType, texType, lightType, AOType);
}

void Mesh::CreateVertexBuffer(int vertexStride, const void* vertexPointer, int texcoordStride, const void* texcoordPointer, unsigned int posType, unsigned int texType)
{
	vertexBuffer = std::make_unique<VertexBuffer>(
		static_cast<int>(vertexStride), vertexPointer,
		static_cast<int>(texcoordStride), texcoordPointer,
		posType, texType);
}

void Mesh::CreateIndexBuffer()
{
	indexBuffer = std::make_unique<IndexBuffer>(meshIndices.size() * sizeof(unsigned int), &meshIndices.front());
	indicesCount = meshIndices.size();
	meshIndices.clear();
	meshIndices.shrink_to_fit();
}

void Mesh::SetVertexBufferData(size_t size, const void* data)
{
	vertexBuffer->SetBufferData(size, data);
}

void Mesh::SetIndexBufferVector(std::vector<unsigned int>& indexData)
{
	meshIndices.clear();
	meshIndices.shrink_to_fit();
	meshIndices.assign(indexData.begin(), indexData.end());
}

void Mesh::BindVertexBuffer()
{
	vertexBuffer->Bind();
}

void Mesh::UnBindVertexBuffer()
{
	vertexBuffer->UnBind();
}

void Mesh::BindVertexArray()
{
	vertexArray->Bind();
}

void Mesh::UnBindVertexArray()
{
	vertexArray->UnBind();
}

void Mesh::DeleteMesh()
{
	if (indexBuffer != nullptr)
		indexBuffer->DeleteBuffer();
	if (vertexBuffer != nullptr)
		vertexBuffer->DeleteBuffer();
	if (vertexArray != nullptr)
		vertexArray->DeleteBuffer();

	indexBuffer.reset();
	vertexBuffer.reset();
	vertexArray.reset();
	indicesCount = 0;
}
