#include <cassert>
#define _USE_MATH_DEFINES
#include <math.h>

#include "Ball.h"
#include "GameManager.h"
#include "RenderManager.h"
#include "TileManager.h"
#include "PlayerManager.h"

float Ball::BALL_BASE_SPEED = 0.5f;

void Ball::Update() {

	bool CollisionWithWall = CheckForWallCollision();

	sf::Vector2f originalBallDirection = ballDirection;
	sf::Vector2f playerCollisionBounceDirection;
	sf::Vector2f tileCollisionBounceDirection;
	sf::Vector2f* ptrToNewDirectionVector = nullptr;

	bool collisionWithPlayer = CheckForCollisionWithPlayer(&playerCollisionBounceDirection, ptrToNewDirectionVector);
	bool collisionWithTile = CheckForBallTileCollisionAndMovementChanges( &tileCollisionBounceDirection);


	if (collisionWithPlayer) {

		ballDirection = playerCollisionBounceDirection;
	}

	if (collisionWithTile) {
		ballDirection = tileCollisionBounceDirection;
	}


	sf::Vector2f newBallPosition = ballPosition + ballDirection;
	ballPosition = newBallPosition;
}

bool Ball::CheckForWallCollision() {
	float wallDistanceXMin = ballPosition.x;
	ManagerInterface* ptrToRenderManager = GameManager::GetManagerByType(ManagerType::renderManager_T);
	if (ptrToRenderManager == nullptr) {
		std::cout << "RenderManager not yet initialized!" << std::endl;
		return false;
	}
	RenderManager& renderManager = static_cast<RenderManager&>(*ptrToRenderManager);
	assert(renderManager.HasFinishedInitialization(), "Render Manager not yet fully initialized! See initialization process & order.");


	float wallDistanceXMax = std::abs(renderManager.m_WindowDimensions_px.x - ballPosition.x);

	if (wallDistanceXMin < repellantDistance || wallDistanceXMax < repellantDistance) {
		ballDirection = sf::Vector2f(ballDirection.x * -1.0f, ballDirection.y);
		return true;
	}

	float wallDistanceYMin = ballPosition.y;
	float wallDistanceYMax = std::abs(renderManager.m_WindowDimensions_px.y - ballPosition.y);

	if (wallDistanceYMin < repellantDistance || wallDistanceYMax < repellantDistance) {
		ballDirection = sf::Vector2f(ballDirection.x, ballDirection.y * -1.0f);
		return true;
	}
	return false;
}

bool Ball::CheckForCollisionWithPlayer(sf::Vector2f* bounceDirection, sf::Vector2f* nextBallPosition) {
	//Todo: copy contents from main.cpp 
	//TODO 
	//1) add a safeguard that the actual setting of the new direction will only commence once this collision check comes back as negative for the possible next location
	bool playerCollideVertical = false;
	bool playerCollideHorizontal = false;

	sf::Vector2f futureBallPosition;
	futureBallPosition = (ballPosition + sf::Vector2f(ballRadius, ballRadius)) + ballDirection;
	if (nextBallPosition != nullptr) {
		futureBallPosition = *nextBallPosition;
	}

	float testingX = futureBallPosition.x;
	float testingY = futureBallPosition.y;

	
	ManagerInterface* ptrToPlayerManager = GameManager::GetManagerByType(ManagerType::playerManager_T);
	if (!ptrToPlayerManager) {
		std::cout << "Player Manager not yet initialized!" << std::endl;
		return false;
	}
	PlayerManager& playerManager = static_cast<PlayerManager&>(*ptrToPlayerManager);
	assert(playerManager.HasFinishedInitialization(), "Player Manager not yet fully initialized! See initialization process & order.");


	for (int i = 0; i < playerManager.m_Players.size(); i++) {


		Player& currentPlayer = *playerManager.m_Players[i];
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

		//insert corner check here (check for (if pos == one of the corner pos) -> give direction that yeets it in a line from the tile pos- tile corner)
		//code here

		bool cornerWasHit = false;
		bool leftUpperCorner = false;
		bool rightUpperCorner = false;
		bool leftLowerCorner = false;
		bool rightLowerCorner = false;

		if (overallDistance <= ballRadius) {

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
			*bounceDirection = sf::Vector2f(ballDirection.x * -1.0f, ballDirection.y * -1.0f);
			return true;
		}
		if (playerCollideVertical) {
			*bounceDirection = CalculateBounceVector(futureBallPosition, CollisionType::VerticalCollision, (*playerManager.m_Players[i]));
			return true;
		}

		if (playerCollideHorizontal) {
			*bounceDirection = sf::Vector2f(ballDirection.x * -1.0f, ballDirection.y);
			return true;
		}



	}



	return false;
}

sf::Vector2f Ball::CalculateBounceVector(sf::Vector2f futureBallPosition, CollisionType type, Player& player) {

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

			float ballDirTimesNormal = (ballDirection.x * leftRimNormal.x) + (ballDirection.y * leftRimNormal.y);
			float absoluteBallDir = sqrt((ballDirection.x * ballDirection.x) + (ballDirection.y * ballDirection.y));
			float absoluteNormal = sqrt((leftRimNormal.x * leftRimNormal.x) + (leftRimNormal.y * leftRimNormal.y));
			float angleBetweenBallDirectionAndNormal = acos(ballDirTimesNormal / (absoluteBallDir * absoluteNormal)) / (M_PI / 180.0f);
			float newVectorAngle = angleBetweenBallDirectionAndNormal * -2.0f;

			newBallDirectionVector = sf::Vector2f(ballDirection.x * cos((newVectorAngle) * (M_PI / 180.0f)) - ballDirection.y * sin(newVectorAngle * (M_PI / 180.0f)), ballDirection.x * sin(newVectorAngle * (M_PI / 180.0f)) + ballDirection.y * cos(newVectorAngle * (M_PI / 180.0f)));
			std::cout << "old: " << angleBetweenBallDirectionAndNormal << ", new: " << newVectorAngle << std::endl;
		}
		if (percentageOnXAxis > 1.0f - Player::RIM_PERCENTAGE) {

			float ballDirTimesNormal = (ballDirection.x * rightRimNormal.x) + (ballDirection.y * rightRimNormal.y);
			float absoluteBallDir = sqrt((ballDirection.x * ballDirection.x) + (ballDirection.y * ballDirection.y));
			float absoluteNormal = sqrt((rightRimNormal.x * rightRimNormal.x) + (rightRimNormal.y * rightRimNormal.y));
			float angleBetweenBallDirectionAndNormal = acos(ballDirTimesNormal / (absoluteBallDir * absoluteNormal)) / (M_PI / 180.0f);
			float newVectorAngle = angleBetweenBallDirectionAndNormal * -2.0f;

			newBallDirectionVector = sf::Vector2f(ballDirection.x * cos((newVectorAngle) * (M_PI / 180.0f)) - ballDirection.y * sin(newVectorAngle * (M_PI / 180.0f)), ballDirection.x * sin(newVectorAngle * (M_PI / 180.0f)) + ballDirection.y * cos(newVectorAngle * (M_PI / 180.0f)));
			std::cout << "old: " << angleBetweenBallDirectionAndNormal << ", new: " << newVectorAngle << std::endl;
		}
		else {
			newBallDirectionVector = sf::Vector2f(ballDirection.x, ballDirection.y * -1.0f);
		}
		//UpdateCollisionDebugDrawings(leftRimNormal, rightRimNormal, ballDirection, newBallDirectionVector);
	}

	//2) calculate the normal for the player area (make it customizable based on initial deviation degree for normal)
	//3) calculate the angle between the normal and the ball direction vector
	//4) give new direction vector based on the angle 

	return newBallDirectionVector;
}

bool Ball::CheckForBallTileCollisionAndMovementChanges(sf::Vector2f* bounceDirection) {


	bool playerCollideVertical = false;
	bool playerCollideHorizontal = false;


	sf::Vector2f futureBallPosition = (ballPosition + sf::Vector2f(ballRadius, ballRadius)) + ballDirection;

	ManagerInterface* ptrToTileManager = GameManager::GetManagerByType(ManagerType::tileManager_T);
	if (!ptrToTileManager) {
		std::cout << "TileManager not yet initialized!" << std::endl;
		return false;
	}
	TileManager& tileManager = static_cast<TileManager&>(*ptrToTileManager);
	int tileVectorLength = (tileManager.m_Tiles).size();

	sf::Vector2f tileDimensions = tileManager.TILE_DIMENSIONS;

	for (int i = 0; i < tileVectorLength; i++) {
		bool alive = tileManager.m_Tiles[i].isAlive;
		if (!alive) {
			continue;
		}
		sf::Vector2f currentTilePosition = tileManager.m_Tiles[i].position;
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

		//insert corner check here (check for (if pos == one of the corner pos) -> give direction that yeets it in a line from the tile pos- tile corner)
		//code here


		if (overallDistance <= ballRadius) {

			//test for which section the ball is in 

			if (leftX || rightX) {
				playerCollideHorizontal = true;
			}

			if (upY || downY) {
				playerCollideVertical = true;
			}


		}



		if (playerCollideVertical) {

			*bounceDirection = sf::Vector2f(ballDirection.x, ballDirection.y * -1.0f);

			tileManager.UpdateTileAfterCollision(i);

			return true;
		}

		if (playerCollideHorizontal) {
			*bounceDirection = sf::Vector2f(ballDirection.x * -1.0f, ballDirection.y);
			
			tileManager.UpdateTileAfterCollision(i);
			return true;
		}
	}

	return false;

}
