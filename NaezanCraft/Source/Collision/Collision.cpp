#include "Collision.h"

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
