// internal
#include "weapon_system/weapon_system.hpp"

void WeaponSystem::step(float elapsed_ms, RenderSystem* renderer, Entity& player) 
{
	step_projectile_lifetime(elapsed_ms);
	step_projectile_movement(elapsed_ms);
	step_weapon_timers(elapsed_ms);

	Player& p = registry.players.get(player);
	Motion& p_m = registry.motions.get(player);
	
	// Handle reloading
	if (init_reload) {
		init_reload = false;
		//registry.texts.get(ammo_text).content = "Ammo: Reloading...";
		if (p.total_ammo_count[p.weapon_type] > 0) {
			play_sound(reload_start_sound);
			p.is_reloading = true;
		}
		else if (p.total_ammo_count[p.weapon_type] != INT_MIN) {
			play_sound(no_ammo_sound);
		}
	}
	
	if (p.is_reloading) {
		p.reload_timer_ms -= elapsed_ms;

		if (p.instant_ammo_reload || p.reload_timer_ms < 0) {
			// Reload complete, refill ammo
			p.is_reloading = false;
			p.reload_timer_ms = weapon_stats[p.weapon_type].reload_time;
			int ammo_to_refill = std::min(weapon_stats[p.weapon_type].magazine_size - p.ammo_count, p.total_ammo_count[p.weapon_type]);
			p.ammo_count += ammo_to_refill; // Refill ammo after reload
			
			p.total_ammo_count[p.weapon_type] -= ammo_to_refill;
			
			play_sound(reload_end_sound);
		}
	}
	// Handle firing
	if (p.is_firing && !p.is_reloading && p.ammo_count > 0) {
		p.fire_length_ms += elapsed_ms;
		if (p.fire_rate_timer_ms <= 0) {
			createMuzzleFlash(renderer, player);
			p.fire_rate_timer_ms = weapon_stats[p.weapon_type].fire_rate;
			float rng_gen = uniform_dist(rng);
			if (p.accuracy_boost) {
				// Reduce the spread of the bullets
				rng_gen = uniform_dist(rng) * 0.5f;
			}
			switch (p.weapon_type)
			{
			case WeaponType::GATLING_GUN:
				createProjectile(renderer, p_m.position, p_m.look_angle - M_PI / 2, rng_gen, p.fire_length_ms, player);
				play_sound(gatling_gun_sound);
				break;

			case WeaponType::SNIPER:
				createSniperProjectile(renderer, p_m.position, p_m.look_angle - M_PI / 2, rng_gen, p.fire_length_ms, player);
				play_sound(sniper_sound);
				break;

			case WeaponType::SHOTGUN:
				for (int i = 0; i < 10; i++) {
					createShotgunProjectile(renderer, p_m.position, p_m.look_angle - M_PI / 2, rng_gen, p.fire_length_ms, i, player);
				}
				play_sound(shotgun_sound);
				break;

			case WeaponType::ROCKET_LAUNCHER:
				createRocketProjectile(renderer, p_m.position, p_m.look_angle - M_PI / 2, rng_gen, p.fire_length_ms, player);
				play_sound(rocket_launcher_sound);
				break;

			case WeaponType::FLAMETHROWER:
				createFlamethrowerProjectile(renderer, p_m.position, p_m.look_angle - M_PI / 2, rng_gen, p.fire_length_ms, player);
				play_sound(flamethrower_sound);
				break;

			case WeaponType::ENERGY_HALO:
				for (int i = 0; i < 16; i++) {
					createEnergyHaloProjectile(renderer, p_m.position, p_m.look_angle - M_PI / 2, uniform_dist(rng), p.fire_length_ms, i, player);
				}
				play_sound(energy_halo_sound);
				break;

			default:
				// Handle an unknown weapon type (should never reach here, hopefully...)
				break;
			}
			p.ammo_count -= 1;
			// registry.texts.get(ammo_text).content = "Ammo: " + std::to_string(p.ammo_count) + " / " + std::to_string(p.magazine_sizes[p.weapon_type]);

			if (p.ammo_count <= 0) {
				reload_weapon();
			}
		}
	}
	if (p.is_firing && !p.is_reloading && p.ammo_count <= 0) {
		reload_weapon();
	}
	p.fire_rate_timer_ms -= elapsed_ms;
}


void WeaponSystem::step_projectile_lifetime(float elapsed_ms) 
{
	for (auto e : registry.projectiles.entities) {
		registry.projectiles.get(e).lifetime -= elapsed_ms;
		if (registry.projectiles.get(e).lifetime <= 0.0f) {
			registry.remove_all_components_of(e);
		}
	}
}

void WeaponSystem::step_projectile_movement(float elapsed_ms)
{
	for (auto e : registry.projectiles.entities) {
		if (registry.projectiles.get(e).weapon_type == WeaponType::ENERGY_HALO) {
			Motion& projectile_m = registry.motions.get(e);
			Motion& player_m = registry.motions.get(registry.players.entities[0]);

			// Calculate the new position of the projectile around the player
			float radius = 100.0f; // Adjust this radius as needed
			float angularSpeed = 0.002f; // Adjust this angular speed as needed
			float angle = projectile_m.look_angle + angularSpeed * elapsed_ms; // Increment angle over time

			// Calculate the new position relative to the player
			float newX = player_m.position.x + radius * cos(angle);
			float newY = player_m.position.y + radius * sin(angle);

			// Update the position of the projectile
			projectile_m.position = vec2({ newX, newY });

			// Update the rotation angle of the projectile to make it rotate around the player
			float rotationSpeed = 0.002f; // Adjust this rotation speed as needed
			projectile_m.look_angle += rotationSpeed * elapsed_ms;
		}
	}
}

void WeaponSystem::step_weapon_timers(float elapsed_ms) 
{
	for (Entity entity : registry.onFireTimers.entities) {
		// progress timer
		OnFireTimer& timer = registry.onFireTimers.get(entity);
		timer.counter_ms -= elapsed_ms;

		// move the fire
		registry.motions.get(timer.fire).position = registry.motions.get(entity).position;
		// registry.motions.get(timer.fire).look_angle = registry.motions.get(entity).look_angle;

		// deal dot damage
		if (registry.healths.has(entity) && !registry.players.has(entity)) {
			float dot_damage = 0.0f;
			if (timer.total_time_ms - timer.counter_ms > 0) {
				dot_damage = (timer.total_damage / timer.total_time_ms) * elapsed_ms;
			}
		
			registry.healths.get(entity).current_health -= dot_damage;
		}

		// remove fire when timer is up
		if (timer.counter_ms <= 0) {
			registry.remove_all_components_of(timer.fire);
			registry.onFireTimers.remove(entity);
		}
	}

	for (Entity entity : registry.muzzleFlashTimers.entities) {
		// progress timer
		MuzzleFlashTimer& timer = registry.muzzleFlashTimers.get(entity);
		timer.counter_ms -= elapsed_ms;

		if (timer.counter_ms <= 0 || !registry.motions.has(timer.source) || !registry.motions.has(entity)) {
			registry.muzzleFlashTimers.remove(entity);
		}
		else {
			// move the flash
			registry.motions.get(entity).position = registry.motions.get(timer.source).position + (52.f * vec2{ cos(registry.motions.get(timer.source).look_angle - M_PI/2), sin(registry.motions.get(timer.source).look_angle - M_PI/2) });
			registry.motions.get(entity).look_angle = registry.motions.get(timer.source).look_angle - M_PI;
		}
		
	}
}

void WeaponSystem::reload_weapon() 
{
	init_reload = true;
}

// Function to cycle player weapons (-1 for previous, 1 for next)
void WeaponSystem::cycle_weapon(int direction, Player& player) 
{
	// Get the current player's weapon type
	WeaponType currentWeapon = player.weapon_type;

	// Store the current ammo count
	player.magazine_ammo_count[player.weapon_type] = max(player.ammo_count, 0);

	// Get the total number of weapon types
	int numWeapons = static_cast<int>(WeaponType::TOTAL_WEAPON_TYPES);

	// Calculate the new weapon index
	int newWeaponIndex = static_cast<int>(currentWeapon) + direction;
	if (newWeaponIndex < 0) {
		newWeaponIndex = numWeapons - 1;  // Wrap around to the last weapon
	}
	else if (newWeaponIndex >= numWeapons) {
		newWeaponIndex = 0;  // Wrap around to the first weapon
	}

	// Set the new weapon type
	WeaponType newWeapon = static_cast<WeaponType>(newWeaponIndex);
	player.weapon_type = newWeapon;

	// Convert the enum value to a string for printing
	std::string weaponString;
	switch (newWeapon) {
	case WeaponType::GATLING_GUN:
		weaponString = "Weapon: Gatling Gun";
		break;
	case WeaponType::SNIPER:
		weaponString = "Weapon: Sniper";
		break;
	case WeaponType::SHOTGUN:
		weaponString = "Weapon: Shotgun";
		break;
	case WeaponType::ROCKET_LAUNCHER:
		weaponString = "Weapon: Rocket Launcher";
		break;
	case WeaponType::FLAMETHROWER:
		weaponString = "Weapon: Flamethrower";
		break;
	case WeaponType::ENERGY_HALO:
		weaponString = "Weapon: Energy Halo";
		break;
	default:
		weaponString = "Unknown Weapon";
		break;
	}
	std::cout << weaponString << std::endl;

	// Update ammo counters and reload timers
	player.ammo_count = player.magazine_ammo_count[player.weapon_type];

	play_sound(cycle_weapon_sound);
}

// Handles collisions for rockets
void WeaponSystem::handle_rocket_collision(RenderSystem* renderer, Entity projectile, Entity player)
{
	Deadly& deadly = registry.deadlies.get(projectile);
	vec2 rocket_position = registry.motions.get(projectile).position;
	Player& p = registry.players.get(player);
	int EXPLOSION_RADIUS = 50;

	createExplosion(renderer, rocket_position, 2.0f, false); // Create a BIG explosion
	play_sound(explosion_sound);

	// Look for other enemies in radius and apply damage
	for (Entity e : registry.ais.entities) {
		if (registry.healths.has(e)) {
			vec2 e_position = registry.motions.get(e).position;

			// Calculate the distance between rocket and enemy and apply damage if within
			float distance = glm::distance(rocket_position, e_position);
			if (distance <= EXPLOSION_BB_HEIGHT) {
				registry.healths.get(e).current_health -= deadly.damage;
			}
		}
	}

	// Check distance to player and apply damage if within explosion radius
	vec2 playerPosition = registry.motions.get(player).position;
	float distanceToPlayer = glm::distance(rocket_position, playerPosition);
	if (distanceToPlayer <= EXPLOSION_RADIUS) {
		// Check if the player has a shield
		if (registry.shields.has(player)) {
			// If the player has a shield, deduct damage from the shield first
			if (registry.shields.get(player).current_shield > 0) {
				registry.shields.get(player).current_shield -= 30;
			}
			else if (registry.shields.get(player).current_shield <= 0) {
				// If the shield is depleted, deduct the remaining damage from the player's health
				//registry.healths.get(player).current_health += registry.shields.get(player).current_shield;
				//registry.shields.get(player).current_shield = 0;
				registry.healths.get(player).current_health -= 10;
				if (registry.healths.get(player).current_health <= 0) {
					registry.healths.get(player).current_health = 1;
				}

			}

		}
	}
}

// Handles collisions for flamethrower
void WeaponSystem::handle_flamethrower_collision(RenderSystem* renderer, Entity projectile, Entity enemy)
{
	if (!registry.onFireTimers.has(enemy)) {
		OnFireTimer& timer = registry.onFireTimers.emplace(enemy);
		timer.fire = createFire(renderer, registry.motions.get(enemy).position, 2.0, true);
	}
}

// Check if the player hasn't unlocked a weapon type
bool is_weapon_locked(WeaponType weapon_type) {
	if (weapon_type == WeaponType::GATLING_GUN || weapon_type == WeaponType::TOTAL_WEAPON_TYPES) {
		return false;
	}

	Player& player = registry.players.components[0];
	return player.total_ammo_count[weapon_type] == INT_MIN;
}

// Unlock a weapon
bool unlock_weapon(WeaponType weapon_type) {
	if (!is_weapon_locked(weapon_type)) {
		return false;
	}

	Player& player = registry.players.components.back();
	player.total_ammo_count[weapon_type] = new_weapon_ammo_counts[weapon_type];
	player.magazine_ammo_count[weapon_type] = weapon_stats[weapon_type].magazine_size;

	return true;
}