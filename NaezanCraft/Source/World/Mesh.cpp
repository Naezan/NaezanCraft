#include "../pch.h"
#include "Mesh.h"

#include "../Renderer/VertexArray.h"
#include "../Renderer/Buffer.h"

Mesh::Mesh()
{
	vertexArray = std::make_unique<VertexArray>();
	vertexArray->Bind();

	indicesCount = 0;
}

Mesh::~Mesh()
{
	vertexArray.reset();
	vertexBuffer.reset();
	indexBuffer.reset();
}

void Mesh::CreateVertexBuffer(int vertexStride, const void* vertexPointer, int texcoordStride, const void* texcoordPointer, unsigned int posType, unsigned int texType)
{
	vertexBuffer = std::make_unique<VertexBuffer>(
		static_cast<int>(vertexStride), vertexPointer,
		static_cast<int>(texcoordStride), texcoordPointer, posType, texType);
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

void Mesh::DeleteMesh()
{
	vertexArray->DeleteArray();
	vertexBuffer->DeleteBuffer();
}
