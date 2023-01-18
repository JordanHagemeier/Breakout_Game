#include <SFML/Graphics.hpp>
#include <iostream>
#define _USE_MATH_DEFINES
#include <math.h>
#include <cmath>
#include "Tile.h"
#include "Ball.h"
#include "DroppedBooster.h"
#include "Main_Fuctions.h"
#include <algorithm>
#include <chrono>
#include <list>
#include <random>
#include "RenderManager.h"
#include "TileManager.h"
#include "BallManager.h"
#include "GameManager.h"
#include "PlayerManager.h"
#include "ParticleEffectManager.h"


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
const int AMOUNT_WINDOW_SEGMENTS_WIDTH = 8;
const int AMOUNT_WINDOW_SEGMENTS_HEIGHT = 6;
const int TILE_ARRAY_LENGTH = AMOUNT_WINDOW_SEGMENTS_WIDTH*AMOUNT_WINDOW_SEGMENTS_HEIGHT;
sf::RenderWindow* m_Window;

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
sf::Color m_NoHitColor;
sf::Color m_FinalHitColor;


const int amountOfInputs = 10;
const int frameLimit = 600;
bool inputAllowed [amountOfInputs];
int inputTimer [amountOfInputs];
int inputTimerLimit[amountOfInputs];


bool m_BallControlEnabled = false;
bool moveUp = false;
bool moveDown = false;
bool moveRight = false;
bool moveLeft = false;

std::map<TileType, sf::Color> m_TileTypeToColor;

//Debugging Visuals

sf::Vertex m_PlayerNormalLineCoords [8]; 

//Manager
RenderManager* renderManager; 
BoosterDropManager* droppingEffectManager;
TileManager* tileManager;
BallManager* ballManager; 
GameManager* gameManager;
PlayerManager* playerManager;
ParticleEffectManager* particleEffectManager;



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
	m_Window = new sf::RenderWindow (sf::VideoMode(WINDOW_WIDTH_PX, WINDOW_HEIGHT_PX), "SFML works!");

	CreateManagers();
	InitializeManagers();

	
	//setup input arrays
	SetUpInputCountingSystem();

	
	
	while (m_Window->isOpen())
	{

		
		sf::Event event;
		while (m_Window->pollEvent(event))
		{
			if (event.type == sf::Event::Resized)
			{
				// update the view to the new size of the window
				float aspectRatio = (float)WINDOW_WIDTH_PX / WINDOW_HEIGHT_PX;
				sf::FloatRect visibleArea(0, 0, event.size.width, event.size.height);
				m_Window->setSize(sf::Vector2u(event.size.height * aspectRatio, event.size.height));

			}

			if (event.type == sf::Event::Closed) {
				gameManager->TerminateGame();// IT AINT WORKING AS INTENDED, SON
				m_Window->close();
				return 0;
			}

		}
		//Check for Input
		CheckForInput(*ballManager->m_Balls_In_Use[0]);
		

		if (m_CalculateOneFrame) {
			DoGameLoopCalculations();
			m_CalculateOneFrame = false;
		}

		if (m_GamePaused) {
			MoveBallThroughInput(*ballManager->m_Balls_In_Use[0]);
			RenderGameData();
			
			continue;
		}

		
		DoGameLoopCalculations();
		RenderGameData();
	}

	return 0;
}

void CreateManagers() {
	//Manager Creation
		//1) Render Manager, which is essential for all others
	RenderInfo renderInfo;
	renderInfo.window = m_Window;
	renderInfo.windowDimensions_px = sf::Vector2f(WINDOW_WIDTH_PX, WINDOW_HEIGHT_PX);
	renderInfo.windowSegmentAmountPerDimension = sf::Vector2i(AMOUNT_WINDOW_SEGMENTS_WIDTH, AMOUNT_WINDOW_SEGMENTS_HEIGHT);
	renderInfo.scalingFactor = scalingFactor;
	renderManager = new RenderManager(renderInfo);

	//2)Tile Manager
	tileManager = new TileManager();


	//3) Effect Manager for Tile Dropping Effects
	droppingEffectManager = new BoosterDropManager(TILE_ARRAY_LENGTH);

	//4) Ball Manager
	ballManager = new BallManager();

	//(optional: 5) Player Manager?)
	playerManager = new PlayerManager();

	//Particle Effect Manager
	particleEffectManager = new ParticleEffectManager();

	//6) Game Manager which combines all of the above
	GameManagerInfo GM_Info;
	GM_Info.tileTypeToColorMap = m_TileTypeToColor;


	std::map<ManagerType, ManagerInterface*> managersMap;
	managersMap[ManagerType::boosterDropManager_T] = droppingEffectManager;
	managersMap[ManagerType::ballManager_T] = ballManager;
	managersMap[ManagerType::tileManager_T] = tileManager;
	managersMap[ManagerType::renderManager_T] = renderManager;
	managersMap[ManagerType::playerManager_T] = playerManager;
	managersMap[ManagerType::particleManager_T] = particleEffectManager;


	GM_Info.managerMap = managersMap;
	gameManager = new GameManager(GM_Info);


}

void InitializeManagers() {
	std::map<ManagerType, ManagerInterface*>::iterator it;

	for (it = GameManager::m_ManagerMap.begin(); it != GameManager::m_ManagerMap.end(); it++)
	{
		it->second->TickBeforeStart();

	}

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


		std::shared_ptr<sf::CircleShape> ballVisual = std::static_pointer_cast<sf::CircleShape>(renderManager->GetShape(ball.m_VisualID));
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




bool DoGameLoopCalculations() {

	std::map<ManagerType, ManagerInterface*>::iterator it;

	for (it = GameManager::m_ManagerMap.begin(); it != GameManager::m_ManagerMap.end(); it++)
	{
		it->second->Tick();

	}


	return true;
}

bool RenderGameData() {


	m_Window->clear();

	//draw debugging lines
	for (int k = 0; k < 8; k += 2) {
		sf::Vertex currentCoordsPair[2] = {m_PlayerNormalLineCoords[k], m_PlayerNormalLineCoords[k+1]};
		m_Window->draw(currentCoordsPair, 2, sf::LinesStrip);
		
	}

	//draw ball
	renderManager->Render();
	m_Window->display();

	return true;
}


bool CheckForInput(Ball& ball) {

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
		
		playerManager->m_Players[0]->m_MovementToApply.x -= Player::BASE_MOVEMENT_SPEED;
		playerManager->m_Players[0]->m_PlayerWasGivenInput = true;
	
		return true;
	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
	{
		// left key is pressed: move our character
		playerManager->m_Players[0]->m_MovementToApply.x += Player::BASE_MOVEMENT_SPEED;
		playerManager->m_Players[0]->m_PlayerWasGivenInput = true;
		
		return true;
	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::P) && inputAllowed[0]) {
		
		std::cout <<"Ball Pos:" << ball.ballPosition.x <<" ," << ball.ballPosition.y <<std::endl;
		inputAllowed[0] = false;
	}

	if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && inputAllowed[7]) {

		particleEffectManager->AddParticleEventAtPosition((sf::Vector2f)sf::Mouse::getPosition(*renderManager->GetRenderWindow()));
		inputAllowed[7] = false;
	}

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
		playerManager->m_Players[0]->m_RemainingSpeedBuffTime = playerManager->m_Players[0]->m_SpeedIncreasedDuration;
		playerManager->m_Players[0]->m_SpeedIsIncreased = true;
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

	sf::Vector2f leftPos = sf::Vector2f(playerManager->m_Players[0]->m_Position.x - (playerManager->m_Players[0]->m_Dimensions.x * Player::RIM_PERCENTAGE), playerManager->m_Players[0]->m_Position.y);
	sf::Vector2f rightPos = sf::Vector2f(playerManager->m_Players[0]->m_Position.x + (playerManager->m_Players[0]->m_Dimensions.x * Player::RIM_PERCENTAGE), playerManager->m_Players[0]->m_Position.y);
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
	m_PlayerNormalLineCoords[4].position = playerManager->m_Players[0]->m_Position - (ballToPlayerVector * 1000.0f);
	m_PlayerNormalLineCoords[4].color = sf::Color::Red;

	m_PlayerNormalLineCoords[5].position = playerManager->m_Players[0]->m_Position;
	m_PlayerNormalLineCoords[5].color = sf::Color::Red;

	m_PlayerNormalLineCoords[6].position = playerManager->m_Players[0]->m_Position;
	m_PlayerNormalLineCoords[6].color = sf::Color::Red;

	m_PlayerNormalLineCoords[7].position = playerManager->m_Players[0]->m_Position + (newBallDirVector * 1000.0f);
	m_PlayerNormalLineCoords[7].color = sf::Color::Red;
}

