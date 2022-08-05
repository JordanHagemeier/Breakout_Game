#pragma once
#include <vector>
#include  "Ball.h"
#include "Player.h"
enum CollisionType {
	VerticalCollision,
	HorizontalCollision,
	CornerCollision,
	ColCount
};


const float scalingFactor = 3.0f;


//sf::Vector2f Get2DPositionWithIndex(int index);
//bool InitializeTileArrayWithData(Tile* tiles[], int tileArrayLength);
bool CheckForInput(Ball& ball, std::vector<Tile*> gametiles, sf::RenderWindow& window);
bool CheckForCollisionWithPlayer(Ball& ball, sf::Vector2f* bounceDirection, sf::Vector2f* nextBallPosition);
bool CheckForBallTileCollisionAndMovementChanges(Ball& ball, std::vector<Tile*> gametiles, sf::Vector2f* bounceDirection);
bool RenderGameData(sf::RenderWindow& window, std::vector<Tile*> gametiles);
bool SetUpInputCountingSystem();
bool DoGameLoopCalculations(Ball& ball, std::vector<Tile*> gametiles);
bool MoveBallThroughInput(Ball& ball);
sf::Vector2f CalculateBounceVector(sf::Vector2f futureBallPosition, CollisionType type, Ball& ball, Player& player);
TileType CheckEffectWithPlayerCollision(bool* collisionBool, Player& player);
bool CheckForEffectUsage();
bool CreateNewBall();
void UpdateCollisionDebugDrawings(sf::Vector2f leftRimNormal, sf::Vector2f rightRimNormal, sf::Vector2f ballToPlayerVector, sf::Vector2f newBallDirVector);
bool UpdatePlayerPosition();
bool CheckPlayersForBuffEffect();
bool CreateNewPlayer();