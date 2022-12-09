#pragma once

#include "../../Renderer/Shader.h"

//Something wrong in shader class.. :(

class SkyBoxShader;
class SunMoonShader;
class Mesh;
class Camera;
class VertexArray;
class VertexBuffer;
class IndexBuffer;
class Cloud;

class SkyBox
{
public:
	SkyBox();
	~SkyBox();

	void Update(glm::vec3& playerPos);
	void Render(std::shared_ptr<Camera>& camera);

private:
	uint32_t skyShaderProgram;
	std::map<ShaderType, std::unique_ptr<SkyBoxShader>> skyBoxShaders;

	//Skybox WorldTransform
	glm::mat4 TransformMatrix;

	//SkyBox Mesh
	std::shared_ptr<VertexArray> vertexArray;
	std::shared_ptr<VertexBuffer> vertexBuffer;
	std::shared_ptr<IndexBuffer> indexBuffer;
	int skyIndicesSize;

	//SunMoon Mesh
	uint32_t sunMoonShaderProgram;
	std::map<ShaderType, std::unique_ptr<SunMoonShader>> sunMoonShaders;
	std::unique_ptr<Mesh> sunMesh;
	std::unique_ptr<Mesh> moonMesh;
	int sunMoonIndicesSize;

	std::unique_ptr<Cloud> cloud;
};