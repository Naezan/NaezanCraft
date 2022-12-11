#pragma once
#include "../Shader.h"

class Camera;

class SunMoonShader : public Shader
{
public:
	SunMoonShader(const std::string& path, ShaderType shadertype);
	~SunMoonShader() = default;

	void GetUniform(uint32_t shaderID);
	void Update(std::weak_ptr<Camera>& camera, const glm::mat4& _modelMatrix);

private:
	unsigned int SunMoonTime;
	unsigned int projectionViewMatrix;
	unsigned int modelMatrix;
};