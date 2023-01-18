#pragma once
#include <SFML/Graphics.hpp>
#include "TileType.h"
#include "RenderManager.h"
#include <vector>
#include "Tile.h"

class DroppedBooster {
public:
	TileType			m_TileEffectType = TileType::NoEvent;
	int					m_VisualID;
	bool				m_IsActive = false;
	sf::Vector2f		m_Position; 
	sf::Color			m_BaseColor;

	DroppedBooster() {
	}
	void SetType(TileType type){ m_TileEffectType = type;}
	void SetActive(bool state){ m_IsActive = state;}
};


