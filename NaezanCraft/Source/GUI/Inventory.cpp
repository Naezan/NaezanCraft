#include "../pch.h"
#include "Inventory.h"
#include "../../World/Mesh.h"
#include "../TextureManager.h"

Inventory::Inventory(int shaderID)
{
	shaderProgram = shaderID;

	//Vertex
	struct QuadCoord
	{
		glm::vec3 pos;
		glm::vec2 texcoord;
	};

	std::vector<unsigned int> IndexCoords
	{
		0, 1, 2,
		2, 3, 0
	};

	std::vector<QuadCoord> SlotsCoords
	{
		{ glm::vec3(SCREEN_WIDTH / 2.f - INVEN_SLOT_HALF_WIDTH, SCREEN_HEIGHT, 1.f),	glm::vec2(0, 1) },
		{ glm::vec3(SCREEN_WIDTH / 2.f + INVEN_SLOT_HALF_WIDTH, SCREEN_HEIGHT, 1.f),	glm::vec2(1, 1) },
		{ glm::vec3(SCREEN_WIDTH / 2.f + INVEN_SLOT_HALF_WIDTH, SCREEN_HEIGHT - INVEN_SLOT_HEIGHT, 1.f),	glm::vec2(1, 0) },
		{ glm::vec3(SCREEN_WIDTH / 2.f - INVEN_SLOT_HALF_WIDTH, SCREEN_HEIGHT - INVEN_SLOT_HEIGHT, 1.f),	glm::vec2(0, 0) }
	};

	std::vector<QuadCoord> SlotPointCoords
	{
		{ glm::vec3(SCREEN_WIDTH / 2.f - (INVEN_SLOT_HALF_WIDTH + 1), SCREEN_HEIGHT, 1.f),	glm::vec2(0, 1) },
		{ glm::vec3(SCREEN_WIDTH / 2.f - (INVEN_SLOT_HALF_WIDTH + 1) + INVEN_SLOT_POINT_HALF_WIDTH, SCREEN_HEIGHT, 1.f),	glm::vec2(1, 1) },
		{ glm::vec3(SCREEN_WIDTH / 2.f - (INVEN_SLOT_HALF_WIDTH + 1) + INVEN_SLOT_POINT_HALF_WIDTH, SCREEN_HEIGHT - INVEN_SLOT_POINT_HEIGHT, 1.f),	glm::vec2(1, 0) },
		{ glm::vec3(SCREEN_WIDTH / 2.f - (INVEN_SLOT_HALF_WIDTH + 1), SCREEN_HEIGHT - INVEN_SLOT_POINT_HEIGHT, 1.f),	glm::vec2(0, 0) }
	};

	//inventorySlotsMesh
	{
		inventorySlotsMesh = std::make_unique<Mesh>();
		inventorySlotsMesh->CreateVertexArray();

		inventorySlotsMesh->SetIndexBufferVector(IndexCoords);
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

		slotPointMesh->SetIndexBufferVector(IndexCoords);
		slotPointMesh->CreateIndexBuffer();

		slotPointMesh->CreateVertexBuffer(static_cast<int>(sizeof(QuadCoord)), (void*)offsetof(QuadCoord, pos),
			static_cast<int>(sizeof(QuadCoord)), (void*)offsetof(QuadCoord, texcoord),
			GL_FLOAT, GL_FLOAT, 3, 2);
		slotPointMesh->SetVertexBufferData(SlotPointCoords.size() * sizeof(QuadCoord), &SlotPointCoords.front());

		slotPointMesh->UnBindVertexBuffer();
		slotPointMesh->UnBindVertexArray();
	}
}

Inventory::~Inventory()
{
	inventorySlotsMesh->DeleteMesh();
	slotPointMesh->DeleteMesh();
}

void Inventory::Update()
{
}

void Inventory::Render()
{
	glEnable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);

	glUseProgram(shaderProgram);
	glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(SCREEN_WIDTH),
		static_cast<float>(SCREEN_HEIGHT), 0.0f);
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
	inventoryModel = glm::mat4(1.0f);
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(inventoryModel));

	TextureManager::BindTexture("InventorySlots");

	//Render Slots
	inventorySlotsMesh->BindVertexArray();
	glDrawElements(GL_TRIANGLES, inventorySlotsMesh->GetIndicesCount(), GL_UNSIGNED_INT, nullptr);
	inventorySlotsMesh->UnBindVertexArray();

	TextureManager::BindTexture("InventorySlotPoint");

	//Render SlotPoint
	slotPointMesh->BindVertexArray();
	glDrawElements(GL_TRIANGLES, slotPointMesh->GetIndicesCount(), GL_UNSIGNED_INT, nullptr);
	slotPointMesh->UnBindVertexArray();
}
