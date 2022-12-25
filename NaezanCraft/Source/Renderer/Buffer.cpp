#include "../pch.h"
#include "Buffer.h"

#include <glad/glad.h>

Buffer::Buffer()
{
	glGenBuffers(1, &bufferID);
}

Buffer::~Buffer() = default;


VertexBuffer::VertexBuffer(int vertexStride, const void* vertexPointer)
{
	glBindBuffer(GL_ARRAY_BUFFER, bufferID);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, vertexStride, vertexPointer);
}

VertexBuffer::VertexBuffer(
	int vertexStride, const void* vertexPointer,
	int texcoordStride, const void* texcoordPointer,
	unsigned int posType, unsigned int texType)
{
	glBindBuffer(GL_ARRAY_BUFFER, bufferID);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, posType, GL_FALSE, vertexStride, vertexPointer);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, texType, GL_FALSE, texcoordStride, texcoordPointer);
}

VertexBuffer::VertexBuffer(
	int vertexStride, const void* vertexPointer,
	int texcoordStride, const void* texcoordPointer,
	int lightStride, const void* lightPointer,
	int AOStride, const void* AOPointer,
	unsigned int posType, unsigned int texType, unsigned int lightType, unsigned int AOType)
{
	glBindBuffer(GL_ARRAY_BUFFER, bufferID);

	//index, size(vec3 개수), type(점 하나의 타입, 크기), normalized(정규화 여부), stride(건너뛸 거리), pointer(상대적 시작 위치, 거리?)
	//position(u8vec3)
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, posType, GL_FALSE, vertexStride, vertexPointer);

	//texture(16vec2)
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, texType, GL_FALSE, texcoordStride, texcoordPointer);

	//texture(uint8_t)
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 1, lightType, GL_FALSE, lightStride, lightPointer);

	//AO(uint8_t)
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 1, AOType, GL_FALSE, AOStride, AOPointer);
}

VertexBuffer::~VertexBuffer() = default;

void VertexBuffer::Bind()
{
	glBindBuffer(GL_ARRAY_BUFFER, bufferID);
}

void VertexBuffer::UnBind()
{
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void VertexBuffer::DeleteBuffer()
{
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
	glDisableVertexAttribArray(3);
	if (bufferID)
		glDeleteBuffers(1, &bufferID);
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

IndexBuffer::~IndexBuffer() = default;

void IndexBuffer::Bind()
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferID);
}

void IndexBuffer::UnBind()
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void IndexBuffer::DeleteBuffer()
{
	if (bufferID)
		glDeleteBuffers(1, &bufferID);
}