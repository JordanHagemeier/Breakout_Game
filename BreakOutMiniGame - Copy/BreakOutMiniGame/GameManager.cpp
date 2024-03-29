#include "GameManager.h"



std::map<ManagerType, ManagerInterface*>GameManager::m_ManagerMap;
std::map<TileType, sf::Color> GameManager::m_TileTypeToColorMap;
sf::Color GameManager::m_CrashColor = sf::Color::Magenta;

sf::Color GameManager::GetTileColorByType(TileType type) {
	std::map<TileType, sf::Color>::iterator it = m_TileTypeToColorMap.find(type);
	if (it != m_TileTypeToColorMap.end()) {
		return m_TileTypeToColorMap[type];
	}
	return m_CrashColor;
}

ManagerInterface* GameManager::GetManagerByType(ManagerType type) {
	std::map< ManagerType, ManagerInterface*>::iterator it = m_ManagerMap.find(type);
	if (it != m_ManagerMap.end()) {
		ManagerInterface* ptrToManager = m_ManagerMap[type];
		return ptrToManager;
	}
	return nullptr;
}

void GameManager::TerminateGame() {
	std::map< ManagerType, ManagerInterface*>::iterator it;
	for (it = m_ManagerMap.begin(); it != m_ManagerMap.end(); it++) {
		it->second->TerminateManager();
	}
	
}