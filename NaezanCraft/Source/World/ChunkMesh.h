#pragma once
#include "Mesh.h"

#include "Block.h"

enum FaceType
{
	//+y,-y,+z,-z,+x,-x
	Top, Bottom, Front, Back, Right, Left, FaceEnd
};

class Chunk;
class VertexArray;
class VertexBuffer;
class IndexBuffer;

struct VertTexCoord
{
	glm::u8vec3 pos;
	glm::u16vec2 texcoord;
	uint8_t lightlevel;
	uint8_t AO;
};

class ChunkMesh : public Mesh
{
public:
	ChunkMesh(std::shared_ptr<Chunk>&& chunk, bool isReload = false);
	~ChunkMesh();

	void CreateMesh();
	void CreateBuffer();
	void AddFaces(const glm::u8vec3& pos, BlockType& type, const glm::u16vec2& texcoord);
	void AddFace(const glm::u8vec3& pos, const BlockType& Blocktype, const FaceType& faceType, const glm::u16vec2& texcoord);

	glm::u16vec2 GetTexCoord(BlockType& type);

private:
	static const std::array<glm::u8vec3, 4> vertices[];
	static const std::array<glm::u8vec3, 2> indices;
	//청크 하나의 모든 점이 여기에 저장된다. 65,536 보다 큰 수의 점들이 저장된다.
	std::vector<VertTexCoord> meshVertices;

	std::weak_ptr<Chunk> parentChunk;
};