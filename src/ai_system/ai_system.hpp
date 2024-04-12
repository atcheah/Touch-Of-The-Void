#pragma once

#include <vector>

#include "render_system/render_system.hpp"
#include "ecs_registry/ecs_registry.hpp"
#include "common/common.hpp"

class AISystem
{
private:
	RenderSystem* renderer;
public:
	AISystem(RenderSystem* _renderer) : renderer(_renderer) {};

	bool AISystem::lineOfSightClear(const vec2& start, const vec2& end);

	bool isObstacleAtPosition(const vec2& gridPosition);

	std::vector<vec2> generateNeighbors(const vec2& position);

	std::vector<vec2> findPathAStar(const vec2& start, const vec2& goal);
	
	void step(float elapsed_ms);

	void idleState(Entity entity, AI& ai, Motion& motion, float elapsed_ms);

	void activeState(Entity entity, AI& ai, Motion& motion, float elapsed_ms);

	void handleRangedAI(Entity entity, Motion& motion, AI& ai, float elapsed_ms, const vec2& playerPosition);

	void handleMeleeAI(Entity entity, Motion& motion, AI& ai, float elapsed_ms, const vec2& playerPosition);

	vec2 limit(vec2 v, float max);

	void handleBoidMovement(Entity entity, Motion& motion, float elapsed_ms, const vec2& playerPosition, float separationForce, float separationWeight, float friendlyAvoidanceDistance, float obstacleAvoidanceDistance, float projectileAvoidanceDistance, float playerAvoidanceDistance, float alignmentForce, float alignmentDistance, float alignmentWeight, float cohesionForce, float cohesionWeight, float cohesionDistance, float playerFollowDistance, float maxSpeed, float forceWeight);
	
	void handleTurretAI(Entity entity, Motion& motion, AI& ai, float elapsed_ms, const vec2& playerPosition);

	void handleShotgunAI(Entity entity, Motion& motion, AI& ai, float elapsed_ms, const vec2& playerPosition);

	void handleRocketAI(Entity entity, Motion& motion, AI& ai, float elapsed_ms, const vec2& playerPosition);

	void handleFlameAI(Entity entity, Motion& motion, AI& ai, float elapsed_ms, const vec2& playerPosition);

	vec2 clampPositionToBounds(const vec2& position);
};