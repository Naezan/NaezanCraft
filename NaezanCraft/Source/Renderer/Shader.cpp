#include "../pch.h"
#include "Shader.h"
#include "../MessageLog.h"

#include <glad/glad.h>

Shader::Shader(const std::string& path, ShaderType shadertype)
{
	std::string ShaderCode;
	std::ifstream vShaderFile(path, std::ios::in);

	if (vShaderFile.is_open())
	{
		std::stringstream vShaderStream;
		vShaderStream << vShaderFile.rdbuf();
		ShaderCode = vShaderStream.str();
		vShaderFile.close();
	}
	else
	{
		NC_LOG_ERROR("ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ");
	}
	const char* vShaderCode = ShaderCode.c_str();

	int success;
	shaderID = glCreateShader(GetGLShader(shadertype));
	glShaderSource(shaderID, 1, &vShaderCode, NULL);
	glCompileShader(shaderID);

	glGetShaderiv(shaderID, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		NC_LOG_ERROR("ERROR::SHADER::{0}::COMPILATION_FAILED", ToString(shaderType));
	};
}

Shader::~Shader()
{
	glDeleteProgram(shaderID);
}

void Shader::Use()
{
	//TO DO move to renderer.cpp
	glUseProgram(shaderID);
}

void Shader::LinkComplete(unsigned int programID)
{
	glDetachShader(programID, shaderID);
	glDeleteShader(shaderID);
}

void Shader::SetBool(const std::string& name, bool value) const
{
	glUniform1i(glGetUniformLocation(shaderID, name.c_str()), (int)value);
}
void Shader::SetInt(const std::string& name, int value) const
{
	glUniform1i(glGetUniformLocation(shaderID, name.c_str()), value);
}
void Shader::SetFloat(const std::string& name, float value) const
{
	glUniform1f(glGetUniformLocation(shaderID, name.c_str()), value);
}

inline const unsigned int Shader::GetGLShader(ShaderType shadertype)
{
	switch (shadertype)
	{
	case ShaderType::VERTEX: return GL_VERTEX_SHADER;
	case ShaderType::FRAGMENT: return GL_FRAGMENT_SHADER;
	}

	return -1;
}
