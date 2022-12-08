#include "SunMoonShader.h"

#include <glad/glad.h>

SunMoonShader::SunMoonShader(const std::string& path, ShaderType shadertype) :
	Shader(path, shadertype)
{
}

void SunMoonShader::GetUniform(uint32_t shaderID)
{
	dayTime = glGetUniformLocation(shaderID, "dayTime");
}

void SunMoonShader::Update()
{
	//TO DO SetTime
	static int deltaTime = 4000;
	glUniform1f(dayTime, deltaTime);
	deltaTime += 1;
	if (deltaTime > 23999) {
		deltaTime = 0;
	}
}
