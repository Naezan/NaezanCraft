#pragma once

#include "../../Renderer/Shader.h"

class CloudShader;

class Cloud
{
public:
	Cloud();
	~Cloud();

	void Update();
	void Render();

private:
	uint32_t cloudShaderProgram;
	std::map<ShaderType, std::unique_ptr<CloudShader>> cloudShaders;
	std::unique_ptr<Mesh> cloudMesh;
	int cloudIndicesSize;
};