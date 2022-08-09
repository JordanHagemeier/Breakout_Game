#pragma once
#include <vector>
#include  "Ball.h"
#include "Player.h"
#include "CollisionType.h"



const float scalingFactor = 3.0f;



bool CreateNewPlayer();
bool SetUpInputCountingSystem();

bool CheckForInput(Ball& ball, std::vector<Tile*> gametiles, sf::RenderWindow& window);
bool DoGameLoopCalculations();
bool RenderGameData(sf::RenderWindow& window);

bool MoveBallThroughInput(Ball& ball);
void UpdateCollisionDebugDrawings(sf::Vector2f leftRimNormal, sf::Vector2f rightRimNormal, sf::Vector2f ballToPlayerVector, sf::Vector2f newBallDirVector);

bool UpdatePlayerPosition();
bool CheckPlayersForBuffEffect();

TileType CheckEffectWithPlayerCollision(bool* collisionBool, Player& player);
bool CheckForEffectUsage();