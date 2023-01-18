#pragma once

#include "../World/Actor.h"
#include "../Renderer/Shader.h"
#include "../World/Block.h"
#include "../World/Mesh.h"

struct Slot
{
	Slot(const BlockType& type = BlockType::Air);

	void CreateSlotMesh();

	glm::mat4 slotModel;
	BlockType slotBlockType;
	std::unique_ptr<Mesh> slotMesh;
};

class Inventory : public Actor
{
public:
	Inventory(int shaderID);
	~Inventory();

	virtual void Update() override;
	virtual void Render() override;

	inline int GetSlotID() { return slotID; }
	inline void SetSlotID(int id) { slotID = id; }

	BlockType GetInventorySlotBlockType();

public:
	static std::unordered_map<BlockType, int> InvenBlockCoordData;
private:
	int shaderProgram;

	glm::mat4 inventoryModel;
	std::unique_ptr<Mesh> inventorySlotsMesh;
	std::unique_ptr<Mesh> slotPointMesh;

	std::vector<Slot> inventorySlots;
	int slotID;
};