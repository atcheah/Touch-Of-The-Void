#pragma once

// internal
#include "audio_manager/audio_manager.hpp"
#include "common/common.hpp"
#include "render_system/render_system.hpp"
#include "ui_system/ui_system.hpp"
#include "weapon_system/weapon_system.hpp"
#include "powerup_system/powerup_system.hpp"

// stlib
#include <vector>
#include <random>

// Container for all our entities and game logic. Individual rendering / update is
// deferred to the relative update() methods
class WorldSystem
{
//public:
//	WorldSystem();
//
//	float restart_delay_timer = 0.0f; // Delay before restarting the game after death
//
//	// Creates a window
//	GLFWwindow* create_window();
//
//	// starts the game
//	void init(RenderSystem* renderer_arg, UISystem* ui_arg, WeaponSystem* weapon_arg);
//
//	// Releases all associated resources
//	~WorldSystem();
//
//	// Steps the game ahead by ms milliseconds
//	bool step(float elapsed_ms);
//
//
//	// Check for collisions
//	void handle_collisions(float elapsed_ms);
//
//	void bounce_back(Entity player, Entity obstacle);
//
//	// Should the game be over ?
//	bool is_over()const;
private:
	// Input callback functions
	void on_key(int key, int, int action, int mod);
	void on_scroll(double x_offset, double y_offset);
	void on_mouse_move(vec2 pos);
	void on_mouse_click(int button, int action, int mod);


	// restart level
	void restart_game();

	// enter room
	void enter_room(vec2 player_pos);
	void remove_entities_on_entry();

	// Progress game timers
	bool progress_timers(Player& player, float elapsed_ms_since_last_update);

	// OpenGL window handle
	GLFWwindow* window;
	Entity cursor; // custom cursor

	// Game state
	RenderSystem* renderer;
	UISystem* ui;
	PowerupSystem* powerups;
	WeaponSystem* weapons;
	Entity player;
	Entity background;

	std::vector<Entity> p_mesh_lines; // for debug

	// the current level 
	Entity level;

	enum class GAME_STATE {
		START_MENU,
		GAME,
		SHOP_MENU,
		PAUSE_MENU,
		GAME_OVER,
		GAME_WIN,
		TOTAL_GAME_STATES // Keep this as the last element
	};

	GAME_STATE game_state = GAME_STATE::START_MENU;

	// HUD
	// Entity player_hp_text;
	// Entity weapon_text;
	// Entity ammo_text;
	Entity score_text;
	int score;
	int high_score;
	float multiplier; // Min 0.0, Max 9.9
	int delta_score; // Score gained in the current period TBD what a period is

	// C++ random number generator
	std::default_random_engine rng;
	std::uniform_real_distribution<float> uniform_dist; // number between 0..1


public:
	WorldSystem();

	float restart_delay_timer = 0.0f; // Delay before restarting the game after death

	// Creates a window
	GLFWwindow* create_window();

	// starts the game
	void init(RenderSystem* renderer_arg, UISystem* ui_arg, WeaponSystem* weapon_arg, PowerupSystem* powerups_arg);

	// Releases all associated resources
	~WorldSystem();

	// Steps the game ahead by ms milliseconds
	bool step(float elapsed_ms);


	// Check for collisions
	void handle_collisions(float elapsed_ms);

	void bounce_back(Entity player, Entity obstacle);

	// Should the game be over ?
	bool is_over()const;

	// is the game paused
	bool is_paused = false;
	bool invincible = false; // whether the player is invincible

	// function for switching game state
	//void switch_game_state(GAME_STATE new_state);
};