#include "GameManager.h"


std::vector<Ball*>* GameManager::m_Balls_In_Use = nullptr;
std::vector<Tile*>* GameManager::m_Tiles = nullptr;
std::map<ManagerType, ManagerInterface*>GameManager::m_ManagerMap;
std::map<TileType, sf::Color> GameManager::m_TileTypeToColorMap;