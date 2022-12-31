#pragma once

#include "../../Renderer/Shader.h"

class CloudShader;
class Camera;

class Cloud
{
public:
	Cloud();
	~Cloud();

	void Update();
	void Render(std::weak_ptr<Camera>& camera, glm::mat4& transformMatrix);

private:
	uint32_t cloudShaderProgram;
	std::map<ShaderType, std::unique_ptr<CloudShader>> cloudShaders;
	std::unique_ptr<Mesh> cloudMesh;
	int cloudIndicesSize;
};