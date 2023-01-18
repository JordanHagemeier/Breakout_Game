#pragma once
#include "ManagerInterface.h"
#include <vector>
#include "Tile.h"
#include <SFML/Graphics.hpp>
#include "DroppedBooster.h"
#include "Player.h"



class BoosterDropManager : public ManagerInterface {
	int m_TileArrayLength;
	bool m_InitializationIsDone = false;
public:
	
	std::vector<DroppedBooster> m_CurrentlyShownEffects;

	BoosterDropManager() {}
	BoosterDropManager(const int tileArrayLength) {

		m_TileArrayLength = tileArrayLength;
	}


	virtual ManagerType GetManagerType() { return ManagerType::boosterDropManager_T; }
	virtual void TickBeforeStart() {

		SetupDroppingEffects();
		m_InitializationIsDone = true;
	}

	virtual bool HasFinishedInitialization() const { return m_InitializationIsDone; }
	virtual void Tick()
	{
		UpdateTileEffectVisuals();
		CheckForEffectUsage();
	}
	virtual void TerminateManager();

	void SetupDroppingEffects();
	void UpdateTileEffectVisuals();
	void ActivateTileEffect(int arrayPosition);
	TileType CheckEffectWithPlayerCollision(bool* collisionBool, Player& player);
	bool CheckForEffectUsage();
};