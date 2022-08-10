#pragma once
#include <vector>
#include  "Ball.h"
#include "Player.h"
#include "CollisionType.h"



const float scalingFactor = 3.0f;


void CreateManagers();
void InitializeManagers();
bool SetUpInputCountingSystem();

bool CheckForInput(Ball& ball);
bool DoGameLoopCalculations();
bool RenderGameData();

bool MoveBallThroughInput(Ball& ball);
void UpdateCollisionDebugDrawings(sf::Vector2f leftRimNormal, sf::Vector2f rightRimNormal, sf::Vector2f ballToPlayerVector, sf::Vector2f newBallDirVector);
