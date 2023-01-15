#pragma once

#include "../World/Actor.h"
#include "../Renderer/Shader.h"

class Inventory : public Actor
{
public:
	Inventory(int shaderID);
	~Inventory();

	virtual void Update() override;
	virtual void Render() override;
private:
	int shaderProgram;
};