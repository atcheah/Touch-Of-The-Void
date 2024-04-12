// internal
#include "ai_system/ai_system.hpp"
#include <world_system/world_system.hpp>
#include <world_init/world_init.hpp>
#include <components/components.hpp>
#include <queue> // For priority queue (open list)
#include <unordered_set>  
#include <algorithm> 
#include <vector>
#include <cmath>
#include <memory>
#include <map>
#include <iostream>

// coordinate system details
// world is 960x960
// each cell is 64x64 so there are 15x15 cells
// the top left cell is (0,0) and the bottom right cell is (14,14)
// the world is placed in the middle of the screen (where the screen is 1920x1024)
// so the top left corner of the world is at (480, 32) and the bottom right corner is at (1440, 992)
const float cellSize = 64.0f;
const int numCells = 15;
const int maxIterations = 1000;
const float xMin = 480.0f; 
const float yMin = 32.0f;
const float xMax = 1440.0f;
const float yMax = 992.0f;
const float halfCellSize = cellSize / 2.0f;

struct Vec2Hash {
    std::size_t operator()(const vec2& v) const {
        return std::hash<float>()(v.x) ^ std::hash<float>()(v.y);
    }
};

vec2 AISystem::clampPositionToBounds(const vec2& position) {
    float clampedX = std::max(xMin + (cellSize + halfCellSize), std::min(position.x, xMax - (cellSize + halfCellSize)));
    float clampedY = std::max(yMin + (cellSize + halfCellSize), std::min(position.y, yMax - (cellSize + halfCellSize)));
    return vec2(clampedX, clampedY);
}

// http://www.cse.yorku.ca/~amana/research/grid.pdf
 // Amanatides, J., & Woo, A. (1987). A fast voxel traversal algorithm for ray tracing. Eurographics, 87(3), 3-10.
 // This is a simple implementation of the Bresenham's line algorithm
bool AISystem::lineOfSightClear(const vec2& start, const vec2& end) {
    // Convert start and end positions from world coordinates to grid coordinates
    auto toGridCoord = [&](const vec2& pos) -> vec2 {
        return vec2(floor((pos.x - xMin) / cellSize), floor((pos.y - yMin) / cellSize));
        };

    vec2 startGrid = toGridCoord(start);
    vec2 endGrid = toGridCoord(end);

    // Bresenham's Line Algorithm variables
    int dx = abs(int(endGrid.x - startGrid.x));
    int sx = startGrid.x < endGrid.x ? 1 : -1;
    int dy = -abs(int(endGrid.y - startGrid.y));
    int sy = startGrid.y < endGrid.y ? 1 : -1;
    int err = dx + dy; // error value e_xy
    int e2; 

    while (true) {
        // Check for obstacle at current grid position
        if (isObstacleAtPosition(vec2(startGrid.x, startGrid.y))) {
            return false; // Obstacle found, line of sight is not clear
        }
        if (startGrid.x == endGrid.x && startGrid.y == endGrid.y) break; // Reached target cell

        e2 = 2 * err;
        if (e2 >= dy) { // e_xy+e_x > 0
            err += dy;
            startGrid.x += sx;
        }
        if (e2 <= dx) { // e_xy+e_y < 0
            err += dx;
            startGrid.y += sy;
        }
    }

    return true; // No obstacles found, line of sight is clear
}


// Adjusted obstacle detection to consider the full bounding box of obstacles
bool AISystem::isObstacleAtPosition(const vec2& gridPosition) {
    Level& level = registry.levels.get(registry.levels.entities[0]);
    Room& room = registry.rooms.get(level.rooms[level.current_room]);

    for (const auto& obstaclePos : room.obstacle_positions) {
        // Direct comparison, as both are in grid coordinates
        if (obstaclePos == gridPosition) {
            return true; // Obstacle found at the specified grid position
        }
    }

    return false; // No obstacle at the specified grid position
}


// Basic structure for A* nodes
struct AStarNode {
    vec2 position;
    float gCost; // Cost from start to this node
    float hCost; // Heuristic cost to goal
    float fCost() const { return gCost + hCost; } // Total cost
    AStarNode* parent; // For reconstructing the path
};

struct CompareAStarNode {
    bool operator()(AStarNode* a, AStarNode* b) const {
        return a->fCost() > b->fCost();
    }
};
std::vector<vec2> AISystem::generateNeighbors(const vec2& position) {
    std::vector<vec2> neighbors;
    //generate neighbors in four directions
    float stepSize = 10.0; // adjust as necessary
    neighbors.push_back(position + vec2(stepSize, 0)); // Right
    neighbors.push_back(position + vec2(-stepSize, 0)); // Left
    neighbors.push_back(position + vec2(0, stepSize)); // Up
    neighbors.push_back(position + vec2(0, -stepSize)); // Down
    neighbors.push_back(position + vec2(stepSize, stepSize)); // Up-right
    neighbors.push_back(position + vec2(-stepSize, stepSize)); // Up-left
    neighbors.push_back(position + vec2(stepSize, -stepSize)); // Down-right
    neighbors.push_back(position + vec2(-stepSize, -stepSize)); // Down-left

    return neighbors;
}


std::vector<vec2> AISystem::findPathAStar(const vec2& start, const vec2& goal) {
    auto heuristic = [](const vec2& a, const vec2& b) -> float {
        // Use Euclidean distance as the heuristic
        return length(a - b);
        };

    std::priority_queue<AStarNode*, std::vector<AStarNode*>, CompareAStarNode> openSet;
    std::unordered_map<vec2, AStarNode*, Vec2Hash> allNodes;
    std::vector<vec2> path;

    AStarNode* startNode = new AStarNode{ start, 0, heuristic(start, goal), nullptr };
    openSet.push(startNode);
    allNodes[start] = startNode;

    while (!openSet.empty()) {
        AStarNode* currentNode = openSet.top();
        openSet.pop();

        if (length(currentNode->position - goal) < 10.0f) { // Threshold to consider as goal reached
            while (currentNode != nullptr) {
                path.push_back(currentNode->position);
                currentNode = currentNode->parent;
            }
            std::reverse(path.begin(), path.end());
            break;
        }

        std::vector<vec2> neighbors = generateNeighbors(currentNode->position);

        for (auto& neighborPos : neighbors) {
            if (isObstacleAtPosition(neighborPos)) continue;

            float tentative_gScore = currentNode->gCost + length(neighborPos - currentNode->position);
            AStarNode* neighborNode = allNodes.find(neighborPos) != allNodes.end() ? allNodes[neighborPos] : nullptr;

            if (neighborNode == nullptr || tentative_gScore < neighborNode->gCost) {
                if (neighborNode == nullptr) {
                    neighborNode = new AStarNode{ neighborPos, tentative_gScore, heuristic(neighborPos, goal), currentNode };
                    allNodes[neighborPos] = neighborNode;
                    openSet.push(neighborNode);
                }
                else {
                    neighborNode->gCost = tentative_gScore;
                    neighborNode->parent = currentNode;
                }
            }
        }
    }

    for (auto& nodePair : allNodes) {
        delete nodePair.second;
    }
    allNodes.clear();

    return path;
}

// Utility function to limit the magnitude of a vector to max
vec2 AISystem::limit(vec2 v, float max) {
    if (length(v) > max) {
        return normalize(v) * max;
    }
    return v;
}

void AISystem::step(float elapsed_ms)
{
    auto& ai_registry = registry.ais;
    for (uint i = 0; i < ai_registry.size(); i++)
    {
        AI& ai = ai_registry.components[i];
        Entity entity = ai_registry.entities[i];
        Motion& motion = registry.motions.get(entity);

        // Check AI state and perform actions accordingly
        switch (ai.state) {
        case AI::AIState::IDLE:
            idleState(entity, ai, motion, elapsed_ms); // Now passing elapsed_ms
            break;
        case AI::AIState::ACTIVE:
            activeState(entity, ai, motion, elapsed_ms);
            break;
        default:
            printf("Unknown state\n");
            break;
        }

        // Always update position based on velocity, regardless of state << shouldn't physics system do this?
        // motion.position += motion.velocity * elapsed_ms / 1000.0f;
    }
}

void AISystem::idleState(Entity entity, AI& ai, Motion& motion, float elapsed_ms) {
    // Example condition to switch to active state based on a simple timer
    ai.counter += elapsed_ms;
    if (ai.counter >= ai.frequency) {
        ai.counter = 0;
        ai.state = AI::AIState::ACTIVE;
    }

    // Optionally, apply some basic motion or behavior even in idle state
    // For example, slowing down or stopping:
    // motion.velocity *= 0.95f; // Slow down effect
}

void AISystem::activeState(Entity entity, AI& ai, Motion& motion, float elapsed_ms) {
    vec2 playerPosition = registry.motions.get(registry.players.entities[0]).position;

    switch (ai.type) {
    case AI::AIType::RANGED:
        handleRangedAI(entity, motion, ai, elapsed_ms, playerPosition);
        break;
    case AI::AIType::MELEE:
        handleMeleeAI(entity, motion, ai, elapsed_ms, playerPosition);
        break;
    case AI::AIType::TURRET:
        handleTurretAI(entity, motion, ai, elapsed_ms, playerPosition);
        break;
    case AI::AIType::SHOTGUN:
        handleShotgunAI(entity, motion, ai, elapsed_ms, playerPosition);
        break;
    case AI::AIType::ROCKET:
		handleRocketAI(entity, motion, ai, elapsed_ms, playerPosition);
        break;
    case AI::AIType::FLAMETHROWER:
		handleFlameAI(entity, motion, ai, elapsed_ms, playerPosition);
        break;
    default:
        break;
    }
}

void rotateTowardsPlayer(Motion& motion, const vec2& playerPosition, float viewThreshold, float turnSpeed, float maxTurnSpeed, float offset) {
    vec2 direction = normalize(playerPosition - motion.position);
    float desired_look_angle = atan2(direction.y, direction.x) - offset;
    if (desired_look_angle > M_PI) { // can only go over as i just added pi/2, no need to check for -M_PI
        desired_look_angle -= 2 * M_PI;
    }
    desired_look_angle -= motion.look_angle;
    if (desired_look_angle >= M_PI) {
        desired_look_angle -= 2 * M_PI;
    }
    else if (desired_look_angle <= -M_PI) {
        desired_look_angle += 2 * M_PI;
    }

    if (desired_look_angle > viewThreshold) {
        motion.turn_speed -= turnSpeed;
        if (motion.turn_speed < -maxTurnSpeed) {
            motion.turn_speed = -maxTurnSpeed;
        }
    }
    else if (desired_look_angle < -viewThreshold) {
        motion.turn_speed += turnSpeed;
        if (motion.turn_speed > maxTurnSpeed) {
            motion.turn_speed = maxTurnSpeed;
        }
    }
    else {
        motion.turn_speed = 0.0f;
        motion.look_angle = atan2(direction.y, direction.x) + offset;
    }
}

// https://vanhunteradams.com/Pico/Animal_Movement/Boids-algorithm.html
// Algorithm adapted from the above link
void AISystem::handleBoidMovement(Entity entity, Motion& motion, float elapsed_ms, const vec2& playerPosition, float separationForce, float separationWeight, float friendlyAvoidanceDistance, float obstacleAvoidanceDistance, float projectileAvoidanceDistance, float playerAvoidanceDistance, float alignmentForce, float alignmentDistance, float alignmentWeight, float cohesionForce, float cohesionWeight, float cohesionDistance, float playerFollowDistance, float maxSpeed, float forceWeight) {
    // SEPERATION
    vec2 separation(0.0f, 0.0f); // Separation accumulator
    // Calculate separation force based on all motion entities
    float close_dx = 0.0f, close_dy = 0.0f;
    // avoid other ais
    for (auto& otherEntity : registry.ais.entities) { // Could be optimized to consider only nearby entities or a subset
        if (otherEntity == entity) continue; // Skip self

        Motion& otherMotion = registry.motions.get(otherEntity);
        float distance = length(motion.position - otherMotion.position);
        // avoid other entities if too close
        if (distance < friendlyAvoidanceDistance) {
            close_dx += motion.position.x - otherMotion.position.x;
            close_dy += motion.position.y - otherMotion.position.y;

        }
    }
    // avoid obstacles
    for (auto& otherEntity : registry.obstacles.entities) { // Could be optimized to consider only nearby entities or a subset
        if (otherEntity == entity || registry.ais.has(otherEntity)) continue; // Skip self

        Motion& otherMotion = registry.motions.get(otherEntity);
        float distance = length(motion.position - otherMotion.position);
        // avoid other entities if too close
        if (distance < obstacleAvoidanceDistance) {
            close_dx += motion.position.x - otherMotion.position.x;
            close_dy += motion.position.y - otherMotion.position.y;
        }
    }
    // avoid projectiles
    for (auto& otherEntity : registry.projectiles.entities) { // Could be optimized to consider only nearby entities or a subset
        if (otherEntity == entity || registry.ais.has(otherEntity)) continue; // Skip self

        Motion& otherMotion = registry.motions.get(otherEntity);
        float distance = length(motion.position - otherMotion.position);
        // avoid other entities if too close
        if (distance < projectileAvoidanceDistance) {
            close_dx += motion.position.x - otherMotion.position.x;
            close_dy += motion.position.y - otherMotion.position.y;
        }
    }
    // avoid player
    float distanceToPlayer = length(motion.position - playerPosition);
    if (distanceToPlayer < playerAvoidanceDistance) {
        close_dx += motion.position.x - playerPosition.x;
        close_dy += motion.position.y - playerPosition.y;
    }
    separation.x += close_dx * separationForce;
    separation.y -= close_dy * separationForce;


    // ALLIGNMENT
    vec2 alignment(0.0f, 0.0f); // Alignment accumulator
    // Calculate group's average velocity for alignment force
    float x_vel_avg = 0.0f, y_vel_avg = 0.0f;
    int neighborCount = 0;
    for (auto& otherAI : registry.ais.entities) {
        if (otherAI == entity) continue; // Skip self

        Motion& otherMotion = registry.motions.get(otherAI);
        float distance = length(motion.position - otherMotion.position);

        if (distance < alignmentDistance) {
            x_vel_avg += otherMotion.velocity.x;
            y_vel_avg += otherMotion.velocity.y;
            neighborCount++;
        }
    }
    if (neighborCount > 0) {
        x_vel_avg /= neighborCount;
        y_vel_avg /= neighborCount;
        alignment.y += (y_vel_avg - motion.velocity.y) * alignmentForce;
        alignment.x += (x_vel_avg - motion.velocity.x) * alignmentForce;
    }


    // COHESION
    vec2 cohesion(0.0f, 0.0f); // Cohesion accumulator
    // Calculate group's center of mass for cohesion force
    float x_pos_avg = 0.0f, y_pos_avg = 0.0f;
    neighborCount = 0;
    //for (auto& otherAI : registry.ais.entities) {
    //    if (otherAI == entity) continue; // Skip self

    //    Motion& otherMotion = registry.motions.get(otherAI);
    //    float distance = length(motion.position - otherMotion.position);

    //    if (distance < cohesionDistance) {
    //        x_pos_avg += otherMotion.position.x;
    //        y_pos_avg += otherMotion.position.y;
    //        neighborCount++;
    //    }
    //}
    // stay close to player
    if (distanceToPlayer > playerFollowDistance) {
        x_pos_avg += playerPosition.x;
        y_pos_avg += playerPosition.y;
        neighborCount++;
    }
    if (neighborCount > 0) {
        x_pos_avg /= neighborCount;
        y_pos_avg /= neighborCount;
        cohesion.x += (x_pos_avg - motion.position.x) * cohesionForce;
        cohesion.y += (y_pos_avg - motion.position.y) * cohesionForce;
    }

    // Combine all behaviors with weighted factors
    vec2 steer = (alignment * alignmentWeight) + (cohesion * cohesionWeight) + (separation * separationWeight);

    // Turn towards the center of the world if near the edge
    float topMargin = yMin + 2 * cellSize + halfCellSize;
    float bottomMargin = yMax - 2 * cellSize - halfCellSize;
    float leftMargin = xMin + 2 * cellSize + halfCellSize;
    float rightMargin = xMax - 2 * cellSize - halfCellSize;

    float turn_factor = 100.f;
    if (motion.position.x <= leftMargin) {
        steer.x += turn_factor;
    }
    else if (rightMargin <= motion.position.x) {
        steer.x -= turn_factor;
    }
    if (motion.position.y <= topMargin) {
        steer.y += turn_factor;
    }
    else if (bottomMargin <= motion.position.y) {
        steer.y -= turn_factor;
    }

    motion.velocity += steer * forceWeight;
    motion.velocity = limit(motion.velocity, maxSpeed);
}

void AISystem::handleMeleeAI(Entity entity, Motion& motion, AI& ai, float elapsed_ms, const vec2& playerPosition) {
    float speed = 100.0f; // Tune this value as needed
    float maxTurnSpeed = 3.f; // Maximum turn speed
    float turnSpeed = 0.1f; // Turn acceleration
    float viewThreshold = .5f; // Threshold for angle difference

    float separationForce = 1.0f; // base separation force
    float separationWeight = .5f; // separation weight when combined with other forces
    float friendlyAvoidanceDistance = 150.0f; // Distance to avoid other entities
    float obstacleAvoidanceDistance = 50.0f; // Distance to avoid obstacles
    float projectileAvoidanceDistance = 150.0f; // Distance to avoid projectiles
    float playerAvoidanceDistance = 0.0f; // Distance to avoid player

    float alignmentForce = 1.f; // base alignment force
    float alignmentWeight = .5; // alignment weight when combined with other forces
    float alignmentDistance = 200.0f; // Distance to consider for alignment

    float cohesionForce = 10.f; // base cohesion force
    float cohesionWeight = 10.5f; // cohesion weight when combined with other forces
    float cohesionDistance = 0.0f; // Distance to consider for cohesion
    float playerFollowDistance = 0.0f; // Distance to follow the player

    float maxSpeed = 200.0f; // Adjust as needed
    float forceWeight = 1.f; // Overall weight for steering force

    float distanceToPlayer = length(playerPosition - motion.position);

    handleBoidMovement(entity, motion, elapsed_ms, playerPosition, separationForce, separationWeight, friendlyAvoidanceDistance, obstacleAvoidanceDistance, projectileAvoidanceDistance, playerAvoidanceDistance, alignmentForce, alignmentDistance, alignmentWeight, cohesionForce, cohesionWeight, cohesionDistance, playerFollowDistance, maxSpeed, forceWeight);

    // Check for line of sight
    if (lineOfSightClear(motion.position, playerPosition)) {
        // Rotate towards player if in line of sight
        rotateTowardsPlayer(motion, playerPosition, viewThreshold, turnSpeed, maxTurnSpeed, M_PI / 2);
    }
    else {
        motion.turn_speed = 0.0f;
    }
}

void AISystem::handleRangedAI(Entity entity, Motion& motion, AI& ai, float elapsed_ms, const vec2& playerPosition) {
    float maxTurnSpeed = 3.f; // Maximum turn speed
    float turnSpeed = 0.1f; // Turn acceleration
    float viewThreshold = .5f; // Threshold for angle difference that determines when it is facing the player

    float shootingRange = 500.0f; // Distance to shoot at the player

    float separationForce = 1.0f; // base separation force
    float separationWeight = 1.0f; // separation weight when combined with other forces
    float friendlyAvoidanceDistance = 150.0f; // Distance to avoid other entities
    float obstacleAvoidanceDistance = 50.0f; // Distance to avoid obstacles
    float projectileAvoidanceDistance = 150.0f; // Distance to avoid projectiles
    float playerAvoidanceDistance = 200.0f; // Distance to avoid player

    float alignmentForce = 1.f; // base alignment force
    float alignmentWeight = .5f; // alignment weight when combined with other forces
    float alignmentDistance = 200.0f; // Distance to consider for alignment

    float cohesionForce = 1.f; // base cohesion force
    float cohesionWeight = .5f; // cohesion weight when combined with other forces
    float cohesionDistance = 200.0f; // Distance to consider for cohesion
    float playerFollowDistance = 300.0f; // Distance to follow the player

    float maxSpeed = 200.0f; // Adjust as needed
    float forceWeight = 0.01f; // Overall weight for steering force

    float distanceToPlayer = length(playerPosition - motion.position);

    handleBoidMovement(entity, motion, elapsed_ms, playerPosition, separationForce, separationWeight, friendlyAvoidanceDistance, obstacleAvoidanceDistance, projectileAvoidanceDistance, playerAvoidanceDistance, alignmentForce, alignmentDistance, alignmentWeight, cohesionForce, cohesionWeight, cohesionDistance, playerFollowDistance, maxSpeed, forceWeight);

    // Shooting logic
    ai.shootingCooldown -= elapsed_ms / 1000.f; // Convert milliseconds to seconds
    if (lineOfSightClear(motion.position, playerPosition)) {
        vec2 direction = normalize(playerPosition - motion.position);
        rotateTowardsPlayer(motion, playerPosition, viewThreshold, turnSpeed, maxTurnSpeed, -M_PI / 2);

        if (distanceToPlayer <= shootingRange && ai.shootingCooldown <= 0) {
            vec2 shootingDirection = normalize(playerPosition - motion.position);
            float shootingAngle = atan2(shootingDirection.y, shootingDirection.x);
            createEnemySniperProjectile(renderer, motion.position, shootingAngle, entity);
            ai.shootingCooldown = 1.f; // Reset cooldown
        }
    }
    else {
        motion.turn_speed = 0.0f;
    }
}

void AISystem::handleShotgunAI(Entity entity, Motion& motion, AI& ai, float elapsed_ms, const vec2& playerPosition) {
    float maxTurnSpeed = 3.f; // Maximum turn speed
    float turnSpeed = 0.1f; // Turn acceleration
    float viewThreshold = .5f; // Threshold for angle difference that determines when it is facing the player

    float shootingRange = 250.0f; // Distance to shoot at the player

    float separationForce = 1.0f; // base separation force
    float separationWeight = 1.0f; // separation weight when combined with other forces
    float friendlyAvoidanceDistance = 150.0f; // Distance to avoid other entities
    float obstacleAvoidanceDistance = 50.0f; // Distance to avoid obstacles
    float projectileAvoidanceDistance = 150.0f; // Distance to avoid projectiles
    float playerAvoidanceDistance = 50.0f; // Distance to avoid player

    float alignmentForce = 1.f; // base alignment force
    float alignmentWeight = .5f; // alignment weight when combined with other forces
    float alignmentDistance = 200.0f; // Distance to consider for alignment

    float cohesionForce = 1.f; // base cohesion force
    float cohesionWeight = .5f; // cohesion weight when combined with other forces
    float cohesionDistance = 200.0f; // Distance to consider for cohesion
    float playerFollowDistance = 100.f; // Distance to follow the player

    float maxSpeed = 250.0f; // Adjust as needed
    float forceWeight = 0.01f; // Overall weight for steering force

    float distanceToPlayer = length(playerPosition - motion.position);

    // Movement logic considering player's distance and projectiles
    handleBoidMovement(entity, motion, elapsed_ms, playerPosition, separationForce, separationWeight, friendlyAvoidanceDistance, obstacleAvoidanceDistance, projectileAvoidanceDistance, playerAvoidanceDistance, alignmentForce, alignmentDistance, alignmentWeight, cohesionForce, cohesionWeight, cohesionDistance, playerFollowDistance, maxSpeed, forceWeight);

    // Shooting logic
    ai.shootingCooldown -= elapsed_ms / 1000.f; // Convert milliseconds to seconds
    if (lineOfSightClear(motion.position, playerPosition)) {
        vec2 direction = normalize(playerPosition - motion.position);
        //motion.look_angle = atan2(direction.y, direction.x) - M_PI/2;
        rotateTowardsPlayer(motion, playerPosition, viewThreshold, turnSpeed, maxTurnSpeed, - M_PI / 2);

        if (distanceToPlayer <= shootingRange && ai.shootingCooldown <= 0) {
            vec2 shootingDirection = normalize(playerPosition - motion.position);
            float shootingAngle = atan2(shootingDirection.y, shootingDirection.x);
            for (int i = 0; i < 10; i++) {
                createShotgunProjectile(renderer, motion.position, shootingAngle, 0.0, 0.0, i, entity);
            }
            ai.shootingCooldown = 1.5f; // Reset cooldown
        }
    }
    else {
		motion.turn_speed = 0.0f;
	}
}

void AISystem::handleRocketAI(Entity entity, Motion& motion, AI& ai, float elapsed_ms, const vec2& playerPosition) {
    float maxTurnSpeed = 3.f; // Maximum turn speed
    float turnSpeed = 0.1f; // Turn acceleration
    float viewThreshold = .5f; // Threshold for angle difference that determines when it is facing the player

    float shootingRange = 500.0f; // Distance to shoot at the player

    float separationForce = 1.0f; // base separation force
    float separationWeight = 1.5f; // separation weight when combined with other forces
    float friendlyAvoidanceDistance = 150.0f; // Distance to avoid other entities
    float obstacleAvoidanceDistance = 100.0f; // Distance to avoid obstacles
    float projectileAvoidanceDistance = 250.0f; // Distance to avoid projectiles
    float playerAvoidanceDistance = 50.0f; // Distance to avoid player

    float alignmentForce = 1.f; // base alignment force
    float alignmentDistance = 200.0f; // Distance to consider for alignment
    float alignmentWeight = .5f; // alignment weight when combined with other forces

    float cohesionForce = 1.f; // base cohesion force
    float cohesionWeight = .5f; // cohesion weight when combined with other forces
    float cohesionDistance = 200.0f; // Distance to consider for cohesion
    float playerFollowDistance = 300.0f; // Distance to follow the player

    float maxSpeed = 50.0f; // Adjust as needed
    float forceWeight = 0.01f; // Overall weight for steering force

    float distanceToPlayer = length(playerPosition - motion.position);

    // Movement logic considering player's distance and projectiles
    handleBoidMovement(entity, motion, elapsed_ms, playerPosition, separationForce, separationWeight, friendlyAvoidanceDistance, obstacleAvoidanceDistance, projectileAvoidanceDistance, playerAvoidanceDistance, alignmentForce, alignmentDistance, alignmentWeight, cohesionForce, cohesionWeight, cohesionDistance, playerFollowDistance, maxSpeed, forceWeight);
    
    // Shooting logic
    ai.shootingCooldown -= elapsed_ms / 1000.f; // Convert milliseconds to seconds
    if (lineOfSightClear(motion.position, playerPosition)) {
        vec2 direction = normalize(playerPosition - motion.position);
        //motion.look_angle = atan2(direction.y, direction.x) + M_PI / 2;
        rotateTowardsPlayer(motion, playerPosition, viewThreshold, turnSpeed, maxTurnSpeed, M_PI / 2);

        if (distanceToPlayer <= shootingRange && ai.shootingCooldown <= 0) {
            vec2 shootingDirection = normalize(playerPosition - motion.position);
            float shootingAngle = atan2(shootingDirection.y, shootingDirection.x);
            for (int i = 0; i < 5; i++) {
                createEnemyRocketProjectile(renderer, motion.position, shootingAngle, entity);
            }
            ai.shootingCooldown = 1.5f; // Reset cooldown
        }
    }
    else {
        motion.turn_speed = 0.0f;
	
    }
}

void AISystem::handleFlameAI(Entity entity, Motion& motion, AI& ai, float elapsed_ms, const vec2& playerPosition) {
    float maxTurnSpeed = 3.f; // Maximum turn speed
    float turnSpeed = 0.1f; // Turn acceleration
    float viewThreshold = .5f; // Threshold for angle difference that determines when it is facing the player

    float shootingRange = 250.0f; // Distance to shoot at the player

    float separationForce = 1.0f; // base separation force
    float separationWeight = 1.0f; // separation weight when combined with other forces
    float friendlyAvoidanceDistance = 150.0f; // Distance to avoid other entities
    float obstacleAvoidanceDistance = 100.0f; // Distance to avoid obstacles
    float projectileAvoidanceDistance = 250.0f; // Distance to avoid projectiles
    float playerAvoidanceDistance = 50.0f; // Distance to avoid player

    float alignmentForce = 1.f; // base alignment force
    float alignmentDistance = 50.0f; // Distance to consider for alignment
    float alignmentWeight = .5f; // alignment weight when combined with other forces

    float cohesionForce = 1.f; // base cohesion force
    float cohesionWeight = .5f; // cohesion weight when combined with other forces
    float cohesionDistance = 50.0f; // Distance to consider for cohesion
    float playerFollowDistance = 300.0f; // Distance to follow the player

    float maxSpeed = 150.0f; // Adjust as needed
    float forceWeight = 0.01f; // Overall weight for steering force

    float distanceToPlayer = length(playerPosition - motion.position);

    // Movement logic considering player's distance and projectiles
    handleBoidMovement(entity, motion, elapsed_ms, playerPosition, separationForce, separationWeight, friendlyAvoidanceDistance, obstacleAvoidanceDistance, projectileAvoidanceDistance, playerAvoidanceDistance, alignmentForce, alignmentDistance, alignmentWeight, cohesionForce, cohesionWeight, cohesionDistance, playerFollowDistance, maxSpeed, forceWeight);

    // Shooting logic
    ai.shootingCooldown -= elapsed_ms / 1000.f; // Convert milliseconds to seconds
    if (lineOfSightClear(motion.position, playerPosition)) {
        vec2 direction = normalize(playerPosition - motion.position);
        //motion.look_angle = atan2(direction.y, direction.x) - M_PI / 2;
        // too short need to add pi/4
        // pi/2 + pi/4 = 3pi/4
        rotateTowardsPlayer(motion, playerPosition, viewThreshold, turnSpeed, maxTurnSpeed, -3 * M_PI / 4);

        if (distanceToPlayer <= shootingRange && ai.shootingCooldown <= 0) {
            vec2 shootingDirection = normalize(playerPosition - motion.position);
            float shootingAngle = atan2(shootingDirection.y, shootingDirection.x);
            createEnemyFlamethrowerProjectile(renderer, motion.position, shootingAngle, entity);
            ai.shootingCooldown = 0.05f; // Reset cooldown
        }
    }
    else {
        motion.turn_speed = 0.0f;
    }
}

void AISystem::handleTurretAI(Entity entity, Motion& motion, AI& ai, float elapsed_ms, const vec2& playerPosition) {
    float maxTurnSpeed = 2.f; // Maximum turn speed
    float turnSpeed = 2.f; // Turn acceleration
    float viewThreshold = .5f; // Threshold for angle difference
    // Check for line of sight to the player
    if (lineOfSightClear(motion.position, playerPosition)) {
        // Rotate turret to face player - Calculate the angle between the turret and the player
        rotateTowardsPlayer(motion, playerPosition, viewThreshold, turnSpeed, maxTurnSpeed, M_PI / 2);

        // Shooting logic, no range for these long ranged turrets
        ai.shootingCooldown -= elapsed_ms / 1000.f; // Cooldown reduction
        if (ai.shootingCooldown <= 0) {
            vec2 shootingDirection = normalize(playerPosition - motion.position);
            float shootingAngle = atan2(shootingDirection.y, shootingDirection.x);
            createEnemyProjectile(renderer, motion.position, shootingAngle, entity);
            ai.shootingCooldown = .5f; // Reset cooldown
        }
    }
    else {
        motion.turn_speed = 0.0f; // Stop turning if player is not in line of sight
    }
}
