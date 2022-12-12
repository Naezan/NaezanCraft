#include "../pch.h"
#include "FrustomCulling.h"

#include "../World/Camera.h"

void Frustum::UpdateFrustum(const glm::mat4& vp)
{
	Faces[Face::TOP].normal.x = vp[0][3] - vp[0][1];//x scale(xdir) - someX pos?
	Faces[Face::TOP].normal.y = vp[1][3] - vp[1][1];
	Faces[Face::TOP].normal.z = vp[2][3] - vp[2][1];
	Faces[Face::TOP].distance = vp[3][3] - vp[3][1];//i think this has dot product value

	Faces[Face::BOTTOM].normal.x = vp[0][3] + vp[0][1];
	Faces[Face::BOTTOM].normal.y = vp[1][3] + vp[1][1];
	Faces[Face::BOTTOM].normal.z = vp[2][3] + vp[2][1];
	Faces[Face::BOTTOM].distance = vp[3][3] + vp[3][1];

	Faces[Face::RIGHT].normal.x = vp[0][3] - vp[0][0];
	Faces[Face::RIGHT].normal.y = vp[1][3] - vp[1][0];
	Faces[Face::RIGHT].normal.z = vp[2][3] - vp[2][0];
	Faces[Face::RIGHT].distance = vp[3][3] - vp[3][0];

	Faces[Face::LEFT].normal.x = vp[0][3] + vp[0][0];
	Faces[Face::LEFT].normal.y = vp[1][3] + vp[1][0];
	Faces[Face::LEFT].normal.z = vp[2][3] + vp[2][0];
	Faces[Face::LEFT].distance = vp[3][3] + vp[3][0];

	Faces[Face::FARP].normal.x = vp[0][3] - vp[0][2];
	Faces[Face::FARP].normal.y = vp[1][3] - vp[1][2];
	Faces[Face::FARP].normal.z = vp[2][3] - vp[2][2];
	Faces[Face::FARP].distance = vp[3][3] - vp[3][2];

	Faces[Face::NEARP].normal.x = vp[0][3] + vp[0][2];
	Faces[Face::NEARP].normal.y = vp[1][3] + vp[1][2];
	Faces[Face::NEARP].normal.z = vp[2][3] + vp[2][2];
	Faces[Face::NEARP].distance = vp[3][3] + vp[3][2];

	for (auto& face : Faces)
	{
		float length = glm::length(face.normal);
		face.normal /= length;
		face.distance /= length;
	}
}

float Plan::GetDistanceToPlane(const glm::vec3& point)
{
	return glm::dot(normal, point) + distance;
}

int Frustum::AABB(AABox& box)
{
	for (auto& face : Faces)
	{
		//큐브 위치 조정및 거리 계산 비교
		if (face.GetDistanceToPlane(box.GetVertexP(face.normal)) < 0)
			return OUTSIDE;
		else if (face.GetDistanceToPlane(box.GetVertexN(face.normal)) < 0)
			return INTERSECT;
	}

	return INSIDE;
}

glm::vec3 AABox::GetVertexP(glm::vec3& normal)
{
	glm::vec3 res = corner;

	if (normal.x > 0)
		res.x += x;

	if (normal.y > 0)
		res.y += y;

	if (normal.z > 0)
		res.z += z;

	return(res);
}

glm::vec3 AABox::GetVertexN(glm::vec3& normal)
{
	glm::vec3 res = corner;

	if (normal.x < 0)
		res.x += x;

	if (normal.y < 0)
		res.y += y;

	if (normal.z < 0)
		res.z += z;

	return(res);
}
