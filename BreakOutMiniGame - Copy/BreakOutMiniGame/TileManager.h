#pragma once
#include <vector>
#include <SFML/Graphics.hpp>
#include "Tile.h"
#include <random>
#include "MathHelper.h"
#include "RenderManager.h"
#include "DroppingEffectManager.h"
#include "TileType.h"
#include "ManagerInterface.h"
#include "GameManager.h"

class TileManager : public ManagerInterface {
	bool m_InitializationIsDone = false;
	public:

	static sf::Vector2f TILE_DIMENSIONS;
	static int TILE_ARRAY_LENGTH;

	std::vector<Tile*> m_Tiles;
	float coveredWindowPercentage = 0.33f;


	TileManager() = default;

	virtual ManagerType GetManagerType() { return ManagerType::tileManager_T; }
	virtual void TickBeforeStart() {
		RenderManager& renderManager = static_cast<RenderManager&>(*GameManager::m_ManagerMap[ManagerType::renderManager_T]);
		TILE_DIMENSIONS = sf::Vector2<float>((float)(renderManager.m_WindowDimensions_px.x / renderManager.m_WindowSegmentDimensions_px.x), (float)(renderManager.m_WindowDimensions_px.y / renderManager.m_WindowSegmentDimensions_px.y) * coveredWindowPercentage);
		TILE_ARRAY_LENGTH = renderManager.m_WindowSegmentDimensions_px.x * renderManager.m_WindowSegmentDimensions_px.y;
		InitializeTileVector();
		m_InitializationIsDone = true;
	}
	virtual bool HasFinishedInitialization() const { return m_InitializationIsDone; }

	virtual void Tick() {
		for (int i = 0; i < m_Tiles.size(); i++) {
			RenderManager& renderManager = static_cast<RenderManager&>(*GameManager::m_ManagerMap[ManagerType::renderManager_T]);
			m_Tiles[i]->UpdateTileColorBasedOnHits(renderManager);
		}
	}
	virtual void TerminateManager();

	bool InitializeTileVector();
	void UpdateTileAfterCollision(int tileID);
};