#pragma once

#include "../../Renderer/Shader.h"

class CloudShader;
class Camera;

class Cloud
{
public:
	Cloud(glm::vec3 _position = glm::vec3());
	~Cloud();

	void Update(glm::vec3 playerPos);
	void Render(std::weak_ptr<Camera>& camera, glm::mat4& transformMatrix);

private:
	uint32_t cloudShaderProgram;
	std::map<ShaderType, std::unique_ptr<CloudShader>> cloudShaders;
	std::unique_ptr<Mesh> cloudMesh;
	bool isStartRender;
	glm::vec3 renderPosition;
	glm::vec3 updatePosition;
	float deltaPos;
};