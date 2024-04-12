#pragma once

#include "common/common.hpp"
#include "ecs/ecs.hpp"
#include "components/components.hpp"
#include "ecs_registry/ecs_registry.hpp"

// A simple physics system that moves rigid bodies and checks for collision
class PhysicsSystem
{
public:
	void step(float elapsed_ms);

	PhysicsSystem()
	{
	}
};