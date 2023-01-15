#pragma once

#include "../World/Actor.h"
#include "../Renderer/Shader.h"

class Mesh;

class Inventory : public Actor
{
public:
	Inventory(int shaderID);
	~Inventory();

	virtual void Update() override;
	virtual void Render() override;
private:
	int shaderProgram;

	glm::mat4 inventoryModel;
	std::unique_ptr<Mesh> inventorySlotsMesh;
	std::unique_ptr<Mesh> slotPointMesh;
};