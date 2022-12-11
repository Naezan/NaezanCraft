#pragma once

#include "../Shader.h"

class Camera;

class CloudShader : public Shader
{
public:
	CloudShader(const std::string& path, ShaderType shadertype);
	~CloudShader() = default;

	void GetUniform(uint32_t shaderID);
	void Update(std::weak_ptr<Camera>& camera, const glm::mat4& _modelMatrix);

private:
	//Uniform ID
	//unsigned int v;

};