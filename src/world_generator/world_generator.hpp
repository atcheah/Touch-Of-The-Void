#pragma once
#include <components/components.hpp>

// World generator that creates rooms / levels using RNG
class WorldGenerator
{
public:
	WorldGenerator() {}

	void populateFirstRoom(Room& room);

	void populateTutorialRoom(Room& room);

	void generateTutorialRoomOne(Room& room, Level& level);

	void generateTutorialRoomTwo(Room& room, Level& level);

	void generateNewRoom(Room& room, Level& level);

	// populates the fields of a Room
	void populateRoom(Room& room);

	// populates the fields of a Boss Room
	void populateBossRoom(Room& room);

	// populates a shop Room with their objects
	void populateShopRoom(Room& room);
};
