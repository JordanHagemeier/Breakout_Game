#pragma once
#include "ManagerInterface.h"
#include <SFML/Graphics.hpp>
#include "Player.h"
#include <vector>
#include <cassert>

class PlayerManager : public ManagerInterface {

	bool m_InitializationIsDone = false;
	static sf::Vector2f m_Dimensions;
	static sf::Vector2f m_Position;
public: 
	std::vector<Player*> m_Players;
	PlayerManager() = default;

	virtual bool HasFinishedInitialization() const { return m_InitializationIsDone; };
	virtual ManagerType GetManagerType()	{return ManagerType::playerManager_T;}
	virtual void TickBeforeStart(){

		SetUpPlayerInitData();
		CreateNewPlayer();
		m_InitializationIsDone = true;
	}
	virtual void Tick() {
		UpdatePlayers();
	}

	virtual void TerminateManager();
	void SetUpPlayerInitData();
	void CreateNewPlayer();
	void UpdatePlayers();
};