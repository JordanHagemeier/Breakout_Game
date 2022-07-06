#pragma once
#include <vector>
#include <SFML/Main.hpp>
//setup tile gameobjects in array
//-> is this thing on?
//-> position
//-> id?
//-> graphics
//-> hit count

struct Tile {
	bool	isAlive = true;
	sf::Vector2f position;
	int		hitCount = 0;
};