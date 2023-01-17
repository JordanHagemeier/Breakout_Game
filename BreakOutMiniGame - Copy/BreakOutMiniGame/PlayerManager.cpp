#include "PlayerManager.h"
#include "GameManager.h"
#include "RenderManager.h"

sf::Vector2f PlayerManager::m_Dimensions = sf::Vector2f(0.0f, 0.0f);
sf::Vector2f PlayerManager::m_Position = sf::Vector2f(0.0f, 0.0f);

void PlayerManager::SetUpPlayerInitData() {
	ManagerInterface* ptrToRenderManager = GameManager::GetManagerByType(ManagerType::renderManager_T);
	if (ptrToRenderManager == nullptr) {
		std::cout << "RenderManager not yet initialized!" << std::endl;
		return;
	}
	RenderManager& renderManager = static_cast<RenderManager&>(*ptrToRenderManager);
	assert(renderManager.HasFinishedInitialization(), "Render Manager not yet fully initialized! See initialization process & order.");


	m_Dimensions = sf::Vector2f(55.0f, 10.0f) * renderManager.m_ScalingFactor;
	m_Position = sf::Vector2f(0.33f * renderManager.m_WindowDimensions_px.x, 0.8f * renderManager.m_WindowDimensions_px.y);
}


void PlayerManager::CreateNewPlayer() {
	
	Player* const newPlayer = new Player(m_Dimensions, m_Position);
	newPlayer->InitializeVisuals();
	m_Players.push_back(newPlayer);
}


void PlayerManager::UpdatePlayers() {

	for (Player* player : m_Players) {
		
		player->Update();
		
	}
}

void PlayerManager::TerminateManager() {
	
	for (Player* player : m_Players) {
		player->TerminatePlayer();
		delete player;

	}
	m_Players.clear();
}