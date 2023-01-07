#include "../../pch.h"
#include "SkyBoxShader.h"
#include "../../World/Camera.h"
#include "../../World/Scene.h"
#include "../../Util/Math.h"

#include "../../Input/InputSystem.h"

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

void SkyBoxShader::Update(std::weak_ptr<Camera>& camera, const glm::mat4& _modelMatrix)
{
	worldTime += .1f;
	if (worldTime > 23999) {
		worldTime = 0;
	}

	if (Input::GetIsKeyPressed(GLFW_KEY_HOME))
	{
		worldTime+= 100.f;
	}
	if (Input::GetIsKeyPressed(GLFW_KEY_END))
	{
		worldTime -= 100.f;
	}

	//0 -> 오전6시, 6000 정오, 12000 오후6시, 18000 밤12시
	if (worldTime < 3000) { //오전6시~오전9시라면 점점밝아짐
		sunIntensity = blend(0.6f, 1.0f, (float)worldTime / 3000);
	}
	if (worldTime > 3000 && worldTime < 12000) { //오전9시~오후6시 최대 밝기
		sunIntensity = 1.0f;
	}
	if (worldTime > 12000 && worldTime < 15000) { //오후6시~오후9시 점점 어두워짐
		sunIntensity = blend(1.0f, 0.6f, (float)((float)worldTime - 12000) / 3000);
	}
	if (worldTime > 15000 && worldTime < 18000) { //오후9시~밤12시 더 어두워짐
		sunIntensity = blend(0.6f, 0.1f, (float)((float)worldTime - 15000) / 3000);
	}
	if (worldTime > 18000 && worldTime < 21000) { //밤12시~오전3시 어둠 그자치
		sunIntensity = 0.1f;
	}
	if (worldTime > 21000 && worldTime < 24000) { //오전3시~오전6시 밝아짐
		sunIntensity = blend(0.1f, 0.6f, (float)((float)worldTime - 21000) / 3000);
	}

	glUniformMatrix4fv(projectionViewMatrix, 1, GL_FALSE, glm::value_ptr(camera.lock()->GetViewProjectionMatrix()));
	glUniformMatrix4fv(modelMatrix, 1, GL_FALSE, glm::value_ptr(_modelMatrix));
	//원지름에서의 현재 위치 비율
	glUniform1f(dayTime, worldTime);
	glUniform1f(light, 1);
}
