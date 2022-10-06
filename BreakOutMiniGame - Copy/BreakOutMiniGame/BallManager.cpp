#include "BallManager.h"
#include "PlayerManager.h"
#include <cassert>

 sf::Vector2f BallManager::m_BallStarterPosition_UNALTERED = sf::Vector2f(0.0f, 0.0f);
 sf::Vector2f BallManager::m_BallStarterPosition_ALTERED = sf::Vector2f(0.0f, 0.0f);
 const float BallManager::STARTER_ANGLE = 315.0f;

void BallManager::CreateNewBall() {
	RenderManager& renderManager = static_cast<RenderManager&>(*GameManager::m_ManagerMap[ManagerType::renderManager_T]);
	assert(renderManager.HasFinishedInitialization(), "Render Manager not yet fully initialized! See initialization process & order.");

	std::shared_ptr<sf::CircleShape> ball_Visual = std::make_shared<sf::CircleShape>(sf::CircleShape(0.0f));
	int ballVisID = renderManager.AddShape(ball_Visual);

	BallInfo info;
	info.radius			= 2.0f * renderManager.m_ScalingFactor;
	info.position		= m_BallStarterPosition_UNALTERED;
	info.angle			= STARTER_ANGLE;
	info.scalingFactor	= renderManager.m_ScalingFactor;
	info.visualID		= ballVisID;
	Ball* ball		= new Ball(info);

	//ball->ballPosition = m_BallStarterPosition_UNALTERED;
	//ball->ballDirection = sf::Vector2f(std::cos(ball->ballAngle) * Ball::BALL_BASE_SPEED, std::sin(ball->ballAngle) * Ball::BALL_BASE_SPEED);

	ball_Visual->setRadius(ball->ballRadius);
	ball_Visual->setFillColor(sf::Color::Red);
	ball_Visual->setPosition(ball->ballPosition);

	m_Balls_In_Use.push_back(ball);
	
}

void BallManager::UpdateBallBehavior() {
	for(Ball* ball : m_Balls_In_Use)
	{
		ball->Update();

	
	}
}

void BallManager::UpdateBallVisualPositions() {
	for (Ball* ball : m_Balls_In_Use) {
		RenderManager& renderManager = static_cast<RenderManager&>(*GameManager::m_ManagerMap[ManagerType::renderManager_T]);
		std::shared_ptr<sf::CircleShape> currentBallVisual = std::static_pointer_cast<sf::CircleShape>(renderManager.GetShape(ball->m_VisualID));
		currentBallVisual->setPosition(ball->ballPosition);

	}
}

void BallManager::TerminateManager() {
	ManagerInterface* ptrToRenderManager = GameManager::GetManagerByType(ManagerType::renderManager_T);
	if (ptrToRenderManager == nullptr) {
		std::cout << "RenderManager not yet initialized!" << std::endl;
		return;
	}
	RenderManager& renderManager = static_cast<RenderManager&>(*ptrToRenderManager);
	assert(renderManager.HasFinishedInitialization(), "Render Manager not yet fully initialized! See initialization process & order.");

	for (Ball* ball : m_Balls_In_Use) {
		renderManager.DeleteShape(ball->m_VisualID);
		delete ball;
	}
}