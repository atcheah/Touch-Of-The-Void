#include <powerup_system/powerup_system.hpp>
#include <components/components.hpp>
#include <ecs_registry/ecs_registry.hpp>
#include <world_init/world_init.hpp>
#include <render_system/render_system.hpp>

// stlib
#include <random>

void PowerupSystem::init(RenderSystem* renderer_arg)
{
	renderer = renderer_arg;
}

void PowerupSystem::createPopup(PowerupType powerup)
{
	TEXTURE_ASSET_ID powerup_icon_id;
	std::string powerup_name;
	std::string powerup_description;
	switch (powerup)
	{
	case PowerupType::MAX_HEALTH:
		powerup_icon_id = TEXTURE_ASSET_ID::POWERUP_HEALTH;
		powerup_name = "BLOOD SACKS";
		powerup_description = "Doubles max health";
		break;
	case PowerupType::HEALTH_REGEN:
		powerup_icon_id = TEXTURE_ASSET_ID::POWERUP_REGEN;
		powerup_name = "EXTRA HEART";
		powerup_description = "Health regenerates";
		break;
	case PowerupType::MAX_SHIELD:
		powerup_icon_id = TEXTURE_ASSET_ID::POWERUP_SHIELD;
		powerup_name = "BIG BATTERY";
		powerup_description = "Doubles max shield";
		break;
	case PowerupType::INSTANT_AMMO_RELOAD:
		powerup_icon_id = TEXTURE_ASSET_ID::POWERUP_RELOAD;
		powerup_name = "EXTRA ARM";
		powerup_description = "Instant reloads";
		break;
	case PowerupType::DAMAGE_BOOST:
		powerup_icon_id = TEXTURE_ASSET_ID::POWERUP_DAMAGE;
		powerup_name = "HARD HITTER";
		powerup_description = "Doubles damage output";
		break;
	case PowerupType::DEFENSE_BOOST:
		powerup_icon_id = TEXTURE_ASSET_ID::POWERUP_DEFENSE;
		powerup_name = "IRON SKIN";
		powerup_description = "Halves damage taken";
		break;
	case PowerupType::SPEED_BOOST:
		powerup_icon_id = TEXTURE_ASSET_ID::POWERUP_SPEED_BOOST;
		powerup_name = "NITROUS";
		powerup_description = "1.5x speed";
		break;
	case PowerupType::MULTIPLIER_BOOST:
		powerup_icon_id = TEXTURE_ASSET_ID::POWERUP_MULTIPLIER;
		powerup_name = "BROKEN MATH";
		powerup_description = "Randomized multiplier";
		break;
	case PowerupType::ACCURACY_BOOST:
		powerup_icon_id = TEXTURE_ASSET_ID::POWERUP_ACCURACY;
		powerup_name = "NEW EYE";
		powerup_description = "Doubles accuracy";
		break;
	// TODO: implement the rest of the powerups
	case PowerupType::MAX_AMMO:
		//powerup_icon_id = TEXTURE_ASSET_ID::POWERUP_AMMO;
		break;
	case PowerupType::TIME_SLOW:
		//powerup_icon_id = TEXTURE_ASSET_ID::POWERUP_TIME_SLOW;
		break;
	case PowerupType::INSTANT_KILL:
		//powerup_icon_id = TEXTURE_ASSET_ID::POWERUP_INSTANT_KILL;
		break;
	case PowerupType::MORE_ENEMIES:
		//powerup_icon_id = TEXTURE_ASSET_ID::POWERUP_MORE_ENEMIES;
		break;
	case PowerupType::MORE_OBSTACLES:
		break;
	case PowerupType::MORE_POWERUPS:
		
		break;
	case PowerupType::BLEED:
		//powerup_icon_id = TEXTURE_ASSET_ID::POWERUP_BLEED;
		break;
	case PowerupType::BIGGER_BULLETS:
		//powerup_icon_id = TEXTURE_ASSET_ID::POWERUP_BIGGER_BULLETS;
		break;
	case PowerupType::BOOST:
		//powerup_icon_id = TEXTURE_ASSET_ID::POWERUP_BOOST;
		break;
	case PowerupType::SHUFFLER:
		//powerup_icon_id = TEXTURE_ASSET_ID::POWERUP_SHUFFLER;
		break;
	default:
		break;
	}

	currentPowerupPopup = createPowerupPopup(renderer, { 240.f, 900.f });
	currentPowerupPopupIcon = createPowerupIcon(renderer, vec2{ 80.f, 900.f }, powerup_icon_id);
	currentPowerupPopupTitleText = createText(renderer, powerup_name, {130.0f, 910.0f}, 1.0, COLOR_BRIGHT_GREEN, TextAlignment::LEFT);
	currentPowerupPopupDescriptionText = createText(renderer, powerup_description, { 30.0f, 960.0f }, 0.75, COLOR_GREEN, TextAlignment::LEFT);
}

void PowerupSystem::destroyPopup() {
	registry.remove_all_components_of(currentPowerupPopup);
	registry.remove_all_components_of(currentPowerupPopupIcon);
	registry.remove_all_components_of(currentPowerupPopupTitleText);
	registry.remove_all_components_of(currentPowerupPopupDescriptionText);
}

void PowerupSystem::awardPowerup(Entity entity, std::default_random_engine rng, std::uniform_real_distribution<float> uniform_dist)
{
	Player& player = registry.players.get(entity);
	player.powerups_collected++;

	// choose a random powerup that has not been collected yet
	std::vector<PowerupType> powerups = {
		// COMMENT OR UNCOMMENT TO ENABLE OR DISABLE POWERUPS FROM SPAWNING
		PowerupType::MAX_HEALTH,
		PowerupType::HEALTH_REGEN,
		PowerupType::MAX_SHIELD,
		PowerupType::INSTANT_AMMO_RELOAD,
		PowerupType::DAMAGE_BOOST,
		PowerupType::DEFENSE_BOOST,
		PowerupType::SPEED_BOOST,
		PowerupType::MULTIPLIER_BOOST,
		PowerupType::ACCURACY_BOOST,
		/*
		PowerupType::MAX_AMMO,
		PowerupType::TIME_SLOW,
		PowerupType::INSTANT_KILL,
		PowerupType::MORE_ENEMIES,
		PowerupType::MORE_OBSTACLES,
		PowerupType::MORE_POWERUPS,
		PowerupType::BLEED,
		PowerupType::BIGGER_BULLETS,
		PowerupType::BOOST,
		PowerupType::SHUFFLER
		*/
	};
	std::vector<PowerupType> powerups_collected = player.powerups;
	std::vector<PowerupType> powerups_not_collected;
	for (PowerupType powerup : powerups) {
		if (std::find(powerups_collected.begin(), powerups_collected.end(), powerup) == powerups_collected.end()) {
			powerups_not_collected.push_back(powerup);
		}
	}

	if (powerups_not_collected.size() > 0) {
		// maybe we should give certain powerups a higher chance of spawning than others
		std::uniform_int_distribution<int> powerup_dist(0, powerups_not_collected.size() - 1);
		PowerupType powerup = powerups_not_collected[powerup_dist(rng)];
		player.powerups.push_back(powerup);
		Motion& playerMotion = registry.motions.get(entity);
		switch (powerup) {
		case PowerupType::MAX_HEALTH:
			registry.healths.get(entity).max_health += 30;
			registry.healths.get(entity).current_health += 30;
			break;
		case PowerupType::HEALTH_REGEN:
			registry.healths.get(entity).regen_rate += 0.1f;
			break;
		case PowerupType::MAX_SHIELD:
			registry.shields.get(entity).max_shield += 100;
			registry.shields.get(entity).current_shield += 100;
			break;
		case PowerupType::INSTANT_AMMO_RELOAD:
			player.instant_ammo_reload = true;
			break;
		case PowerupType::DAMAGE_BOOST:
			player.damage_boost = true;
			break;
		case PowerupType::DEFENSE_BOOST:
			player.defense_boost = true;
			break;
		case PowerupType::SPEED_BOOST:
			playerMotion.acceleration_rate *= 1.5;
			playerMotion.deceleration_rate *= 1.5;
			playerMotion.max_velocity *= 1.5;
			break;
		case PowerupType::MULTIPLIER_BOOST:
			registry.multiplierBoostPowerupTimers.emplace(entity);
			break;
		case PowerupType::ACCURACY_BOOST:
			player.accuracy_boost = true;
			break;
		case PowerupType::MAX_AMMO:
			// TODO: implement max ammo boost
			// should boost the ammo limit of the player for all weapons
			break;
		case PowerupType::TIME_SLOW:
			// TODO: implement time slow
			// should introduce a new bullet time mechanic (slow down time, but player moves at normal speed for a short duration)
			// on the press of a button
			break;
		case PowerupType::INSTANT_KILL:
			// TODO: implement instant kill
			// bullets kill enemies instantly for a short duration on the press of a button
			break;
		case PowerupType::MORE_ENEMIES:
			// TODO: implement more enemies spawning
			// more of a curse than a powerup, but could be interesting
			// more enemies spawn in every room
			break;
		case PowerupType::MORE_OBSTACLES:
			// TODO: implement more obstacles spawning
			// more obstacles spawn in every room
			break;
		case PowerupType::MORE_POWERUPS:
			// TODO: implement more powerups spawning
			// better chance of powerups spawning on enemy death
			break;
		case PowerupType::BLEED:
			// TODO: implement bleed
			// enemies take damage over time after being hit
			break;
		case PowerupType::BIGGER_BULLETS:
			// TODO: implement bigger bullets
			// bullets are 2x bigger in size (should be goofy)
			break;
		case PowerupType::BOOST:
			// TODO: implement boost
			// player is propelled in the direction they are moving in, like a dash
			break;
		case PowerupType::SHUFFLER:
			// TODO: implement shuffler
			// rerolls all current power-ups (replaces all current power-ups with new ones, but doesn't change the number of power-ups)
			break;
		default:
			break;
		}

		destroyPopup();
		createPopup(powerup);
	}
}