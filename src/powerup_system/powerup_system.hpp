#pragma once

#include "common/common.hpp"
#include "ecs/ecs.hpp"
#include "components/components.hpp"
#include "render_system/render_system.hpp"

#include <random>

class PowerupSystem
{
	RenderSystem* renderer;
	Entity currentPowerupPopup;
	Entity currentPowerupPopupTitleText;
	Entity currentPowerupPopupDescriptionText;
	Entity currentPowerupPopupIcon;
public:
	void init(RenderSystem* renderer);
	void createPopup(PowerupType powerup);
	void destroyPopup();
	void awardPowerup(Entity entity, std::default_random_engine rng, std::uniform_real_distribution<float> uniform_dist);
};