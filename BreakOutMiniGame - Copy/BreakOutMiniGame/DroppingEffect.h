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


class DroppingEffectManager {
	public:
	std::vector<DroppingEffect> m_CurrentlyShownEffects;

	DroppingEffectManager(){}
	DroppingEffectManager(std::vector<Tile*> tiles, const int tileArrayLength, RenderManager& renderManager){

		for (int i = 0; i < tileArrayLength; i++) {
			//setup effects for each tile
			DroppingEffect* newEffect = new DroppingEffect();

			sf::RectangleShape* rect = new sf::RectangleShape(sf::Vector2f(10.0f, 10.0f));
			rect->setPosition(tiles[i]->position);
			rect->setFillColor(tiles[i]->baseColor);
			std::shared_ptr<sf::RectangleShape>rect_shared = std::make_shared<sf::RectangleShape>(*rect);

			newEffect->m_TileEffectType = tiles[i]->tileType;
			newEffect->m_VisualID = renderManager.AddShape(rect_shared);
			newEffect->m_Position = rect->getPosition();
			newEffect->m_BaseColor = rect->getFillColor();
			m_CurrentlyShownEffects.push_back(*newEffect);
		}
	}
	void DrawTileEffects(RenderManager& renderManager);
	
};