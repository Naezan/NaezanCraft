#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

struct AABox
{
	AABox(const glm::vec3& _corner, float _x, float _y, float _z) :
		corner(_corner), x(_x), y(_y), z(_z) 
		{
			maxcorner.x = corner.x + x;
			maxcorner.y = corner.y + y;
			maxcorner.z = corner.z + z;
		}


	glm::vec3 corner;
	float x, y, z;
	glm::vec3 maxcorner;

	glm::vec3 GetVertexP(glm::vec3& normal);
	glm::vec3 GetVertexN(glm::vec3& normal);
};