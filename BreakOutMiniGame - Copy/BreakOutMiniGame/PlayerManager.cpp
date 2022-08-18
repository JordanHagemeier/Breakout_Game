#include "PlayerManager.h"
#include "GameManager.h"

sf::Vector2f PlayerManager::m_Dimensions = sf::Vector2f(0.0f, 0.0f);
sf::Vector2f PlayerManager::m_Position = sf::Vector2f(0.0f, 0.0f);

void PlayerManager::SetUpPlayerInitData() {
	RenderManager& renderManager = static_cast<RenderManager&>(*GameManager::m_ManagerMap[ManagerType::renderManager_T]);
	assert(renderManager.HasFinishedInitialization(), "Render Manager not yet fully initialized! See initialization process & order.");

	m_Dimensions = sf::Vector2f(55.0f, 10.0f) * renderManager.m_ScalingFactor;
	m_Position = sf::Vector2f(0.33f * renderManager.m_WindowDimensions_px.x, 0.8f * renderManager.m_WindowDimensions_px.y);
}


void PlayerManager::CreateNewPlayer() {
	RenderManager& renderManager = static_cast<RenderManager&>(*GameManager::m_ManagerMap[ManagerType::renderManager_T]);
	assert(renderManager.HasFinishedInitialization(), "Render Manager not yet fully initialized! See initialization process & order.");

	Player* newPlayer = new Player(m_Dimensions, m_Position);
	newPlayer->InitializeVisuals(renderManager);
	m_Players.push_back(newPlayer);
}


void PlayerManager::UpdatePlayers() {
	RenderManager& renderManager = static_cast<RenderManager&>(*GameManager::m_ManagerMap[ManagerType::renderManager_T]);
	
	for (Player* player : m_Players) {
		if (player->m_PlayerWasGivenInput) {
			player->CheckPlayerForBuffEffect(renderManager);
			player->UpdatePlayerVisuals(renderManager);
			player->m_PlayerWasGivenInput = false;
		}
		
	}
}

void PlayerManager::TerminateManager() {
	ManagerInterface* ptrToRenderManager = GameManager::GetManagerByType(ManagerType::renderManager_T);
	if (ptrToRenderManager == nullptr) {
		std::cout << "RenderManager not yet initialized!" << std::endl;
		return;
	}
	RenderManager& renderManager = static_cast<RenderManager&>(*ptrToRenderManager);
	assert(renderManager.HasFinishedInitialization(), "Render Manager not yet fully initialized! See initialization process & order.");

	for (Player* player : m_Players) {
		renderManager.DeleteShape(player->m_VisualID[0]);
		renderManager.DeleteShape(player->m_VisualID[1]);
		renderManager.DeleteShape(player->m_VisualID[2]);

	}
}