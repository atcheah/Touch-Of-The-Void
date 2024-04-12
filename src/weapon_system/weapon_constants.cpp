#include "weapon_system/weapon_constants.hpp"
#include "components/components.hpp"

// Weapon Stats Mapping (damage, lifetime, magazine_size, reload_time, fire_rate)
std::unordered_map<WeaponType, WeaponStats> weapon_stats = {
	{WeaponType::GATLING_GUN, {20.0f, 600.0f, 30, 2000.0f, 40.0f}},
	{WeaponType::SNIPER, {500.0f, 1000.0f, 4, 1500.0f, 500.0f}},
	{WeaponType::SHOTGUN, {50.0f, 400.0f, 6, 1500.0f, 300.0f}},
	{WeaponType::ROCKET_LAUNCHER, {1000.0f, 3000.0f, 1, 2500.0f, 1000.0f}},
	{WeaponType::FLAMETHROWER, {5.0f, 600.0f, 200, 2500.0f, 10.0f}},
	{WeaponType::ENERGY_HALO, {200.0f, 10000.0f, 2, 2500.0f, 1000.0f}},
	// Add more weapons as needed
};

// Default weapon ammo count when weapon is first unlocked
std::unordered_map<WeaponType, int> new_weapon_ammo_counts = {
	{WeaponType::SNIPER, 20},
	{WeaponType::SHOTGUN, 60},
	{WeaponType::ROCKET_LAUNCHER, 5},
	{WeaponType::FLAMETHROWER, 400},
	{WeaponType::ENERGY_HALO, 8},
};
