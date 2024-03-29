#pragma once
//#include "Ball.h"
//#include "Tile.h"
//#include "Player.h"
#include <vector>
#include <map>
#include <SFML/Graphics.hpp>
//#include "DroppingEffect.h"
//#include "RenderManager.h"
//#include "BallManager.h"
//#include "TileManager.h"
#include "ManagerInterface.h"
#include "TileType.h"


//FORWARD INCLUDES
struct GameManagerInfo {
	//std::vector<Ball*>* balls;
	//std::vector<Tile*>* tiles;
	//std::vector<Player*>* players;

	std::map<ManagerType, ManagerInterface*> managerMap;
	std::map<TileType, sf::Color> tileTypeToColorMap;


};
class GameManager : public ManagerInterface{


	public:
		static std::map<ManagerType, ManagerInterface*> m_ManagerMap;
		static std::map<TileType, sf::Color> m_TileTypeToColorMap;
		static sf::Color m_CrashColor;

		GameManager(GameManagerInfo info) {
		
			m_ManagerMap			= info.managerMap;
			m_TileTypeToColorMap	= info.tileTypeToColorMap;

		}

		virtual bool HasFinishedInitialization() const { return true; };
		virtual ManagerType GetManagerType() { return ManagerType::gameManager_T; };
		virtual void TickBeforeStart() {};
		virtual void Tick() {};
		virtual void TerminateManager(){ m_TileTypeToColorMap.clear(); m_ManagerMap.clear(); };
		
		static sf::Color GetTileColorByType(TileType type);
		static ManagerInterface* GetManagerByType(ManagerType type);
		static void TerminateGame();
};
