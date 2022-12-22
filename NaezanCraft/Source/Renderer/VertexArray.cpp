#include "../pch.h"
#include "VertexArray.h"
#include <glad/glad.h>

VertexArray::VertexArray()
{
	glGenVertexArrays(1, &arrayID);
}

VertexArray::~VertexArray()
{
	glBindVertexArray(0);
	glDeleteVertexArrays(1, &arrayID);
}

void VertexArray::Bind()
{
	glBindVertexArray(arrayID);
}

void VertexArray::UnBind()
{
	glBindVertexArray(0);
}