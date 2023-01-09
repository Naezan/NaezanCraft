#include "../pch.h"
#include "Water.h"
#include "Mesh.h"

Water::Water()
{
	waterMesh = std::make_unique<Mesh>();
}

Water::~Water()
{

}

void Water::CreateBuffer(std::vector<VertTexCoord>& vertexcoords)
{
	std::array<glm::i8vec3, 2> indices
	{
		glm::i8vec3(0, 1, 2) , glm::i8vec3(2, 3, 0)
	};

	float blockCount = (vertexcoords.size() / 4.f);
	meshIndices.resize(blockCount * 6);

	for (int i = 0; i < blockCount; ++i)
	{
		meshIndices[i * 6 + 0] = indices[0].x + 4 * i;
		meshIndices[i * 6 + 1] = indices[0].y + 4 * i;
		meshIndices[i * 6 + 2] = indices[0].z + 4 * i;
		meshIndices[i * 6 + 3] = indices[1].x + 4 * i;
		meshIndices[i * 6 + 4] = indices[1].y + 4 * i;
		meshIndices[i * 6 + 5] = indices[1].z + 4 * i;
	}

	waterMesh->CreateVertexArray();

	waterMesh->SetIndexBufferVector(meshIndices);
	waterMesh->CreateIndexBuffer();

	waterMesh->CreateVertexBuffer(static_cast<int>(sizeof(VertTexCoord)), (void*)offsetof(VertTexCoord, pos),
		static_cast<int>(sizeof(VertTexCoord)), (void*)offsetof(VertTexCoord, texcoord),
		static_cast<int>(sizeof(VertTexCoord)), (void*)offsetof(VertTexCoord, lightlevel),
		static_cast<int>(sizeof(VertTexCoord)), (void*)offsetof(VertTexCoord, AO),
		GL_UNSIGNED_BYTE, GL_UNSIGNED_SHORT, GL_UNSIGNED_BYTE, GL_UNSIGNED_BYTE);
	waterMesh->SetVertexBufferData(vertexcoords.size() * sizeof(VertTexCoord), &vertexcoords.front());

	//meshIndices.clear();
	//meshIndices.shrink_to_fit();

	waterMesh->UnBindVertexBuffer();
	waterMesh->UnBindVertexArray();
}

