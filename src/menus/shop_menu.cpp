#include <iostream>

#include "shop_menu.hpp"
#include "shop_system/shop_constants.hpp"
#include "weapon_system/weapon_system.hpp"

// Defining static variables
RenderSystem* ShopMenu::renderer;

int ShopMenu::quantity = 0;
int ShopMenu::items_per_list = 5;

int ShopMenu::current_list_index = 0;
ITEM_TYPE ShopMenu::current_page = ITEM_TYPE::WEAPON;
WeaponType ShopMenu::current_item = WeaponType::TOTAL_WEAPON_TYPES;

Entity ShopMenu::balance_text_e;
WeaponType ShopMenu::weapon_on_sale;
std::vector<WeaponType> ShopMenu::unlocked_weapons;

std::vector<Entity> ShopMenu::button_entities;
std::vector<Entity> ShopMenu::text_entities;

std::vector<Entity> ShopMenu::page_entities;
std::vector<Entity> ShopMenu::list_entities;

void ShopMenu::init(
	RenderSystem* renderer, 
	WeaponType weapon_on_sale,
	std::function<void()> close_shop_callback
) {
	// Initialize data
	ShopMenu::renderer = renderer;
	ShopMenu::weapon_on_sale = weapon_on_sale;
	ShopMenu::current_page = ITEM_TYPE::WEAPON;

	Player& player = registry.players.components.back();
	for (int i = 0; i < (int)WeaponType::TOTAL_WEAPON_TYPES; i++) {
		if (!is_weapon_locked((WeaponType)i) && (WeaponType)i != WeaponType::GATLING_GUN) {
			unlocked_weapons.push_back((WeaponType)i);
		}
	}

	if (unlocked_weapons.size() > 0) {
		current_item = unlocked_weapons[0];
	}
	
	// Create a balance text
	int text_scale = 1.5f;
	balance_text_e = createText(renderer,
		"Gold Balance: " + std::to_string(player.gold_balance) + "G", {window_width_px - 100.f, 100.f}, text_scale, COLOR_GREEN, TextAlignment::RIGHT
	);

	registry.renderRequests.emplace(balance_text_e).used_render_layer = RENDER_LAYER::GAME_MENU;

	// Create a close window button
	Entity close_text_e = createText(
		renderer, "X", {100.f, 100.f}, 1.5f, COLOR_RED, TextAlignment::CENTER
	);
	
	Text& close_text = registry.texts.get(close_text_e);
	Motion& close_text_m = registry.motions.get(close_text_e);
	
	Entity close_btn_e = createButton(
		renderer, { close_text_m.position.x, window_height_px - close_text_m.position.y - close_text.rect_size.y / 2 }, close_text.rect_size
	);

	bind_button_hover(close_btn_e, close_text_e, COLOR_RED, COLOR_GREEN);

	registry.renderRequests.emplace(close_text_e).used_render_layer = RENDER_LAYER::GAME_MENU;
	registry.buttons.get(close_btn_e).on_click = close_shop_callback;

	button_entities.push_back(close_btn_e);
	text_entities.push_back(close_text_e);

	std::cout << "Close text rect: " << close_text.rect_size.x << " " << close_text.rect_size.y << '\n';
	std::cout << "Close position: " << close_text_m.position.x << " " << close_text_m.position.y << '\n';

	// Create the tab buttons
	Motion& close_btn_m = registry.motions.get(close_btn_e);
	vec2 prev_pos = { close_btn_m.position.x + close_btn_m.scale.x + 75.f, window_height_px - close_text_m.position.y };
	
	std::string tab_title[] = {"Weapon", "Ammo", "Misc."};
	std::function<void()> tab_callbacks[] = {render_weapons_page, render_ammo_page, render_misc_page};
	
	for (int i = 0; i < (int)ITEM_TYPE::TYPE_COUNT; i++) {
		Entity tab_btn_text_e = createText(
			renderer, tab_title[i], prev_pos, 1.5f, COLOR_RED, TextAlignment::LEFT
		);

		Text& text = registry.texts.get(tab_btn_text_e);
		Entity tab_btn_e = createButton(
			renderer, prev_pos + vec2({ text.rect_size.x / 2, -text.rect_size.y / 2 }), text.rect_size
		);

		registry.renderRequests.emplace(tab_btn_text_e).used_render_layer = RENDER_LAYER::GAME_MENU;
		Button& tab_btn = registry.buttons.get(tab_btn_e);
		tab_btn.on_click = tab_callbacks[i];
		
		bind_button_hover(tab_btn_e, tab_btn_text_e, COLOR_RED, COLOR_GREEN);

		button_entities.push_back(tab_btn_e);
		text_entities.push_back(tab_btn_text_e);

		prev_pos.x += (text.rect_size.x + 100.f);
	}

	render_weapons_page();
}

void ShopMenu::clear_page_entities() {
	for (Entity e : page_entities) {
		registry.remove_all_components_of(e);
	}

	page_entities.clear();
}

Entity ShopMenu::createStatText(std::string content, std::string value, vec2 position) {
	Entity e = createText(
		renderer, content + value, position, 0.75f, COLOR_GREEN, TextAlignment::LEFT
	);
	
	registry.renderRequests.emplace(e).used_render_layer = RENDER_LAYER::GAME_MENU;
	
	list_entities.push_back(e);
	page_entities.push_back(e);
	
	return e;
}

void ShopMenu::render_weapons_page() {
	clear_page_entities();

	// Page when all weapons are unlocked
	if (weapon_on_sale == WeaponType::TOTAL_WEAPON_TYPES) {
		Entity display_text_e = createText(
			renderer, "You have unlocked all weapons", 
			{ window_width_px / 2, window_height_px / 2 }, 2.f, COLOR_GREEN, TextAlignment::CENTER
		);
		
		registry.renderRequests.emplace(display_text_e).used_render_layer = RENDER_LAYER::GAME_MENU;
		page_entities.push_back(display_text_e);
		return;
	}

	// Get the information of the weapon being sold
	WeaponShopInfo w_info = weapon_shop_infos[ShopMenu::weapon_on_sale];
	WeaponStats w_stat = weapon_stats[ShopMenu::weapon_on_sale];
	
	std::cout << (int)w_info.texture << " " << w_info.weapon_name << std::endl;
	std::cout << w_stat.damage << "-" << w_stat.magazine_size << std::endl;

	// Create the weapon icon
	Entity weapon_icon_e = createWeaponEquippedIcon(renderer, { 354.f, 420.f }, w_info.texture);
	registry.renderRequests.get(weapon_icon_e).used_render_layer = RENDER_LAYER::GAME_MENU;
	registry.motions.get(weapon_icon_e).scale = {308.f, 308.f};
	page_entities.push_back(weapon_icon_e);

	// Create weapon price
	Entity weapon_price_e = createText(renderer, std::to_string(w_info.weapon_price) + "G", { 354.f, 420.f + 154.f + 1.25f * renderer->default_font_size }, 1.25f, COLOR_GREEN, TextAlignment::CENTER);
	registry.renderRequests.emplace(weapon_price_e).used_render_layer = RENDER_LAYER::GAME_MENU;
	page_entities.push_back(weapon_price_e);

	// Create the weapon title
	Entity weapon_name_e = createText(renderer, w_info.weapon_name, {554.f, 280.f}, 2.25f, COLOR_GREEN, TextAlignment::LEFT);
	registry.renderRequests.emplace(weapon_name_e).used_render_layer = RENDER_LAYER::GAME_MENU;
	page_entities.push_back(weapon_name_e);

	// Create weapon stats
	vec2 top_left_stats = { 554, 374 };

	createStatText("Damage: ", std::to_string(w_stat.damage), top_left_stats);
	top_left_stats += vec2({0, 0.75f * renderer->default_font_size + 10.f});
	createStatText("Lifetime: " , std::to_string(w_stat.lifetime), top_left_stats);
	top_left_stats += vec2({ 0, 0.75f * renderer->default_font_size + 10.f});
	createStatText("Magazine size: ", std::to_string(w_stat.magazine_size), top_left_stats);
	top_left_stats += vec2({ 0, 0.75f * renderer->default_font_size + 10.f});
	createStatText("Reload time: ", std::to_string(w_stat.reload_time), top_left_stats);
	top_left_stats += vec2({ 0, 0.75f * renderer->default_font_size + 10.f});
	createStatText("Fire rate: ", std::to_string(w_stat.fire_rate), top_left_stats);

	// Create the buy button
	Entity buy_btn_e = createButton(renderer, { 348.f, window_height_px - 250.f }, { 256.f, 128.f });
	Entity buy_text_e = createText(renderer, "Buy", { 348.f, window_height_px - 250.f }, 1.f, COLOR_GREEN, TextAlignment::CENTER);

	bind_button_hover(buy_btn_e, buy_text_e, COLOR_GREEN, COLOR_RED);

	registry.renderRequests.emplace(buy_text_e).used_render_layer = RENDER_LAYER::GAME_MENU;
	
	page_entities.push_back(buy_btn_e);
	page_entities.push_back(buy_text_e);

	Text& buy_text = registry.texts.get(buy_text_e);
	Button& buy_btn = registry.buttons.get(buy_btn_e);
	Player& player = registry.players.components.back();

	if (!is_weapon_locked(weapon_on_sale)) {
		buy_text.content = "Owned";
		buy_btn.disabled = true;
	} else {
		buy_btn.on_click = [w_info]() {
			// Unlock weapon and rerender the weapon page
			unlock_weapon(weapon_on_sale);

			// Update the player's balance
			Player& player = registry.players.components.back();
			player.gold_balance -= w_info.weapon_price;
			registry.texts.get(balance_text_e).content = "Gold balance: " + std::to_string(player.gold_balance) + "G";

			// Update the unlocked weapons list
			unlocked_weapons.clear();
			for (int i = 0; i < (int)WeaponType::TOTAL_WEAPON_TYPES; i++) {
				if (!is_weapon_locked((WeaponType)i) && (WeaponType)i != WeaponType::GATLING_GUN) {
					unlocked_weapons.push_back((WeaponType)i);
				}
			}
			
			if (unlocked_weapons.size() > 0) {
				current_item = unlocked_weapons[0];
			}

			// Rerender the weapons page
			render_weapons_page();
		};
	}

	int balance_minus_price = player.gold_balance - w_info.weapon_price;
	std::string after_text_content;
	if (!is_weapon_locked(weapon_on_sale)) {
		after_text_content = "You have already purchased this weapon";
	}
	else if (balance_minus_price < 0) {
		after_text_content = "You don't have enough gold. You need " + std::to_string(abs(balance_minus_price)) + "G";
	} else {
		after_text_content = "Post-transaction balance: " + std::to_string(balance_minus_price) + "G";
	}

	Entity after_text = createText(
		renderer, after_text_content, 
		{348.f + 128.f + 25.f, window_height_px - 250.f}, 1.f, COLOR_GREEN, TextAlignment::LEFT
	);

	registry.renderRequests.emplace(after_text).used_render_layer = RENDER_LAYER::GAME_MENU;
	page_entities.push_back(after_text);
}

void ShopMenu::render_ammo_page() {
	// Clear the page entities
	clear_page_entities();

	if (unlocked_weapons.size() < 1) {
		return;
	}

	// Create the list for storing the different buttons for selecting which ammo type to buy
	int start_index = current_list_index * items_per_list;
	int end_index = min(start_index + items_per_list, (int)unlocked_weapons.size());

	vec2 btn_size = { 420.f, 128.f };
	vec2 btn_position = { 150.f + 160.f, 266.f };
	for (int i = start_index; i < end_index; i++) {
		WeaponType current_weapon_type = unlocked_weapons[i];
		WeaponShopInfo w_info = weapon_shop_infos[current_weapon_type];
		
		Entity item_icon_e = createWeaponEquippedIcon(renderer, btn_position, w_info.texture);
		Entity item_text_e = createText(renderer, w_info.weapon_name, btn_position, 1.f, COLOR_RED, TextAlignment::LEFT);
		Entity button_e = createButton(renderer, btn_position, btn_size);

		Motion& item_icon_m = registry.motions.get(item_icon_e);
		Motion& item_text_m = registry.motions.get(item_text_e);
		Text& item_text = registry.texts.get(item_text_e);

		item_icon_m.position += vec2({ -btn_size.x / 2, 0 });
		item_icon_m.scale = 1.25f * vec2({ game_window_block_size, game_window_block_size });
		item_text_m.position.x -= btn_size.x / 4;
		item_text_m.position.y -= item_text.rect_size.y / 2;

		registry.renderRequests.emplace(item_text_e).used_render_layer = RENDER_LAYER::GAME_MENU;
		registry.renderRequests.get(item_icon_e).used_render_layer = RENDER_LAYER::GAME_MENU;
		bind_button_hover(button_e, item_text_e, COLOR_RED, COLOR_GREEN);

		Button& button = registry.buttons.get(button_e);
		button.on_click = [current_weapon_type]() {
			ShopMenu::current_item = current_weapon_type;
			render_ammo_page();
		};

		page_entities.push_back(button_e);
		page_entities.push_back(item_icon_e);
		page_entities.push_back(item_text_e);

		btn_position += vec2({0, btn_size.y + 10.f});
	}

	// Create the display page for displaying the ammo type
	// Create the weapon icon
	WeaponStats w_stat = weapon_stats[current_item];
	WeaponShopInfo w_info = weapon_shop_infos[current_item];
	
	int display_section_x = btn_position.x + btn_size.x + 50.f + 154.f + 10.f;
	int display_section_y = 266.f;

	Entity weapon_icon_e = createWeaponEquippedIcon(renderer, { display_section_x, display_section_y }, w_info.texture);
	registry.renderRequests.get(weapon_icon_e).used_render_layer = RENDER_LAYER::GAME_MENU;
	registry.motions.get(weapon_icon_e).scale = {154.f, 154.f};
	page_entities.push_back(weapon_icon_e);

	// Create weapon price
	Entity weapon_price_e = createText(renderer, std::to_string(w_info.price_per_ammo) + "G / bullet", 
		{ display_section_x, display_section_y + 77.f + 1.25f * renderer->default_font_size }, 1.f, COLOR_GREEN, TextAlignment::CENTER
	);
	
	registry.renderRequests.emplace(weapon_price_e).used_render_layer = RENDER_LAYER::GAME_MENU;
	page_entities.push_back(weapon_price_e);

	display_section_x += (77.f + 75.f);
	
	// Create the weapon title
	Entity weapon_name_e = createText(renderer, w_info.weapon_name, { display_section_x, display_section_y }, 2.f, COLOR_GREEN, TextAlignment::LEFT);
	registry.renderRequests.emplace(weapon_name_e).used_render_layer = RENDER_LAYER::GAME_MENU;
	page_entities.push_back(weapon_name_e);

	// Create weapon stats
	vec2 top_left_stats = { display_section_x, display_section_y + 1.75f * renderer->default_font_size };

	createStatText("Damage: ", std::to_string(w_stat.damage), top_left_stats);
	top_left_stats += vec2({ 0, 0.75f * renderer->default_font_size + 10.f });
	createStatText("Lifetime: ", std::to_string(w_stat.lifetime), top_left_stats);
	top_left_stats += vec2({ 0, 0.75f * renderer->default_font_size + 10.f });
	createStatText("Magazine size: ", std::to_string(w_stat.magazine_size), top_left_stats);
	top_left_stats += vec2({ 0, 0.75f * renderer->default_font_size + 10.f });
	createStatText("Reload time: ", std::to_string(w_stat.reload_time), top_left_stats);
	top_left_stats += vec2({ 0, 0.75f * renderer->default_font_size + 10.f });
	createStatText("Fire rate: ", std::to_string(w_stat.fire_rate), top_left_stats);

	// Create the interaction part where the player can set the amount to buy and the button to actually buy
	// Create a buy button
	display_section_x -= (77.f + 75.f);

	Entity buy_btn_e = createButton(renderer, { display_section_x, window_height_px - 125.f }, { 192.f, 128.f });
	Entity buy_text_e = createText(renderer, "Buy", { display_section_x, window_height_px - 125.f }, 1.75f, COLOR_GREEN, TextAlignment::CENTER);

	bind_button_hover(buy_btn_e, buy_text_e, COLOR_GREEN, COLOR_RED);

	registry.renderRequests.emplace(buy_text_e).used_render_layer = RENDER_LAYER::GAME_MENU;

	Button& buy_btn = registry.buttons.get(buy_btn_e);
	buy_btn.on_click = [w_info]() {
		Player& player = registry.players.components.back();

		player.total_ammo_count[current_item] += ShopMenu::quantity;
		player.gold_balance -= ShopMenu::quantity * w_info.price_per_ammo;

		ShopMenu::quantity = 0;
		
		registry.texts.get(balance_text_e).content = "Gold balance: " + std::to_string(player.gold_balance) + "G";
		render_ammo_page();
	};

	page_entities.push_back(buy_btn_e);
	page_entities.push_back(buy_text_e);

	// Create a post-transaction indicator
	std::string indicator_content;
	std::string post_t_content;
	Player& player = registry.players.components.back();

	if (player.gold_balance < w_info.price_per_ammo) {
		indicator_content = "Not enough balance";
	} else {
		indicator_content = "Quantity: " + std::to_string(ShopMenu::quantity);
	}

	if (indicator_content == "Not enough balance") {
		post_t_content = "";
	} else {
		post_t_content = "Post - transaction balance : " + std::to_string(player.gold_balance - (ShopMenu::quantity * w_info.price_per_ammo)) + "G.";
	}

	Entity indicator_text_e = createText(
		renderer, indicator_content, 
		{ display_section_x + 96.f + 1.5f * game_window_block_size, window_height_px - 150.f },
		1.5f, COLOR_RED, TextAlignment::LEFT
	);
	registry.renderRequests.emplace(indicator_text_e).used_render_layer = RENDER_LAYER::GAME_MENU;
	page_entities.push_back(indicator_text_e);
	

	Entity post_transaction_e = createText(
		renderer, post_t_content,
		{ display_section_x + 96.f + 1.5f * game_window_block_size, window_height_px - 102.f },
		1.f, COLOR_RED, TextAlignment::LEFT
	);

	registry.renderRequests.emplace(post_transaction_e).used_render_layer = RENDER_LAYER::GAME_MENU;
	page_entities.push_back(post_transaction_e);

	// Create a inventory display text
	Entity inventory_text_e = createText(
		renderer, "Inventory: " + std::to_string(player.total_ammo_count[current_item]) + " bullets",
		{ display_section_x - 77.f - 75.f, window_height_px - 250.f },
		1.25f, COLOR_RED, TextAlignment::LEFT
	);

	registry.renderRequests.emplace(inventory_text_e).used_render_layer = RENDER_LAYER::GAME_MENU;
	page_entities.push_back(inventory_text_e);

	// Create an increase and decrease button
	Entity inc_btn_e = createButton(renderer, { display_section_x + 96.f + 0.5f * game_window_block_size, window_height_px - 125.f - (0.25f + 0.5f) * game_window_block_size }, { 0.5f * game_window_block_size, 0.5f * game_window_block_size });
	Entity dec_btn_e = createButton(renderer, { display_section_x + 96.f + 0.5f * game_window_block_size, window_height_px - 125.f - (0.25f - 0.5f) * game_window_block_size }, { 0.5f * game_window_block_size, 0.5f * game_window_block_size });

	registry.renderRequests.insert(inc_btn_e, {
		TEXTURE_ASSET_ID::ACTIVE_UP_BUTTON,
		EFFECT_ASSET_ID::TEXTURED,
		GEOMETRY_BUFFER_ID::SPRITE,
		RENDER_LAYER::GAME_MENU
	});
	
	registry.renderRequests.insert(dec_btn_e, {
		TEXTURE_ASSET_ID::INACTIVE_DOWN_BUTTON,
		EFFECT_ASSET_ID::TEXTURED,
		GEOMETRY_BUFFER_ID::SPRITE,
		RENDER_LAYER::GAME_MENU
	});

	Button& inc_btn = registry.buttons.get(inc_btn_e);
	Button& dec_btn = registry.buttons.get(dec_btn_e);
	dec_btn.disabled = true;

	// Bind the inc/decrease buttons to the callbacks
	inc_btn.on_mouse_in = [inc_btn_e]() {
		Button& inc_btn = registry.buttons.get(inc_btn_e);
		inc_btn.hover = true;
	};
	dec_btn.on_mouse_in = [dec_btn_e]() {
		Button& dec_btn = registry.buttons.get(dec_btn_e);
		dec_btn.hover = true;
	};

	inc_btn.on_mouse_out = [inc_btn_e]() {
		Button& inc_btn = registry.buttons.get(inc_btn_e);
		inc_btn.hover = false;
	};
	dec_btn.on_mouse_out = [dec_btn_e]() {
		Button& dec_btn = registry.buttons.get(dec_btn_e);
		dec_btn.hover = false;
	};

	inc_btn.on_click = [inc_btn_e, dec_btn_e, indicator_text_e, post_transaction_e, inventory_text_e]() {
		WeaponShopInfo w_info = weapon_shop_infos[ShopMenu::current_item];
		Player& player = registry.players.components.back();

		if (player.gold_balance >= w_info.price_per_ammo * (ShopMenu::quantity + 1)) {
			ShopMenu::quantity++;
			registry.texts.get(indicator_text_e).content = "Quantity: " +
				std::to_string(ShopMenu::quantity);
			registry.texts.get(post_transaction_e).content = "Post - transaction balance : " +
				std::to_string(player.gold_balance - (ShopMenu::quantity * w_info.price_per_ammo)) + "G.";
			registry.texts.get(inventory_text_e).content = ("Inventory : " + 
				std::to_string(player.total_ammo_count[current_item]) + " bullets + " +
				std::to_string(ShopMenu::quantity) + " bullets"
			);

			if (player.gold_balance < w_info.price_per_ammo * (ShopMenu::quantity + 1)) {
				Button& inc_btn = registry.buttons.get(inc_btn_e);
				registry.renderRequests.get(inc_btn_e).used_texture = TEXTURE_ASSET_ID::INACTIVE_UP_BUTTON;
				inc_btn.hover = false;
				inc_btn.disabled = true;
				return;
			}

			Button& dec_btn = registry.buttons.get(dec_btn_e);
			if (dec_btn.disabled) {
				registry.renderRequests.get(dec_btn_e).used_texture = TEXTURE_ASSET_ID::ACTIVE_DOWN_BUTTON;
				dec_btn.disabled = false;
			}
		}
	};
	dec_btn.on_click = [inc_btn_e, dec_btn_e, indicator_text_e, post_transaction_e, inventory_text_e]() {
		WeaponShopInfo w_info = weapon_shop_infos[ShopMenu::current_item];
		Player& player = registry.players.components.back();

		if (w_info.price_per_ammo * (ShopMenu::quantity - 1) >= 0) {
			ShopMenu::quantity--;
			registry.texts.get(indicator_text_e).content = "Quantity: " +
				std::to_string(ShopMenu::quantity);
			registry.texts.get(post_transaction_e).content = "Post - transaction balance : " +
				std::to_string(player.gold_balance - (ShopMenu::quantity * w_info.price_per_ammo)) + "G.";
			registry.texts.get(inventory_text_e).content = ("Inventory : " +
				std::to_string(player.total_ammo_count[current_item]) + " bullets + " +
				std::to_string(ShopMenu::quantity) + " bullets"
				);

			if (w_info.price_per_ammo * (ShopMenu::quantity - 1) < 0) {
				Button& dec_btn = registry.buttons.get(dec_btn_e);
				registry.renderRequests.get(dec_btn_e).used_texture = TEXTURE_ASSET_ID::INACTIVE_DOWN_BUTTON;
				dec_btn.hover = false;
				dec_btn.disabled = true;
				return;
			}

			Button& inc_btn = registry.buttons.get(inc_btn_e);
			if (inc_btn.disabled) {
				registry.renderRequests.get(inc_btn_e).used_texture = TEXTURE_ASSET_ID::ACTIVE_UP_BUTTON;
				inc_btn.disabled = false;
			}
		}
	};

	page_entities.push_back(inc_btn_e);
	page_entities.push_back(dec_btn_e);
	
}

void ShopMenu::render_misc_page() {
	// Clear all of the page entities
	clear_page_entities();

	// Create the health icon
	//Entity weapon_icon_e = createWeaponEquippedIcon(renderer, { 354.f, 420.f }, );
	//registry.renderRequests.get(weapon_icon_e).used_render_layer = RENDER_LAYER::GAME_MENU;
	//registry.motions.get(weapon_icon_e).scale = { 308.f, 308.f };
	//page_entities.push_back(weapon_icon_e);

	vec2 top_left_pos = { 200.f, 266.f };
	Health& p_health = registry.healths.get(registry.players.entities.back());
	Player& player = registry.players.components.back();

	// Create the weapon title
	Entity weapon_name_e = createText(renderer, "Health", top_left_pos, 2.25f, COLOR_GREEN, TextAlignment::LEFT);
	registry.renderRequests.emplace(weapon_name_e).used_render_layer = RENDER_LAYER::GAME_MENU;
	page_entities.push_back(weapon_name_e);

	// Create weapon price
	Entity weapon_price_e = createText(renderer, "10G / unit", { top_left_pos.x, top_left_pos.y + 2.25f * renderer->default_font_size }, 1.25f, COLOR_GREEN, TextAlignment::LEFT);
	registry.renderRequests.emplace(weapon_price_e).used_render_layer = RENDER_LAYER::GAME_MENU;
	page_entities.push_back(weapon_price_e);
	
	if (p_health.current_health == p_health.max_health) {
		Entity full_text_e = createText(renderer, "Your health is full.", { 348.f, window_height_px - 125.f }, 1.f, COLOR_GREEN, TextAlignment::CENTER);
		registry.renderRequests.emplace(full_text_e).used_render_layer = RENDER_LAYER::GAME_MENU;
		page_entities.push_back(full_text_e);

		return;
	}

	// Create the buy button
	Entity buy_btn_e = createButton(renderer, { 348.f, window_height_px - 125.f }, { 256.f, 128.f });
	Entity buy_text_e = createText(renderer, "Buy", { 348.f, window_height_px - 125.f }, 1.f, COLOR_GREEN, TextAlignment::CENTER);

	bind_button_hover(buy_btn_e, buy_text_e, COLOR_GREEN, COLOR_RED);

	registry.renderRequests.emplace(buy_text_e).used_render_layer = RENDER_LAYER::GAME_MENU;

	page_entities.push_back(buy_btn_e);
	page_entities.push_back(buy_text_e);

	Text& buy_text = registry.texts.get(buy_text_e);
	Button& buy_btn = registry.buttons.get(buy_btn_e);
	buy_btn.on_click = []() {
		Player& player = registry.players.components.back();
		Health& player_health = registry.healths.get(registry.players.entities.back());

		player_health.current_health += ShopMenu::quantity;
		player.gold_balance -= ShopMenu::quantity * misc_shop_prices[(int)MiscType::HEALTH];

		ShopMenu::quantity = 0;

		registry.texts.get(balance_text_e).content = "Gold balance: " + std::to_string(player.gold_balance) + "G";
		render_misc_page();
	};

	//////////
	// Create a post-transaction indicator
	std::string indicator_content;
	std::string post_t_content;

	if (player.gold_balance < misc_shop_prices[(int)MiscType::HEALTH]) {
		indicator_content = "Not enough balance";
	} else {
		indicator_content = "Quantity: " + std::to_string(ShopMenu::quantity);
	}

	if (indicator_content == "Not enough balance") {
		post_t_content = "";
	} else {
		post_t_content = "Post - transaction balance : " + std::to_string(player.gold_balance - (ShopMenu::quantity * misc_shop_prices[(int)MiscType::HEALTH])) + "G.";
	}

	Entity indicator_text_e = createText(
		renderer, indicator_content,
		{ top_left_pos.x + 225.f + 2.5f * game_window_block_size, window_height_px - 150.f },
		1.5f, COLOR_RED, TextAlignment::LEFT
	);
	registry.renderRequests.emplace(indicator_text_e).used_render_layer = RENDER_LAYER::GAME_MENU;
	page_entities.push_back(indicator_text_e);


	Entity post_transaction_e = createText(
		renderer, post_t_content,
		{ top_left_pos.x + 225.f + 2.5f * game_window_block_size, window_height_px - 102.f },
		1.f, COLOR_RED, TextAlignment::LEFT
	);

	registry.renderRequests.emplace(post_transaction_e).used_render_layer = RENDER_LAYER::GAME_MENU;
	page_entities.push_back(post_transaction_e);

	// Create a inventory display text
	Entity inventory_text_e = createText(
		renderer, "Health: " + std::to_string((int)p_health.current_health) + " units",
		{ top_left_pos.x, window_height_px - 250.f },
		1.25f, COLOR_RED, TextAlignment::LEFT
	);

	registry.renderRequests.emplace(inventory_text_e).used_render_layer = RENDER_LAYER::GAME_MENU;
	page_entities.push_back(inventory_text_e);

	// Create an increase and decrease button
	Entity inc_btn_e = createButton(renderer, { top_left_pos.x + 256.f + 1.f * game_window_block_size, window_height_px - 125.f - (0.25f + 0.5f) * game_window_block_size }, { 0.5f * game_window_block_size, 0.5f * game_window_block_size });
	Entity dec_btn_e = createButton(renderer, { top_left_pos.x + 256.f + 1.f * game_window_block_size, window_height_px - 125.f - (0.25f - 0.5f) * game_window_block_size }, { 0.5f * game_window_block_size, 0.5f * game_window_block_size });

	registry.renderRequests.insert(inc_btn_e, {
		TEXTURE_ASSET_ID::ACTIVE_UP_BUTTON,
		EFFECT_ASSET_ID::TEXTURED,
		GEOMETRY_BUFFER_ID::SPRITE,
		RENDER_LAYER::GAME_MENU
		});

	registry.renderRequests.insert(dec_btn_e, {
		TEXTURE_ASSET_ID::INACTIVE_DOWN_BUTTON,
		EFFECT_ASSET_ID::TEXTURED,
		GEOMETRY_BUFFER_ID::SPRITE,
		RENDER_LAYER::GAME_MENU
		});

	Button& inc_btn = registry.buttons.get(inc_btn_e);
	Button& dec_btn = registry.buttons.get(dec_btn_e);
	dec_btn.disabled = true;

	// Bind the inc/decrease buttons to the callbacks
	inc_btn.on_mouse_in = [inc_btn_e]() {
		Button& inc_btn = registry.buttons.get(inc_btn_e);
		inc_btn.hover = true;
	};
	dec_btn.on_mouse_in = [dec_btn_e]() {
		Button& dec_btn = registry.buttons.get(dec_btn_e);
		dec_btn.hover = true;
	};

	inc_btn.on_mouse_out = [inc_btn_e]() {
		Button& inc_btn = registry.buttons.get(inc_btn_e);
		inc_btn.hover = false;
	};
	dec_btn.on_mouse_out = [dec_btn_e]() {
		Button& dec_btn = registry.buttons.get(dec_btn_e);
		dec_btn.hover = false;
	};

	inc_btn.on_click = [inc_btn_e, dec_btn_e, indicator_text_e, post_transaction_e, inventory_text_e, p_health]() {
		WeaponShopInfo w_info = weapon_shop_infos[ShopMenu::current_item];
		Player& player = registry.players.components.back();

		if (player.gold_balance >= misc_shop_prices[(int)MiscType::HEALTH] * (ShopMenu::quantity + 1) && 
			p_health.current_health + (ShopMenu::quantity + 1) < p_health.max_health
		) {
			ShopMenu::quantity++;
			registry.texts.get(indicator_text_e).content = "Quantity: " +
				std::to_string(ShopMenu::quantity);
			registry.texts.get(post_transaction_e).content = "Post - transaction balance : " +
				std::to_string(player.gold_balance - (ShopMenu::quantity * w_info.price_per_ammo)) + "G.";
			registry.texts.get(inventory_text_e).content = ("Inventory : " +
				std::to_string((int)p_health.current_health) + " units + " +
				std::to_string(ShopMenu::quantity) + " units"
				);

			if (player.gold_balance < misc_shop_prices[(int)MiscType::HEALTH] * (ShopMenu::quantity + 1) || 
				p_health.current_health + (ShopMenu::quantity + 1) > p_health.max_health
			) {
				Button& inc_btn = registry.buttons.get(inc_btn_e);
				registry.renderRequests.get(inc_btn_e).used_texture = TEXTURE_ASSET_ID::INACTIVE_UP_BUTTON;
				inc_btn.disabled = true;
				return;
			}

			Button& dec_btn = registry.buttons.get(dec_btn_e);
			if (dec_btn.disabled) {
				registry.renderRequests.get(dec_btn_e).used_texture = TEXTURE_ASSET_ID::ACTIVE_DOWN_BUTTON;
				dec_btn.disabled = false;
			}
		}
	};

	dec_btn.on_click = [inc_btn_e, dec_btn_e, indicator_text_e, post_transaction_e, inventory_text_e, p_health]() {
		WeaponShopInfo w_info = weapon_shop_infos[ShopMenu::current_item];
		Player& player = registry.players.components.back();

		if (misc_shop_prices[(int)MiscType::HEALTH] * (ShopMenu::quantity - 1) >= 0) {
			ShopMenu::quantity--;
			registry.texts.get(indicator_text_e).content = "Quantity: " +
				std::to_string(ShopMenu::quantity);
			registry.texts.get(post_transaction_e).content = "Post - transaction balance : " +
				std::to_string(player.gold_balance - (ShopMenu::quantity * w_info.price_per_ammo)) + "G.";
			registry.texts.get(inventory_text_e).content = ("Inventory : " +
				std::to_string((int)p_health.current_health) + " units + " +
				std::to_string(ShopMenu::quantity) + " units"
				);

			if (misc_shop_prices[(int)MiscType::HEALTH] * (ShopMenu::quantity - 1) < 0) {
				Button& dec_btn = registry.buttons.get(dec_btn_e);
				registry.renderRequests.get(dec_btn_e).used_texture = TEXTURE_ASSET_ID::INACTIVE_DOWN_BUTTON;
				dec_btn.disabled = true;
				return;
			}

			Button& inc_btn = registry.buttons.get(inc_btn_e);
			if (inc_btn.disabled) {
				registry.renderRequests.get(inc_btn_e).used_texture = TEXTURE_ASSET_ID::ACTIVE_UP_BUTTON;
				inc_btn.disabled = false;
			}
		}
	};

	page_entities.push_back(inc_btn_e);
	page_entities.push_back(dec_btn_e);
}

int ShopMenu::on_mouse_move(vec2 mouse_position) {
	int cursor = GLFW_ARROW_CURSOR;

	for (int i = 0; i < button_entities.size(); i++) {
		Entity& e = button_entities[i];

		if (!registry.buttons.has(e)) {
			continue;
		}

		Motion& motion = registry.motions.get(e);
		Button& button = registry.buttons.get(e);

		if (
			mouse_position.x <= motion.position.x + motion.scale.x / 2 &&
			mouse_position.x >= motion.position.x - motion.scale.x / 2 &&
			mouse_position.y >= motion.position.y - 0.5f * motion.scale.y &&
			mouse_position.y <= motion.position.y + 0.5f * motion.scale.y &&
			!button.disabled
		) {
			cursor = GLFW_HAND_CURSOR;
			button.on_mouse_in();
		} else if (!button.disabled) {
			button.on_mouse_out();
		}
	}

	for (int i = 0; i < page_entities.size(); i++) {
		Entity& e = page_entities[i];

		if (!registry.buttons.has(e)) {
			continue;
		}

		Motion& motion = registry.motions.get(e);
		Button& button = registry.buttons.get(e);

		if (
			mouse_position.x <= motion.position.x + motion.scale.x / 2 &&
			mouse_position.x >= motion.position.x - motion.scale.x / 2 &&
			mouse_position.y >= motion.position.y - 0.5f * motion.scale.y &&
			mouse_position.y <= motion.position.y + 0.5f * motion.scale.y &&
			!button.disabled
		) {
			cursor = GLFW_HAND_CURSOR;
			button.on_mouse_in();
		} else if (!button.disabled) {
			button.on_mouse_out();
		}
	}
	
	return cursor;
}

int ShopMenu::on_mouse_click(int button, int action, int mods) {
	for (Entity btn_e : button_entities) {
		// assert(registry.buttons.has(btn_e);
		if (!registry.buttons.has(btn_e)) {
			continue;
		}

		Button& button = registry.buttons.get(btn_e);

		if (button.hover && !button.disabled && action == GLFW_PRESS) {
			button.on_click();
			break;
		}
	}

	for (Entity btn_e : page_entities) {
		// assert(registry.buttons.has(btn_e);
		if (!registry.buttons.has(btn_e)) {
			continue;
		}

		Button& button = registry.buttons.get(btn_e);

		if (button.hover && !button.disabled && action == GLFW_PRESS) {
			button.on_click();
			break;
		}
	}

	return 0;
}

void ShopMenu::close() {
	// Reset states
	current_page = ITEM_TYPE::WEAPON;

	// Remove all related components
	registry.remove_all_components_of(balance_text_e);

	for (Entity e : text_entities) {
		registry.remove_all_components_of(e);
	}

	for (Entity e : button_entities) {
		registry.remove_all_components_of(e);
	}

	for (Entity e : page_entities) {
		registry.remove_all_components_of(e);
	}

	for (Entity e : list_entities) {
		registry.remove_all_components_of(e);
	}

	unlocked_weapons.clear();
	button_entities.clear();
	text_entities.clear();

	page_entities.clear();
	list_entities.clear();
}