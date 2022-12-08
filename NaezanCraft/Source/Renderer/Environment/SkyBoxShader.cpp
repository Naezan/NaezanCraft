#include "../../pch.h"
#include "SkyBoxShader.h"
#include "../../World/Camera.h"

#include <glad/glad.h>

SkyBoxShader::SkyBoxShader(const std::string& path, ShaderType shadertype) :
	Shader(path, shadertype)
{
}

void SkyBoxShader::GetUniform(uint32_t shaderID)
{
	dayTime = glGetUniformLocation(shaderID, "dayTime");
	light = glGetUniformLocation(shaderID, "light");
	projectionMatrix = glGetUniformLocation(shaderID, "projectionMatrix");
	viewMatrix = glGetUniformLocation(shaderID, "viewMatrix");
}

void SkyBoxShader::Update(std::shared_ptr<Camera>& camera)
{
	static int deltaTime = 4000;
	glUniformMatrix4fv(viewMatrix, 1, GL_FALSE, glm::value_ptr(camera->GetViewMatrix()));
	glUniformMatrix4fv(projectionMatrix, 1, GL_FALSE, glm::value_ptr(camera->GetProjectionMatrix()));
	glUniform1f(dayTime, deltaTime);//DeltaTime?
	++deltaTime;
	if (deltaTime > 23999) {
		deltaTime = 0;
	}
	glUniform1f(light, 1);
}
