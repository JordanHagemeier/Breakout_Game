#pragma once
#include <SFML/Graphics.hpp>
#include "TileType.h"
#include "RenderManager.h"
#include <vector>
#include "Tile.h"

class DroppingEffect {
public:
	TileType			m_TileEffectType = TileType::NoEvent;
	int					m_VisualID;
	bool				m_IsActive = false;
	sf::Vector2f		m_Position; 
	sf::Color			m_BaseColor;

	DroppingEffect() {
	}
	void SetType(TileType type){ m_TileEffectType = type;}
	void SetActive(bool state){ m_IsActive = state;}
};


