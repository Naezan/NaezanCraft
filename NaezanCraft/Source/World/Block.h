#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

enum BlockType : uint8_t
{
	Dirt,
	Sand,
	Stone,
	Grass,
	Water,
	Lava,
	Iron,
	Gold,
	Diamond,
	Bedrock,
	Air
};

struct Block
{
	Block(const BlockType& type = BlockType::Air) : blockType(type) {}
	BlockType blockType = BlockType::Air;

	inline bool IsTransparent() { return blockType == BlockType::Air; }
};