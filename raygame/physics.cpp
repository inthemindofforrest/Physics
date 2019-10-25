#include "physics.h"
#include "raylib.h"

physObject::physObject()
{
	pos = { 0,0 };
	vel = { 0,0 };
	forces = { 0,0 };

	mass = 1.0f;
	Drag = 0.0f;
	Gravity = 9.80665f;
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

	AddForce({ 0,Gravity });
}

void physObject::Draw() const
{
	Collider.match([this](circle c) {DrawCircleLines(pos.x, pos.y, c.Radius, GRAY); },
		[this](aabb a) {DrawRectangleLines(pos.x - a.HalfExtents.x, pos.y - a.HalfExtents.y, a.HalfExtents.x * 2, a.HalfExtents.y * 2, GRAY); });
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
	forces += accel;
}

void physObject::AddVelocityChange(glm::vec2 delta)
{
	vel += delta;
}
