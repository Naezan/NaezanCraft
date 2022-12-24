#pragma once

class VertexBuffer
{
public:
	//This is for SkyBox
	VertexBuffer(int vertexStride, const void* vertexPointer);
	VertexBuffer(int vertexStride, const void* vertexPointer,
		int texcoordStride, const void* texcoordPointer,
		unsigned int posType, unsigned int texType);
	VertexBuffer(int vertexStride, const void* vertexPointer,
		int texcoordStride, const void* texcoordPointer,
		int lightStride, const void* lightPointer,
		int AOStride, const void* AOPointer,
		unsigned int posType, unsigned int texType, unsigned int lightType, unsigned int AOType);
	~VertexBuffer();

	void Bind();
	void UnBind();
	void DeleteBuffer();

	void SetBufferData(size_t size, const void* data);

private:
	uint32_t bufferID = 0;
};

class IndexBuffer
{
public:
	IndexBuffer(size_t size, const void* data);
	~IndexBuffer();

	void Bind();
	void UnBind();
	void DeleteBuffer();

private:
	uint32_t bufferID = 0;
};