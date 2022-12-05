#include "../pch.h"
#include "Mesh.h"
#include "Chunk.h"
#include "World.h"
#include "../Application.h"

#include "../Renderer/VertexArray.h"
#include "../Renderer/Buffer.h"

const std::array<glm::vec3, 4> Mesh::vertices[]
{
	{ glm::vec3(0.5f, 0.5f, -0.5f),		glm::vec3(-0.5f, 0.5f, -0.5f),	glm::vec3(-0.5f, 0.5f, 0.5f),	glm::vec3(0.5f, 0.5f, 0.5f) },
	{ glm::vec3(0.5f, -0.5f,  0.5f),	glm::vec3(-0.5f, -0.5f,  0.5f),	glm::vec3(-0.5f, -0.5f, -0.5f),	glm::vec3(0.5f, -0.5f, -0.5f) },
	{ glm::vec3(-0.5f, -0.5f,  0.5f),	glm::vec3(0.5f, -0.5f,  0.5f),	glm::vec3(0.5f,  0.5f,  0.5f),	glm::vec3(-0.5f,  0.5f,  0.5f) },
	{ glm::vec3(0.5f, -0.5f, -0.5f),	glm::vec3(-0.5f, -0.5f, -0.5f),	glm::vec3(-0.5f,  0.5f, -0.5f),	glm::vec3(0.5f,  0.5f, -0.5f) },
	{ glm::vec3(0.5f, -0.5f,  0.5f),	glm::vec3(0.5f, -0.5f, -0.5f),	glm::vec3(0.5f,  0.5f, -0.5f),	glm::vec3(0.5f,  0.5f,  0.5f) },
	{ glm::vec3(-0.5f, -0.5f, -0.5f),	glm::vec3(-0.5f, -0.5f,  0.5f),	glm::vec3(-0.5f,  0.5f,  0.5f),	glm::vec3(-0.5f,  0.5f, -0.5f) }
};

const std::array<glm::u8vec3, 2> Mesh::indices
{
	glm::u8vec3(0, 1, 2) , glm::u8vec3(2, 3, 1)
};

const std::array<glm::u8vec2, 4> Mesh::texcoords
{
	glm::u8vec2(0, 1) ,glm::u8vec2(1, 1), glm::u8vec2(1, 0), glm::u8vec2(0, 0)
};

Mesh::Mesh(std::shared_ptr<Chunk> chunk)
{
	parentChunk = chunk;
	//TO DO too much
	meshVertices.reserve(CHUNK_SIZE * 6);

	//CreateArray & Buffer
	vertexArray = VertexArray::CreateArray();

	vertexBuffer = Buffer::CreateBuffer<VertexBuffer>(
		static_cast<int>(sizeof(VertTexCoord)), (void*)offsetof(VertTexCoord, pos),
		static_cast<int>(sizeof(VertTexCoord)), (void*)offsetof(VertTexCoord, texcoord));

	std::vector<GLuint> meshIndices;
	meshIndices.resize(CHUNK_SIZE * 6);
	for (int i = 0; i < CHUNK_SIZE; ++i)
	{
		meshIndices[i * 6 + 0] = indices[0].x + 4 * i;
		meshIndices[i * 6 + 1] = indices[0].y + 4 * i;
		meshIndices[i * 6 + 2] = indices[0].z + 4 * i;
		meshIndices[i * 6 + 3] = indices[1].x + 4 * i;
		meshIndices[i * 6 + 4] = indices[1].y + 4 * i;
		meshIndices[i * 6 + 5] = indices[1].z + 4 * i;
	}
	std::shared_ptr<IndexBuffer> indexBuffer = Buffer::CreateBuffer<IndexBuffer>(CHUNK_SIZE * 6 * sizeof(GLuint), &meshIndices.front());
	meshIndices.clear();
}

Mesh::~Mesh()
{
	meshVertices.clear();
}

void Mesh::CreateMesh()
{
	meshVertices.clear();//size to zero
	//GetSideChunk
	LeftChunk = GET_World()->GetChunkByPos(std::pair<int, int>(static_cast<int>(parentChunk->position.x - 1), static_cast<int>(parentChunk->position.z)));
	RightChunk = GET_World()->GetChunkByPos(std::pair<int, int>(static_cast<int>(parentChunk->position.x + 1), static_cast<int>(parentChunk->position.z)));
	BackChunk = GET_World()->GetChunkByPos(std::pair<int, int>(static_cast<int>(parentChunk->position.x), static_cast<int>(parentChunk->position.z - 1)));
	FrontChunk = GET_World()->GetChunkByPos(std::pair<int, int>(static_cast<int>(parentChunk->position.x), static_cast<int>(parentChunk->position.z + 1)));

	for (int x = 0; x < CHUNK_X; ++x)
	{
		for (int y = 0; y < CHUNK_Y; ++y)
		{
			for (int z = 0; z < CHUNK_Z; ++z)
			{
				Block& block = parentChunk->chunkBlocks[x][y][z];
				if (block.blockType == BlockType::Air)
					continue;

				glm::vec3 tempPos = glm::vec3(x, y, z);
				glm::vec3 worldPosition;
				worldPosition.x = parentChunk->position.x * CHUNK_X + x;
				worldPosition.y = y;
				worldPosition.x = parentChunk->position.z * CHUNK_Z + z;
				worldPosition /= 2;//scaled pos?
				AddFaces(tempPos, block.blockType);
			}
		}
	}

	//SetVertexBuffer && texCoord
	if (!meshVertices.empty())
	{
		vertexBuffer->SetBufferData(meshVertices.size() * sizeof(VertTexCoord), &meshVertices.front());
	}
}

void Mesh::AddFaces(glm::vec3& pos, BlockType& type)
{
	//X Left
	if (pos.x > 0)
	{
		//만약 이전박스가 없다면 비어있으면 안되므로 현재 왼쪽면의 정보를 추가해준다
		if (parentChunk->GetBlock(glm::vec3(pos.x - 1, pos.y, pos.z)).IsTransparent())
			AddFace(pos, type, FaceType::Left);
	}
	else
	{
		//만약 이전 청크의 마지막이 없다면 0번째위치 왼쪽면의 정보를 추가해준다
		if (LeftChunk == nullptr || LeftChunk->GetBlock(glm::vec3(CHUNK_X - 1, pos.y, pos.z)).IsTransparent())
			AddFace(pos, type, FaceType::Left);
	}

	//X Right
	if (pos.x < CHUNK_X - 1)
	{
		//만약 다음박스가 없다면 비어있으면 안되므로 현재 오른쪽면의 정보를 추가해준다
		if (parentChunk->GetBlock(glm::vec3(pos.x + 1, pos.y, pos.z)).IsTransparent())
			AddFace(pos, type, FaceType::Right);
	}
	else
	{
		//만약 다음 청크의 0번째가 없다면 CHUNK_X - 1위치 으론쪽면의 정보를 추가해준다
		if (RightChunk == nullptr || RightChunk->GetBlock(glm::vec3(0, pos.y, pos.z)).IsTransparent())
			AddFace(pos, type, FaceType::Right);
	}

	//Y Bottom
	if (pos.y > 0)
	{
		if (parentChunk->GetBlock(glm::vec3(pos.x, pos.y - 1, pos.z)).IsTransparent())
			AddFace(pos, type, FaceType::Bottom);
	}
	else
	{
		//Just add Bottom Face
		AddFace(pos, type, FaceType::Bottom);
	}

	//Y Top
	if (pos.y < CHUNK_Y - 1)
	{
		if (parentChunk->GetBlock(glm::vec3(pos.x, pos.y + 1, pos.z)).IsTransparent())
			AddFace(pos, type, FaceType::Top);
	}
	else
	{
		//Just add Top Face
		AddFace(pos, type, FaceType::Top);
	}

	//Z Back
	if (pos.z > 0)
	{
		if (parentChunk->GetBlock(glm::vec3(pos.x, pos.y, pos.z - 1)).IsTransparent())
			AddFace(pos, type, FaceType::Back);
	}
	else
	{
		if (BackChunk == nullptr || BackChunk->GetBlock(glm::vec3(pos.x, pos.y, CHUNK_Z - 1)).IsTransparent())
			AddFace(pos, type, FaceType::Back);
	}

	//Z Front
	if (pos.z < CHUNK_Z - 1)
	{
		if (parentChunk->GetBlock(glm::vec3(pos.x, pos.y, pos.z + 1)).IsTransparent())
			AddFace(pos, type, FaceType::Front);
	}
	else
	{
		if (FrontChunk == nullptr || FrontChunk->GetBlock(glm::vec3(pos.x, pos.y, 0)).IsTransparent())
			AddFace(pos, type, FaceType::Front);
	}
}

void Mesh::AddFace(const glm::vec3& pos, const BlockType& Blocktype, const FaceType& faceType)
{
	switch (faceType)
	{
	case Top:
		meshVertices.push_back({ pos + vertices[Top][0],texcoords[0] });
		meshVertices.push_back({ pos + vertices[Top][1],texcoords[1] });
		meshVertices.push_back({ pos + vertices[Top][2],texcoords[2] });
		meshVertices.push_back({ pos + vertices[Top][3],texcoords[3] });
		break;
	case Bottom:
		meshVertices.push_back({ pos + vertices[Bottom][0],texcoords[0] });
		meshVertices.push_back({ pos + vertices[Bottom][1],texcoords[1] });
		meshVertices.push_back({ pos + vertices[Bottom][2],texcoords[2] });
		meshVertices.push_back({ pos + vertices[Bottom][3],texcoords[3] });
		break;
	case Front:
		meshVertices.push_back({ pos + vertices[Front][0],texcoords[0] });
		meshVertices.push_back({ pos + vertices[Front][1],texcoords[1] });
		meshVertices.push_back({ pos + vertices[Front][2],texcoords[2] });
		meshVertices.push_back({ pos + vertices[Front][3],texcoords[3] });
		break;
	case Back:
		meshVertices.push_back({ pos + vertices[Back][0],texcoords[0] });
		meshVertices.push_back({ pos + vertices[Back][1],texcoords[1] });
		meshVertices.push_back({ pos + vertices[Back][2],texcoords[2] });
		meshVertices.push_back({ pos + vertices[Back][3],texcoords[3] });
		break;
	case Right:
		meshVertices.push_back({ pos + vertices[Right][0],texcoords[0] });
		meshVertices.push_back({ pos + vertices[Right][1],texcoords[1] });
		meshVertices.push_back({ pos + vertices[Right][2],texcoords[2] });
		meshVertices.push_back({ pos + vertices[Right][3],texcoords[3] });
		break;
	case Left:
		meshVertices.push_back({ pos + vertices[Left][0],texcoords[0] });
		meshVertices.push_back({ pos + vertices[Left][1],texcoords[1] });
		meshVertices.push_back({ pos + vertices[Left][2],texcoords[2] });
		meshVertices.push_back({ pos + vertices[Left][3],texcoords[3] });
		break;
	}
}

void Mesh::BindVertexArray()
{
	vertexArray->Bind();
}

void Mesh::UnBindVertexArray()
{
	vertexArray->UnBind();
}
