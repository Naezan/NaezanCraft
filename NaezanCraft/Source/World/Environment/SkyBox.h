#pragma once

#include "../../Renderer/Shader.h"

class SkyBoxShader;
class Mesh;
class Camera;
class VertexArray;
class VertexBuffer;
class IndexBuffer;

class SkyBox
{
public:
	SkyBox();
	~SkyBox();

	void Render(std::shared_ptr<Camera>& camera);

private:
	std::map<ShaderType, std::unique_ptr<SkyBoxShader>> skyBoxShaders;
	//std::unique_ptr<Mesh> skyMesh;
	std::shared_ptr<VertexArray> vertexArray;
	std::shared_ptr<VertexBuffer> vertexBuffer;
	std::shared_ptr<IndexBuffer> indexBuffer;
	int indicesSize;
};