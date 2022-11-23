#include "../pch.h"
#include "Buffer.h"

Buffer::Buffer()
{
	glGenBuffers(1, &bufferID);
}

Buffer::~Buffer()
{
	glDeleteBuffers(1, &bufferID);
}

VertexBuffer::VertexBuffer(uint32_t size, const void* data)
{
	glBindBuffer(GL_ARRAY_BUFFER, bufferID);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * size, data, GL_STATIC_DRAW);
}

IndexBuffer::IndexBuffer(uint32_t size, const void* data)
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(uint32_t) * size, data, GL_STATIC_DRAW);
}