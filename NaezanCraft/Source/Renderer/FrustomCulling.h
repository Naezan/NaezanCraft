#pragma once

#include "../../Collision/Collision.h"

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

struct Frustum
{
	Plan Faces[6];

	int AABB(AABox& box);

	void UpdateFrustum(const glm::mat4& vp);
};
