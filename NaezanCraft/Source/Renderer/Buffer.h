#pragma once

class Buffer
{
public:
	Buffer();
	virtual ~Buffer();

	virtual void Bind() = 0;
	virtual void UnBind() = 0;

protected:
	uint32_t bufferID = 0;
};

class VertexBuffer : public Buffer
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

	virtual void Bind() override;
	virtual void UnBind() override;

	void SetBufferData(size_t size, const void* data);

private:

};

class IndexBuffer : public Buffer
{
public:
	IndexBuffer(size_t size, const void* data);
	~IndexBuffer();

	virtual void Bind() override;
	virtual void UnBind() override;

private:

};