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
	Air,
	None
};

struct Block
{
	Block() : blockType(BlockType::None) {};
	Block(const BlockType& type) : blockType(type) {}
	BlockType blockType;

	inline bool IsTransparent() {
		return this->blockType == BlockType::Air;
	}

	bool operator==(Block rv) const
	{
		return this->blockType == rv.blockType;
	}
	bool operator!=(Block rv) const
	{
		return !(*this == rv);
	}
};