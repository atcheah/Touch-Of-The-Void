#pragma once

#include "ecs/ecs.hpp"
#include "common/common.hpp"
#include "components/components.hpp"
#include "ui_init/ui_init.hpp"
#include "world_init/world_init.hpp"
#include "weapon_system/weapon_constants.hpp"
#include "shop_system/shop_constants.hpp"

enum class ITEM_TYPE {
	WEAPON = 0,
	AMMO = WEAPON + 1,
	MISC = AMMO + 1,
	TYPE_COUNT = MISC + 1
};

class ShopMenu {
private:
	static RenderSystem* renderer;

	static int items_per_list;

	static int quantity;
	static ITEM_TYPE current_page;
	static int current_list_index;
	static WeaponType current_item;
	static std::vector<WeaponType> unlocked_weapons;

	static std::vector<Entity> page_entities;
	static std::vector<Entity> list_entities;

	static std::vector<Entity> button_entities;
	static std::vector<Entity> text_entities;

	static void render_weapons_page();
	static void render_ammo_page();
	static void render_misc_page();
	
	static Entity createStatText(std::string content, std::string value, vec2 position);

	static void clear_page_entities();

public:
	static WeaponType weapon_on_sale;

	static Entity balance_text_e;
	static Entity buy_btn_e;
	static Entity increase_amount_btn_e;
	static Entity decrease_amount_btn_e;

	static void init(
		RenderSystem* renderer, 
		WeaponType weapon_on_sale,
		std::function<void()> close_shop_callback
	);

	static int on_mouse_move(vec2 mouse_position);
	static int on_mouse_click(int button, int action, int mods);

	static void close();
};
