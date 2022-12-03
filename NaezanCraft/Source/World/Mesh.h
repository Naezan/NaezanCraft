#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Block.h"

//this is chunk mesh

enum FaceType
{
	//+y,-y,+z,-z,+x,-x
	Top, Bottom, Front, Back, Right, Left, FaceEnd
};

class Chunk;

class Mesh
{
public:
	Mesh(std::shared_ptr<Chunk> chunk);
	~Mesh() = default;

	void CreateMesh();
	void AddFaces(glm::vec3& pos, BlockType& type);
	void AddFace(const glm::vec3& pos, const BlockType& Blocktype, const FaceType& faceType);

private:
	static const std::array<glm::vec3, 4> vertices [];
	static const std::array<glm::u8vec3, 2> indices;
	static const std::array<glm::vec2, 4> texcoords;
	std::shared_ptr<Chunk> parentChunk;
	//Neighbor chunk
	std::shared_ptr<Chunk> neighborChunks[4];
};