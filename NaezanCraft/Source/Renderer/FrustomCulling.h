#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Camera;

enum Face
{
	TOP, BOTTOM, RIGHT, LEFT, FARP, NEARP
};

enum CullingState
{
	OUTSIDE, INTERSECT, INSIDE
};

struct Plan
{
	// < 0 -> inside frustom, == 0 contact face, > 0 outside frustom
	float GetDistanceToPlane(const glm::vec3& point);

	glm::vec3 normal{ 0.f, 1.f, 0.f };

	float distance = 0.f;
};

struct AABox
{
	AABox(const glm::vec3& _corner, float _x, float _y, float _z) :
		corner(_corner), x(_x), y(_y), z(_z) {}


	glm::vec3 corner;
	float x, y, z;

	glm::vec3 GetVertexP(glm::vec3& normal);
	glm::vec3 GetVertexN(glm::vec3& normal);
};

struct Frustum
{
	Plan Faces[6];

	int AABB(AABox& box);

	void UpdateFrustum(const glm::mat4& vp);
};
