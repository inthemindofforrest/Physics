#pragma once

#include "glm/vec2.hpp"
#include "mapbox/variant.hpp"

struct circle
{
	float Radius;

};
struct aabb
{
	glm::vec2 HalfExtents;
};

using shape = mapbox::util::variant<circle, aabb>;

bool CheckCircleCircle(glm::vec2 _posA, circle _CircleA, glm::vec2 _posB, circle _CircleB);
bool CheckAABBAABB(glm::vec2 _posA, aabb _aabbA, glm::vec2 _posB, aabb _aabbB);
bool CheckCircleAABB(glm::vec2 _posA, circle _Circle, glm::vec2 _posB, aabb _aabb);

bool CheckCircleX(glm::vec2 _posA, circle _lhs, glm::vec2 _posB, shape _rhs);
bool CheckAABBX(glm::vec2 _posA, aabb _lhs, glm::vec2 _posB, shape _rhs);

void ResolvePhysBodies(class physObject& _lhs, class physObject& _rhs);
void ResolveCollision(glm::vec2 _posA, glm::vec2 _velA, float _massA,
					  glm::vec2 _posB, glm::vec2 _velB, float _massB,
	                  float _elasticity, glm::vec2 _normal, glm::vec2* _dst);