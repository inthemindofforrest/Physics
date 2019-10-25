#include "shapes.h"

#include "glm/glm.hpp"

bool CheckCircleCircle(glm::vec2 _posA, circle _CircleA, glm::vec2 _posB, circle _CircleB)
{
	//get the distance
	float Dist = glm::length(_posA - _posB);
	//get the sum of the radii
	float Sum = _CircleA.Radius + _CircleB.Radius;
	return Dist < Sum;
}

bool CheckAABBAABB(glm::vec2 _posA, aabb _aabbA, glm::vec2 _posB, aabb _aabbB)
{
	return  _posA.x - _aabbA.HalfExtents.x < _posB.x + _aabbB.HalfExtents.x &&//LR
			_posA.x + _aabbA.HalfExtents.x > _posB.x - _aabbB.HalfExtents.x &&//RL
			_posA.y - _aabbA.HalfExtents.y < _posB.y + _aabbB.HalfExtents.y &&//TB
			_posA.y + _aabbA.HalfExtents.y > _posB.y - _aabbB.HalfExtents.y;  //BT
}

bool CheckCircleAABB(glm::vec2 _posA, circle _Circle, glm::vec2 _posB, aabb _aabb)
{
	float DistanceX = _posA.x - glm::clamp(_posA.x, _posB.x - _aabb.HalfExtents.x, _posB.x + _aabb.HalfExtents.x);
	float DistanceY = _posA.y - glm::clamp(_posA.y, _posB.y - _aabb.HalfExtents.y, _posB.y + _aabb.HalfExtents.y);
	return (DistanceX * DistanceX + DistanceY * DistanceY) < (_Circle.Radius * _Circle.Radius);
}

bool CheckCircleX(glm::vec2 _posA, circle _lhs, glm::vec2 _posB, shape _rhs)
{
	return _rhs.match([_posA, _lhs, _posB](circle s) {return CheckCircleCircle(_posA, _lhs, _posB, s); },
					  [_posA, _lhs, _posB](aabb s)   {return CheckCircleAABB(_posA, _lhs, _posB, s); });
}

bool CheckAABBX(glm::vec2 _posA, aabb _lhs, glm::vec2 _posB, shape _rhs)
{
	return _rhs.match([_posA, _lhs, _posB](circle s) {return CheckCircleAABB(_posA, s, _posB, _lhs); },
		              [_posA, _lhs, _posB](aabb s)   {return CheckAABBAABB(_posA, _lhs, _posB, s); });
}
