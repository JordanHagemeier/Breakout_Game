#pragma once
#include <vector>
#include <SFML/Main.hpp>
//setup tile gameobjects in array
//-> is this thing on?
//-> position
//-> id?
//-> graphics
//-> hit count

enum TileType {
	AddedBall,
	QuickerPlayer,
	NoEvent,
	TileTCount
};

class Tile {
public:
	bool			isAlive = true;
	sf::Vector2f	position;
	int				hitCount = 0;
	sf::Color		color;
	TileType		tileType; 
	Tile(){}
	Tile(sf::Vector2f pos, TileType type, sf::Color col) {
		isAlive = true;
		position = pos;
		hitCount = 0;
		color = col;
	}
};