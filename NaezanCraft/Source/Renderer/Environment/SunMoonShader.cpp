#include "../../pch.h"
#include "SunMoonShader.h"
#include "../../World/Camera.h"
#include "../../World/Scene.h"

#include <glad/glad.h>

SunMoonShader::SunMoonShader(const std::string& path, ShaderType shadertype) :
	Shader(path, shadertype)
{
	
}

void SunMoonShader::GetUniform(uint32_t shaderID)
{
	SunMoonTime = glGetUniformLocation(shaderID, "SunMoonTime");
	projectionViewMatrix = glGetUniformLocation(shaderID, "projectionview");
	modelMatrix = glGetUniformLocation(shaderID, "model");
}

void SunMoonShader::Update(std::weak_ptr<Camera>& camera, const glm::mat4& _modelMatrix)
{
	glUniformMatrix4fv(projectionViewMatrix, 1, GL_FALSE, glm::value_ptr(camera.lock()->GetViewProjectionMatrix()));
	glUniformMatrix4fv(modelMatrix, 1, GL_FALSE, glm::value_ptr(_modelMatrix));
	glUniform1f(SunMoonTime, (worldTime / 24000) * 2 * 3.141592f);
}
