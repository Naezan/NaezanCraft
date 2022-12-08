#pragma once
#include "../Shader.h"

class SunMoonShader : public Shader
{
public:
	SunMoonShader(const std::string& path, ShaderType shadertype);
	~SunMoonShader() = default;

	void GetUniform(uint32_t shaderID);
	void Update();

private:
	unsigned int dayTime;
};