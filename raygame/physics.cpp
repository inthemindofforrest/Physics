#include "physics.h"
#include "raylib.h"

physObject::physObject()
{
	pos = { 0,0 };
	vel = { 0,0 };
	forces = { 0,0 };

	mass = 1.0f;
	Drag = 1.0f;
}

void physObject::TickPhysics(float delta)
{
	//INtigrating forces into velocity
	vel += forces * delta;
	forces = { 0,0 };

	//intigrate Linear drag
	vel = vel * (1 - delta * Drag);

	//intigrating velocity into position
	pos += vel * delta;
}

void physObject::Draw() const
{
	DrawCircleLines(pos.x, pos.y, 15.0f, GRAY);
}

void physObject::AddForce(glm::vec2 force)
{
	forces += force * (1.0f / mass);
}

void physObject::AddImpulse(glm::vec2 impulse)
{
	vel += impulse * (1.0f / mass);
}

void physObject::AddAccel(glm::vec2 accel)
{
	assert(false);
}

void physObject::AddVelocityChange(glm::vec2 delta)
{
	vel += delta;
}
