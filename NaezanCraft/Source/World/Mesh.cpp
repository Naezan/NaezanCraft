#include "../pch.h"
#include "Mesh.h"

#include "../Renderer/VertexArray.h"
#include "../Renderer/Buffer.h"

Mesh::Mesh(bool isCreateArrayBuffer) : isReloadMesh(isCreateArrayBuffer)
{
	if (isCreateArrayBuffer)
	{
		vertexArray = std::make_unique<VertexArray>();
	}

	indicesCount = 0;
}

Mesh::~Mesh()
{
	vertexBuffer.reset();
	indexBuffer.reset();
	vertexArray.reset();
	indicesCount = 0;
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
}

void Mesh::SetVertexBufferData(size_t size, const void* data)
{
	vertexBuffer->SetBufferData(size, data);
}

void Mesh::SetIndexBufferVector(std::vector<unsigned int>& indexData)
{
	meshIndices.clear();
	meshIndices.assign(indexData.begin(), indexData.end());
}

void Mesh::BindVertexArray()
{
	vertexArray->Bind();
}

void Mesh::UnBindVertexArray()
{
	vertexArray->UnBind();
}

void Mesh::BindAll()
{
	if (vertexArray != nullptr && vertexBuffer != nullptr && indexBuffer != nullptr)
	{
		vertexArray->Bind();
		vertexBuffer->Bind();
		indexBuffer->Bind();
	}
}

void Mesh::UnbindAll()
{
	if (vertexArray != nullptr && vertexBuffer != nullptr && indexBuffer != nullptr)
	{
		indexBuffer->UnBind();
		vertexBuffer->UnBind();
		vertexArray->UnBind();
	}
}
