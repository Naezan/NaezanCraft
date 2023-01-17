#pragma once

#include "../World/Actor.h"
#include "../Renderer/Shader.h"

class Mesh;
class Inventory;

class HUD : public Actor
{
public:
	HUD();
	~HUD();

	virtual void Update() override;
	virtual void Render() override;

	inline std::unique_ptr<Inventory>& GetInventory() { return inventory; }

private:
	std::unique_ptr<Inventory> inventory;

	std::map<ShaderType, std::unique_ptr<Shader>> rectShaders;

	int shaderProgram;
	glm::mat4 crosshairModel;
	std::unique_ptr<Mesh> crosshairMesh;



};