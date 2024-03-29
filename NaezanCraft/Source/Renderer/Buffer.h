#pragma once

class Buffer
{
public:
	Buffer();
	virtual ~Buffer();

	virtual void Bind() = 0;
	virtual void UnBind() = 0;
	virtual void DeleteBuffer() = 0;

protected:
	uint32_t bufferID = 0;
};
class VertexBuffer : public Buffer
{
public:
	//This is for SkyBox
	VertexBuffer(int vertexStride, const void* vertexPointer, unsigned int posType, int size = 4);

	VertexBuffer(int vertexStride, const void* vertexPointer,
		int texcoordStride, const void* texcoordPointer,
		unsigned int posType, unsigned int texType, int posSize = 3, int texSize = 2);

	VertexBuffer(int vertexStride, const void* vertexPointer,
		int texcoordStride, const void* texcoordPointer,
		int lightStride, const void* lightPointer,
		int AOStride, const void* AOPointer,
		unsigned int posType, unsigned int texType, unsigned int lightType, unsigned int AOType,
		int posSize = 3, int texSize = 2, int lightSize = 1, int AOSize = 1);

	~VertexBuffer();

	virtual void Bind() override;
	virtual void UnBind() override;
	virtual void DeleteBuffer() override;

	void SetBufferData(size_t size, const void* data);
};

class IndexBuffer : public Buffer
{
public:
	IndexBuffer(size_t size, const void* data);
	~IndexBuffer();

	virtual void Bind() override;
	virtual void UnBind() override;
	virtual void DeleteBuffer() override;
};