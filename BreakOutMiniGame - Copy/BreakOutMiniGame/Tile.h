#pragma once
#include <vector>
#include <SFML/Main.hpp>
//setup tile gameobjects in array
//-> is this thing on?
//-> position
//-> id?
//-> graphics
//-> hit count

enum class TileType {
	AddedBall,
	QuickerPlayer,
	NoEvent,
	TileTCount
};

class Tile {
public:
	bool			isAlive = true;
	sf::Vector2f	position = sf::Vector2f(0.0f, 0.0f);
	int				hitCount = 0;
	int				allowedHits = 0;
	sf::Color		color = sf::Color::White;
	TileType		tileType = TileType::AddedBall; 
	Tile(){}
	Tile(sf::Vector2f pos, TileType type, sf::Color col) {
		isAlive = true;
		position = pos;
		hitCount = 0;
		color = col;
		tileType = type;
		if (type == TileType::AddedBall) {
			hitCount = 3;
			
		}
		if (type == TileType::QuickerPlayer) {
			hitCount = 4;
		}
		if (type == TileType::NoEvent) {
			hitCount = 2;
		}
		allowedHits = hitCount;
	}
};

class DroppingEffect {
	public:
		TileType			effectType = TileType::NoEvent;
		sf::RectangleShape visual;

		DroppingEffect() {}
		DroppingEffect(TileType type) {
			effectType = type;
		}
		DroppingEffect(TileType type, sf::RectangleShape vis) {
			visual = vis;
			effectType = type;
		}
};