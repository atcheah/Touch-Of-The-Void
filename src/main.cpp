
#define GL3W_IMPLEMENTATION
#include <gl3w.h>

// stlib
#include <chrono>

// internal
#include "physics_system/physics_system.hpp"
#include "render_system/render_system.hpp"
#include "world_system/world_system.hpp"
#include "ai_system/ai_system.hpp"
#include "ui_system/ui_system.hpp"
#include "weapon_system/weapon_system.hpp"
#include "powerup_system/powerup_system.hpp"
#include <boss/boss.hpp>

using Clock = std::chrono::high_resolution_clock;

// Entry point
int main()
{
	// Global systems
	WorldSystem world;
	RenderSystem renderer;
	PowerupSystem powerups;
	UISystem ui;
	WeaponSystem weapons;
	PhysicsSystem physics;
	AISystem ai(&renderer);
	Boss boss(&renderer);

	// Initializing window
	GLFWwindow* window = world.create_window();
	if (!window) {
		// Time to read the error message
		printf("Press any key to exit");
		getchar();
		return EXIT_FAILURE;
	}

	// initialize the main systems
	renderer.init(window);
	world.init(&renderer, &ui, &weapons, &powerups);

	renderer.initializeFonts();

	// variable timestep loop
	auto t = Clock::now();
	while (!world.is_over()) {
		// Processes system messages, if this wasn't present the window would become unresponsive
		glfwPollEvents();

		// Calculating elapsed times in milliseconds from the previous iteration
		auto now = Clock::now();
		float elapsed_ms =
			(float)(std::chrono::duration_cast<std::chrono::microseconds>(now - t)).count() / 1000;
		t = now;

		world.step(elapsed_ms);

		if (!world.is_paused) {
			if (!world.invincible) {
				ai.step(elapsed_ms);
			}
			physics.step(elapsed_ms);
			world.handle_collisions(elapsed_ms);
			boss.step(elapsed_ms);
			boss.updateGuidedMissiles(elapsed_ms);
		}

		renderer.draw();

	}

	return EXIT_SUCCESS;
}
