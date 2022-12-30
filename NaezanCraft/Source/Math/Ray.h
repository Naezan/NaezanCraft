#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

struct Block;

class Ray final
{
	[[nodiscard]] constexpr Ray() {}


public:
	static Block BlockTraversal(const glm::vec3& ray_start, const glm::vec3& dir, glm::vec3& outPosition, glm::vec3& outBlockFacePosition);
};