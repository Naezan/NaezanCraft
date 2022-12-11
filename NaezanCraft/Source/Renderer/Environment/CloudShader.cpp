#include "CloudShader.h"

#include <glad/glad.h>

CloudShader::CloudShader(const std::string& path, ShaderType shadertype) :
	Shader(path, shadertype)
{
}

void CloudShader::GetUniform(uint32_t shaderID)
{
	//TODO
	//glGetUniformLocation(shaderID, );
}

void CloudShader::Update(std::weak_ptr<Camera>& camera, const glm::mat4& _modelMatrix)
{
	//TODO SetUniformValue
}
