#pragma once

#include <glad/glad.h>

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
	VertexBuffer(uint32_t size, const void* data);
	~VertexBuffer() = default;

	virtual inline void Bind() override { glBindBuffer(GL_ARRAY_BUFFER, bufferID); }
	virtual inline void UnBind() override { glBindBuffer(GL_ARRAY_BUFFER, 0); }

private:

};

class IndexBuffer : public Buffer
{
public:
	IndexBuffer(uint32_t size, const void* data);
	~IndexBuffer() = default;

	virtual inline void Bind() override { glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferID); }
	virtual inline void UnBind() override { glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); }

private:

};