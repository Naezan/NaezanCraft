#pragma once

#include "../../Renderer/Shader.h"

//Something wrong in shader class.. :(

class SkyBoxShader;
class Mesh;
class Camera;
class VertexArray;
class VertexBuffer;
class IndexBuffer;
class SunMoonShader;

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

	//SkyBox Mesh
	std::shared_ptr<VertexArray> vertexArray;
	std::shared_ptr<VertexBuffer> vertexBuffer;
	std::shared_ptr<IndexBuffer> indexBuffer;
	int skyIndicesSize;

	uint32_t sunMoonShaderProgram;
	std::map<ShaderType, std::unique_ptr<SunMoonShader>> sunMoonShaders;
	std::unique_ptr<Mesh> sunMesh;
	std::unique_ptr<Mesh> moonMesh;
	int sunMoonIndicesSize;

	glm::mat4 sunMoonTransformMatrix;
};