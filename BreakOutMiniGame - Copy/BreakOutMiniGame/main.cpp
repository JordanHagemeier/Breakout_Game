#include <SFML/Graphics.hpp>
#include <iostream>
#define _USE_MATH_DEFINES
#include <math.h>
#include <cmath>
#include "Tile.h"
#include "Ball.h"
#include "DroppingEffect.h"
#include "Main_Fuctions.h"
#include <algorithm>
#include <chrono>
#include <list>
#include <random>
#include "RenderManager.h"
#include "TileManager.h"
#include "BallManager.h"
#include "GameManager.h"


/* //F

Feedback 31.07

1. Rendermanager:
- map <id, sf::Shape*>
-> GetShape(id);
-> id AddShape(Shape*)
-> DeleteShape(id);
-> Render() --> rendert alle yoinker
-> jedes Object was rendern will, holt sich vom RManager ne id für sein rendering dings
--> Render & Gamelogic mehr trennen

2. Klassen für die einzelnen GameElemente (player, ball, tile)
-> Funktionen in die klassen

3. Unbenutzen Kram löschen


*/


//INITIALIZATION
//All tiles should be set in their order, player tile + bouncy ball should be centered below them
//setup background graphics and boundaries
//setup tile gameobjects in array
//-> is this thing on?
//-> position
//-> id?
//-> graphics
//-> hit count

//setup player tile
//-> graphics
//-> position

//setup bouncy ball 
//-> graphics
//-> position

//LOOP
//player can move tile left and right
//bouncy ball:
//-> moves with constant speed
//-> if it collides with a boundary, a tile or the player tile, then it bounces off in the exactly mirrored angle to its orginal angle 
//-> if it collides with the lower boundary aka misses the player, then an event is send (heart is lost)

//Checking against tiles:
//probably checks against the array (what tiles are active, where are the tiles) (two arrays? removal from one array?) 
//-> centered position of tile + length and width of a tile should make it possible to calculate the bounding box 
//if a tile is hit, then it bounces off + tile loses one "tile life" (changes graphics, or disables if it was hit too many times)

//F constexpr
const int WINDOW_SEGMENTS_WIDTH = 8;
const int WINDOW_SEGMENTS_HEIGHT = 6;
const int TILE_ARRAY_LENGTH = WINDOW_SEGMENTS_WIDTH*WINDOW_SEGMENTS_HEIGHT;

const int WINDOW_WIDTH_PX = 200 * scalingFactor; 
const int WINDOW_HEIGHT_PX = 200 * scalingFactor;

const float m_CoveredWindowPercentage = 0.33f;

bool m_GamePaused = false;
bool m_CalculateOneFrame = false;

const sf::Color NO_HIT_COLOR = sf::Color::Blue;
const sf::Color ONE_HIT_COLOR = sf::Color::Red;

bool	m_NewInputAvailable = false; //F  not player related, but application related "m_NewInputAvailable"

//F === Until here ===

//Tile Variables
sf::Vector2f m_TileDimensions = sf::Vector2<float>((float)(WINDOW_WIDTH_PX/WINDOW_SEGMENTS_WIDTH), (float)(WINDOW_HEIGHT_PX/WINDOW_SEGMENTS_HEIGHT) / 3.0f);
sf::Color m_NoHitColor;
sf::Color m_FinalHitColor;
int m_TileHitsAllowed = 3;

const int amountOfInputs = 10;
const int frameLimit = 600;
bool inputAllowed [amountOfInputs];
int inputTimer [amountOfInputs];
int inputTimerLimit[amountOfInputs];


sf::Vector2f m_BallStarterPosition_UNALTERED = sf::Vector2f(WINDOW_WIDTH_PX * 0.05f, WINDOW_HEIGHT_PX * 0.4f);
sf::Vector2f m_BallStarterPosition_ALTERED = sf::Vector2f(0.0f, 0.0f);

bool m_BallControlEnabled = false;
bool moveUp = false;
bool moveDown = false;
bool moveRight = false;
bool moveLeft = false;

//std::vector<Ball*> m_Balls_In_Game; 
std::map<TileType, sf::Color> m_TileTypeToColor;
std::vector<Player*> m_Players;

//Debugging Visuals

sf::Vertex m_PlayerNormalLineCoords [8]; 

//Manager
RenderManager* renderManager; 
DroppingEffectManager* droppingEffectManager;
TileManager* tileManager;
BallManager* ballManager; 
GameManager* gameManager;

std::vector<ManagerInterface*> m_Managers;
int main()
{
	
	//Tile Color Setup

	sf::Color muddyTurquoise;
	muddyTurquoise.r = 32;
	muddyTurquoise.g = 107;
	muddyTurquoise.b = 89;
	muddyTurquoise.a = 255;

	sf::Color grassGreen;
	grassGreen.r = 164;
	grassGreen.g = 200;
	grassGreen.b = 102;
	grassGreen.a = 255;

	sf::Color racingRed;
	racingRed.r = 235;
	racingRed.g = 73;
	racingRed.b = 22;
	racingRed.a = 255;


	m_TileTypeToColor[TileType::AddedBall] = grassGreen;
	m_TileTypeToColor[TileType::QuickerPlayer] = racingRed;
	m_TileTypeToColor[TileType::NoEvent] = muddyTurquoise;

	m_NoHitColor = sf::Color::Blue;
	m_FinalHitColor = sf::Color::Red;


	///-------------------------------
	sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH_PX, WINDOW_HEIGHT_PX), "SFML works!");

	//Manager Creation
	//1) Render Manager, which is essential for all others
	renderManager = new RenderManager(window, sf::Vector2f(WINDOW_WIDTH_PX, WINDOW_HEIGHT_PX), sf::Vector2f(WINDOW_SEGMENTS_WIDTH, WINDOW_SEGMENTS_HEIGHT), scalingFactor);

	//2)Tile Manager
	tileManager = new TileManager();
	

	//3) Effect Manager for Tile Dropping Effects
	droppingEffectManager = new DroppingEffectManager(TILE_ARRAY_LENGTH);

	//4) Ball Manager
	ballManager = new BallManager();

	//(optional: 5) Player Manager?)

	//6) Game Manager which combines all of the above
	GameManagerInfo info;
	
	info.balls = &ballManager->m_Balls_In_Use;
	info.tiles = &tileManager->m_Tiles;
	info.players = &m_Players;
	info.tileTypeToColorMap = m_TileTypeToColor;
	

	std::map<ManagerType, ManagerInterface*> managersMap;
	managersMap[ManagerType::ballManager_T] = ballManager;
	managersMap[ManagerType::tileManager_T] = tileManager;
	managersMap[ManagerType::droppingEffectManager_T] = droppingEffectManager;
	managersMap[ManagerType::renderManager_T] = renderManager;
	
	info.managerMap = managersMap;

	gameManager = new GameManager(info);

	

	//gather all managers
	//this should be obsolete because of the game manager singleton
	m_Managers.push_back(renderManager);
	m_Managers.push_back(tileManager);
	m_Managers.push_back(droppingEffectManager);
	m_Managers.push_back(ballManager);

	for (auto& manager : m_Managers) {
		manager->TickBeforeStart();
	}

	//Create player tile
	CreateNewPlayer();

	//create ball
	/*CreateNewBall();*/
	
	//setup input arrays
	SetUpInputCountingSystem();

	
	
	while (window.isOpen())
	{

		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Resized)
			{
				// update the view to the new size of the window
				float aspectRatio = (float)WINDOW_WIDTH_PX/WINDOW_HEIGHT_PX;
				sf::FloatRect visibleArea(0, 0, event.size.width, event.size.height);
				window.setSize(sf::Vector2u(event.size.height * aspectRatio, event.size.height));
				
			}

			if (event.type == sf::Event::Closed)
				window.close();
		}

		//Check for Input
		if (!m_NewInputAvailable)
		{
			m_NewInputAvailable = CheckForInput(*ballManager->m_Balls_In_Use[0], tileManager->m_Tiles, window);
		}

		if (m_CalculateOneFrame) {
			DoGameLoopCalculations(*ballManager->m_Balls_In_Use[0], tileManager->m_Tiles);
			//std::cout << "Calculate one frame" << std::endl;
			m_CalculateOneFrame = false;
		}

		if (m_GamePaused) {
			MoveBallThroughInput(*ballManager->m_Balls_In_Use[0]);
			RenderGameData(window, tileManager->m_Tiles);
			
			continue;
		}

		
		DoGameLoopCalculations(*ballManager->m_Balls_In_Use[0], tileManager->m_Tiles);
		RenderGameData(window, tileManager->m_Tiles);
	}

	return 0;
}


bool CreateNewPlayer() {
	sf::Vector2f dimensions = sf::Vector2f(55.0f, 10.0f) * scalingFactor;
	sf::Vector2f position = sf::Vector2f(0.33f * WINDOW_WIDTH_PX, 0.8f * WINDOW_HEIGHT_PX);
	Player* newPlayer = new Player(dimensions, position, *renderManager);
	m_Players.push_back(newPlayer);
	(*GameManager::m_Players).push_back(newPlayer);

	return true;
}

//F in ball.h --> UpdateMovement & UpdateMovement_DebugInput, ein if/else entscheidet welche von den beiden genutzt wird
bool MoveBallThroughInput(Ball& ball) {

	bool inputGiven = false;
	ball = *ballManager->m_Balls_In_Use[0];
	sf::Vector2f oldDirection = ball.ballDirection;
	if (moveUp)		{ball.ballDirection = sf::Vector2f(0.0f, -1.0f);		moveUp    = false; inputGiven = true; }
	if (moveDown)	{ball.ballDirection = sf::Vector2f(0.0f, 1.0f);		moveDown  = false; inputGiven = true;}
	if (moveLeft)	{ball.ballDirection = sf::Vector2f(-1.0f, 0.0f);	moveLeft  = false; inputGiven = true;}
	if (moveRight)	{ball.ballDirection = sf::Vector2f(1.0f, 0.0f);	moveRight = false; inputGiven = true;}

	if (inputGiven) {
		ball.ballPosition = ball.ballPosition + (ball.ballDirection * 2.0f);


		std::shared_ptr<sf::CircleShape> ballVisual = std::static_pointer_cast<sf::CircleShape>(renderManager->GetShape(ball.ballVisualID));
		//std::shared_ptr<sf::Shape> ballVisual = (std::shared_ptr<sf::Shape>)renderManager->GetShape(ball.ballVisualID);
		ballVisual->setPosition(ball.ballPosition);
	}
	
	ball.ballDirection = oldDirection;
	return true;
	
}

bool SetUpInputCountingSystem() {
	for (int i = 0; i < amountOfInputs; i++) {
		inputAllowed[i] = true;
		inputTimer[i] = 0;
		inputTimerLimit[i] = frameLimit;
	}
	return true;
}

//Player.h
bool UpdatePlayerPosition() {

	//THIS IS ONLY MEANT FOR ONE PLAYER RIGHT NOW
	for (int i = 0; i < m_Players.size(); i++) {
		
		m_Players[i]->UpdatePlayerVisuals(*renderManager, sf::Vector2f(WINDOW_WIDTH_PX, WINDOW_HEIGHT_PX));
		m_NewInputAvailable = false;
	}
	
	return true;
}

bool CheckPlayersForBuffEffect() {
	for (int j = 0; j < m_Players.size(); j++) {
		if (m_Players[j]->m_SpeedIsIncreased) {
			m_Players[j]->m_RemainingSpeedBuffTime--;

			if (m_Players[j]->m_RemainingSpeedBuffTime < 0.0f) {
				m_Players[j]->m_SpeedIsIncreased = false;
				std::shared_ptr<sf::RectangleShape> playerVisual_Middle = std::static_pointer_cast<sf::RectangleShape>(renderManager->GetShape(m_Players[j]->m_VisualID[0]));
				playerVisual_Middle->setFillColor(Player::BASE_COLOR);
				continue;
			}

			m_Players[j]->m_MovementToApply.x = m_Players[j]->m_MovementToApply.x * m_Players[j]->m_IncreasedSpeedMultiplier;
			float effectDurationPercentage = (float)m_Players[j]->m_RemainingSpeedBuffTime / (float)m_Players[j]->m_SpeedIncreasedDuration;

			sf::Color effectBasedColor;
			effectBasedColor.r = MathHelper::Lerp(Player::BUFF_COLOR.r, Player::BASE_COLOR.r, effectDurationPercentage);
			effectBasedColor.g = MathHelper::Lerp(Player::BUFF_COLOR.g, Player::BASE_COLOR.g, effectDurationPercentage);
			effectBasedColor.b = MathHelper::Lerp(Player::BUFF_COLOR.b, Player::BASE_COLOR.b, effectDurationPercentage);

			std::shared_ptr<sf::RectangleShape> playerVisual_Middle = std::static_pointer_cast<sf::RectangleShape>(renderManager->GetShape(m_Players[j]->m_VisualID[0]));
			playerVisual_Middle->setFillColor(effectBasedColor);

		}

	}
	return true;
}

bool DoGameLoopCalculations(Ball& ball, std::vector<Tile*> gametiles) {

	CheckPlayersForBuffEffect();
	//Update Player Position
	
	if (m_NewInputAvailable) {
	
		UpdatePlayerPosition();
		
	}
	CheckForEffectUsage();

	for (ManagerInterface* interface : m_Managers) {
		interface->Tick();
	}


	return true;
}

bool RenderGameData(sf::RenderWindow& window, std::vector<Tile*> gametiles) {

	//Update Tiles 

	window.clear();


	//draw tiles
	for (int j = 0; j < TILE_ARRAY_LENGTH; j++) {
		gametiles[j]->UpdateTileColorBasedOnHits(*renderManager);
	}

	

	for (int i = 0; i < ballManager->m_Balls_In_Use.size(); i++) {
		
		std::shared_ptr<sf::CircleShape> currentBallVisual = std::static_pointer_cast<sf::CircleShape>(renderManager->GetShape(ballManager->m_Balls_In_Use[i]->ballVisualID));
		currentBallVisual->setPosition(ballManager->m_Balls_In_Use[i]->ballPosition);

	}


	//draw debugging lines
	for (int k = 0; k < 8; k += 2) {
		sf::Vertex currentCoordsPair[2] = {m_PlayerNormalLineCoords[k], m_PlayerNormalLineCoords[k+1]};
		window.draw(currentCoordsPair, 2, sf::LinesStrip);
		
	}

	//draw ball
	renderManager->Render();
	window.display();

	return true;
}


bool CheckForInput(Ball& ball, std::vector<Tile*> gametiles, sf::RenderWindow& window) {

	//check all bools for progession

	for (int i = 0; i < amountOfInputs; i++) {
		if (!inputAllowed[i]) {
			inputTimer[i]++;
			if (inputTimer[i] >= inputTimerLimit[i]) {
				inputAllowed[i] = true;
				inputTimer[i] = 0;
			}
		}
	}
	
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
	{
		// left key is pressed: move our character
		m_Players[0]->m_MovementToApply.x -= Player::BASE_MOVEMENT_SPEED;
		
		return true;
	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
	{
		// left key is pressed: move our character
		m_Players[0]->m_MovementToApply.x += Player::BASE_MOVEMENT_SPEED;
		
		return true;
	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::P) && inputAllowed[0]) {
		
		std::cout <<"Ball Pos:" << ball.ballPosition.x <<" ," << ball.ballPosition.y <<std::endl;
		inputAllowed[0] = false;
	}

	/*if (sf::Keyboard::isKeyPressed(sf::Keyboard::O) && inputAllowed[7]) {

		std::cout << "Player Pos:" << m_PlayerPosition.x << " ," << m_PlayerPosition.y << std::endl;
		inputAllowed[7] = false;
	}*/

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space) && inputAllowed[1]) {
		m_GamePaused = !m_GamePaused;
		std::cout <<"Game Pause: " << m_GamePaused <<std::endl;
		inputAllowed[1] = false;
	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Enter) && m_GamePaused && inputAllowed[2]) {
		m_CalculateOneFrame = true;
		
		inputAllowed[2] = false;
	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::T) && inputAllowed[8]) {
		/*DropTileEffect(*gameTiles[10], &tileShapes[10]);
		
		DrawTileEffects(window);*/
		//SpawnBall();
		m_Players[0]->m_RemainingSpeedBuffTime = m_Players[0]->m_SpeedIncreasedDuration;
		m_Players[0]->m_SpeedIsIncreased = true;
		inputAllowed[8] = false;
	}

	if (m_GamePaused) {
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::W) && inputAllowed[3]) {

			moveUp = true;
			inputAllowed[3] = false;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::S) && inputAllowed[4]) {

			moveDown = true;
			inputAllowed[4] = false;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) && inputAllowed[5]) {

			moveLeft = true;
			inputAllowed[5] = false;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::D) && inputAllowed[6]) {

			moveRight = true;
			inputAllowed[6] = false;
		}
	}
	

	return false;
}



void UpdateCollisionDebugDrawings(sf::Vector2f leftRimNormal, sf::Vector2f rightRimNormal, sf::Vector2f ballToPlayerVector, sf::Vector2f newBallDirVector) {

	sf::Vector2f leftPos = sf::Vector2f(m_Players[0]->m_Position.x - (m_Players[0]->m_Dimensions.x * Player::RIM_PERCENTAGE), m_Players[0]->m_Position.y);
	sf::Vector2f rightPos = sf::Vector2f(m_Players[0]->m_Position.x + (m_Players[0]->m_Dimensions.x * Player::RIM_PERCENTAGE), m_Players[0]->m_Position.y);
	//left coords & color
	m_PlayerNormalLineCoords[0].position = leftPos;
	m_PlayerNormalLineCoords[0].color = sf::Color::Cyan;

	m_PlayerNormalLineCoords[1].position = leftPos + (leftRimNormal * 100.0f);
	m_PlayerNormalLineCoords[1].color = sf::Color::Cyan;

	//right coords & color

	m_PlayerNormalLineCoords[2].position = rightPos;
	m_PlayerNormalLineCoords[2].color = sf::Color::Cyan;

	m_PlayerNormalLineCoords[3].position = rightPos + (rightRimNormal * 100.0f);
	m_PlayerNormalLineCoords[3].color = sf::Color::Cyan;

	//reflection incoming + outgoing vectors
	m_PlayerNormalLineCoords[4].position = m_Players[0]->m_Position - (ballToPlayerVector * 1000.0f);
	m_PlayerNormalLineCoords[4].color = sf::Color::Red;

	m_PlayerNormalLineCoords[5].position = m_Players[0]->m_Position;
	m_PlayerNormalLineCoords[5].color = sf::Color::Red;

	m_PlayerNormalLineCoords[6].position = m_Players[0]->m_Position;
	m_PlayerNormalLineCoords[6].color = sf::Color::Red;

	m_PlayerNormalLineCoords[7].position = m_Players[0]->m_Position + (newBallDirVector * 1000.0f);
	m_PlayerNormalLineCoords[7].color = sf::Color::Red;
}


TileType CheckEffectWithPlayerCollision(bool* collisionBool, Player& player) {

	TileType foundType = TileType::NoEvent;
	int length = droppingEffectManager->m_CurrentlyShownEffects.size();
	for (int i = 0; i < length; i++) {
		//check if tile is colliding with player
		if (!droppingEffectManager->m_CurrentlyShownEffects[i].m_IsActive) {
			continue;
		}
		sf::Vector2f effectPos = droppingEffectManager->m_CurrentlyShownEffects[i].m_Position;

		float distanceEffectPlayerY = abs(effectPos.y - player.m_Position.y);
		if (distanceEffectPlayerY < 2.0f) {
			if (effectPos.x < (player.m_Position.x + (player.m_Dimensions.x / 2.0f)) && effectPos.x >(player.m_Position.x - (player.m_Dimensions.x / 2.0f))) {
				foundType = droppingEffectManager->m_CurrentlyShownEffects[i].m_TileEffectType;
				droppingEffectManager->m_CurrentlyShownEffects[i].m_IsActive = false;
				*collisionBool = true;
				
				return foundType;
			}
		}

		
		//if yes, return 
	}

	*collisionBool = false;
	return foundType;
}

//I NEED A DELETE FUCTION FOR WHEN THE EFFECT FALLS OUT OF WINDOW SCOPE
bool CheckForEffectUsage() {
	bool collisionPlayerWithEffect = false;
	for (int i = 0; i < m_Players.size(); i++) {
		TileType effectType = CheckEffectWithPlayerCollision(&collisionPlayerWithEffect, *m_Players[i]);
		if (collisionPlayerWithEffect) {
			switch (effectType) {
			case TileType::AddedBall:
				std::cout << "Added Ball!" << std::endl;
				ballManager->CreateNewBall();
				break;
			case TileType::QuickerPlayer:
				std::cout << "Quicker Player!" << std::endl;
				m_Players[i]->m_RemainingSpeedBuffTime = m_Players[i]->m_SpeedIncreasedDuration;
				m_Players[i]->m_SpeedIsIncreased = true;
				break;
			case TileType::NoEvent:
				std::cout << "No Event!" << std::endl;
				break;

			}
			return true;
		}
	}
	
	return false;
}

