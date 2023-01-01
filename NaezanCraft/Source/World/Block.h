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
	OakLog,
	OakLeaves,
	BirchLog,
	BirchLeaves,
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
	unsigned char TopAO = 0xff;
	unsigned char BottomAO = 0xff;
	unsigned char RightAO = 0xff;
	unsigned char LeftAO = 0xff;
	unsigned char FrontAO = 0xff;
	unsigned char BackAO = 0xff;

	inline bool IsTransparent() {
		return (this->blockType == BlockType::Air) || (this->blockType == BlockType::OakLeaves) ||
			(this->blockType == BlockType::BirchLeaves);
	}
	inline bool IsFluid() {
		return (this->blockType == BlockType::Air) || (this->blockType == BlockType::Water) || (this->blockType == BlockType::Lava);
	}
	inline bool IsSmooth() {
		return 
		(this->blockType == BlockType::Air) || 
		(this->blockType == BlockType::Water) || 
		(this->blockType == BlockType::Lava) || 
		(this->blockType == BlockType::OakLeaves) ||
		(this->blockType == BlockType::BirchLeaves);
	}

	inline bool IsGrowable() {
		return (this->blockType == BlockType::Grass) || (this->blockType == BlockType::Dirt);
	}

	bool operator==(Block rv) const
	{
		return this->blockType == rv.blockType;
	}
	bool operator!=(Block rv) const
	{
		return !(*this == rv);
	}

	//r = right, l = left, t = top, B = bottom, f = front, b = back
	//Right
	uint8_t Get_rtf_AO();
	uint8_t Get_rtb_AO();
	uint8_t Get_rBf_AO();
	uint8_t Get_rBb_AO();
	void Set_rtf_AO(uint8_t ao);
	void Set_rtb_AO(uint8_t ao);
	void Set_rBf_AO(uint8_t ao);
	void Set_rBb_AO(uint8_t ao);

	//Left
	uint8_t Get_ltf_AO();
	uint8_t Get_ltb_AO();
	uint8_t Get_lBf_AO();
	uint8_t Get_lBb_AO();
	void Set_ltf_AO(uint8_t ao);
	void Set_ltb_AO(uint8_t ao);
	void Set_lBf_AO(uint8_t ao);
	void Set_lBb_AO(uint8_t ao);

	//Top
	uint8_t Get_trf_AO();
	uint8_t Get_trb_AO();
	uint8_t Get_tlf_AO();
	uint8_t Get_tlb_AO();
	void Set_trf_AO(uint8_t ao);
	void Set_trb_AO(uint8_t ao);
	void Set_tlf_AO(uint8_t ao);
	void Set_tlb_AO(uint8_t ao);

	//Bottom
	uint8_t Get_Brf_AO();
	uint8_t Get_Brb_AO();
	uint8_t Get_Blf_AO();
	uint8_t Get_Blb_AO();
	void Set_Brf_AO(uint8_t ao);
	void Set_Brb_AO(uint8_t ao);
	void Set_Blf_AO(uint8_t ao);
	void Set_Blb_AO(uint8_t ao);

	//Front
	uint8_t Get_frt_AO();
	uint8_t Get_frB_AO();
	uint8_t Get_flt_AO();
	uint8_t Get_flB_AO();
	void Set_frt_AO(uint8_t ao);
	void Set_frB_AO(uint8_t ao);
	void Set_flt_AO(uint8_t ao);
	void Set_flB_AO(uint8_t ao);

	//Back
	uint8_t Get_brt_AO();
	uint8_t Get_brB_AO();
	uint8_t Get_blt_AO();
	uint8_t Get_blB_AO();
	void Set_brt_AO(uint8_t ao);
	void Set_brB_AO(uint8_t ao);
	void Set_blt_AO(uint8_t ao);
	void Set_blB_AO(uint8_t ao);
};