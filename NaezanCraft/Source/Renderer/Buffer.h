#pragma once

class Buffer
{
public:
	Buffer();
	virtual ~Buffer();

	virtual void Bind() = 0;
	virtual void UnBind() = 0;

	template<typename T,typename... Args>
	inline static std::shared_ptr<T> CreateBuffer(Args... args) {return std::make_shared<T>(args...); }

protected:
	uint32_t bufferID;
};

class VertexBuffer : public Buffer
{
public:
	VertexBuffer(int vertexStride, const void* vertexPointer);
	VertexBuffer(int vertexStride, const void* vertexPointer, int texcoordStride, const void* texcoordPointer);
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
	~IndexBuffer() = default;

	virtual void Bind() override;
	virtual void UnBind() override;

private:

};