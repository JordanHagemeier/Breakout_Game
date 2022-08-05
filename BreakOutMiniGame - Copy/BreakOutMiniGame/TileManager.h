#pragma once
#include <vector>
#include <SFML/Graphics.hpp>
#include "Tile.h"
#include <random>
#include "MathHelper.h"
#include "RenderManager.h"
#include "TileType.h"

class TileManager {
	sf::Vector2f m_WindowDimensions_px;
	sf::Vector2f m_WindowSegmentDimensions_px;
	//float coveredWindowPercentage;
	
	public:
	static sf::Vector2f TILE_DIMENSIONS;
	static int TILE_ARRAY_LENGTH;
	std::vector<Tile*> m_Tiles;
	TileManager() = default;

	TileManager(sf::Vector2f windowDimensions_px, sf::Vector2f windowSegmentDimensions_px, float coveredWindowPercentage) {
		m_WindowDimensions_px = windowDimensions_px;
		m_WindowSegmentDimensions_px = windowSegmentDimensions_px;
		TILE_DIMENSIONS = sf::Vector2<float>((float)(windowDimensions_px.x / windowSegmentDimensions_px.x), (float)(windowDimensions_px.y / windowSegmentDimensions_px.y) * coveredWindowPercentage);
		TILE_ARRAY_LENGTH = windowSegmentDimensions_px.x * windowSegmentDimensions_px.y;
	}

	bool InitializeTileVector(RenderManager& renderManager, std::map<TileType, sf::Color> tileTypeToColorMap);
};