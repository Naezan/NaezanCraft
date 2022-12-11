#pragma once

class VertexArray
{
public:
	VertexArray();
	~VertexArray();

	void Bind();
	void UnBind();

	inline const uint32_t GetArrayID() const { return arrayID; }

private:
	uint32_t arrayID;
};