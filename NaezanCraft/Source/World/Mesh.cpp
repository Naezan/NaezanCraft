#include "../pch.h"
#include "Mesh.h"

#include "../Renderer/VertexArray.h"
#include "../Renderer/Buffer.h"

Mesh::Mesh()
{
	vertexArray = VertexArray::CreateArray();
	vertexArray->Bind();

	indicesCount = 0;
}

Mesh::~Mesh() = default;

void Mesh::CreateVertexBuffer(int vertexStride, const void* vertexPointer, int texcoordStride, const void* texcoordPointer, unsigned int posType, unsigned int texType)
{
	vertexBuffer = Buffer::CreateBuffer<VertexBuffer>(
		static_cast<int>(vertexStride), vertexPointer,
		static_cast<int>(texcoordStride), texcoordPointer, posType, texType);
}

void Mesh::CreateIndexBuffer()
{
	indexBuffer = Buffer::CreateBuffer<IndexBuffer>(meshIndices.size() * sizeof(unsigned int), &meshIndices.front());
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