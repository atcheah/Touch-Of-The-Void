#pragma once

#include "common/common.hpp"
#include "render_system/render_system.hpp"
#include "world_init/world_init.hpp"

class UISystem
{
private:
	void fpsCalculate();

	RenderSystem* renderer;

	// Health
	Entity player_health_text;
	Entity player_max_health_text;

	// Shield
	Entity player_shield_text;

	// Score
	Entity score_text;
	Entity funds_text;
	Entity multiplier_text;

	// Weapon slots
	Entity weapon_slot_1; // unequipped 
	Entity weapon_slot_2; // equipped
	Entity weapon_slot_3; // unequipped
	Entity weapon_slot_4; // unequipped
	Entity total_ammo_icon;
	Entity current_ammo_icon;
	Entity total_ammo_text;
	WeaponType current_weapon;

	// FPS 
	Entity fps_text;
	float fps;
	float maxFps;
	float frameTime;
	bool showingFPS;

	// Map
	std::pair<int, int> current_room;
	std::vector<Entity> drawn_rooms;

	

	void createMap(Level& level);
	void updateMap(Level& level);
	void createPlayerStatus(Health& player_health, Shield& player_shield);
	void updatePlayerStatus(Health& player_health, Shield& player_shield);
	void createScoreboard(int score, float multiplier);
	void updateScoreboard(int score, float multiplier, int deltaScore);
	void createWeaponMenu(Player& player);
	void createFirstTutorialRoomText();
	void createSecondTutorialRoomText();
	void updateWeaponMenu(Player& player);
public:
	void init(RenderSystem* renderer, Health& player_health, Shield& player_shield, Player& player, int score, float multiplier, Level& current_level);
	void reinit(Health& player_health, Shield& player_shield, Player& player, int score, float multiplier, int deltaScore, Level& level);
	void update(Health& player_health, Shield& player_shield, Player& player, int score, float multiplier, int deltaScore, bool showFPS, Level& current_level);
};