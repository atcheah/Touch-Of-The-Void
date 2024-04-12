#include "pause_menu.hpp"

#include "world_init/world_init.hpp"
#include "ui_init/ui_init.hpp"


// Initialize static variables
Entity PauseMenu::title_entity;
std::vector<Entity> PauseMenu::button_entities;
std::vector<Entity> PauseMenu::text_entities;

bool	PauseMenu::is_hovering = false;
bool	PauseMenu::has_state_changed = false;
Entity	PauseMenu::current_entity;
Entity	PauseMenu::previous_entity;

void PauseMenu::init(
	RenderSystem* renderer,
	std::function<void()> resume_callback,
	std::function<void()> exit_callback,
	std::function<void()> close_window_callback
) {
	const int top_padding = 200.f;
	const int left_padding = 200.f;
	const int button_offset = 150;
	int offset_multiplier = 1;
	vec2 button_size = { 675, 112 };
	vec3 button_color = { 0.682, 0.18, 0.18 };

	// Create the title
	Entity title_e = createText(renderer, "Pause",
		{ left_padding, 150 }, 3.f, {1.f, 0.f, 0.f},
		TextAlignment::LEFT
	);

	registry.renderRequests.emplace(title_e).used_render_layer = RENDER_LAYER::GAME_MENU;

	// Create buttons
	// Resume button
	Entity resume_btn_e = createButton(
		renderer, 
		{ left_padding + button_size.x / 2, top_padding + offset_multiplier * button_offset },
		button_size
	);

	Motion& resume_motion = registry.motions.get(resume_btn_e);
	Entity resume_e = createText(
		renderer, "Resume",
		{ resume_motion.position.x - resume_motion.scale.x / 2, resume_motion.position.y },
		1.25f,
		button_color,
		TextAlignment::LEFT
	);
	
	registry.renderRequests.emplace(resume_e).used_render_layer = RENDER_LAYER::GAME_MENU;
	registry.buttons.get(resume_btn_e).on_click = resume_callback;
	bind_button_hover(resume_btn_e, resume_e, COLOR_RED, COLOR_GREEN);

	offset_multiplier++;

	// Exit game button

	Entity exit_btn_e = createButton(
		renderer,
		{ left_padding + button_size.x / 2, top_padding + offset_multiplier * button_offset },
		button_size
	);

	Motion& exit_motion = registry.motions.get(exit_btn_e);
	Entity exit_e = createText(
		renderer, "Exit to main menu",
		{ exit_motion.position.x - exit_motion.scale.x / 2, exit_motion.position.y },
		1.25f,
		button_color,
		TextAlignment::LEFT
	);

	registry.renderRequests.emplace(exit_e).used_render_layer = RENDER_LAYER::GAME_MENU;
	registry.buttons.get(exit_btn_e).on_click = exit_callback;
	bind_button_hover(exit_btn_e, exit_e, COLOR_RED, COLOR_GREEN);

	offset_multiplier++;

	// Close window button
	Entity close_btn_e = createButton(
		renderer,
		{ left_padding + button_size.x / 2, top_padding + offset_multiplier * button_offset },
		button_size
	);

	Motion& close_motion = registry.motions.get(close_btn_e);
	Entity close_e = createText(
		renderer, "Exit to Desktop",
		{ close_motion.position.x - close_motion.scale.x / 2, close_motion.position.y },
		1.25f,
		button_color,
		TextAlignment::LEFT
	);

	registry.renderRequests.emplace(close_e).used_render_layer = RENDER_LAYER::GAME_MENU;
	registry.buttons.get(close_btn_e).on_click = close_window_callback;
	bind_button_hover(close_btn_e, close_e, COLOR_RED, COLOR_GREEN);

	title_entity = title_e;
	text_entities = { resume_e, exit_e, close_e };
	button_entities = { resume_btn_e, exit_btn_e, close_btn_e };
}

int PauseMenu::on_mouse_move(vec2 mouse_position) {
	int cursor = GLFW_ARROW_CURSOR;

	for (Entity btn_e : button_entities) {
		Button& button = registry.buttons.get(btn_e);
		Motion& motion = registry.motions.get(btn_e);
		
		if (
			mouse_position.x <= motion.position.x + motion.scale.x / 2 &&
			mouse_position.x >= motion.position.x - motion.scale.x / 2 &&
			mouse_position.y <= motion.position.y + 0.5f * motion.scale.y &&
			mouse_position.y >= motion.position.y - 0.5f * motion.scale.y
		) {
			cursor = GLFW_HAND_CURSOR;
			button.on_mouse_in();
		} else {
			button.on_mouse_out();
		}
	}

	return cursor;
}

int PauseMenu::on_mouse_click() {
	for (Entity btn_e : button_entities) {
		assert(registry.buttons.has(btn_e));
		Button& button = registry.buttons.get(btn_e);

		if (button.hover) {
			button.on_click();
			break;
		}
	}

	return 0;
}

void PauseMenu::close() {
	registry.remove_all_components_of(title_entity);

	for (Entity e : text_entities) {
		registry.remove_all_components_of(e);
	}

	for (Entity e : button_entities) {
		registry.remove_all_components_of(e);
	}

	text_entities.clear();
	button_entities.clear();
}