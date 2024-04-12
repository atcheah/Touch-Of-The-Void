#pragma once

#include <vector>
#include <functional>

#include "common/common.hpp"
#include "ecs/ecs.hpp"
#include "render_system/render_system.hpp"
#include "world_system/world_system.hpp"

class PauseMenu {
public:
	static Entity title_entity;
	static std::vector<Entity> button_entities;
	static std::vector<Entity> text_entities;

	enum class BUTTON_ID {
		RESUME_BUTTON,
		EXIT_BUTTON,
		CLOSE_MENU_BUTTON,
		BUTTON_COUNT
	};

	static bool has_state_changed;
	static bool is_hovering;
	static Entity current_entity;
	static Entity previous_entity;

	static void init(
		RenderSystem* renderer,
		std::function<void()> resume_callback,
		std::function<void()> exit_callback,
		std::function<void()> close_window_callback
	);
	
	static int on_mouse_move(vec2 mouse_position);
	static int on_mouse_click();

	static void close();
};