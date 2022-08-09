#include "BallManager.h"
#include <cassert>

 sf::Vector2f BallManager::m_BallStarterPosition_UNALTERED = sf::Vector2f(0.0f, 0.0f);
 sf::Vector2f BallManager::m_BallStarterPosition_ALTERED = sf::Vector2f(0.0f, 0.0f);

void BallManager::CreateNewBall() {
	RenderManager& renderManager = static_cast<RenderManager&>(*GameManager::m_ManagerMap[ManagerType::renderManager_T]);
	assert(renderManager.HasFinishedInitialization(), "Render Manager not yet fully initialized! See initialization process & order.");

	sf::CircleShape* ballNotSharedPtr = new sf::CircleShape(0.0f);
	std::shared_ptr<sf::CircleShape> ball_Visual = std::make_shared<sf::CircleShape>(*ballNotSharedPtr);

	int ballVisID = renderManager.AddShape(ball_Visual);
	Ball* ball = new Ball(2.0f * renderManager.m_ScalingFactor, m_BallStarterPosition_UNALTERED, 315.0f, renderManager.m_ScalingFactor, *ball_Visual, ballVisID);

	ball->ballPosition = m_BallStarterPosition_UNALTERED;
	ball->ballDirection = sf::Vector2f(std::cos(ball->ballAngle) * Ball::BALL_BASE_SPEED, std::sin(ball->ballAngle) * Ball::BALL_BASE_SPEED);

	ball_Visual->setRadius(ball->ballRadius);
	ball_Visual->setFillColor(sf::Color::Red);
	ball_Visual->setPosition(ball->ballPosition);

	m_Balls_In_Use.push_back(ball);
	
}

void BallManager::UpdateBallBehavior() {
	for(int i = 0; i < m_Balls_In_Use.size(); i++)
	{
		Ball& currentBall = *m_Balls_In_Use[i];
		bool CollisionWithWall = CheckForWallCollision(currentBall);

		sf::Vector2f originalBallDirection = currentBall.ballDirection;
		sf::Vector2f playerCollisionBounceDirection;
		sf::Vector2f tileCollisionBounceDirection;
		sf::Vector2f* ptrToNewDirectionVector = nullptr;

		bool collisionWithPlayer = CheckForCollisionWithPlayer(*m_Balls_In_Use[i], &playerCollisionBounceDirection, ptrToNewDirectionVector);
		bool collisionWithTile = CheckForBallTileCollisionAndMovementChanges(currentBall, &tileCollisionBounceDirection);


		if (collisionWithPlayer) {
			
			currentBall.ballDirection = playerCollisionBounceDirection;
		}

		if (collisionWithTile) {
			currentBall.ballDirection = tileCollisionBounceDirection;
		}
	

		sf::Vector2f newBallPosition = currentBall.ballPosition + currentBall.ballDirection;
		currentBall.ballPosition = newBallPosition;

	}
}

bool BallManager::CheckForWallCollision(Ball& currentBall) {
	float wallDistanceXMin = currentBall.ballPosition.x;
	RenderManager* renderManager = static_cast<RenderManager*>(GameManager::m_ManagerMap[ManagerType::renderManager_T]);

	float wallDistanceXMax = std::abs(renderManager->m_WindowDimensions_px.x - currentBall.ballPosition.x);

	if (wallDistanceXMin < currentBall.repellantDistance || wallDistanceXMax < currentBall.repellantDistance) {
		currentBall.ballDirection = sf::Vector2f(currentBall.ballDirection.x * -1.0f, currentBall.ballDirection.y);
		return true;
	}

	float wallDistanceYMin = currentBall.ballPosition.y;
	float wallDistanceYMax = std::abs(renderManager->m_WindowDimensions_px.y - currentBall.ballPosition.y);

	if (wallDistanceYMin < currentBall.repellantDistance || wallDistanceYMax < currentBall.repellantDistance) {
		currentBall.ballDirection = sf::Vector2f(currentBall.ballDirection.x, currentBall.ballDirection.y * -1.0f);
		return true;
	}
	return false;
}

bool BallManager::CheckForCollisionWithPlayer(Ball& ball, sf::Vector2f* bounceDirection, sf::Vector2f* nextBallPosition) {
	//Todo: copy contents from main.cpp 
	//TODO 
	//1) add a safeguard that the actual setting of the new direction will only commence once this collision check comes back as negative for the possible next location
	bool playerCollideVertical = false;
	bool playerCollideHorizontal = false;

	sf::Vector2f futureBallPosition;
	futureBallPosition = (ball.ballPosition + sf::Vector2f(ball.ballRadius, ball.ballRadius)) + ball.ballDirection;
	if (nextBallPosition != nullptr) {
		futureBallPosition = *nextBallPosition;
	}

	float testingX = futureBallPosition.x;
	float testingY = futureBallPosition.y;

	for (int i = 0; i < GameManager::m_Players->size(); i++) {


		Player& currentPlayer = *(*GameManager::m_Players)[i];
		/*std::vector<Player*>& currentPlayer = *GameManager::m_Players;*/
		float minPlayerX = currentPlayer.m_Position.x - (currentPlayer.m_Dimensions.x / 2.0f);
		float maxPlayerX = currentPlayer.m_Position.x + (currentPlayer.m_Dimensions.x / 2.0f);
		float minPlayerY = currentPlayer.m_Position.y - (currentPlayer.m_Dimensions.y / 2.0f);
		float maxPlayerY = currentPlayer.m_Position.y + (currentPlayer.m_Dimensions.y / 2.0f);

		//      |
		//		*-------*
		//		|       |  rightX
		//		*-------*
		//      |
		//
		//bools to check for sector position of the ball
		bool leftX = false;
		bool rightX = false;
		bool upY = false;
		bool downY = false;

		if (futureBallPosition.x < minPlayerX) { testingX = minPlayerX; leftX = true; }
		else if (futureBallPosition.x > maxPlayerX) { testingX = maxPlayerX; rightX = true; }

		if (futureBallPosition.y < minPlayerY) { testingY = minPlayerY; upY = true; }
		else if (futureBallPosition.y > maxPlayerY) { testingY = maxPlayerY; downY = true; }

		float distanceX = futureBallPosition.x - testingX;
		float distanceY = futureBallPosition.y - testingY;
		float overallDistance = sqrt((distanceX * distanceX) + (distanceY * distanceY));

		//insert corner check here (check for (if ball.pos == one of the corner pos) -> give direction that yeets it in a line from the tile pos- tile corner)
		//code here

		bool cornerWasHit = false;
		bool leftUpperCorner = false;
		bool rightUpperCorner = false;
		bool leftLowerCorner = false;
		bool rightLowerCorner = false;

		if (overallDistance <= ball.ballRadius) {

			//test for which section the ball is in 


			if (upY && leftX) {
				leftUpperCorner = true;
				cornerWasHit = true;
			}
			else if (downY && leftX) {
				leftLowerCorner = true;
				cornerWasHit = true;
			}
			else if (upY && rightX) {
				rightUpperCorner = true;
				cornerWasHit = true;
			}
			else if (downY && rightX) {
				rightLowerCorner = true;
				cornerWasHit = true;
			}

			if (!cornerWasHit && (leftX || rightX)) {


				playerCollideHorizontal = true;
			}

			if (!cornerWasHit && (upY || downY)) {
				playerCollideVertical = true;
			}


		}



		if (cornerWasHit) {
			*bounceDirection = sf::Vector2f(ball.ballDirection.x * -1.0f, ball.ballDirection.y * -1.0f);
			return true;
		}
		if (playerCollideVertical) {
			*bounceDirection = CalculateBounceVector(futureBallPosition, CollisionType::VerticalCollision, ball, *(*GameManager::m_Players)[i]);
			return true;
		}

		if (playerCollideHorizontal) {
			*bounceDirection = sf::Vector2f(ball.ballDirection.x * -1.0f, ball.ballDirection.y);
			return true;
		}



	}



	return false;
}

sf::Vector2f BallManager::CalculateBounceVector(sf::Vector2f futureBallPosition, CollisionType type, Ball& ball, Player& player) {

	sf::Vector2f newBallDirectionVector;
	float deviationAngle = 5.0f;
	float leftAngleInRadianX = cos(((double)270.0f + (double)deviationAngle) * (M_PI / 180.0f));
	float leftAngleInRadianY = sin(((double)270.0f + (double)deviationAngle) * (M_PI / 180.0f));
	float rightAngleInRadianX = cos(((double)270.0f - (double)deviationAngle) * (M_PI / 180.0f));
	float rightAngleInRadianY = sin(((double)270.0f - (double)deviationAngle) * (M_PI / 180.0f));


	sf::Vector2f leftRimNormal = sf::Vector2f(leftAngleInRadianX, leftAngleInRadianY);
	sf::Vector2f rightRimNormal = sf::Vector2f(rightAngleInRadianX, rightAngleInRadianY);


	//player collision ball bounce vector calculation based on where on the player it lands
	//1) where on the player did we land? (percentage areas)
	if (type == CollisionType::VerticalCollision) {
		float proportionalXPos = futureBallPosition.x - (player.m_Position.x - (player.m_Dimensions.x / 2.0f));
		float percentageOnXAxis = proportionalXPos / player.m_Dimensions.x;
		if (percentageOnXAxis < Player::RIM_PERCENTAGE) {
			//cos-1 [ (a · b) / (|a| |b|) ]

			float ballDirTimesNormal = (ball.ballDirection.x * leftRimNormal.x) + (ball.ballDirection.y * leftRimNormal.y);
			float absoluteBallDir = sqrt((ball.ballDirection.x * ball.ballDirection.x) + (ball.ballDirection.y * ball.ballDirection.y));
			float absoluteNormal = sqrt((leftRimNormal.x * leftRimNormal.x) + (leftRimNormal.y * leftRimNormal.y));
			float angleBetweenBallDirectionAndNormal = acos(ballDirTimesNormal / (absoluteBallDir * absoluteNormal)) / (M_PI / 180.0f);
			float newVectorAngle = angleBetweenBallDirectionAndNormal * -2.0f;

			newBallDirectionVector = sf::Vector2f(ball.ballDirection.x * cos((newVectorAngle) * (M_PI / 180.0f)) - ball.ballDirection.y * sin(newVectorAngle * (M_PI / 180.0f)), ball.ballDirection.x * sin(newVectorAngle * (M_PI / 180.0f)) + ball.ballDirection.y * cos(newVectorAngle * (M_PI / 180.0f)));
			std::cout << "old: " << angleBetweenBallDirectionAndNormal << ", new: " << newVectorAngle << std::endl;
		}
		if (percentageOnXAxis > 1.0f - Player::RIM_PERCENTAGE) {

			float ballDirTimesNormal = (ball.ballDirection.x * rightRimNormal.x) + (ball.ballDirection.y * rightRimNormal.y);
			float absoluteBallDir = sqrt((ball.ballDirection.x * ball.ballDirection.x) + (ball.ballDirection.y * ball.ballDirection.y));
			float absoluteNormal = sqrt((rightRimNormal.x * rightRimNormal.x) + (rightRimNormal.y * rightRimNormal.y));
			float angleBetweenBallDirectionAndNormal = acos(ballDirTimesNormal / (absoluteBallDir * absoluteNormal)) / (M_PI / 180.0f);
			float newVectorAngle = angleBetweenBallDirectionAndNormal * -2.0f;

			newBallDirectionVector = sf::Vector2f(ball.ballDirection.x * cos((newVectorAngle) * (M_PI / 180.0f)) - ball.ballDirection.y * sin(newVectorAngle * (M_PI / 180.0f)), ball.ballDirection.x * sin(newVectorAngle * (M_PI / 180.0f)) + ball.ballDirection.y * cos(newVectorAngle * (M_PI / 180.0f)));
			std::cout << "old: " << angleBetweenBallDirectionAndNormal << ", new: " << newVectorAngle << std::endl;
		}
		else {
			newBallDirectionVector = sf::Vector2f(ball.ballDirection.x, ball.ballDirection.y * -1.0f);
		}
		//UpdateCollisionDebugDrawings(leftRimNormal, rightRimNormal, ball.ballDirection, newBallDirectionVector);
	}

	//2) calculate the normal for the player area (make it customizable based on initial deviation degree for normal)
	//3) calculate the angle between the normal and the ball direction vector
	//4) give new direction vector based on the angle 

	return newBallDirectionVector;
}

bool BallManager::CheckForBallTileCollisionAndMovementChanges(Ball& ball, sf::Vector2f* bounceDirection) {


	bool playerCollideVertical = false;
	bool playerCollideHorizontal = false;


	sf::Vector2f futureBallPosition = (ball.ballPosition + sf::Vector2f(ball.ballRadius, ball.ballRadius)) + ball.ballDirection;


	int tileVectorLength = (*GameManager::m_Tiles).size();
	TileManager* tileManager = static_cast<TileManager*>(GameManager::m_ManagerMap[ManagerType::tileManager_T]);

	sf::Vector2f tileDimensions = tileManager->TILE_DIMENSIONS;

	for (int i = 0; i < tileVectorLength; i++) {
		bool alive = tileManager->m_Tiles[i]->isAlive;
		if (!alive) {
			continue;
		}
		sf::Vector2f currentTilePosition = tileManager->m_Tiles[i]->position;
		float testingX = futureBallPosition.x;
		float testingY = futureBallPosition.y;

		float minTileX = currentTilePosition.x - (tileDimensions.x / 2.0f);
		float maxTileX = currentTilePosition.x + (tileDimensions.x / 2.0f);
		float minTileY = currentTilePosition.y - (tileDimensions.y / 2.0f);
		float maxTileY = currentTilePosition.y + (tileDimensions.y / 2.0f);

		//bools to check for section position of the ball
		bool leftX = false;
		bool rightX = false;
		bool upY = false;
		bool downY = false;

		if (futureBallPosition.x < minTileX) { testingX = minTileX; leftX = true; }
		else if (futureBallPosition.x > maxTileX) { testingX = maxTileX; rightX = true; }

		if (futureBallPosition.y < minTileY) { testingY = minTileY; upY = true; }
		else if (futureBallPosition.y > maxTileY) { testingY = maxTileY; downY = true; }

		float distanceX = futureBallPosition.x - testingX;
		float distanceY = futureBallPosition.y - testingY;
		float overallDistance = sqrt((distanceX * distanceX) + (distanceY * distanceY));

		//insert corner check here (check for (if ball.pos == one of the corner pos) -> give direction that yeets it in a line from the tile pos- tile corner)
		//code here


		if (overallDistance <= ball.ballRadius) {

			//test for which section the ball is in 

			if (leftX || rightX) {
				playerCollideHorizontal = true;
			}

			if (upY || downY) {
				playerCollideVertical = true;
			}


		}



		if (playerCollideVertical) {

			*bounceDirection = sf::Vector2f(ball.ballDirection.x, ball.ballDirection.y * -1.0f);

			ManagerInterface* tm = GameManager::m_ManagerMap[ManagerType::tileManager_T];
			TileManager* tileManager = static_cast<TileManager*>(tm);
			tileManager->UpdateTileAfterCollision(i);

			return true;
		}

		if (playerCollideHorizontal) {
			*bounceDirection = sf::Vector2f(ball.ballDirection.x * -1.0f, ball.ballDirection.y);
			ManagerInterface* tm = GameManager::m_ManagerMap[ManagerType::tileManager_T];
			TileManager* tileManager = static_cast<TileManager*>(tm);
			tileManager->UpdateTileAfterCollision(i);
			return true;
		}
	}

	return false;

}