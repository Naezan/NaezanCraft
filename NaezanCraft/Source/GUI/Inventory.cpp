#include "../pch.h"
#include "Inventory.h"
#include "../TextureManager.h"

std::unordered_map<BlockType, int> Inventory::InvenBlockCoordData;

//Vertex
struct QuadCoord
{
	glm::vec3 pos;
	glm::vec2 texcoord;
};

std::vector<unsigned int> QuadIndexCoords
{
	0, 1, 2,
	2, 3, 0
};

Slot::Slot(const BlockType& type) : slotBlockType(type)
{
	CreateSlotMesh();
}

void Slot::CreateSlotMesh()
{
	std::vector<QuadCoord> SlotBlockCoords
	{
		//leftbottom
		{
		glm::vec3(SCREEN_WIDTH / 2.f - INVEN_SLOT_HALF_WIDTH + 6, SCREEN_HEIGHT - 7, 1.f),
		glm::vec2(Inventory::InvenBlockCoordData[slotBlockType] * INVEN_SPRITE_WIDTH, INVEN_SPRITE_HEIGHT)
		},

		//rightbottom
		{
		glm::vec3(SCREEN_WIDTH / 2.f - INVEN_SLOT_HALF_WIDTH + 38, SCREEN_HEIGHT - 7, 1.f),
		glm::vec2((1 + Inventory::InvenBlockCoordData[slotBlockType]) * INVEN_SPRITE_WIDTH, INVEN_SPRITE_HEIGHT)
		},

		//righttop
		{
		glm::vec3(SCREEN_WIDTH / 2.f - INVEN_SLOT_HALF_WIDTH + 38, SCREEN_HEIGHT - 39, 1.f),
		glm::vec2((1 + Inventory::InvenBlockCoordData[slotBlockType]) * INVEN_SPRITE_WIDTH, 0)
		},

		//lefttop
		{
		glm::vec3(SCREEN_WIDTH / 2.f - INVEN_SLOT_HALF_WIDTH + 6, SCREEN_HEIGHT - 39, 1.f),
		glm::vec2(Inventory::InvenBlockCoordData[slotBlockType] * INVEN_SPRITE_WIDTH, 0)
		}
	};

	slotMesh = std::make_unique<Mesh>();
	slotMesh->CreateVertexArray();

	slotMesh->SetIndexBufferVector(QuadIndexCoords);
	slotMesh->CreateIndexBuffer();

	slotMesh->CreateVertexBuffer(static_cast<int>(sizeof(QuadCoord)), (void*)offsetof(QuadCoord, pos),
		static_cast<int>(sizeof(QuadCoord)), (void*)offsetof(QuadCoord, texcoord),
		GL_FLOAT, GL_FLOAT, 3, 2);
	slotMesh->SetVertexBufferData(SlotBlockCoords.size() * sizeof(QuadCoord), &SlotBlockCoords.front());

	slotMesh->UnBindVertexBuffer();
	slotMesh->UnBindVertexArray();
}

Inventory::Inventory(int shaderID) : slotID(0)
{
	shaderProgram = shaderID;

	std::vector<QuadCoord> SlotsCoords
	{
		{ glm::vec3(SCREEN_WIDTH / 2.f - INVEN_SLOT_HALF_WIDTH, SCREEN_HEIGHT, 1.f),	glm::vec2(0, 22) },
		{ glm::vec3(SCREEN_WIDTH / 2.f + INVEN_SLOT_HALF_WIDTH, SCREEN_HEIGHT, 1.f),	glm::vec2(182, 22) },
		{ glm::vec3(SCREEN_WIDTH / 2.f + INVEN_SLOT_HALF_WIDTH, SCREEN_HEIGHT - INVEN_SLOT_HEIGHT, 1.f),	glm::vec2(182, 0) },
		{ glm::vec3(SCREEN_WIDTH / 2.f - INVEN_SLOT_HALF_WIDTH, SCREEN_HEIGHT - INVEN_SLOT_HEIGHT, 1.f),	glm::vec2(0, 0) }
	};

	std::vector<QuadCoord> SlotPointCoords
	{
		{ glm::vec3(SCREEN_WIDTH / 2.f - (INVEN_SLOT_HALF_WIDTH + 2), SCREEN_HEIGHT, 1.f),	glm::vec2(0, 24) },
		{ glm::vec3(SCREEN_WIDTH / 2.f - (INVEN_SLOT_HALF_WIDTH + 2) + INVEN_SLOT_POINT_HALF_WIDTH, SCREEN_HEIGHT, 1.f),	glm::vec2(24, 24) },
		{ glm::vec3(SCREEN_WIDTH / 2.f - (INVEN_SLOT_HALF_WIDTH + 2) + INVEN_SLOT_POINT_HALF_WIDTH, SCREEN_HEIGHT - INVEN_SLOT_POINT_HEIGHT, 1.f),	glm::vec2(24, 0) },
		{ glm::vec3(SCREEN_WIDTH / 2.f - (INVEN_SLOT_HALF_WIDTH + 2), SCREEN_HEIGHT - INVEN_SLOT_POINT_HEIGHT, 1.f),	glm::vec2(0, 0) }
	};

	//inventorySlotsMesh
	{
		inventorySlotsMesh = std::make_unique<Mesh>();
		inventorySlotsMesh->CreateVertexArray();

		inventorySlotsMesh->SetIndexBufferVector(QuadIndexCoords);
		inventorySlotsMesh->CreateIndexBuffer();

		inventorySlotsMesh->CreateVertexBuffer(static_cast<int>(sizeof(QuadCoord)), (void*)offsetof(QuadCoord, pos),
			static_cast<int>(sizeof(QuadCoord)), (void*)offsetof(QuadCoord, texcoord),
			GL_FLOAT, GL_FLOAT, 3, 2);
		inventorySlotsMesh->SetVertexBufferData(SlotsCoords.size() * sizeof(QuadCoord), &SlotsCoords.front());

		inventorySlotsMesh->UnBindVertexBuffer();
		inventorySlotsMesh->UnBindVertexArray();
	}

	//slotPointMesh
	{
		slotPointMesh = std::make_unique<Mesh>();
		slotPointMesh->CreateVertexArray();

		slotPointMesh->SetIndexBufferVector(QuadIndexCoords);
		slotPointMesh->CreateIndexBuffer();

		slotPointMesh->CreateVertexBuffer(static_cast<int>(sizeof(QuadCoord)), (void*)offsetof(QuadCoord, pos),
			static_cast<int>(sizeof(QuadCoord)), (void*)offsetof(QuadCoord, texcoord),
			GL_FLOAT, GL_FLOAT, 3, 2);
		slotPointMesh->SetVertexBufferData(SlotPointCoords.size() * sizeof(QuadCoord), &SlotPointCoords.front());

		slotPointMesh->UnBindVertexBuffer();
		slotPointMesh->UnBindVertexArray();
	}

	//invenSlotBlockData
	{
		InvenBlockCoordData[Dirt] = 0;
		InvenBlockCoordData[Grass] = 1;
		InvenBlockCoordData[Stone] = 2;
		InvenBlockCoordData[Sand] = 3;
		InvenBlockCoordData[GrowStone] = 4;
		InvenBlockCoordData[Bedrock] = 5;
	}

	inventorySlots.push_back(Slot(BlockType::Dirt));
	inventorySlots.push_back(Slot(BlockType::Grass));
	inventorySlots.push_back(Slot(BlockType::Stone));
	inventorySlots.push_back(Slot(BlockType::Sand));
	inventorySlots.push_back(Slot(BlockType::GrowStone));
	inventorySlots.push_back(Slot(BlockType::Bedrock));
}

Inventory::~Inventory()
{
	inventorySlotsMesh->DeleteMesh();
	slotPointMesh->DeleteMesh();
}

void Inventory::Update()
{
	//SetSlotInfo
}

void Inventory::Render()
{
	glEnable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);

	glUseProgram(shaderProgram);
	glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(SCREEN_WIDTH),
		static_cast<float>(SCREEN_HEIGHT), 0.0f);
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

	{
		inventoryModel = glm::mat4(1.0f);
		glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(inventoryModel));

		TextureManager::BindTexture("InventorySlots");

		//Render Slots
		inventorySlotsMesh->BindVertexArray();
		glDrawElements(GL_TRIANGLES, inventorySlotsMesh->GetIndicesCount(), GL_UNSIGNED_INT, nullptr);
		inventorySlotsMesh->UnBindVertexArray();
	}

	{
		inventoryModel = glm::mat4(1.0f);
		inventoryModel = glm::translate(inventoryModel, glm::vec3(INVEN_SLOT_POINT_INER_WIDTH * slotID, 0.f, 0.f));
		glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(inventoryModel));

		TextureManager::BindTexture("InventorySlotPoint");

		//Render SlotPoint
		slotPointMesh->BindVertexArray();
		glDrawElements(GL_TRIANGLES, slotPointMesh->GetIndicesCount(), GL_UNSIGNED_INT, nullptr);
		slotPointMesh->UnBindVertexArray();
	}

	for (int i = 0; i < inventorySlots.size(); ++i)
	{
		inventorySlots[i].slotModel = glm::mat4(1.0f);
		inventorySlots[i].slotModel = glm::translate(inventorySlots[i].slotModel, glm::vec3(INVEN_SLOT_POINT_INER_WIDTH * i, 0.f, 0.f));
		glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(inventorySlots[i].slotModel));

		TextureManager::BindTexture("InventoryBlockSprite");

		inventorySlots[i].slotMesh->BindVertexArray();
		glDrawElements(GL_TRIANGLES, inventorySlots[i].slotMesh->GetIndicesCount(), GL_UNSIGNED_INT, nullptr);
		inventorySlots[i].slotMesh->UnBindVertexArray();
	}
}

BlockType Inventory::GetInventorySlotBlockType()
{
	if (slotID < inventorySlots.size())
		return inventorySlots[slotID].slotBlockType;
	return BlockType::Air;
}
