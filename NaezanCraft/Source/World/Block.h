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
	Nothing
};

struct Block
{
	BlockType blockType = BlockType::Nothing;
};