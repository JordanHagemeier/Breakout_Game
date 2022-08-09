#pragma once
#include "Ball.h"
#include "Tile.h"
#include "Player.h"
#include <vector>
//#include "DroppingEffect.h"
//#include "RenderManager.h"
//#include "BallManager.h"
//#include "TileManager.h"

//FORWARD INCLUDES
struct GameManagerInfo {
	std::vector<Ball*>* balls;
	std::vector<Tile*>* tiles;
	std::vector<Player*>* players;

	std::map<ManagerType, ManagerInterface*> managerMap;
	std::map<TileType, sf::Color> tileTypeToColorMap;


};
class GameManager {

	public:
		static std::vector<Ball*>* m_Balls_In_Use;
		static std::vector<Tile*>* m_Tiles;
		static std::vector<Player*>* m_Players;


		static std::map<ManagerType, ManagerInterface*> m_ManagerMap;
		static std::map<TileType, sf::Color> m_TileTypeToColorMap;

		GameManager(GameManagerInfo info) {
			m_Balls_In_Use = info.balls;
			m_Tiles = info.tiles;
			m_Players = info.players;

			m_ManagerMap			= info.managerMap;
			m_TileTypeToColorMap	= info.tileTypeToColorMap;

		}
};
