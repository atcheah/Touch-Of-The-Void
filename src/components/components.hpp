#pragma once
#include <iostream>
#include <map>

#include "common/common.hpp"
#include "weapon_system/weapon_constants.hpp"

#include <vector>
#include <unordered_map>
#include "../ext/stb_image/stb_image.h"

enum class PowerupType {
	MAX_HEALTH,
	HEALTH_REGEN,
	MAX_SHIELD,
	INSTANT_AMMO_RELOAD,
	DAMAGE_BOOST,
	DEFENSE_BOOST,
	SPEED_BOOST,
	MULTIPLIER_BOOST,
	ACCURACY_BOOST,
	MAX_AMMO,
	TIME_SLOW,
	INSTANT_KILL,
	MORE_ENEMIES,
	MORE_OBSTACLES,
	MORE_POWERUPS,
	BLEED,
	BIGGER_BULLETS,
	BOOST,
	SHUFFLER
};

struct Level {
	// the current level of the game
	int current_level = 1;
	// the 2d array of rooms in the level
	std::map<std::pair<int, int>, Entity> rooms;
	// the current room the player is in
	std::pair<int, int> current_room;
	// the number of rooms the player needs to clear until the boss appears
	int num_rooms_until_boss = 10;
	// number of rooms the player has cleared
	int num_rooms_cleared = 0;
	// number of unique rooms the player has visited
	int num_rooms_visited = 0;
	
	// number of rooms cleared since last shop is spawned
	int num_shop_spawn_counter = 0;
	// number of shop levels spawned in
	int num_shop_spawned = 0;
};

struct vec2comp {
	bool operator() (vec2 lhs, vec2 rhs) const
	{
		if (lhs.x < rhs.x) return true;
		if (lhs.x == rhs.x && lhs.y < rhs.y) return true;
		return false;
	}
};


// Room type
enum class ROOM_TYPE {
	UNDEFINED_ROOM = 0,
	NORMAL_ROOM = UNDEFINED_ROOM + 1,
	BOSS_ROOM = NORMAL_ROOM + 1,
	SHOP_ROOM = BOSS_ROOM + 1
};

// All data relevant to the contents of a game room
struct Room {

	Room()
	{
		//std::cout << "Room constructor:" << std::addressof(this->is_cleared) << std::endl;
	}

	~Room()
	{
		//std::cout << "Room deconstructor:" << std::addressof(this->is_cleared) << std::endl;
	}

	ROOM_TYPE room_type = ROOM_TYPE::UNDEFINED_ROOM;

	bool is_cleared = false; // if the room has been cleared of enemies, can contain upgrade
	bool is_visited = false; // if the room has been visited
	bool is_boss_room = false; // if the room contains a boss
	// The number of enemies in the room
	int enemy_count = 0;

	// the positions of all entities in the room
	std::set<vec2, vec2comp> all_positions;

	// the positions of the enemies in the room
	std::set<vec2, vec2comp> enemy_positions;

	// The number of obstacles in the room
	int obstacle_count = 0;
	// the positions of the obstacles in the room
	std::set<vec2, vec2comp> obstacle_positions;

	WeaponType weapon_on_sale = WeaponType::TOTAL_WEAPON_TYPES;

	// The number of powerups in the room
	//int powerup_count = 0;
	// the positions of the powerups in the room
	//std::set<vec2, vec2comp> powerup_positions;

	// fields concerning the doors of the room
	bool has_left_door = false;
	bool has_right_door = false;
	bool has_top_door = false;
	bool has_bottom_door = false;

	bool is_tutorial_room = false;
};

// access panel to the shop
struct ShopPanel {
	WeaponType weapon_on_sale;
};

// Cannot move component
struct Immobile {
};

// Player component
struct Player
{
	// Store the total ammo count for each weapon
	std::unordered_map<WeaponType, int> total_ammo_count = {
		{WeaponType::GATLING_GUN, INT_MAX},
		{WeaponType::SNIPER, 20},
		{WeaponType::SHOTGUN, 60},
		{WeaponType::ROCKET_LAUNCHER, INT_MIN}, // 5
		{WeaponType::FLAMETHROWER, 400},
		{WeaponType::ENERGY_HALO, 8},
	};

	//std::unordered_map<WeaponType, int> total_ammo_count = {
	//	{WeaponType::GATLING_GUN, INT_MAX},
	//	{WeaponType::SNIPER, INT_MIN},
	//	{WeaponType::SHOTGUN, INT_MIN},
	//	{WeaponType::ROCKET_LAUNCHER, INT_MIN},
	//	{WeaponType::FLAMETHROWER, INT_MIN},
	//	{WeaponType::ENERGY_HALO, INT_MIN},
	//};

	// Store the current ammo count for each weapon
	std::unordered_map<WeaponType, int> magazine_ammo_count = {
		{WeaponType::GATLING_GUN, 100},
		{WeaponType::SNIPER, 4},
		{WeaponType::SHOTGUN, 6},
		{WeaponType::ROCKET_LAUNCHER, INT_MIN}, // 1
		{WeaponType::FLAMETHROWER, 200},
		{WeaponType::ENERGY_HALO, 2},
	};

	//std::unordered_map<WeaponType, int> magazine_ammo_count = {
	//	{WeaponType::GATLING_GUN, 100},
	//	{WeaponType::SNIPER, INT_MIN},
	//	{WeaponType::SHOTGUN, INT_MIN},
	//	{WeaponType::ROCKET_LAUNCHER, INT_MIN},
	//	{WeaponType::FLAMETHROWER, INT_MIN},
	//	{WeaponType::ENERGY_HALO, INT_MIN},
	//};

	bool is_firing = false; // player is currently firing projectiles
	float fire_length_ms = 0.0f; // time the player has been firing
	float fire_rate_timer_ms = 0.0f; // timer for fully-automatic weapons
	WeaponType weapon_type = WeaponType::GATLING_GUN;
	int max_ammo_count;
	int ammo_count;
	bool is_reloading = false; // player is currently reloading and cannot fire
	float reload_timer_ms = 0.0f;
	int gold_balance = 0;

	// if the player is moving between rooms
	bool is_moving_rooms = false;

	float rotation_factor = 0.0f; // used to choose frame of sprite to display

	std::vector<PowerupType> powerups;
	int powerups_collected = 0;
	bool instant_ammo_reload = false;
	bool damage_boost = false;
	bool defense_boost = false; // only makes shield stronger, not health at the moment
	bool accuracy_boost = false;
	// Constructor to set the initial values
	Player() : 
		max_ammo_count(weapon_stats[weapon_type].magazine_size),
		ammo_count(max_ammo_count),
		reload_timer_ms(weapon_stats[weapon_type].reload_time) {}
};

// Projectile component
struct Projectile
{
	float lifetime = 0.0f;	// time before the projectile disappears
	Entity source; // New attribute to store the source entity of the projectile
	WeaponType weapon_type;
};

// Burned status timer
struct OnFireTimer
{
	float total_damage = 300.0f; // amount of damage that will be dealt over counter_ms
	float counter_ms = 2000.0f;
	float total_time_ms = 2000.0f;

	Entity fire;
};

struct MuzzleFlashTimer
{
	float counter_ms = 0.0f;
	Entity source;
};

// Obstacle component
struct Obstacle
{
	// player can pass through this obstacle (i,e. a door)
	bool is_passable = false;

	// if this is a door, which one
	bool is_top_door = false;
	bool is_bottom_door = false;
	bool is_left_door = false;
	bool is_right_door = false;

	// if this is a door, is it locked
	bool is_locked_door = true;
	// if this is a wall
	bool is_wall = false;
};

// No collision check indicator component (for background)
struct NoCollisionCheck 
{

};

struct AI
{
	enum class AIType { MELEE, RANGED, TURRET, SHOTGUN, ROCKET, FLAMETHROWER };
	AIType type = AIType::MELEE;
	enum class AIState { IDLE, ACTIVE };
	AIState state = AIState::ACTIVE;
	float safe_distance = 150.0f; // the distance that the AI will start behaving from the player
	float attack_distance = 100.0f; // the distance that the AI will start attacking the player
	float shootingCooldown = 0.0f; // time in seconds before the next shot can be made for ranged enemies
	int frequency = 0; // frequency of think cycles
	int counter = 0; // counter for think cycles
	bool in_boss_room = false; // if the AI is in a boss room
};

struct BossAI
{
	enum class BossState {DEFENSIVE, OFFENSIVE,GUIDED_MISSILE};
	BossState state = BossState::DEFENSIVE;
	float shootTimer = 0.0f; // Timer to control shooting rate
	float shootCooldown = 1.0f; // Cooldown in seconds between shots
	float missileTimer = 0.0f; // Timer to control missile creation rate
	float missileCooldown = 2.0f; // Cooldown in seconds between missile creation
	float stateTimer = 0.0f; // Timer to track time in the current state
	float stateDuration = 10.0f; // Duration to spend in each state before switching
	int aliveEnemyCount = 0; // New member to track the number of alive enemies
	float enemyCreationTimer = 0.0f; // Timer for enemy creation in the offensive state
	float enemyCreationCooldown = 1.5f; // Cooldown for enemy creation
	int totalSpawnedEnemies = 0; // New member to track the number of alive enemies
	float zigzagTimer = 0.0f; // Timer to track when to switch directions
	float zigzagInterval = 4.0f; // Interval in seconds between direction changes
	glm::vec2 zigzagDirection = glm::vec2(50.0f, 50.0f); // Initial direction and speed
};

// Harmful collision component
struct Deadly
{
	float damage = 0.0f; // damage to be dealt to the other entity on collision
};

// Health component 
struct Health
{
	float current_health = 100.0f; // health points of an entity
	float max_health = 100.0f; // maximum health points of an entity
	float regen_rate = 0.0f; // rate at which the health regenerates
};

// Shield component 
struct Shield
{
	float current_shield = 100.0f; // shield points of an entity
	float max_shield = 100.0f; // maximum shield points of an entity
	float recharge_rate = 0.0f; // rate at which the shield recharges
	float recharge_delay = 0.0f; // delay before the shield starts recharging
};

// All data relevant to the shape and motion of entities
struct Motion {
	vec2 position = { 0, 0 };
	vec2 velocity = { 0, 0 };
	vec2 scale = { 10, 10 };

	vec2 previous_position = {-1, -1};

	float look_angle = 0; // angle the entity is looking at

	bool complex = false; // if the entity has complex motion, i.e. not just a straight line

	bool is_moving_up = false;
	bool is_moving_down = false;
	bool is_moving_left = false;
	bool is_moving_right = false;

	float acceleration_rate = 0.0f;	// amount to be added to velocity if not moving in direction
	float deceleration_rate = 0.0f; // amount to be shaved off velocity if moving in direction
	float max_velocity = 0.0f; 	// maximum velocity in any direction
	float turn_rate = 0.0f;		// how fast the entity can turn
	float turn_speed = 0.0f;		// how fast the entity is currently turning

	bool is_passable = false; // if the entity is passable (cannot be collided with)
};

// Stucture to store collision information
struct Collision
{
	// Note, the first object is stored in the ECS container.entities
	Entity other; // the second object involved in the collision
	Collision(Entity& other) { this->other = other; };

	// Scalar of the displacement vector
	float scalar;
};

// Data structure for toggling debug mode
struct Debug 
{
	bool in_debug_mode = 0;
	bool in_freeze_mode = 0;
	bool show_fps = 0;
};
extern Debug debugging;

// Sets the brightness of the screen
struct ScreenState
{
	float darken_screen_factor = -1;
};

// A struct to refer to debugging graphics in the ECS
struct DebugComponent
{
	// Note, an empty struct has size 1
};

struct DamagedTimer
{
	float damage = 0.0f;
	float counter_ms = 0.0f;
};

// A timer that will be associated to dying player
struct DeathTimer
{
	float counter_ms = 3000;
};

// A timer that will be associated to player moving rooms
struct RoomTransitionTimer
{
	float counter_ms = 0;
};

struct EarnedGoldTimer {
	float counter_ms = 0;
	float duration_ms = 1000;
	float gold_earned = 0;
};

// Single Vertex Buffer element for non-textured meshes (coloured.vs.glsl & chicken.vs.glsl)
struct ColoredVertex
{
	vec3 position;
	vec3 color;
};

// Single Vertex Buffer element for textured sprites (textured.vs.glsl)
struct TexturedVertex
{
	vec3 position;
	vec2 texcoord;
};

// Mesh datastructure for storing vertex and index buffers
struct Mesh
{
	static bool loadFromOBJFile(std::string obj_path, std::vector<ColoredVertex>& out_vertices, std::vector<uint16_t>& out_vertex_indices, vec2& out_size);
	vec2 original_size = { 1,1 };
	std::vector<ColoredVertex> vertices;
	std::vector<uint16_t> vertex_indices;
};

enum class TextAlignment { LEFT, RIGHT, CENTER };

struct Text
{
	std::string content;
	vec3 color;
	vec2 rect_size;
	TextAlignment alignment = TextAlignment::LEFT;
};

// A structure to store the font data of a single character
struct Character {
	unsigned int TextureID;  // ID handle of the glyph texture
	glm::ivec2   Size;       // Size of glyph
	glm::ivec2   Bearing;    // Offset from baseline to left/top of glyph
	unsigned int Advance;    // Offset to advance to next glyph
	char character;
};

struct Button {
	Entity text_entity;
	bool hover = false;	
	bool disabled = false;
	std::function<void()> on_click = []() {};
	std::function<void(void)> on_mouse_in = []() {};
	std::function<void(void)> on_mouse_out = []() {};
};

// A structure to store the data concerning a single sprite sheet texture
struct Sprite {
	unsigned int TextureID;
	vec2 minTexCoords;
	vec2 maxTexCoords;
};

// Empty shell for powerup pickup
struct PowerupRandom {
};

struct MultiplierBoostPowerupTimer {
	float counter_ms = 500.0f; 
};

struct PowerupPopUp {
	float counter_ms = 0.0f; // the counter until destruction of the pop-up
};

/**
 * The following enumerators represent global identifiers refering to graphic
 * assets. For example TEXTURE_ASSET_ID are the identifiers of each texture
 * currently supported by the system.
 *
 * So, instead of referring to a game asset directly, the game logic just
 * uses these enumerators and the RenderRequest struct to inform the renderer
 * how to structure the next draw command.
 *
 * There are 2 reasons for this:
 *
 * First, game assets such as textures and meshes are large and should not be
 * copied around as this wastes memory and runtime. Thus separating the data
 * from its representation makes the system faster.
 *
 * Second, it is good practice to decouple the game logic from the render logic.
 * Imagine, for example, changing from OpenGL to Vulkan, if the game logic
 * depends on OpenGL semantics it will be much harder to do the switch than if
 * the renderer encapsulates all asset data and the game logic is agnostic to it.
 *
 * The final value in each enumeration is both a way to keep track of how many
 * enums there are, and as a default value to represent uninitialized fields.
 */

enum class TEXTURE_ASSET_ID {
	AMMO_PLACEMENT_HELPER = 0,
	// Projectile textures
	BULLET = AMMO_PLACEMENT_HELPER + 1, // 0,
	CURSOR = BULLET + 1,

	// Instructional widget icons
	INSTRUCTION_AIM_ICON = CURSOR + 1,
	INSTRUCTION_MOVE_ICON = INSTRUCTION_AIM_ICON + 1,
	
	// Enemy textures
	ENEMY_SPITTER = INSTRUCTION_MOVE_ICON + 1,
	ENEMY_TURRET_BASE = ENEMY_SPITTER + 1,
	ENEMY_TURRET_GUN = ENEMY_TURRET_BASE + 1,

	// Ammo icon textures
	INFINITY_AMMO = ENEMY_TURRET_GUN + 1,

	// Level 1 textures
	LEVEL1_BACKGROUND = INFINITY_AMMO + 1,
	SHOP_BACKGROUND = LEVEL1_BACKGROUND + 1,
	LEVEL1_DOORS = SHOP_BACKGROUND + 1,
	TOP_LEVEL1_FULL_WALL_CLOSED_DOOR = LEVEL1_DOORS + 1,
	RIGHT_LEVEL1_FULL_WALL_CLOSED_DOOR = TOP_LEVEL1_FULL_WALL_CLOSED_DOOR + 1,
	BOTTOM_LEVEL1_FULL_WALL_CLOSED_DOOR = RIGHT_LEVEL1_FULL_WALL_CLOSED_DOOR + 1,
	LEFT_LEVEL1_FULL_WALL_CLOSED_DOOR = BOTTOM_LEVEL1_FULL_WALL_CLOSED_DOOR + 1,
	TOP_LEVEL1_FULL_WALL_OPEN_DOOR = LEFT_LEVEL1_FULL_WALL_CLOSED_DOOR + 1,
	RIGHT_LEVEL1_FULL_WALL_OPEN_DOOR = TOP_LEVEL1_FULL_WALL_OPEN_DOOR + 1,
	BOTTOM_LEVEL1_FULL_WALL_OPEN_DOOR = RIGHT_LEVEL1_FULL_WALL_OPEN_DOOR + 1,
	LEFT_LEVEL1_FULL_WALL_OPEN_DOOR = BOTTOM_LEVEL1_FULL_WALL_OPEN_DOOR + 1,
	TOP_LEVEL1_FULL_WALL_NO_DOOR = LEFT_LEVEL1_FULL_WALL_OPEN_DOOR + 1,
	RIGHT_LEVEL1_FULL_WALL_NO_DOOR = TOP_LEVEL1_FULL_WALL_NO_DOOR + 1,
	BOTTOM_LEVEL1_FULL_WALL_NO_DOOR = RIGHT_LEVEL1_FULL_WALL_NO_DOOR + 1,
	LEFT_LEVEL1_FULL_WALL_NO_DOOR = BOTTOM_LEVEL1_FULL_WALL_NO_DOOR + 1,
	LEVEL1_OBSTACLE = LEFT_LEVEL1_FULL_WALL_NO_DOOR + 1,
	//LEVEL1_WALL_BOTTOM_CORNER = LEVEL1_OBSTACLE + 1,
	//LEVEL1_WALL_END = LEVEL1_WALL_BOTTOM_CORNER + 1,
	//LEVEL1_WALL_TOP_CORNER = LEVEL1_WALL_END + 1,
	//LEVEL1_WALL = LEVEL1_WALL_TOP_CORNER + 1,

	// Player textures
	PLAYER_STATUS_HUD = LEVEL1_OBSTACLE + 1,
	PLAYER = PLAYER_STATUS_HUD + 1,

	// SCREEN TEXTURES
	DEATH_SCREEN = PLAYER + 1,
	START_SCREEN = DEATH_SCREEN + 1,

	// Map icon textures
	BOSS_ROOM_UNVISITED = START_SCREEN + 1,
	BOSS_ROOM_VISITED = BOSS_ROOM_UNVISITED + 1,
	CURRENT_ROOM = BOSS_ROOM_VISITED + 1,
	SHOP_ROOM_UNVISITED = CURRENT_ROOM + 1,
	SHOP_ROOM_VISITED = SHOP_ROOM_UNVISITED + 1,
	UNVISITED_ROOM = SHOP_ROOM_VISITED + 1,
	VISITED_ROOM = UNVISITED_ROOM + 1,
	MAP_PLACEMENT_HELPER = VISITED_ROOM + 1,

	// Weapon icon textures
	FLAME_THROWER_EQUIPPED = MAP_PLACEMENT_HELPER + 1,
	FLAME_THROWER_UNEQUIPPED = FLAME_THROWER_EQUIPPED + 1,
	GATLING_GUN_EQUIPPED = FLAME_THROWER_UNEQUIPPED + 1,
	GATLING_GUN_UNEQUIPPED = GATLING_GUN_EQUIPPED + 1,
	ROCKET_LAUNCHER_EQUIPPED = GATLING_GUN_UNEQUIPPED + 1,
	ROCKET_LAUNCHER_UNEQUIPPED = ROCKET_LAUNCHER_EQUIPPED + 1,
	SHOTGUN_EQUIPPED = ROCKET_LAUNCHER_UNEQUIPPED + 1,
	SHOTGUN_UNEQUIPPED = SHOTGUN_EQUIPPED + 1,
	SNIPER_EQUIPPED = SHOTGUN_UNEQUIPPED + 1,
	SNIPER_UNEQUIPPED = SNIPER_EQUIPPED + 1,
	ENERGY_HALO_EQUIPPED = SNIPER_UNEQUIPPED + 1,
	ENERGY_HALO_UNEQUIPPED = ENERGY_HALO_EQUIPPED + 1,

	// Powerup icon textures
	POWERUP_BOOST = ENERGY_HALO_UNEQUIPPED + 1,
	POWERUP_ACCURACY = POWERUP_BOOST + 1,
	POWERUP_BIGGER_BULLETS = POWERUP_ACCURACY + 1,
	POWERUP_BLEED = POWERUP_BIGGER_BULLETS + 1,
	POWERUP_DEFENSE = POWERUP_BLEED + 1,
	POWERUP_DAMAGE = POWERUP_DEFENSE + 1,
	POWERUP_HEALTH = POWERUP_DAMAGE + 1,
	POWERUP_REGEN = POWERUP_HEALTH + 1,
	POWERUP_RELOAD = POWERUP_REGEN + 1,
	POWERUP_INSTANT_KILL = POWERUP_RELOAD + 1,
	POWERUP_AMMO = POWERUP_INSTANT_KILL + 1,
	POWERUP_SHIELD = POWERUP_AMMO + 1,
	POWERUP_MORE_ENEMIES = POWERUP_SHIELD + 1,
	POWERUP_MULTIPLIER = POWERUP_MORE_ENEMIES + 1,
	POWERUP_SHUFFLER = POWERUP_MULTIPLIER + 1,
	POWERUP_SPEED_BOOST = POWERUP_SHUFFLER + 1,
	POWERUP_TIME_SLOW = POWERUP_SPEED_BOOST + 1,
	WIN_SCREEN = POWERUP_TIME_SLOW + 1,
	
	// Generic UI textures
	START_BUTTON = WIN_SCREEN + 1,
	ACTIVE_UP_BUTTON = START_BUTTON + 1,
	INACTIVE_UP_BUTTON = ACTIVE_UP_BUTTON + 1,
	ACTIVE_DOWN_BUTTON = INACTIVE_UP_BUTTON + 1,
	INACTIVE_DOWN_BUTTON = ACTIVE_DOWN_BUTTON + 1,

	TEXTURE_COUNT = INACTIVE_DOWN_BUTTON + 1
};
const int texture_count = (int)TEXTURE_ASSET_ID::TEXTURE_COUNT;

enum class FONT_ASSET_ID {
	VERMIN_VIBES_1989 = 0,
	FONT_COUNT = VERMIN_VIBES_1989 + 1
};
const int font_count = (int)FONT_ASSET_ID::FONT_COUNT;

enum class EFFECT_ASSET_ID {
	COLOURED = 0,
	TEXTURED = COLOURED + 1,
	POST_PROCESS = TEXTURED + 1,
	LINE = POST_PROCESS + 1,
	EFFECT_COUNT = LINE + 1
};
const int effect_count = (int)EFFECT_ASSET_ID::EFFECT_COUNT;

enum class GEOMETRY_BUFFER_ID {
	SPRITE = 0,
	SCREEN_TRIANGLE = SPRITE + 1,
	PLAYER_CH = SCREEN_TRIANGLE + 1,
	BULLET_CH = PLAYER_CH + 1,
	ENEMY_SPITTER_CH = BULLET_CH + 1,
	DEBUG_LINE = ENEMY_SPITTER_CH + 1,
	GEOMETRY_COUNT = DEBUG_LINE + 1
};
const int geometry_count = (int)GEOMETRY_BUFFER_ID::GEOMETRY_COUNT;

enum class SPRITE_SHEET_ID {
	AMMO_ENERGY_HALO = 0,
	AMMO_FLAMETHROWER = AMMO_ENERGY_HALO + 1,
	AMMO_GATLING_GUN = AMMO_FLAMETHROWER + 1,
	AMMO_ROCKET_LAUNCHER = AMMO_GATLING_GUN + 1,
	AMMO_SHOTGUN = AMMO_ROCKET_LAUNCHER + 1,
	AMMO_SNIPER = AMMO_SHOTGUN + 1,
	BLUE_EFFECT = AMMO_SNIPER + 1,
	ENEMY_BOSS_IDLE = BLUE_EFFECT + 1,
	ENEMY_BOSS_SHIELD = ENEMY_BOSS_IDLE + 1,
	ENEMY_BOSS_SPAWN = ENEMY_BOSS_SHIELD + 1,
	ENEMY_DRILL = ENEMY_BOSS_SPAWN + 1,
	ENEMY_DROID = ENEMY_DRILL + 1,
	ENEMY_DRONE = ENEMY_DROID + 1,
	ENEMY_SCARAB = ENEMY_DRONE + 1,
	EXPLOSION = ENEMY_SCARAB + 1,
	ENEMY_EXPLODER = EXPLOSION + 1,
	GREEN_EFFECT = ENEMY_EXPLODER + 1,
	INSTRUCTION_RELOAD = GREEN_EFFECT + 1,
	INSTRUCTION_SCROLL = INSTRUCTION_RELOAD + 1,
	INSTRUCTION_SHOOT = INSTRUCTION_SCROLL + 1,
	INSTRUCTION_SWITCH = INSTRUCTION_SHOOT + 1,
	PLAYER = INSTRUCTION_SWITCH + 1,
	POWERUP_POPUP = PLAYER + 1,
	POWERUP = POWERUP_POPUP + 1,
	//PURPLE_EFFECT = GREEN_EFFECT + 1,
	RED_EFFECT = POWERUP + 1,
	YELLOW_EFFECT = RED_EFFECT + 1,
	SPRITE_SHEET_COUNT = YELLOW_EFFECT + 1
};
const int sheet_count = (int)SPRITE_SHEET_ID::SPRITE_SHEET_COUNT;

enum class RENDER_LAYER {
	BACKGROUND = 0,
	MIDDLEGROUND = BACKGROUND + 1,
	FOREGROUND = MIDDLEGROUND + 1,
	UI = FOREGROUND + 1,
	GAME_MENU = UI + 1,
	CURSOR = GAME_MENU + 1,
	RENDER_LAYER_COUNT = CURSOR + 1
};
const int render_layer_count = (int)RENDER_LAYER::RENDER_LAYER_COUNT;

struct RenderRequest {
	TEXTURE_ASSET_ID used_texture = TEXTURE_ASSET_ID::TEXTURE_COUNT;
	EFFECT_ASSET_ID used_effect = EFFECT_ASSET_ID::EFFECT_COUNT;
	GEOMETRY_BUFFER_ID used_geometry = GEOMETRY_BUFFER_ID::GEOMETRY_COUNT;
	RENDER_LAYER used_render_layer = RENDER_LAYER::RENDER_LAYER_COUNT;
};

// A structure to store the data concerning a animation where each frame is a sprite, and the time to display each frame is variable
struct Animation {
	std::vector<std::pair<int, int>> sprites;
	std::vector<float> frame_durations_ms;
	SPRITE_SHEET_ID sheet_id;
	float total_frames;
	float current_frame;
	bool loop;
};

struct AnimationTimer
{
	float counter_ms = 0.0f;
};

// A struct to store data that should only exist in tutorial room.
struct TutorialOnly {

};
