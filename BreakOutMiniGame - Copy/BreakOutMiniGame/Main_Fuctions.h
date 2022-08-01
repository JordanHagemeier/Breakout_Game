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


sf::Vector2f Get2DPositionWithIndex(int index);
bool InitializeTileArrayWithData(Tile* tiles[], int tileArrayLength);
bool CheckForInput(Ball& ball, Tile* gameTiles[], sf::RenderWindow& window);
bool CheckForCollisionWithPlayer(Ball& ball, sf::Vector2f* bounceDirection, sf::Vector2f* nextBallPosition);
bool CheckForBallTileCollisionAndMovementChanges(Ball& ball, Tile** gametiles, sf::Vector2f* bounceDirection);
bool RenderGameData(sf::RenderWindow& window, Tile* gametiles[]);
bool SetUpInputCountingSystem();
bool DoGameLoopCalculations(Ball& ball, Tile* gametiles[]);
bool MoveBallThroughInput(Ball& ball);
sf::Vector2f CalculateBounceVector(sf::Vector2f futureBallPosition, CollisionType type, Ball& ball, Player& player);
bool DropTileEffect(Tile& tile);
bool DrawTileEffects(sf::RenderWindow& window);
TileType CheckEffectWithPlayerCollision(bool* collisionBool, Player& player);
bool CheckForEffectUsage();
bool SpawnBall();
void UpdateCollisionDebugDrawings(sf::Vector2f leftRimNormal, sf::Vector2f rightRimNormal, sf::Vector2f ballToPlayerVector, sf::Vector2f newBallDirVector);
bool UpdatePlayerPosition();
bool CheckPlayersForBuffEffect();