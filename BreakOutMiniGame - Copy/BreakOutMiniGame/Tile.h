#pragma once
#include <vector>
#include <SFML/Graphics.hpp>
#include "TileType.h"
#include "RenderManager.h"
#include "MathHelper.h"

//setup tile gameobjects in array
//-> is this thing on?
//-> position
//-> id?
//-> graphics
//-> hit count

//enum class TileType {
//	AddedBall,
//	QuickerPlayer,
//	NoEvent,
//	TileTCount
//};

class Tile {
public:
	static const float OUTLINE_THICKNESS;
	sf::Color		maxHitColor = sf::Color::Red;

	int				tileVisualID = -1;
	int				hitCount = 0;
	int				allowedHits = 0;
	bool			isAlive = true;
	sf::Color		baseColor = sf::Color::White;
	sf::Vector2f	position = sf::Vector2f(0.0f, 0.0f);
	TileType		tileType = TileType::AddedBall; 
	
	Tile() = default;
	Tile(sf::Vector2f pos, TileType type, sf::Color col, int id);
	

	bool UpdateTileColorBasedOnHits(RenderManager& renderManager);
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