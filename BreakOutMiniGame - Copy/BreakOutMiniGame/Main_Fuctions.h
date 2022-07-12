#pragma once
#include <vector>
enum CollisionType {
	VerticalCollision,
	HorizontalCollision,
	CornerCollision,
	Count
};

sf::Vector2f Get2DPositionWithIndex(int index);
bool FillTileArrayWithData(Tile* tiles[], int tileArrayLength);
bool CheckForInput();
float clip(float n, float lower, float upper);
bool CheckForCollisionWithPlayer(sf::Vector2f playerTilePos, sf::Vector2f* bounceDirection, sf::CircleShape& ball,  sf::Vector2f* nextBallPosition);
bool CheckForBallTileCollisionAndMovementChanges(sf::RectangleShape* tileShapes, Tile** gametiles, sf::CircleShape& ball, sf::Vector2f* bounceDirection);
float lerp(float a, float b, float f);
bool VerticalCollisionCheck(sf::Vector2f circleDistance);
bool HorizontalCollisionCheck(sf::Vector2f circleDistance);
bool CornerCollisionCheck(sf::Vector2f circleDistance);
bool RenderGameData(sf::RenderWindow& window, sf::CircleShape& ball, sf::RectangleShape tileShapes[], Tile* gametiles[]);
bool SetUpInputCountingSystem();
bool DoGameLoopCalculations(sf::CircleShape& ball, sf::RectangleShape tileShapes[], Tile* gametiles[]);
bool MoveBallThroughInput(sf::CircleShape& ball);
sf::Vector2f CalculateBounceVector(sf::Vector2f futureBallPosition, CollisionType type);