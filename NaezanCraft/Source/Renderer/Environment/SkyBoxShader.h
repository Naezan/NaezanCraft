#pragma once

#include "../Shader.h"

class Camera;

class SkyBoxShader : public Shader
{
public:
	SkyBoxShader(const std::string& path, ShaderType shadertype);
	~SkyBoxShader() = default;

	void GetUniform(uint32_t shaderID);
	void Update(std::shared_ptr<Camera>& camera, const glm::mat4& _modelMatrix);

private:
	unsigned int dayTime;
	unsigned int projectionViewMatrix;
	unsigned int modelMatrix;
	float light;
};