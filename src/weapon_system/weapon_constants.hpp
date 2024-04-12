#pragma once

#include <unordered_map>

// Weapon Types
enum class WeaponType {
	GATLING_GUN,
	SNIPER,
	SHOTGUN,
	ROCKET_LAUNCHER,
	FLAMETHROWER,
	ENERGY_HALO,
	TOTAL_WEAPON_TYPES // Keep this as the last element
};

// Weapon Stats
struct WeaponStats {
	float damage;
	float lifetime;
	int magazine_size;
	float reload_time;
	float fire_rate;
};

extern std::unordered_map<WeaponType, WeaponStats> weapon_stats;
extern std::unordered_map<WeaponType, int> new_weapon_ammo_counts;