#include "raylib.h"
#include "game.h"

int main()
{
	game app;
	app.Init();
	

	while (!app.ShouldClose())
	{
		app.Tick();

		while (app.ShouldPhysics())
		{
			app.TickPhys();
		}

		app.draw();
	}

	app.Exit();

	return 0;
}