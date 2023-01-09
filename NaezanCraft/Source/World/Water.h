#pragma once

struct VertTexCoord;
class Mesh;

class Water
{
public:
	Water();
	~Water();

	void CreateBuffer(std::vector<VertTexCoord>& vertexcoords);

	inline std::unique_ptr<Mesh>& GetMesh() { return waterMesh; }

private:
	std::unique_ptr<Mesh> waterMesh;
	std::vector<unsigned int> meshIndices;
};
