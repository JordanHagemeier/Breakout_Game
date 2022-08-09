#pragma once
#include "Ball.h"
#include <vector>
#include "RenderManager.h"
#include "GameManager.h"
#include "CollisionType.h"
#include "TileManager.h"
#define _USE_MATH_DEFINES
#include <math.h>

class BallManager : public ManagerInterface {

	bool m_InitializationIsDone = false;
	public: 
		static sf::Vector2f m_BallStarterPosition_UNALTERED;
		static sf::Vector2f m_BallStarterPosition_ALTERED;

		std::vector<Ball*> m_Balls_In_Use; 


		BallManager() = default;
		

		virtual void TickBeforeStart() {
			RenderManager* renderManager = static_cast<RenderManager*>(GameManager::m_ManagerMap[ManagerType::renderManager_T]);
			m_BallStarterPosition_UNALTERED = sf::Vector2f(renderManager->m_WindowDimensions_px.x * 0.05f, renderManager->m_WindowDimensions_px.y * 0.4f);

			CreateNewBall();
			m_InitializationIsDone = true;
		}

		virtual bool HasFinishedInitialization(){ return m_InitializationIsDone;}
		virtual void Tick(){ 
			UpdateBallBehavior();
		}
		virtual ManagerType GetManagerType() { return ManagerType::ballManager_T; }
		void CreateNewBall();
		void UpdateBallBehavior();

		//I think I would actually have to use a collision manager to make all of this smooth and not twisted with other managers
		bool CheckForWallCollision(Ball& currentBall);
		bool CheckForCollisionWithPlayer(Ball& ball, sf::Vector2f* bounceDirection, sf::Vector2f* nextBallPosition);
		sf::Vector2f CalculateBounceVector(sf::Vector2f futureBallPosition, CollisionType type, Ball& ball, Player& player);
		bool CheckForBallTileCollisionAndMovementChanges(Ball& ball, sf::Vector2f* bounceDirection);
};
