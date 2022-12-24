#pragma once

class VertexArray
{
public:
	VertexArray();
	~VertexArray();

	void Bind();
	void UnBind();
	void DeleteBuffer();

	inline const uint32_t GetArrayID() const { return arrayID; }

private:
	uint32_t arrayID = 0;
};