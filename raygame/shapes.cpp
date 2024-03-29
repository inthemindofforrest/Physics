#include "shapes.h"

#include "glm/glm.hpp"
#include "physics.h"

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
		              [_posA, _lhs, _posB](aabb a)   {return CheckAABBAABB(_posA, _lhs, _posB, a); });
}

void ResolvePhysBodies(physObject & _lhs, physObject & _rhs)
{
	/* 
		       _____  _____
		      /     \/     \
		     |      /\      |
			 |   |------|   |   -Circles colliding with the centers measured out
		     |      \/      |
		      \_____/\_____/
		
			 |--------------|   -Length of Radius of balls combines
			 |------|	        -Difference in Centers of balls
		   - __________________
			        |-------|   -Difference of the Radius sum and the Distance of the ball's radius

		Difference between the two Circles (That force is applied to the two circles to seperate them)
		That is then applied to the positions and velocity to seperate them instantally and push the objects apart
		*/
	glm::vec2 ResImpulse[2];

	glm::vec2 Normal = { 0,0 };
	float Pen = 0.0f;

	Normal = _lhs.Collider.match([_lhs, _rhs, &Pen](circle c)
	{
		return _rhs.Collider.match([_lhs, _rhs, &Pen](circle c)
		{
			float dist = glm::length(_lhs.pos - _rhs.pos);
			float sum = _lhs.Collider.get<circle>().Radius + _rhs.Collider.get<circle>().Radius;

			Pen = sum - dist;
			return glm::normalize(_lhs.pos - _rhs.pos);
		},
			[_lhs, _rhs, &Pen](aabb a)
		{
			Pen = AABBCirclePen(_rhs, _lhs);
			return glm::normalize(_rhs.pos - _lhs.pos);
		});
	},
		[_lhs, _rhs, &Pen](aabb a)
	{
		return _rhs.Collider.match([_lhs, _rhs, &Pen](circle c)
		{
			Pen = AABBCirclePen(_lhs, _rhs);
			return glm::normalize(_lhs.pos - _rhs.pos);
		},
			[_lhs, _rhs, &Pen](aabb a)
		{
			return glm::vec2();//TODO Create AABB and AABB
		});
	}
	);

	ResolveCollision(_lhs.pos, _lhs.vel, _lhs.mass, _rhs.pos, _rhs.vel, _rhs.mass, 1.0f, Normal, ResImpulse);

	_lhs.pos += Normal * Pen;
	_rhs.pos -= Normal * Pen;

	_lhs.vel = ResImpulse[0];
	_rhs.vel = ResImpulse[1];
}

void ResolveCollision(glm::vec2 _posA, glm::vec2 _velA, float _massA, glm::vec2 _posB, glm::vec2 _velB, float _massB, float _elasticity, glm::vec2 _normal, glm::vec2 * _dst)
{
	glm::vec2 RelVel = _velA - _velB;
	float ImpulseMag = glm::dot(-(1.0f + _elasticity) * RelVel, _normal) / glm::dot(_normal, _normal * (1 / _massA + 1 / _massB));

	_dst[0] = _velA + (ImpulseMag / _massA) * _normal;
	_dst[1] = _velB - (ImpulseMag / _massB) * _normal;

}

float AABBCirclePen(physObject _lhs, physObject _rhs)
{
	glm::vec2 HalfExtent = _lhs.Collider.get<aabb>().HalfExtents;

	//If x > y  f(x) = y/x * d ELSE f(x) = x/y * d
	glm::vec2 NormDirection = glm::normalize(_lhs.pos - _rhs.pos);
	glm::vec2 PointInside = NormDirection * _rhs.Collider.get<circle>().Radius;
	glm::vec2 ABSNormalDir = glm::abs(NormDirection);

	if (ABSNormalDir.y > ABSNormalDir.x)//(Top/Bottom) 
	{
		if (PointInside.y >= 0)//Bottom
		{
			float BottomEdgeY = _lhs.pos.y + HalfExtent.y;
			float DistanceFromBottomEdge = glm::abs(BottomEdgeY - (_lhs.pos.y + PointInside.y));
			float NewX = PointInside.x + (PointInside.x / PointInside.y) * DistanceFromBottomEdge;//Point.y + (x/y) * d
			return glm::length(glm::vec2(_lhs.pos.x + NewX, BottomEdgeY));
		}
		else//Top
		{
			float TopEdgeY = _lhs.pos.y - HalfExtent.y;
			float DistanceFromTopEdge = glm::abs((PointInside.y) + TopEdgeY);
			float NewX = (PointInside.x / PointInside.y) * DistanceFromTopEdge + _rhs.pos.x;//Point.y + (x/y) * d
			return glm::length(_lhs.pos.x + PointInside - glm::vec2(NewX, TopEdgeY));
		}
	}
	else//(Left/Right)
	{
		if (PointInside.x >= 0)//Right
		{
			float TopEdgeX = _lhs.pos.x + HalfExtent.x;
			float DistanceFromRightEdge = glm::abs((_lhs.pos.x + PointInside.y) - TopEdgeX);
			float NewY = PointInside.y + (PointInside.y / PointInside.x) * DistanceFromRightEdge;//Point.y + (x/y) * d
			return glm::length(PointInside - glm::vec2(TopEdgeX, _lhs.pos.y + NewY));
		}
		else//Left
		{
			float BottomEdgeX = _lhs.pos.x - HalfExtent.x;
			float DistanceFromLeftEdge = glm::abs(BottomEdgeX - (_lhs.pos.x + PointInside.x));
			float NewY = PointInside.y + (PointInside.y / PointInside.x) * DistanceFromLeftEdge;//Point.y + (x/y) * d
			return glm::length(glm::vec2(BottomEdgeX, _lhs.pos.y + NewY));
		}

	}
}
