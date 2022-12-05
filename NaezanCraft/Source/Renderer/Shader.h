#pragma once

#include <string>

enum class ShaderType
{
	VERTEX, FRAGMENT
};

class Shader
{
public:
	Shader(const std::string& path, ShaderType shadertype);

	void Use();
	void LinkComplete(unsigned int programID);
	void SetBool(const std::string& name, bool value) const;
	void SetInt(const std::string& name, int value) const;
	void SetFloat(const std::string& name, float value) const;

	const inline uint32_t GetShaderID() const { return shaderID; }
	const inline ShaderType GetShaderType() const { return shaderType; }
	const inline unsigned int GetGLShader(ShaderType shadertype);

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