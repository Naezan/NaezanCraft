#include "../../pch.h"
#include "SunMoonShader.h"
#include "../../World/Camera.h"

#include <glad/glad.h>

SunMoonShader::SunMoonShader(const std::string& path, ShaderType shadertype) :
	Shader(path, shadertype)
{
}

void SunMoonShader::GetUniform(uint32_t shaderID)
{
	dayTime = glGetUniformLocation(shaderID, "dayTime");
	projectionViewMatrix = glGetUniformLocation(shaderID, "projectionview");
	modelMatrix = glGetUniformLocation(shaderID, "model");
}

void SunMoonShader::Update(std::shared_ptr<Camera>& camera, const glm::mat4& _modelMatrix)
{
	//TO DO SetTime
	static int deltaTime = 4000;
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, camera->GetPosition());
	model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
	glUniformMatrix4fv(modelMatrix, 1, GL_FALSE, glm::value_ptr(model));
	glUniformMatrix4fv(projectionViewMatrix, 1, GL_FALSE, glm::value_ptr(camera->GetViewProjectionMatrix()));
	glUniform1f(dayTime, ((float)deltaTime / 24000) * 360);
	deltaTime += 1;
	if (deltaTime > 23999) {
		deltaTime = 0;
	}
}
