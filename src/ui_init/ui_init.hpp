#pragma once

#include "render_system/render_system.hpp"
#include "components/components.hpp"
#include "common/common.hpp"
#include "ecs/ecs.hpp"
#include "ecs_registry/ecs_registry.hpp"


// create a button
Entity createTextButton(
	RenderSystem* renderer,
	vec2 position,
	vec2 size,
	vec3 color,
	std::string text_content,
	float font_size,
	TextAlignment alignment,
	std::function<void(void)> on_click
);

Entity createTextureButton(
	RenderSystem* renderer,
	vec2 position,
	vec2 size,
	TEXTURE_ASSET_ID texture,
	std::function<void(void)> on_click
);

Entity createButton(
	RenderSystem* renderer,
	vec2 position,
	vec2 size
);

void bind_button_hover(Entity btn_e, Entity text_e, vec3 idle_color, vec3 active_color);