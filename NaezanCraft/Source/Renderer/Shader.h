#pragma once

#include <string>
#include <glad/glad.h>

enum class ShaderType
{
	VERTEX, FRAGMENT
};

class Shader
{
public:
	Shader(const std::string& path, ShaderType shadertype);

	void Use();
	void LinkComplete(GLuint programID);
	void SetBool(const std::string& name, bool value) const;
	void SetInt(const std::string& name, int value) const;
	void SetFloat(const std::string& name, float value) const;

	const inline uint32_t GetShaderID() const { return shaderID; }
	const inline ShaderType GetShaderType() const { return shaderType; }
	const inline GLenum GetGLShader(ShaderType shadertype)
	{
		switch (shadertype)
		{
		case ShaderType::VERTEX: return GL_VERTEX_SHADER;
		case ShaderType::FRAGMENT: return GL_FRAGMENT_SHADER;
		}

		return -1;
	}

	const static inline std::string ToString(ShaderType type)
	{
		switch (type)
		{
		case ShaderType::VERTEX: return "VERTEX";
		case ShaderType::FRAGMENT: return "FRAGMENT";
		}

		return "ShaderType is Null";
	}

	template<ShaderType T>
	static std::unique_ptr<Shader> CreateShader(std::string path)
	{
		return std::make_unique<Shader>(path, T);
	}

private:
	uint32_t shaderID;
	ShaderType shaderType;
};