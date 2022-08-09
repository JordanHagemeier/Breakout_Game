#pragma once
#include "ManagerInterface.h"
#include <vector>
#include "Tile.h"
#include <SFML/Graphics.hpp>
#include "DroppingEffect.h"



class DroppingEffectManager : public ManagerInterface {
	int m_TileArrayLength;

public:
	
	std::vector<DroppingEffect> m_CurrentlyShownEffects;

	DroppingEffectManager() {}
	DroppingEffectManager(const int tileArrayLength) {

		m_TileArrayLength = tileArrayLength;
	}


	virtual ManagerType GetManagerType() { return ManagerType::droppingEffectManager_T; }
	virtual void TickBeforeStart() {
		SetupDroppingEffects();
	}
	virtual void Tick()
	{
		UpdateTileEffectVisuals();
	}
	void SetupDroppingEffects();
	void UpdateTileEffectVisuals();
	void ActivateTileEffect(int arrayPosition);

};