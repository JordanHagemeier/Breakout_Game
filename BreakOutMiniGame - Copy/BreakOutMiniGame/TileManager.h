#pragma once
#include <vector>
#include <SFML/Graphics.hpp>
#include "Tile.h"
#include <random>
#include "MathHelper.h"
#include "RenderManager.h"
#include "BoosterDropManager.h"
#include "TileType.h"
#include "ManagerInterface.h"
#include "GameManager.h"

class TileManager : public ManagerInterface {
	bool m_InitializationIsDone = false;
	
	
	public:
	std::vector<Tile> m_Tiles;

	static sf::Vector2f TILE_DIMENSIONS;
	static int TILE_ARRAY_LENGTH;
	static const float OUTLINE_THICKNESS;

	float coveredWindowPercentage = 0.33f;


	TileManager() = default;

	virtual ManagerType GetManagerType() { return ManagerType::tileManager_T; }
	virtual void TickBeforeStart();
	virtual bool HasFinishedInitialization() const { return m_InitializationIsDone; }
	virtual void Tick() {
		for (int i = 0; i < m_Tiles.size(); i++) {
			RenderManager& renderManager = static_cast<RenderManager&>(*GameManager::m_ManagerMap[ManagerType::renderManager_T]);
			m_Tiles[i].UpdateTileColorBasedOnHits(renderManager);
		}
	}
	virtual void TerminateManager();

	bool InitializeTileVector();
	void UpdateTileAfterCollision(int tileID);
	sf::Vector2<float> GetTilePosition(int i, RenderManager& renderManager);
	TileType CreateTileType();
	void NotifyBoosterDropManager(Tile& tile);
	void NotifyParticleEffectManager(Tile& tile);


};