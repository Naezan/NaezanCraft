#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

struct Block;

class Ray final
{
	[[nodiscard]] constexpr Ray(const glm::vec3& _origin, const glm::vec3& _direction)
		: origin{ _origin }, direction{ _direction } {}

	// Represents the function p(t) = origin + t * direction,
	// where p is a 3-dimensional position, and t is a scalar.
	[[nodiscard]] inline constexpr glm::vec3 point_at_parameter(const float t) const {
		return this->origin + (this->direction * t);
	}

	[[nodiscard]] inline constexpr glm::vec3 GetOrigin() const { return this->origin; }
	[[nodiscard]] inline constexpr glm::vec3 GetDirection() const { return this->direction; }

public:
	static Block BlockTraversal(const glm::vec3& ray_start, const glm::vec3& dir);

private:
	// The origin of the ray.
	const glm::vec3 origin;
	// The normalized direction of the ray.
	const glm::vec3 direction;
};