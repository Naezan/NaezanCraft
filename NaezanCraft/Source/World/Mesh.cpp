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

Mesh::~Mesh()
{
	meshVertices.clear();
}

void Mesh::CreateVertexBuffer()
{
	vertexBuffer = Buffer::CreateBuffer<VertexBuffer>(
		static_cast<int>(sizeof(VertTexCoord)), (void*)offsetof(VertTexCoord, pos),
		static_cast<int>(sizeof(VertTexCoord)), (void*)offsetof(VertTexCoord, texcoord));
}

void Mesh::CreateIndexBuffer()
{
	indexBuffer = Buffer::CreateBuffer<IndexBuffer>(meshIndices.size() * sizeof(unsigned int), &meshIndices.front());
	indicesCount = meshIndices.size();
	meshIndices.clear();
}

void Mesh::BindVertexArray()
{
	vertexArray->Bind();
}

void Mesh::UnBindVertexArray()
{
	vertexArray->UnBind();
}