#pragma once

#include <vector>
#include "physics.h"

class game 
{
private:
	float AccumulatedFixedTime;

	std::vector<physObject> PhysObjects;

public:
	game();

	void Init();
	void Tick();
	void TickPhys();
	void draw() const;
	void Exit();

	bool ShouldClose() const;
	bool ShouldPhysics() const;

	float TargetFixedStep;
};