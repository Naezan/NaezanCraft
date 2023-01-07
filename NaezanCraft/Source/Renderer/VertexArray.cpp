#include "../pch.h"
#include "VertexArray.h"

VertexArray::VertexArray()
{
	glGenVertexArrays(1, &arrayID);
	glBindVertexArray(arrayID);
}

VertexArray::~VertexArray()
{
	//glDeleteVertexArrays(1, &arrayID);
}

void VertexArray::Bind()
{
	glBindVertexArray(arrayID);
}

void VertexArray::UnBind()
{
	glBindVertexArray(0);
}

void VertexArray::DeleteBuffer()
{
	if (arrayID)
		glDeleteVertexArrays(1, &arrayID);
}
