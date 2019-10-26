#include "game.h"
#include "raylib.h"
#include "CustomMath.h"

#include <iostream>

game::game()
{
	TargetFixedStep = 1.0f / 30.0f;
	AccumulatedFixedTime = 0.0f;
}

void game::Init()
{
	int screenWidth = 800;
	int screenHeight = 450;

	InitWindow(screenWidth, screenHeight, "Custom Physics");

	SetTargetFPS(60);
}

void game::Tick()
{
	//Update
	AccumulatedFixedTime += GetFrameTime();

	bool MB0 = IsMouseButtonPressed(0);
	bool MB1 = IsMouseButtonPressed(1);

	if (MB0 || MB1)
	{
		PhysObjects.emplace_back();
		std::cout << "Added a Physics Object!" << std::endl;

		physObject& babyPhys = PhysObjects[PhysObjects.size() - 1];
		Vector2 mousePos = GetMousePosition();
		babyPhys.pos = { mousePos.x, mousePos.y };

		babyPhys.Collider = circle{ 15.0f };
		if (!MB0)babyPhys.isTrigger = true;
		//if(MB0)babyPhys.Collider = circle{ 15.0f };
		//else babyPhys.Collider = aabb{ {15,15} };
	}
}

void game::TickPhys()
{
	//Physics
	AccumulatedFixedTime -= TargetFixedStep;

	//Foreach without the downside of c#
	for (physObject& i : PhysObjects)
	{
		i.TickPhysics(TargetFixedStep);

		i.WrapScreen();
	}

	for (physObject& i : PhysObjects)
	{
		for (physObject& j : PhysObjects)
		{
			if (&i == &j) { continue; }

			bool Collision = false;

			i.Collider.match([i, j, &Collision](circle c) {if (CheckCircleX(i.pos, c, j.pos, j.Collider)) { Collision = true; } },
				[i, j, &Collision](aabb a) {if (CheckAABBX(i.pos, a, j.pos, j.Collider)) { Collision = true; }});

			if (!i.isTrigger && !j.isTrigger && Collision) { ResolvePhysBodies(i, j); }
		}
	}
}

void game::draw() const
{
	BeginDrawing();

	ClearBackground(BLACK);

	for (const physObject& i : PhysObjects)
	{
		i.Draw();
	}

	EndDrawing();
}

void game::Exit()
{
}

bool game::ShouldClose() const
{
	return WindowShouldClose();
}

bool game::ShouldPhysics() const
{
	return AccumulatedFixedTime >= TargetFixedStep;
}
