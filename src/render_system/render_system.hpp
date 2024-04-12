#pragma once

#include <array>
#include <utility>
#include <map>

#include "common/common.hpp"
#include "components/components.hpp"
#include "ecs/ecs.hpp"

// System responsible for setting up OpenGL and for rendering all the
// visual entities in the game
class RenderSystem {
	/**
	 * The following arrays store the assets the game will use. They are loaded
	 * at initialization and are assumed to not be modified by the render loop.
	 *
	 * Whenever possible, add to these lists instead of creating dynamic state
	 * it is easier to debug and faster to execute for the computer.
	 */
	std::array<GLuint, texture_count> texture_gl_handles;
	std::array<ivec2, texture_count> texture_dimensions;
	// number of sprites per row and column in the sprite sheet
	std::array<ivec2, sheet_count> sheet_sprite_count = {
		ivec2(3,1),
		ivec2(20,1),
		ivec2(31,1),
		ivec2(2,1),
		ivec2(7,1),
		ivec2(5,1),
		ivec2(20,16),
		ivec2(19,1),
		ivec2(19,1),
		ivec2(38,1),
		ivec2(5,1),
		ivec2(6,1),
		ivec2(10,1),
		ivec2(8,1),
		ivec2(12,1),
		ivec2(6,1),
		ivec2(20,16),
		ivec2(18,1),
		ivec2(4,1),
		ivec2(8,1),
		ivec2(4,1),
		ivec2(12,1),
		ivec2(2,1),
		ivec2(17,1),
		ivec2(20,16),
		ivec2(20,16)
	};
	std::array<ivec2, sheet_count> sheet_dimensions;

	// IMPORTANT: Make sure these paths remain in sync with the associated enumerators on components.hpp
	// Associated id with .obj path
	const std::vector < std::pair<GEOMETRY_BUFFER_ID, std::string>> mesh_paths =
	{
		// TODO: specify meshes of other assets here like so:
		// ex std::pair<GEOMETRY_BUFFER_ID, std::string>(GEOMETRY_BUFFER_ID::CHICKEN, mesh_path("chicken.obj"))
		
		// player.obj contains points to a convex hull
		std::pair<GEOMETRY_BUFFER_ID, std::string>(GEOMETRY_BUFFER_ID::PLAYER_CH, mesh_path("player.obj")),
		std::pair<GEOMETRY_BUFFER_ID, std::string>(GEOMETRY_BUFFER_ID::BULLET_CH, mesh_path("bullet.obj")),
		std::pair<GEOMETRY_BUFFER_ID, std::string>(GEOMETRY_BUFFER_ID::ENEMY_SPITTER_CH, mesh_path("enemy_spitter.obj"))
	};

	// IMPORTANT: Make sure these paths remain in sync with the associated enumerators on components.hpp
	const std::array<std::string, texture_count> texture_paths = {
		// TODO: specify textures of other assets here like so:

		textures_path("ammo_placement_helper.png"),

		textures_path("bullet_16x16.png"),
		textures_path("custom_cursor_32x32.png"),

		// INSTRUCTIONS TEXTURES
		textures_path("instructions_aim_256x256.png"),
		textures_path("instructions_move_256x256.png"),

		// ENEMIES TEXTURES
		textures_path("enemy_spitter_64x64.png"),
		textures_path("enemy_turret_base_64x64.png"),
		textures_path("enemy_turret_gun_64x64.png"),

		// ICON TEXTURES
		textures_path("icon_infinity_122x54.png"),

		// LEVEL 1 TEXTURES
		textures_path("lvl1_background_960x960.png"),
		textures_path("shop_background_960x960.png"),
		textures_path("lvl1_doors_192x56.png"),
		textures_path("lvl1_full_top_wall_closed_door_960x64.png"),
		textures_path("lvl1_full_right_wall_closed_door_832x64.png"),
		textures_path("lvl1_full_bottom_wall_closed_door_960x64.png"),
		textures_path("lvl1_full_left_wall_closed_door_832x64.png"),
		textures_path("lvl1_full_top_wall_open_door_960x64.png"),
		textures_path("lvl1_full_right_wall_open_door_832x64.png"),
		textures_path("lvl1_full_bottom_wall_open_door_960x64.png"),
		textures_path("lvl1_full_left_wall_open_door_832x64.png"),
		textures_path("lvl1_full_top_wall_no_door_960x64.png"),
		textures_path("lvl1_full_right_wall_no_door_832x64.png"),
		textures_path("lvl1_full_bottom_wall_no_door_960x64.png"),
		textures_path("lvl1_full_left_wall_no_door_832x64.png"),
		textures_path("lvl1_obstacle_64x64.png"),

		// PLAYER TEXTURES
		textures_path("player_status_hud_1920x1024.png"),
		textures_path("player_64x64.png"),

		// SCREEN TEXTURES
		textures_path("screen_death_1920x1024.png"),
		textures_path("screen_start_1920x1024.png"),

		// MAP TEXTURES
		textures_path("map_icon_boss_unvisited_36x36.png"),
		textures_path("map_icon_boss_visited_36x36.png"),
		textures_path("map_icon_current_36x36.png"),
		textures_path("map_icon_shop_unvisited_36x36.png"),
		textures_path("map_icon_shop_visited_36x36.png"),
		textures_path("map_icon_unvisited_36x36.png"),
		textures_path("map_icon_visited_36x36.png"),
		textures_path("placement_helper_map.png"),

		// WEAPON TEXTURES
		textures_path("weapon_icon_flame_thrower_equipped_192x192.png"),
		textures_path("weapon_icon_flame_thrower_unequipped_128x128.png"),
		textures_path("weapon_icon_gatling_gun_equipped_192x192.png"),
		textures_path("weapon_icon_gatling_gun_unequipped_128x128.png"),
		textures_path("weapon_icon_rocket_launcher_equipped_192x192.png"),
		textures_path("weapon_icon_rocket_launcher_unequipped_128x128.png"),
		textures_path("weapon_icon_shotgun_blast_equipped_192x192.png"),
		textures_path("weapon_icon_shotgun_blast_unequipped_128x128.png"),
		textures_path("weapon_icon_sniper_rifle_equipped_192x192.png"),
		textures_path("weapon_icon_sniper_rifle_unequipped_128x128.png"),
		textures_path("weapon_icon_energy_halo_equipped_192x192.png"),
		textures_path("weapon_icon_energy_halo_unequipped_128x128.png"),

		// POWERUP TEXTURES
		textures_path("upgrades/power_up_ability_boost_64x64.png"),
		textures_path("upgrades/power_up_acc_boost_64x64.png"),
		textures_path("upgrades/power_up_bigger_bullets_64x64.png"),
		textures_path("upgrades/power_up_bleed_64x64.png"),
		textures_path("upgrades/power_up_def_boost_64x64.png"),
		textures_path("upgrades/power_up_dmg_boost_64x64.png"),
		textures_path("upgrades/power_up_hp_boost_64x64.png"),
		textures_path("upgrades/power_up_hp_regen_64x64.png"),
		textures_path("upgrades/power_up_instant_ammo_reload_64x64.png"),
		textures_path("upgrades/power_up_insta_kill_64x64.png"),
		textures_path("upgrades/power_up_max_ammo_64x64.png"),
		textures_path("upgrades/power_up_max_shield_64x64.png"),
		textures_path("upgrades/power_up_more_enemies_64x64.png"),
		textures_path("upgrades/power_up_multiplier_64x64.png"),
		textures_path("upgrades/power_up_shuffler_64x64.png"),
		textures_path("upgrades/power_up_speed_boost_64x64.png"),
		textures_path("upgrades/power_up_time_slow_64x64.png"),
		textures_path("end_screen.png"),

		// Generic UI textures
		textures_path("start_button_640x128.png"),
		textures_path("active_up_button_64x64.png"),
		textures_path("inactive_up_button_64x64.png"),
		textures_path("active_down_button_64x64.png"),
		textures_path("inactive_down_button_64x64.png")

	};

	std::array<GLuint, effect_count> effects;
	// IMPORTANT: Make sure these paths remain in sync with the associated enumerators on components.hpp
	const std::array<std::string, effect_count> effect_paths = {
		// TODO: specify shader scripts here like so:
		shader_path("coloured"),
		shader_path("textured"),
		shader_path("post_process"),
		shader_path("line")
	};

	std::array<GLuint, font_count> fonts;
	// IMPORTANT: Make sure these paths remain in sync with the associated enumerators on components.hpp
	const std::array<std::string, font_count> font_paths = {
		fonts_path("Vermin_Vibes_1989.ttf")
	};

	std::array<GLuint, sheet_count> sheets;
	// IMPORTANT: Make sure these paths remain in sync with the associated enumerators on components.hpp
	const std::array<std::string, sheet_count> sheet_paths = {
		sheets_path("ammo_energy_halo_160x98.png"),
		sheets_path("ammo_flame_thrower_160x98.png"),
		sheets_path("ammo_gatling_gun_160x98.png"),
		sheets_path("ammo_rocket_launcher_160x98.png"),
		sheets_path("ammo_shotgun_160x98.png"),
		sheets_path("ammo_sniper_160x98.png"),
		sheets_path("blue_effect_bullet_impact_explosion_32x32.png"),
		sheets_path("enemy_boss_idle_128x128.png"),
		sheets_path("enemy_boss_shield_128x128.png"),
		sheets_path("enemy_boss_spawning_128x128.png"),
		sheets_path("enemy_drill_64x64.png"),
		sheets_path("enemy_droid_64x64.png"),
		sheets_path("enemy_drone_64x64.png"),
		sheets_path("enemy_fire_scarab_64x64.png"),
		sheets_path("explosion_192x192.png"),
		sheets_path("exploding_skull_64x64.png"),
		sheets_path("green_effect_bullet_impact_explosion_32x32.png"),
		sheets_path("instructions_reload_256x256.png"),
		sheets_path("instructions_scroll_256x256.png"),
		sheets_path("instructions_shoot_256x256.png"),
		sheets_path("instructions_switch_256x256.png"),
		sheets_path("player_sprite_64x64.png"),
		sheets_path("power_up_popup_464x232.png"),
		sheets_path("power_up_random_64x64.png"),
		//sheets_path("purple_effect_bullet_impact_explosion_32x32.png"),
		sheets_path("red_effect_bullet_impact_explosion_32x32.png"),
		sheets_path("yellow_effect_bullet_impact_explosion_32x32.png")
	};

	std::array<GLuint, geometry_count> vertex_buffers;
	std::array<GLuint, geometry_count> index_buffers;
	std::array<Mesh, geometry_count> meshes;

public:
	// Initialize the window
	bool init(GLFWwindow* window);

	template <class T>
	void bindVBOandIBO(GEOMETRY_BUFFER_ID gid, std::vector<T> vertices, std::vector<uint16_t> indices);

	void initializeGlTextures();

	void initializeGlSheets();

	void initializeGlEffects();

	void initializeGlMeshes();

	bool initializeFonts();


	Mesh& getMesh(GEOMETRY_BUFFER_ID id) { return meshes[(int)id]; };

	void initializeGlGeometryBuffers();
	// Initialize the screen texture used as intermediate render target
	// The draw loop first renders to this texture, then it is used for the wind
	// shader
	bool initScreenTexture();

	// Destroy resources associated to one or all entities created by the system
	~RenderSystem();

	// Draw all entities
	void draw();

	// Draw all text entities
	void drawText(const mat3& projection, bool use_framebuffer);

	mat3 createProjectionMatrix();

	bool loadEffectFromFile(
		const std::string& vs_path, const std::string& fs_path, GLuint& out_program);

	bool loadFontFromFile(
		const std::string& font_path, unsigned int font_default_size);

	std::map<char, Character> m_ftCharacters;
	float default_font_size = 48.f;

private:
	// Internal drawing functions for each entity type
	void drawTexturedMesh(Entity entity, const mat3& projection);
	void drawToScreen(const mat3& projection);

	// Window handle
	GLFWwindow* window;

	// Screen texture handles
	GLuint frame_buffer;
	GLuint off_screen_render_buffer_color;
	GLuint off_screen_render_buffer_depth;

	Entity screen_state_entity;

	GLuint vao;
	GLuint vbo;

	// Fonts
	GLuint m_font_shaderProgram;
	GLuint m_font_VAO;
	GLuint m_font_VBO;

	// Sprite Sheets

	std::array<std::map<std::pair<int, int>, Sprite>, sheet_count> m_ftSpriteSheets;
};