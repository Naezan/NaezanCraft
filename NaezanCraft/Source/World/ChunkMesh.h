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
	glm::i8vec3 pos;
	glm::vec2 texcoord;
};

class ChunkMesh : public Mesh
{
public:
	ChunkMesh(std::weak_ptr<Chunk> chunk);
	~ChunkMesh();

	void CreateMesh();
	void AddFaces(const glm::vec3& pos, BlockType& type, const glm::vec2& texcoord);
	void AddFace(const glm::vec3& pos, const BlockType& Blocktype, const FaceType& faceType, const glm::vec2& texcoord);
	virtual void DeleteMesh() override;

	glm::vec2& GetTexCoord(BlockType& type);

private:
	static const std::array<glm::vec3, 4> vertices[];
	static const std::array<glm::u8vec3, 2> indices;
	//청크 하나의 모든 점이 여기에 저장된다. 65,536 보다 큰 수의 점들이 저장된다.
	std::vector<VertTexCoord> meshVertices;

	std::weak_ptr<Chunk> parentChunk;
	//Neighbor chunk
	std::weak_ptr<Chunk> LeftChunk;
	std::weak_ptr<Chunk> RightChunk;
	std::weak_ptr<Chunk> FrontChunk;
	std::weak_ptr<Chunk> BackChunk;
};