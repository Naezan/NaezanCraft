#include "../pch.h"
#include "Buffer.h"

#include <glad/glad.h>

Buffer::Buffer()
{
	glGenBuffers(1, &bufferID);
}

Buffer::~Buffer()
{
	glDeleteBuffers(1, &bufferID);
}

VertexBuffer::VertexBuffer(int vertexStride, const void* vertexPointer, int texcoordStride, const void* texcoordPointer)
{
	glBindBuffer(GL_ARRAY_BUFFER, bufferID);

	//index, size(vec3 ����), type(�� �ϳ��� Ÿ��, ũ��), normalized(����ȭ ����), stride(�ǳʶ� �Ÿ�), pointer(����� ���� ��ġ, �Ÿ�?)
	//position(u8vec3)
	glVertexAttribPointer(0, 3, GL_UNSIGNED_BYTE, GL_FALSE, vertexStride, vertexPointer);
	glEnableVertexAttribArray(0);

	//texture(u8vec2)
	glVertexAttribPointer(1, 2, GL_UNSIGNED_BYTE, GL_FALSE, texcoordStride, texcoordPointer);
	glEnableVertexAttribArray(1);
}

VertexBuffer::~VertexBuffer()
{
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
}

void VertexBuffer::Bind()
{
	glBindBuffer(GL_ARRAY_BUFFER, bufferID);
}

void VertexBuffer::UnBind()
{
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void VertexBuffer::SetBufferData(size_t size, const void* data)
{
	glBindBuffer(GL_ARRAY_BUFFER, bufferID);
	glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
}

IndexBuffer::IndexBuffer(size_t size, const void* data)
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
}

void IndexBuffer::Bind()
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferID);
}

void IndexBuffer::UnBind()
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}