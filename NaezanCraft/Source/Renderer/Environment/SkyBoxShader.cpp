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
	projectionViewMatrix = glGetUniformLocation(shaderID, "projectionview");
	modelMatrix = glGetUniformLocation(shaderID, "model");
}

void SkyBoxShader::Update(std::shared_ptr<Camera>& camera, const glm::mat4& _modelMatrix)
{
	//TO DO AddDeltaTime
	static float deltaTime = 4000;
	glUniformMatrix4fv(projectionViewMatrix, 1, GL_FALSE, glm::value_ptr(camera->GetViewProjectionMatrix()));
	glUniformMatrix4fv(modelMatrix, 1, GL_FALSE, glm::value_ptr(_modelMatrix));
	glUniform1f(dayTime, deltaTime);
	deltaTime += .1f;
	if (deltaTime > 23999) {
		deltaTime = 0;
	}
	glUniform1f(light, 1);
}
