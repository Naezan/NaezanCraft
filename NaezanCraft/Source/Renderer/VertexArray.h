#pragma once

class VertexArray
{
public:
	VertexArray();
	~VertexArray();

	void Bind();
	void UnBind();

	inline const uint32_t GetArrayID() const { return arrayID; }

	inline static std::shared_ptr<VertexArray> CreateArray() { return std::make_shared<VertexArray>(); }

private:
	uint32_t arrayID;
};