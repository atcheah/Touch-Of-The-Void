#pragma once

#include <unordered_map>
#include <vector>

#include "components/components.hpp"
#include "weapon_system/weapon_constants.hpp"

struct WeaponShopInfo {
	TEXTURE_ASSET_ID texture;
	int weapon_price;
	int price_per_ammo;
	std::string weapon_name;
};

enum class MiscType {
	HEALTH = 0
};

// Weapon price mapping (name, texture, base weapon price, ammo price per ammo
extern std::unordered_map<WeaponType, WeaponShopInfo> weapon_shop_infos;
extern std::vector<int> misc_shop_prices;
extern std::unordered_map<WeaponType, int> new_weapon_ammo_counts;
