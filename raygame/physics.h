#pragma once

#include "glm/vec2.hpp"
#include "shapes.h"

class physObject
{
public:
	glm::vec2 pos;
	glm::vec2 vel;
	glm::vec2 forces;

	physObject();

	float mass;
	float Drag;
	float Gravity;

	shape Collider;

	void TickPhysics(float delta);
	void WrapScreen();
	void Draw() const;

	//Add a constant force with respect to mass
	void AddForce(glm::vec2 force);
	//Adds a instant force with respect to mass
	void AddImpulse(glm::vec2 impulse);
	//Accelerates the object without respect to mass
	void AddAccel(glm::vec2 accel);
	//Adds an instant force without repect to mass
	void AddVelocityChange(glm::vec2 delta);
};