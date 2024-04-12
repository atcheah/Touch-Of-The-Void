#include "shop_system/shop_constants.hpp"

// (texture, base weapon price, ammo price per bullet, weapon name)
std::unordered_map<WeaponType, WeaponShopInfo> weapon_shop_infos = {
	{WeaponType::SNIPER, {TEXTURE_ASSET_ID::SNIPER_EQUIPPED, 9, 1, "Sniper"}},
	{WeaponType::SHOTGUN, {TEXTURE_ASSET_ID::SHOTGUN_EQUIPPED, 9, 1, "Shotgun"}},
	{WeaponType::ROCKET_LAUNCHER, {TEXTURE_ASSET_ID::ROCKET_LAUNCHER_EQUIPPED, 9, 2, "Rocket Launcher"}},
	{WeaponType::FLAMETHROWER, {TEXTURE_ASSET_ID::FLAME_THROWER_EQUIPPED, 9, 1, "Flamethrower"}},
	{WeaponType::ENERGY_HALO, {TEXTURE_ASSET_ID::ENERGY_HALO_EQUIPPED, 9, 1, "Energy Halo"}}
};

// price table of the miscellaneous items
std::vector<int> misc_shop_prices = {
	10
};