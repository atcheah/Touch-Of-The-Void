#include "ui_system/ui_system.hpp"
#include "render_system/render_system.hpp"
#include "world_init/world_init.hpp"
#include "ecs_registry/ecs_registry.hpp"
#include "audio_manager/audio_manager.hpp"
#include "weapon_system/weapon_system.hpp"

#include <iomanip>
#include <iostream>
#include <algorithm>

float top_left_corner_x = 208.f, top_left_corner_y = 290.f, box_width = 36.f, box_height = 36.f;
void UISystem::createMap(Level& level) {
	//auto positionhelper = Entity();

	//Motion& helpermotion = registry.motions.emplace(positionhelper);
	//helpermotion.position = { 208.f, 290.f };
	//helpermotion.scale = vec2({ 384.f, 384.f });

	//registry.renderRequests.insert(
	//	positionhelper,
	//	{ TEXTURE_ASSET_ID::MAP_PLACEMENT_HELPER,
	//			 EFFECT_ASSET_ID::TEXTURED,
	//			 GEOMETRY_BUFFER_ID::SPRITE,
	//			RENDER_LAYER::UI });

	
	std::pair<int, int> current_room_coords = level.current_room;
	int max_step = 4;

	auto entity = Entity();

	Motion& motion = registry.motions.emplace(entity);
	motion.position = { top_left_corner_x, top_left_corner_y };
	motion.scale = vec2({ box_width, box_height });

	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::CURRENT_ROOM,
						 EFFECT_ASSET_ID::TEXTURED,
						 GEOMETRY_BUFFER_ID::SPRITE,
						RENDER_LAYER::UI });

	for (int y = -max_step; y <= max_step; y++) {
		for (int x = -max_step; x <= max_step; x++) {
			std::pair<int, int> room_coords = std::pair<int, int>(current_room_coords.first + x, current_room_coords.second + y);
			if (room_coords.first == current_room_coords.first && room_coords.second == current_room_coords.second) {
				continue;
			}


			if (level.rooms.find(room_coords) != level.rooms.end()) {
				assert(registry.rooms.has(level.rooms[room_coords]) && "Room does not exist in registry");
				Room& room = registry.rooms.get(level.rooms[room_coords]);
				auto entity = Entity();
				Motion& motion = registry.motions.emplace(entity);
				motion.position = { top_left_corner_x + (x * box_width), top_left_corner_y - (y * box_height) };
				motion.scale = vec2({ box_width, box_height });

				switch (room.room_type) {
					case ROOM_TYPE::UNDEFINED_ROOM:
					case ROOM_TYPE::NORMAL_ROOM:
						if (room.is_visited) {
							registry.renderRequests.insert(
								entity,
								{ TEXTURE_ASSET_ID::VISITED_ROOM,
													EFFECT_ASSET_ID::TEXTURED,
													GEOMETRY_BUFFER_ID::SPRITE,
												RENDER_LAYER::UI });
						}
						else {
							registry.renderRequests.insert(
								entity,
								{ TEXTURE_ASSET_ID::UNVISITED_ROOM,
													EFFECT_ASSET_ID::TEXTURED,
													GEOMETRY_BUFFER_ID::SPRITE,
												RENDER_LAYER::UI });
						}
						break;
					case ROOM_TYPE::BOSS_ROOM:
						if (room.is_visited) {
							registry.renderRequests.insert(
								entity,
								{ TEXTURE_ASSET_ID::BOSS_ROOM_VISITED,
																					EFFECT_ASSET_ID::TEXTURED,
																					GEOMETRY_BUFFER_ID::SPRITE,
																				RENDER_LAYER::UI });
						}
						else {
							registry.renderRequests.insert(
								entity,
								{ TEXTURE_ASSET_ID::BOSS_ROOM_UNVISITED,
																					EFFECT_ASSET_ID::TEXTURED,
																					GEOMETRY_BUFFER_ID::SPRITE,
																				RENDER_LAYER::UI });
						}
						break;
					case ROOM_TYPE::SHOP_ROOM:
						if (room.is_visited) {
							registry.renderRequests.insert(
								entity,
								{ TEXTURE_ASSET_ID::SHOP_ROOM_VISITED,
																													EFFECT_ASSET_ID::TEXTURED,
																													GEOMETRY_BUFFER_ID::SPRITE,
																												RENDER_LAYER::UI });
						}
						else {
							registry.renderRequests.insert(
								entity,
								{ TEXTURE_ASSET_ID::SHOP_ROOM_UNVISITED,
																													EFFECT_ASSET_ID::TEXTURED,
																													GEOMETRY_BUFFER_ID::SPRITE,
																												RENDER_LAYER::UI });
						}
				}

				drawn_rooms.push_back(entity);
			}
		}
	}
}

void UISystem::updateMap(Level& level) {
	std::pair<int, int> current_room_coords = level.current_room;
	if (current_room == current_room_coords) {
		return;
	}
	else {
		current_room = current_room_coords;
	}
	int max_step = 4;

	// Clear the map
	for (auto entity : drawn_rooms) {
		registry.remove_all_components_of(entity);
	}

	for (int y = -max_step; y <= max_step; y++) {
		for (int x = -max_step; x <= max_step; x++) {
			std::pair<int, int> room_coords = std::pair<int, int>(current_room_coords.first + x, current_room_coords.second + y);
			if (room_coords.first == current_room_coords.first && room_coords.second == current_room_coords.second) {
				continue;
			}
			if (level.rooms.find(room_coords) != level.rooms.end()) {
				assert(registry.rooms.has(level.rooms[room_coords]) && "Room does not exist in registry");
				Room& room = registry.rooms.get(level.rooms[room_coords]);
				auto entity = Entity();

				Motion& motion = registry.motions.emplace(entity);
				motion.position = { top_left_corner_x + (x * box_width), top_left_corner_y - (y * box_height) };
				motion.scale = vec2({ box_width, box_height });

				switch (room.room_type) {
				case ROOM_TYPE::UNDEFINED_ROOM:
				case ROOM_TYPE::NORMAL_ROOM:
					if (room.is_visited) {
						registry.renderRequests.insert(
							entity,
							{ TEXTURE_ASSET_ID::VISITED_ROOM,
												EFFECT_ASSET_ID::TEXTURED,
												GEOMETRY_BUFFER_ID::SPRITE,
											RENDER_LAYER::UI });
					}
					else {
						registry.renderRequests.insert(
							entity,
							{ TEXTURE_ASSET_ID::UNVISITED_ROOM,
												EFFECT_ASSET_ID::TEXTURED,
												GEOMETRY_BUFFER_ID::SPRITE,
											RENDER_LAYER::UI });
					}
					break;
				case ROOM_TYPE::BOSS_ROOM:
					if (room.is_visited) {
						registry.renderRequests.insert(
							entity,
							{ TEXTURE_ASSET_ID::BOSS_ROOM_VISITED,
																				EFFECT_ASSET_ID::TEXTURED,
																				GEOMETRY_BUFFER_ID::SPRITE,
																			RENDER_LAYER::UI });
					}
					else {
						registry.renderRequests.insert(
							entity,
							{ TEXTURE_ASSET_ID::BOSS_ROOM_UNVISITED,
																				EFFECT_ASSET_ID::TEXTURED,
																				GEOMETRY_BUFFER_ID::SPRITE,
																			RENDER_LAYER::UI });
					}
					break;
				case ROOM_TYPE::SHOP_ROOM:
					if (room.is_visited) {
						registry.renderRequests.insert(
							entity,
							{ TEXTURE_ASSET_ID::SHOP_ROOM_VISITED,
																												EFFECT_ASSET_ID::TEXTURED,
																												GEOMETRY_BUFFER_ID::SPRITE,
																											RENDER_LAYER::UI });
					}
					else {
						registry.renderRequests.insert(
							entity,
							{ TEXTURE_ASSET_ID::SHOP_ROOM_UNVISITED,
																												EFFECT_ASSET_ID::TEXTURED,
																												GEOMETRY_BUFFER_ID::SPRITE,
																											RENDER_LAYER::UI });
					}
				}

				drawn_rooms.push_back(entity);
			}
		}
	}
}	

float health_x = 150.f, health_y = 654.0f, max_health_x = 245.f, max_health_y = 654.0f, shield_x = 150.f, shield_y = 739.f;
void UISystem::createPlayerStatus(Health& player_health, Shield& player_shield) {
	// health 
	int roundedHealth = std::max(0, static_cast<int>(player_health.current_health));
	std::string healthText = std::to_string(roundedHealth);
	player_health_text = createText(renderer, healthText, { health_x, health_y }, 1.5f, COLOR_BRIGHT_GREEN, TextAlignment::LEFT);
	int roundedMaxHealth = std::max(0, static_cast<int>(player_health.max_health));
	std::string maxHealthText = "/ " + std::to_string(roundedMaxHealth);
	player_max_health_text = createText(renderer, maxHealthText, { max_health_x, max_health_y }, 1.5f, COLOR_DARK_GREEN, TextAlignment::LEFT);

	// shield
	int roundedShield = std::max(0, static_cast<int>(player_shield.current_shield));
	int roundedMaxShield = std::max(0, static_cast<int>(player_shield.max_shield));
	int percentage = (int)(((float)roundedShield / (float)roundedMaxShield) * 100.f); // percentage of shield remaining
	std::string shieldText = std::to_string(percentage) + " %";
	player_shield_text = createText(renderer, shieldText, { shield_x, shield_y }, 1.5f, COLOR_BRIGHT_GREEN, TextAlignment::LEFT);
}

void UISystem::updatePlayerStatus(Health& player_health, Shield& player_shield) {
	// Update Health
	int roundedHealth = std::max(0, static_cast<int>(player_health.current_health));
	std::string healthText = std::to_string(roundedHealth);
	registry.texts.get(player_health_text).content = healthText;

	int roundedMaxHealth = std::max(0, static_cast<int>(player_health.max_health));
	std::string maxHealthText = "/ " + std::to_string(roundedMaxHealth);
	registry.texts.get(player_max_health_text).content = maxHealthText;

	// Update Shield
	int roundedShield = std::max(0, static_cast<int>(player_shield.current_shield));
	int roundedMaxShield = std::max(0, static_cast<int>(player_shield.max_shield));
	int percentage = (int)(((float)roundedShield / (float)roundedMaxShield) * 100.f);
	std::string shieldText = std::to_string(percentage) + " %";
	registry.texts.get(player_shield_text).content = shieldText;
}

float score_x = 1798.0f, score_y = 932.0f, multiplier_x = 1545.0f, multiplier_y = 919.0f;
size_t n_zero = 4;
void UISystem::createScoreboard(int score, float multiplier) {
	// score
	std::string scoreText = std::to_string(score);
	scoreText = std::string(n_zero - std::min(n_zero, scoreText.length()), '0') + scoreText; 	// one liner to pad with zeros taken from https://stackoverflow.com/questions/6143824/add-leading-zeroes-to-string-without-sprintf
	score_text = createText(renderer, scoreText, { score_x, score_y }, 1.5f, COLOR_GREEN, TextAlignment::RIGHT);
	// multiplier
	std::string multiplierText = std::to_string(multiplier);
	multiplierText = multiplierText.substr(0, multiplierText.find(".") + 2); // 2 decimal places
	multiplier_text = createText(renderer, multiplierText, { multiplier_x, multiplier_y }, 1.0f, COLOR_BLACK, TextAlignment::LEFT);
}

void UISystem::updateScoreboard(int score, float multiplier, int deltaScore) {
	// Update Score
	std::string scoreText = std::to_string(score);
	scoreText = std::string(n_zero - std::min(n_zero, scoreText.length()), '0') + scoreText; 	// one liner to pad with zeros taken from https://stackoverflow.com/questions/6143824/add-leading-zeroes-to-string-without-sprintf
	registry.texts.get(score_text).content = scoreText;

	// Update Multiplier
	std::string multiplierText = std::to_string(multiplier);
	multiplierText = multiplierText.substr(0, multiplierText.find(".") + 2);
	registry.texts.get(multiplier_text).content = multiplierText;
}

const int window_center_px_x = window_width_px / 2;
const int window_center_px_y = window_height_px / 2;
// game window top left x
float x_origin = (window_width_px / 2) - (game_window_size_px / 2) + 32;
// game window top left y
float y_origin = (window_height_px / 2) - (game_window_size_px / 2) + 32;
float x_max = x_origin + game_window_size_px - game_window_block_size;
float y_max = y_origin + game_window_size_px - game_window_block_size;
void UISystem::createFirstTutorialRoomText() {
	Entity moveWidget = createMoveTutorialWidget(renderer, { x_origin + (3 * 64), y_origin + (3 * 64) });
	registry.tutorialOnlys.emplace(moveWidget);

	Entity aimWidget = createAimTutorialWidget(renderer, { x_max - (3 * 64), y_origin + (3 * 64) });
	registry.tutorialOnlys.emplace(aimWidget);

	Entity shootWidget = createShootTutorialWidget(renderer, { x_origin + (3 * 64), y_max - (3 * 64) });
	registry.tutorialOnlys.emplace(shootWidget);

	Entity reloadWidget = createReloadTutorialWidget(renderer, { x_max - (3 * 64), y_max - (3 * 64) });
	registry.tutorialOnlys.emplace(reloadWidget);
}
void UISystem::createSecondTutorialRoomText() {
	Entity scrollWidget = createScrollTutorialWidget(renderer, { x_origin + (3 * 64), y_origin + (3 * 64) });
	registry.tutorialOnlys.emplace(scrollWidget);

	Entity switchWidget = createSwitchTutorialWidget(renderer, { x_max - (3 * 64), y_origin + (3 * 64) });
	registry.tutorialOnlys.emplace(switchWidget);
}

// Weapon slots
float equipped_x = 1616.0f, unequipped_x = 1648.0f, slot_1_y = 130.0f, slot_2_y = 354.0f, slot_3_y = 578.0f, slot_4_y = 738.0f;
// ammo
float ammo_x = 1810.0f, current_ammo_y = 305.0f, total_ammo_icon_y = 405.0f, total_ammo_text_y = 425.0f;
void UISystem::createWeaponMenu(Player& player) {
	current_ammo_icon = createCurrentAmmoIcon(renderer, { ammo_x, current_ammo_y }, player);

	std::string total_ammo_count_content;
	if (is_weapon_locked(player.weapon_type)) {
		total_ammo_count_content = "N/A";
	} else {
		total_ammo_count_content = std::to_string(player.total_ammo_count[player.weapon_type]);
	}

	// TODO: Will need to change once we have weapon pickups
	switch (player.weapon_type) {
	case WeaponType::GATLING_GUN:
		weapon_slot_1 = createWeaponUnequippedIcon(renderer, { unequipped_x, slot_1_y }, TEXTURE_ASSET_ID::ENERGY_HALO_UNEQUIPPED); // 1st slot EQUIPPED
		weapon_slot_2 = createWeaponEquippedIcon(renderer, { equipped_x, slot_2_y }, TEXTURE_ASSET_ID::GATLING_GUN_EQUIPPED); // 2nd slot EQUIPPED
		weapon_slot_3 = createWeaponUnequippedIcon(renderer, { unequipped_x, slot_3_y }, TEXTURE_ASSET_ID::SNIPER_UNEQUIPPED); // 3rd slot UN-EQUIPPED
		weapon_slot_4 = createWeaponUnequippedIcon(renderer, { unequipped_x, slot_4_y }, TEXTURE_ASSET_ID::SHOTGUN_UNEQUIPPED); // 4th slot UN-EQUIPPED
		total_ammo_icon = createIconInfinity(renderer, { ammo_x, total_ammo_icon_y });
		break;
	case WeaponType::SNIPER:
		weapon_slot_1 = createWeaponUnequippedIcon(renderer, { unequipped_x, slot_1_y }, TEXTURE_ASSET_ID::GATLING_GUN_UNEQUIPPED); // 1st slot EQUIPPED
		weapon_slot_2 = createWeaponEquippedIcon(renderer, { equipped_x, slot_2_y }, TEXTURE_ASSET_ID::SNIPER_EQUIPPED); // 2nd slot EQUIPPED
		weapon_slot_3 = createWeaponUnequippedIcon(renderer, { unequipped_x, slot_3_y }, TEXTURE_ASSET_ID::SHOTGUN_UNEQUIPPED); // 3rd slot UN-EQUIPPED
		weapon_slot_4 = createWeaponUnequippedIcon(renderer, { unequipped_x, slot_4_y }, TEXTURE_ASSET_ID::ROCKET_LAUNCHER_UNEQUIPPED); // 4th slot UN-EQUIPPED
		total_ammo_text = createText(renderer, total_ammo_count_content, { ammo_x, total_ammo_text_y }, 1.5f, COLOR_BLACK, TextAlignment::CENTER);
		break;
	case WeaponType::SHOTGUN:
		weapon_slot_1 = createWeaponUnequippedIcon(renderer, { unequipped_x, slot_1_y }, TEXTURE_ASSET_ID::SNIPER_UNEQUIPPED); // 1st slot EQUIPPED
		weapon_slot_2 = createWeaponEquippedIcon(renderer, { equipped_x, slot_2_y }, TEXTURE_ASSET_ID::SHOTGUN_EQUIPPED); // 2nd slot EQUIPPED
		weapon_slot_3 = createWeaponUnequippedIcon(renderer, { unequipped_x, slot_3_y }, TEXTURE_ASSET_ID::ROCKET_LAUNCHER_UNEQUIPPED); // 3rd slot UN-EQUIPPED
		weapon_slot_4 = createWeaponUnequippedIcon(renderer, { unequipped_x, slot_4_y }, TEXTURE_ASSET_ID::FLAME_THROWER_UNEQUIPPED); // 4th slot UN-EQUIPPED
		total_ammo_text = createText(renderer, total_ammo_count_content, { ammo_x, total_ammo_text_y }, 1.5f, COLOR_BLACK, TextAlignment::CENTER);
		break;
	case WeaponType::ROCKET_LAUNCHER:
		weapon_slot_1 = createWeaponUnequippedIcon(renderer, { unequipped_x, slot_1_y }, TEXTURE_ASSET_ID::SHOTGUN_UNEQUIPPED); // 1st slot EQUIPPED
		weapon_slot_2 = createWeaponEquippedIcon(renderer, { equipped_x, slot_2_y }, TEXTURE_ASSET_ID::ROCKET_LAUNCHER_EQUIPPED); // 2nd slot EQUIPPED
		weapon_slot_3 = createWeaponUnequippedIcon(renderer, { unequipped_x, slot_3_y }, TEXTURE_ASSET_ID::FLAME_THROWER_UNEQUIPPED); // 3rd slot UN-EQUIPPED
		weapon_slot_4 = createWeaponUnequippedIcon(renderer, { unequipped_x, slot_4_y }, TEXTURE_ASSET_ID::ENERGY_HALO_UNEQUIPPED); // 4th slot UN-EQUIPPED
		total_ammo_text = createText(renderer, total_ammo_count_content, { ammo_x, total_ammo_text_y }, 1.5f, COLOR_BLACK, TextAlignment::CENTER);
		break;
	case WeaponType::FLAMETHROWER:
		weapon_slot_1 = createWeaponUnequippedIcon(renderer, { unequipped_x, slot_1_y }, TEXTURE_ASSET_ID::ROCKET_LAUNCHER_UNEQUIPPED); // 1st slot EQUIPPED
		weapon_slot_2 = createWeaponEquippedIcon(renderer, { equipped_x, slot_2_y }, TEXTURE_ASSET_ID::FLAME_THROWER_EQUIPPED); // 2nd slot EQUIPPED
		weapon_slot_3 = createWeaponUnequippedIcon(renderer, { unequipped_x, slot_3_y }, TEXTURE_ASSET_ID::ENERGY_HALO_UNEQUIPPED); // 3rd slot UN-EQUIPPED
		weapon_slot_4 = createWeaponUnequippedIcon(renderer, { unequipped_x, slot_4_y }, TEXTURE_ASSET_ID::GATLING_GUN_UNEQUIPPED); // 4th slot UN-EQUIPPED
		total_ammo_text = createText(renderer, total_ammo_count_content, { ammo_x, total_ammo_text_y }, 1.5f, COLOR_BLACK, TextAlignment::CENTER);
		break;
	case WeaponType::ENERGY_HALO:
		weapon_slot_1 = createWeaponUnequippedIcon(renderer, { unequipped_x, slot_1_y }, TEXTURE_ASSET_ID::FLAME_THROWER_UNEQUIPPED); // 1st slot EQUIPPED
		weapon_slot_2 = createWeaponEquippedIcon(renderer, { equipped_x, slot_2_y }, TEXTURE_ASSET_ID::ENERGY_HALO_EQUIPPED); // 2nd slot EQUIPPED
		weapon_slot_3 = createWeaponUnequippedIcon(renderer, { unequipped_x, slot_3_y }, TEXTURE_ASSET_ID::GATLING_GUN_UNEQUIPPED); // 3rd slot UN-EQUIPPED
		weapon_slot_4 = createWeaponUnequippedIcon(renderer, { unequipped_x, slot_4_y }, TEXTURE_ASSET_ID::SNIPER_UNEQUIPPED); // 4th slot UN-EQUIPPED
		total_ammo_text = createText(renderer, total_ammo_count_content, { ammo_x, total_ammo_text_y }, 1.5f, COLOR_BLACK, TextAlignment::CENTER);
		break;
	case WeaponType::TOTAL_WEAPON_TYPES:
		break;
	default:
		break;
	}
}

void UISystem::updateWeaponMenu(Player& player) {
	std::string total_ammo_count_content;
	if (is_weapon_locked(player.weapon_type)) {
		total_ammo_count_content = "N/A";
	} else {
		total_ammo_count_content = std::to_string(player.total_ammo_count[player.weapon_type]);
	}
	
	if (player.weapon_type == current_weapon) {
		assert(registry.animations.has(current_ammo_icon) && "Current ammo icon does not exist in registry");
		Animation& ammo_animation = registry.animations.get(current_ammo_icon);

		switch (player.weapon_type) {
		case WeaponType::GATLING_GUN:
			ammo_animation.current_frame = player.ammo_count;
			break;
		case WeaponType::SNIPER:
		case WeaponType::SHOTGUN:
		case WeaponType::ROCKET_LAUNCHER:
		case WeaponType::ENERGY_HALO:
			assert(registry.texts.has(total_ammo_text) && "Total ammo text does not exist in registry");
			registry.texts.get(total_ammo_text).content = total_ammo_count_content;
			ammo_animation.current_frame = max(0, player.ammo_count);
			break;
		case WeaponType::FLAMETHROWER:
			assert(registry.texts.has(total_ammo_text) && "Total ammo text does not exist in registry");
			registry.texts.get(total_ammo_text).content = total_ammo_count_content;
			// flamethrower has 200 ammo, but we only have 20 frames in the sprite sheet (0-19)
			if (player.ammo_count == 200) {
				ammo_animation.current_frame = 19;
			}
			else if (player.ammo_count >= 0) {
				ammo_animation.current_frame = player.ammo_count / 10;
			} else {
				ammo_animation.current_frame = 0;
			}
			break;
		case WeaponType::TOTAL_WEAPON_TYPES:
		default:
			break;
		}
	}
	else {
		if (current_weapon == WeaponType::GATLING_GUN) {
			// remove infinity icon from gatling gun as it is no longer equipped
			registry.remove_all_components_of(total_ammo_icon);
			total_ammo_text = createText(renderer, total_ammo_count_content, { ammo_x, total_ammo_text_y }, 1.5f, COLOR_BLACK, TextAlignment::CENTER);
		}
		current_weapon = player.weapon_type;
		Animation& ammo_animation = registry.animations.get(current_ammo_icon);

		int animation_frame_idx;
		if (is_weapon_locked(current_weapon)) {
			animation_frame_idx = 0;
		} else {
			animation_frame_idx = player.ammo_count;
		}

		switch (player.weapon_type) {
		case WeaponType::GATLING_GUN:
			registry.renderRequests.get(weapon_slot_1).used_texture = TEXTURE_ASSET_ID::ENERGY_HALO_UNEQUIPPED; // 1st slot EQUIPPED
			registry.renderRequests.get(weapon_slot_2).used_texture = TEXTURE_ASSET_ID::GATLING_GUN_EQUIPPED; // 2nd slot EQUIPPED
			registry.renderRequests.get(weapon_slot_3).used_texture = TEXTURE_ASSET_ID::SNIPER_UNEQUIPPED; // 3rd slot UN-EQUIPPED
			registry.renderRequests.get(weapon_slot_4).used_texture = TEXTURE_ASSET_ID::SHOTGUN_UNEQUIPPED; // 4th slot UN-EQUIPPED
			registry.texts.get(total_ammo_text).content = "";
			total_ammo_icon = createIconInfinity(renderer, { ammo_x, total_ammo_icon_y });
			ammo_animation.sheet_id = SPRITE_SHEET_ID::AMMO_GATLING_GUN;
			ammo_animation.current_frame = animation_frame_idx;
			break;
		case WeaponType::SNIPER:
			registry.renderRequests.get(weapon_slot_1).used_texture = TEXTURE_ASSET_ID::GATLING_GUN_UNEQUIPPED; // 1st slot EQUIPPED
			registry.renderRequests.get(weapon_slot_2).used_texture = TEXTURE_ASSET_ID::SNIPER_EQUIPPED; // 2nd slot EQUIPPED
			registry.renderRequests.get(weapon_slot_3).used_texture = TEXTURE_ASSET_ID::SHOTGUN_UNEQUIPPED; // 3rd slot UN-EQUIPPED
			registry.renderRequests.get(weapon_slot_4).used_texture = TEXTURE_ASSET_ID::ROCKET_LAUNCHER_UNEQUIPPED; // 4th slot UN-EQUIPPED
			registry.texts.get(total_ammo_text).content = total_ammo_count_content;
			ammo_animation.sheet_id = SPRITE_SHEET_ID::AMMO_SNIPER;
			ammo_animation.current_frame = animation_frame_idx;
			break;
		case WeaponType::SHOTGUN:
			registry.renderRequests.get(weapon_slot_1).used_texture = TEXTURE_ASSET_ID::SNIPER_UNEQUIPPED; // 1st slot EQUIPPED
			registry.renderRequests.get(weapon_slot_2).used_texture = TEXTURE_ASSET_ID::SHOTGUN_EQUIPPED; // 2nd slot EQUIPPED
			registry.renderRequests.get(weapon_slot_3).used_texture = TEXTURE_ASSET_ID::ROCKET_LAUNCHER_UNEQUIPPED; // 3rd slot UN-EQUIPPED
			registry.renderRequests.get(weapon_slot_4).used_texture = TEXTURE_ASSET_ID::FLAME_THROWER_UNEQUIPPED; // 4th slot UN-EQUIPPED
			registry.texts.get(total_ammo_text).content = total_ammo_count_content;
			ammo_animation.sheet_id = SPRITE_SHEET_ID::AMMO_SHOTGUN;
			ammo_animation.current_frame = animation_frame_idx;
			break;
		case WeaponType::ROCKET_LAUNCHER:
			registry.renderRequests.get(weapon_slot_1).used_texture = TEXTURE_ASSET_ID::SHOTGUN_UNEQUIPPED; // 1st slot EQUIPPED
			registry.renderRequests.get(weapon_slot_2).used_texture = TEXTURE_ASSET_ID::ROCKET_LAUNCHER_EQUIPPED; // 2nd slot EQUIPPED
			registry.renderRequests.get(weapon_slot_3).used_texture = TEXTURE_ASSET_ID::FLAME_THROWER_UNEQUIPPED; // 3rd slot UN-EQUIPPED
			registry.renderRequests.get(weapon_slot_4).used_texture = TEXTURE_ASSET_ID::ENERGY_HALO_UNEQUIPPED; // 4th slot UN-EQUIPPED
			registry.texts.get(total_ammo_text).content = total_ammo_count_content;
			ammo_animation.sheet_id = SPRITE_SHEET_ID::AMMO_ROCKET_LAUNCHER;
			ammo_animation.current_frame = animation_frame_idx;
			break;
		case WeaponType::FLAMETHROWER:
			registry.renderRequests.get(weapon_slot_1).used_texture = TEXTURE_ASSET_ID::ROCKET_LAUNCHER_UNEQUIPPED; // 1st slot EQUIPPED
			registry.renderRequests.get(weapon_slot_2).used_texture = TEXTURE_ASSET_ID::FLAME_THROWER_EQUIPPED; // 2nd slot EQUIPPED
			registry.renderRequests.get(weapon_slot_3).used_texture = TEXTURE_ASSET_ID::ENERGY_HALO_UNEQUIPPED; // 3rd slot UN-EQUIPPED
			registry.renderRequests.get(weapon_slot_4).used_texture = TEXTURE_ASSET_ID::GATLING_GUN_UNEQUIPPED; // 4th slot UN-EQUIPPED
			registry.texts.get(total_ammo_text).content = total_ammo_count_content;
			ammo_animation.sheet_id = SPRITE_SHEET_ID::AMMO_FLAMETHROWER;
			if (is_weapon_locked(current_weapon)) {
				ammo_animation.current_frame = animation_frame_idx;
			} else if (player.ammo_count == 200) {
				ammo_animation.current_frame = 19;
			}
			else {
				ammo_animation.current_frame = player.ammo_count / 10;
			}
			break;
		case WeaponType::ENERGY_HALO:
			registry.renderRequests.get(weapon_slot_1).used_texture = TEXTURE_ASSET_ID::FLAME_THROWER_UNEQUIPPED; // 1st slot EQUIPPED
			registry.renderRequests.get(weapon_slot_2).used_texture = TEXTURE_ASSET_ID::ENERGY_HALO_EQUIPPED; // 2nd slot EQUIPPED
			registry.renderRequests.get(weapon_slot_3).used_texture = TEXTURE_ASSET_ID::GATLING_GUN_UNEQUIPPED; // 3rd slot UN-EQUIPPED
			registry.renderRequests.get(weapon_slot_4).used_texture = TEXTURE_ASSET_ID::SNIPER_UNEQUIPPED; // 4th slot UN-EQUIPPED
			registry.texts.get(total_ammo_text).content = total_ammo_count_content;
			ammo_animation.sheet_id = SPRITE_SHEET_ID::AMMO_ENERGY_HALO;
			ammo_animation.current_frame = animation_frame_idx;
			break;
		case WeaponType::TOTAL_WEAPON_TYPES:
			break;
		default:
			break;
		}
	}
}

float fps_x = 1760.0f, fps_y = 30.0f;
void UISystem::fpsCalculate() {
	//average these many samples to change the frames smoother
	static const int num_samples = 10;
	//buffer, array of the frames
	static float frameTimes[num_samples];
	static int currentFrame = 0;

	static float prevTicks = SDL_GetTicks();

	float currentTicks;
	currentTicks = SDL_GetTicks();

	frameTime = currentTicks - prevTicks;//note: first few ticks will be wrong
	frameTimes[currentFrame % num_samples] = frameTime;

	//need to set ticks again. after current tick is done, it becomes prev ticks
	prevTicks = currentTicks;
	int count;
	//	currentFrame++;

	if (currentFrame < num_samples) {
		count = currentFrame;
	}
	else {
		count = num_samples;
	}

	float averageFrameTime = 0;
	for (int i = 0; i < count; i++) {
		averageFrameTime += frameTimes[i];

	}
	averageFrameTime /= count;

	if (averageFrameTime > 0) {
		fps = 1000.0f / averageFrameTime;
		// ms/s divided by ms/f returns frames/second which is fps
	}
	else {
		//shouldn't ever reach this else case 
		fps = 60.0f;
	}
	currentFrame++;
}

void UISystem::init(RenderSystem* renderer_arg, Health& player_health, Shield& player_shield, Player& player, int score, float multiplier, Level& current_level)
{
	this->renderer = renderer_arg;
	maxFps = 144.0f;
	current_room = std::pair<int, int>(0, 0);
	current_weapon = player.weapon_type;
	// create template container
	createStatusHud(renderer);
	// create ui elements
	createMap(current_level);
	createPlayerStatus(player_health, player_shield);
	createScoreboard(score, multiplier);
	createWeaponMenu(player);
	createFirstTutorialRoomText();
}

void UISystem::update(Health& player_health, Shield& player_shield, Player& player, int score, float multiplier, int deltaScore, bool showFPS, Level& level)
{
	updateMap(level);
	updatePlayerStatus(player_health, player_shield);
	updateScoreboard(score, multiplier, deltaScore);
	updateWeaponMenu(player);

	// handle fps
	if (showFPS) {
		if (!showingFPS) {
			fps_text = createText(renderer, "", { fps_x, fps_y }, 0.8f, { 0.0f, 1.0f, 1.0f }, TextAlignment::LEFT);
			showingFPS = true;
		}
		// FPS
		float startTicks = SDL_GetTicks();
		fpsCalculate();
		static int frameCounter = 0;
		frameCounter++;
		// update fps every 50 frames 
		if (frameCounter == 25) {
			registry.texts.get(fps_text).content = "FPS: " + std::to_string(static_cast<int>(fps));
			frameCounter = 0;
		}
		float frameTicks = SDL_GetTicks() - startTicks;
	}
	else {
		if (showingFPS) {
			registry.remove_all_components_of(fps_text);
			showingFPS = false;
		}
	}

}

void UISystem::reinit(Health& player_health, Shield& player_shield, Player& player, int score, float multiplier, int deltaScore, Level& level) {
	// remove all components of the UI for safety
	registry.remove_all_components_of(player_health_text);
	registry.remove_all_components_of(player_max_health_text);
	registry.remove_all_components_of(player_shield_text);
	registry.remove_all_components_of(score_text);
	registry.remove_all_components_of(multiplier_text);
	registry.remove_all_components_of(weapon_slot_1);
	registry.remove_all_components_of(weapon_slot_2);
	registry.remove_all_components_of(weapon_slot_3);
	registry.remove_all_components_of(weapon_slot_4);
	registry.remove_all_components_of(total_ammo_icon);
	registry.remove_all_components_of(current_ammo_icon);
	registry.remove_all_components_of(total_ammo_text);
	registry.remove_all_components_of(fps_text);

	// reinitialize the UI
	createStatusHud(renderer);
	createPlayerStatus(player_health, player_shield);
	createScoreboard(score, multiplier);
	createWeaponMenu(player);
	
	// if re-entering tutorial room (0,0).
	bool is_tutorial_room = registry.rooms.get(level.rooms[level.current_room]).is_tutorial_room;
	if (is_tutorial_room) {
		if (level.current_room.first == 0 && level.current_room.second == 0) {
			createFirstTutorialRoomText();
		}
		else {
			createSecondTutorialRoomText();
		}
		
	}
	
}