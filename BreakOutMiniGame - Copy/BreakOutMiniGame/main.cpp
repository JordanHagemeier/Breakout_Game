#include <SFML/Graphics.hpp>
#include <iostream>
#include "Tile.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include <cmath>
#include "Main_Fuctions.h"
#include <algorithm>
#include <chrono>
#include "Ball.h"
#include <list>
#include <random>
#include "RenderManager.h"

/* //F

Feedback 31.07

1. Rendermanager:
- map <id, sf::Shape*>
-> GetShape(id);
-> id AddShape(Shape*)
-> DeleteShape(id);
-> Render() --> rendert alle yoinker
-> jedes Object was rendern will, holt sich vom RManager ne id f�r sein rendering dings
--> Render & Gamelogic mehr trennen

2. Klassen f�r die einzelnen GameElemente (player, ball, tile)
-> Funktionen in die klassen

3. Unbenutzen Kram l�schen


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

const int WINDOW_WIDTH = 200 * scalingFactor; //F einheit? px?
const int WINDOW_HEIGHT = 200 * scalingFactor;

bool m_GamePaused = false;//F m_GamePaused / m_RequestPauseGame
bool m_CalculateOneFrame = false;

const sf::Color NO_HIT_COLOR = sf::Color::Blue;
const sf::Color ONE_HIT_COLOR = sf::Color::Red;

//F -> Player.h
//Player Variables
bool				m_CurrentInputToWorkWith = false; //F  not player related, but application related "m_NewInputAvailable"
Tile*				m_PlayerTile; //F remove?
const sf::Vector2f	m_PlayerDimensions		= sf::Vector2f(55.0f, 10.0f) * scalingFactor;
sf::Vector2f		m_PlayerPosition = sf::Vector2f(0.33f * WINDOW_WIDTH, 0.8f * WINDOW_HEIGHT);
sf::Vector2f		m_PlayerPositionChanges = sf::Vector2f(0.0f, 0.0f); //F could be "this is how much the player moved last tick" "this is how much we want to move next tick" -> m_DesiredMovement? m_MovementToApply m_MovementNextTick
constexpr float		BASE_MOVEMENT_SPEED	= 0.09f; //F MOVEMENT_PER_TICK / m_MovementSpeed (perTick) ||| const wenn sichs nicht �ndert oder sogar constexpr -> BASE_MOVEMENT_SPEED
constexpr float		PLAYER_RIM_PERCENTAGE	= 0.3f;

//  -> Player.h
bool	m_PlayerSpeedIsHeightened = false; //F buffed, increased
int		m_PlayerSpeedHeightenedTimer = 0; //F unclear: could be time since buff start or time until buff end -> m_RemainingSpeedBuffTime | m_SpeedBuffRunningSince
float	m_PlayerHeightenedSpeedMultiplier = 3; //F static constexpr --> could be part of buff class (does not really belong to player)
float	m_PlayerSpeedHeightenedDuration = 5000.0f; //F static constexpr --> could be part of buff class (does not really belong to player)
sf::Color m_PStarterColor = sf::Color::Green; //F Player::BASE_COLOR
sf::Color m_PSpeedHeightenedEffectColor = sf::Color::Cyan; //F Player::BUFF_COLOR

//F === Until here ===

//Tile Variables
sf::Vector2f m_TileDimensions = sf::Vector2<float>((float)(WINDOW_WIDTH/WINDOW_SEGMENTS_WIDTH), (float)(WINDOW_HEIGHT/WINDOW_SEGMENTS_HEIGHT) / 3.0f);
sf::Color m_NoHitColor;
sf::Color m_FinalHitColor;
int m_TileHitsAllowed = 3;

const int amountOfInputs = 10;
const int frameLimit = 600;
bool inputAllowed [amountOfInputs];
int inputTimer [amountOfInputs];
int inputTimerLimit[amountOfInputs];


sf::Vector2f m_BallStarterPosition_UNALTERED = sf::Vector2f(WINDOW_WIDTH * 0.05f, WINDOW_HEIGHT * 0.4f);
sf::Vector2f m_BallStarterPosition_ALTERED = sf::Vector2f(0.0f, 0.0f);

bool m_BallControlEnabled = false;
bool moveUp = false;
bool moveDown = false;
bool moveRight = false;
bool moveLeft = false;

std::vector<Ball*> Balls_In_Game; //F inconsistent
std::vector<DroppingEffect> m_DroppingEffects;
std::map<TileType, sf::Color> tileTypeToColor;

//Debugging Visuals

sf::Vertex m_PlayerNormalLineCoords [8]; 

//Manager
RenderManager* renderManager; 

int main()
{
	
	//Tile Color Setup
	m_DroppingEffects = std::vector<DroppingEffect>();

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


	tileTypeToColor[TileType::AddedBall] = grassGreen;
	tileTypeToColor[TileType::QuickerPlayer] = racingRed;
	tileTypeToColor[TileType::NoEvent] = muddyTurquoise;

	m_NoHitColor = sf::Color::Blue;
	m_FinalHitColor = sf::Color::Red;

	sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "SFML works!");
	renderManager = new RenderManager(window);

	/*sf::CircleShape shape(10.f);
	shape.setFillColor(sf::Color::Green);
	shape.setPosition((WINDOW_WIDTH/2.0f)-10.0f, (WINDOW_HEIGHT/2.0f)-10.0f);*/

	
	Tile* gametiles[TILE_ARRAY_LENGTH];
	FillTileArrayWithData(gametiles, TILE_ARRAY_LENGTH);

	//sf::RectangleShape tileShapes[TILE_ARRAY_LENGTH];


	
	//for (int i = 0; i < TILE_ARRAY_LENGTH; i++) {
	//	float outlineThickness = 0.5f;
	//	sf::Vector2<float>size = m_TileDimensions - sf::Vector2f(2.0f* outlineThickness,2.0f* outlineThickness);
	//	sf::Vector2<float> position = sf::Vector2<float>((gametiles[i]->position.x * (WINDOW_WIDTH/WINDOW_SEGMENTS_WIDTH)) + outlineThickness + (m_TileDimensions.x /2.0f), gametiles[i]->position.y * (WINDOW_HEIGHT/WINDOW_SEGMENTS_HEIGHT)/3.0f + (m_TileDimensions.y /2.0f));
	//	sf::RectangleShape* rect= new sf::RectangleShape(size);

	//	
	//	
	//	rect->setOrigin(m_TileDimensions.x/2.0f, m_TileDimensions.y/2.0f);
	//	rect->setFillColor(gametiles[i]->color);
	//	rect->setOutlineColor(sf::Color::White);
	//	rect->setOutlineThickness(outlineThickness);
	//	rect->setPosition(position);
	///*	tileShapes[i] = rect;*/
	//	gametiles[i]->tileVisualID = renderManager->AddShape(rect);
	//	
	//	
	//}

	//Create player tile
	m_PlayerTile = new Tile();
	m_PlayerTile->position = m_PlayerPosition;
	m_PlayerTile->baseColor = m_PStarterColor;
	
	//create ball
	SpawnBall();
	
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
				float aspectRatio = (float)WINDOW_WIDTH/WINDOW_HEIGHT;
				sf::FloatRect visibleArea(0, 0, event.size.width, event.size.height);
				window.setSize(sf::Vector2u(event.size.height * aspectRatio, event.size.height));
				
			}

			if (event.type == sf::Event::Closed)
				window.close();
		}

		//Check for Input
		if (!m_CurrentInputToWorkWith)
		{
			m_CurrentInputToWorkWith = CheckForInput(*Balls_In_Game[0], gametiles, window);
		}

		if (m_CalculateOneFrame) {
			DoGameLoopCalculations(*Balls_In_Game[0], gametiles);
			//std::cout << "Calculate one frame" << std::endl;
			m_CalculateOneFrame = false;
		}

		if (m_GamePaused) {
			MoveBallThroughInput(*Balls_In_Game[0]);
			RenderGameData(window,gametiles);
			
			continue;
		}

		
		DoGameLoopCalculations(*Balls_In_Game[0], gametiles);
		RenderGameData(window, gametiles);
	}

	return 0;
}

bool SpawnBall() {
	sf::CircleShape* ballNotSharedPtr = new sf::CircleShape(0.0f);
	std::shared_ptr<sf::CircleShape> ball_Visual = std::make_shared<sf::CircleShape>(*ballNotSharedPtr);
	int ballVisID = renderManager->AddShape(ball_Visual);
	Ball* ball = new Ball(2.0f * scalingFactor, m_BallStarterPosition_UNALTERED, 315.0f, 0.05f, scalingFactor, *ball_Visual, ballVisID);
	ball->ballPosition = m_BallStarterPosition_UNALTERED;
	ball->ballDirection = sf::Vector2f(std::cos(ball->ballAngle) * ball->ballSpeed, std::sin(ball->ballAngle) * ball->ballSpeed);
	ball_Visual->setRadius(ball->ballRadius);
	ball_Visual->setFillColor(sf::Color::Red);
	ball_Visual->setPosition(ball->ballPosition);

	Balls_In_Game.push_back(ball);
	return true;
}

//F in ball.h --> UpdateMovement & UpdateMovement_DebugInput, ein if/else entscheidet welche von den beiden genutzt wird
bool MoveBallThroughInput(Ball& ball) {

	bool inputGiven = false;
	ball = *Balls_In_Game[0];
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

	m_PlayerPosition = sf::Vector2f(MathHelper::Clamp((m_PlayerPositionChanges.x + m_PlayerPosition.x), 0.0f + (m_PlayerDimensions.x / 2.0f), WINDOW_WIDTH - (m_PlayerDimensions.x / 2.0f)), m_PlayerPosition.y);
	m_PlayerPositionChanges = sf::Vector2f(0.0f, 0.0f);
	m_CurrentInputToWorkWith = false;
	return true;
}

bool DoGameLoopCalculations(Ball& ball, Tile* gametiles[]) {
	//Update Player Position

	if (m_PlayerSpeedIsHeightened) {
		m_PlayerSpeedHeightenedTimer--;
		m_PlayerPositionChanges.x = m_PlayerPositionChanges.x * m_PlayerHeightenedSpeedMultiplier;
		float effectDurationPercentage = (float)m_PlayerSpeedHeightenedTimer / (float)m_PlayerSpeedHeightenedDuration;

		m_PlayerTile->baseColor.r = MathHelper::Lerp(m_PSpeedHeightenedEffectColor.r, m_PlayerTile->baseColor.r, effectDurationPercentage);
		m_PlayerTile->baseColor.g = MathHelper::Lerp(m_PSpeedHeightenedEffectColor.g, m_PlayerTile->baseColor.g, effectDurationPercentage);
		m_PlayerTile->baseColor.b = MathHelper::Lerp(m_PSpeedHeightenedEffectColor.b, m_PlayerTile->baseColor.b, effectDurationPercentage);
		if (m_PlayerSpeedHeightenedTimer < 0.0f) {
			m_PlayerSpeedIsHeightened = false;
			m_PlayerTile->baseColor = m_PStarterColor;
		}
	}

	if (m_CurrentInputToWorkWith) {
	
		UpdatePlayerPosition();
		sf::Vector2f playerCollisionBounceDirection;
		sf::Vector2f* ptrToNewDirectionVector = nullptr;
		for (int j = 0; j < Balls_In_Game.size(); j++) {
			bool collisionWithPlayer = CheckForCollisionWithPlayer(*Balls_In_Game[j], m_PlayerPosition, &playerCollisionBounceDirection, ptrToNewDirectionVector);
			if (collisionWithPlayer) {
				//check if the ball is already inside the player 
				Ball currentBall = *Balls_In_Game[j];
				if (currentBall.ballPosition.x < (m_PlayerPosition.x + m_PlayerDimensions.x / 2.0f) && currentBall.ballPosition.x > (m_PlayerPosition.x - m_PlayerDimensions.x / 2.0f)) {
					if (currentBall.ballPosition.y < (m_PlayerPosition.y + m_PlayerDimensions.y / 2.0f) && currentBall.ballPosition.y >(m_PlayerPosition.y - m_PlayerDimensions.y / 2.0f)) {
						//teleport it to the side the ball came from and reflect horizontally 
						bool comingInLeft = currentBall.ballDirection.x > 0.0f;
						float distanceBallToPlayerMiddle = abs(currentBall.ballPosition.x - m_PlayerPosition.x);
						float setOffDistance = m_PlayerDimensions.x - distanceBallToPlayerMiddle;
						currentBall.ballPosition = sf::Vector2f(currentBall.ballPosition.x - setOffDistance, currentBall.ballPosition.y);
						currentBall.ballDirection = playerCollisionBounceDirection;

					}
				}
				//teleport outside the area
			}

		}
		//Trigger update checks for all balls
		//if balls are within player tile area, they need to be repelled and probably teleported outside of the area
	}

	//Update Ball 

	for (int i = 0; i < Balls_In_Game.size(); i++) {
		//0) check if ball is colliding with walls
		float wallDistanceXMin = Balls_In_Game[i]->ballPosition.x;
		float wallDistanceXMax = std::abs(WINDOW_WIDTH - Balls_In_Game[i]->ballPosition.x);

		if (wallDistanceXMin < Balls_In_Game[i]->repellantDistance || wallDistanceXMax < Balls_In_Game[i]->repellantDistance) {
			std::cout << "Wall Collide!" << std::endl;
			Balls_In_Game[i]->ballDirection = sf::Vector2f(Balls_In_Game[i]->ballDirection.x * -1.0f, Balls_In_Game[i]->ballDirection.y);
		}

		float wallDistanceYMin = Balls_In_Game[i]->ballPosition.y;
		float wallDistanceYMax = std::abs(WINDOW_HEIGHT - Balls_In_Game[i]->ballPosition.y);

		if (wallDistanceYMin < Balls_In_Game[i]->repellantDistance || wallDistanceYMax < Balls_In_Game[i]->repellantDistance) {
			std::cout << "Wall Collide!" << std::endl;
			Balls_In_Game[i]->ballDirection = sf::Vector2f(Balls_In_Game[i]->ballDirection.x, Balls_In_Game[i]->ballDirection.y * -1.0f);

		}

		sf::Vector2f originalBallDirection = Balls_In_Game[i]->ballDirection;
		sf::Vector2f playerCollisionBounceDirection;
		sf::Vector2f tileCollisionBounceDirection;
		sf::Vector2f* ptrToNewDirectionVector = nullptr;

		/*bool futurePathIsClear = false;*/

		/*while (!futurePathIsClear) {*/

			//give direction vector to work with
			//calculate if player would collide with the given direction vector
			//if yes, calculate new direction vector
			//feed new direction vector back into the loop
			//if the ball wouldn't collide, set new direction to the given vector

			//possible problems:
			//1) could lead to false collision calculation results if the future ball position check lies too far in the collision area

		bool collisionWithPlayer = CheckForCollisionWithPlayer(*Balls_In_Game[i], m_PlayerPosition, &playerCollisionBounceDirection, ptrToNewDirectionVector);
		bool collisionWithTile = CheckForBallTileCollisionAndMovementChanges(*Balls_In_Game[i], gametiles, &tileCollisionBounceDirection);
		bool effectIsUsed = CheckForEffectUsage();

		//ALL OF THIS NEEDS TO BE DONE OUTSIDE OF "COLLISION WITH PLAYER"
		if (collisionWithPlayer) {
			//check if the ball is already inside the player 
			Ball currentBall = *Balls_In_Game[i];
			if (currentBall.ballPosition.x < (m_PlayerPosition.x + m_PlayerDimensions.x / 2.0f) && currentBall.ballPosition.x >(m_PlayerPosition.x - m_PlayerDimensions.x / 2.0f)) {
				if (currentBall.ballPosition.y < (m_PlayerPosition.y + m_PlayerDimensions.y / 2.0f) && currentBall.ballPosition.y >(m_PlayerPosition.y - m_PlayerDimensions.y / 2.0f)) {
					//teleport it to the side the ball came from and reflect horizontally 
					bool comingInLeft = currentBall.ballDirection.x > 0.0f;
					float distanceBallToPlayerMiddle = abs(currentBall.ballPosition.x - m_PlayerPosition.x);
					float setOffDistance = m_PlayerDimensions.x - distanceBallToPlayerMiddle;
					if (comingInLeft) {
						currentBall.ballPosition = sf::Vector2f(m_PlayerPosition.x - (m_PlayerDimensions.x + 10.0f), currentBall.ballPosition.y);

					}
					currentBall.ballDirection = currentBall.ballDirection * -1.0f;

				}
			}
			Balls_In_Game[i]->ballDirection = playerCollisionBounceDirection;
		}

		if (collisionWithTile) {
			Balls_In_Game[i]->ballDirection = tileCollisionBounceDirection;
		}
		/*}*/


		sf::Vector2f newBallPosition = Balls_In_Game[i]->ballPosition + Balls_In_Game[i]->ballDirection;
		Balls_In_Game[i]->ballPosition = newBallPosition;
	}
	

	return true;
}

bool RenderGameData(sf::RenderWindow& window,  Tile* gametiles[]) {

	//Update Tiles 

	window.clear();


	//draw tiles
	for (int j = 0; j < TILE_ARRAY_LENGTH; j++) {
		gametiles[j]->UpdateTileColorBasedOnHits(*renderManager);
	}
	//draw tile effects which are falling down
	DrawTileEffects(window);

	//draw player tile
	//Player.cpp probably 
	sf::RectangleShape playerTileMiddle			= sf::RectangleShape(sf::Vector2f(m_PlayerDimensions.x * (1.0f - PLAYER_RIM_PERCENTAGE), m_PlayerDimensions.y));
	sf::RectangleShape playerTileLeft		= sf::RectangleShape(sf::Vector2f(m_PlayerDimensions.x * PLAYER_RIM_PERCENTAGE, m_PlayerDimensions.y));
	sf::RectangleShape playerTileRight		= sf::RectangleShape(sf::Vector2f(m_PlayerDimensions.x * PLAYER_RIM_PERCENTAGE, m_PlayerDimensions.y));


	playerTileMiddle.setOrigin(sf::Vector2f(m_PlayerDimensions.x/2.0f, m_PlayerDimensions.y/2.0f));
	playerTileLeft.setOrigin(sf::Vector2f((m_PlayerDimensions.x/2.0f), m_PlayerDimensions.y/2.0f));
	playerTileRight.setOrigin(sf::Vector2f((m_PlayerDimensions.x/2.0f), m_PlayerDimensions.y/2.0f));


	sf::Vector2f drawingPos = sf::Vector2f(m_PlayerPosition.x + (m_PlayerDimensions.x * (PLAYER_RIM_PERCENTAGE)), m_PlayerPosition.y);
	sf::Vector2f leftPos = sf::Vector2f(m_PlayerPosition.x + (m_PlayerDimensions.x * (1.0f - PLAYER_RIM_PERCENTAGE)), m_PlayerPosition.y);
	sf::Vector2f rightPos = sf::Vector2f(m_PlayerPosition.x, m_PlayerPosition.y);
	playerTileMiddle.setPosition(drawingPos);
	playerTileLeft.setPosition(leftPos);
	playerTileRight.setPosition(rightPos);

	playerTileMiddle.setFillColor(m_PlayerTile->baseColor);
	playerTileLeft.setFillColor(sf::Color::Red);
	playerTileRight.setFillColor(sf::Color::Blue);
	playerTileMiddle.setOutlineColor(sf::Color::Yellow);

	
	window.draw(playerTileMiddle);
	window.draw(playerTileLeft);
	window.draw(playerTileRight);

	for (int i = 0; i < Balls_In_Game.size(); i++) {
		//std::shared_ptr<sf::CircleShape> currentBallVisual = std::dynamic_pointer_cast<sf::CircleShape>(renderManager->GetShape(Balls_In_Game[i]->ballVisualID));
		std::shared_ptr<sf::CircleShape> currentBallVisual = std::static_pointer_cast<sf::CircleShape>(renderManager->GetShape(Balls_In_Game[i]->ballVisualID));
		//sf::RectangleShape* currentTile = (sf::RectangleShape*) renderManager.GetShape(tileVisualID);

		//std::shared_ptr<sf::Shape>currentBallVisual = renderManager->GetShape(Balls_In_Game[i]->ballVisualID);
		currentBallVisual->setPosition(Balls_In_Game[i]->ballPosition);

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

bool FillTileArrayWithData(Tile* tiles[], int tileArrayLength) {


	
	//get tile type by chance
	std::random_device rd; // obtain a random number from hardware
	std::mt19937 gen(rd()); // seed the generator
	std::uniform_int_distribution<> distr(0, 100); // define the range
	
	
	
	for (int i = 0; i < tileArrayLength; i++) {
		
		//two dimensional "array position" 
		sf::Vector2f index2DPosition = Get2DPositionWithIndex(i);
		sf::Vector2<float> position = sf::Vector2<float>((index2DPosition.x * (WINDOW_WIDTH / WINDOW_SEGMENTS_WIDTH)) + Tile::OUTLINE_THICKNESS + (m_TileDimensions.x / 2.0f),
														(index2DPosition.y * (WINDOW_HEIGHT / WINDOW_SEGMENTS_HEIGHT)) / 3.0f + (m_TileDimensions.y / 2.0f));

		//what sort of tile is this?
		TileType newType = TileType::TileTCount;
		float randomNumber = distr(gen) / 100.0f;
		if (randomNumber < 0.2f) {
			newType = TileType::AddedBall;
		}
		else if (randomNumber >= 0.7f) {
			newType = TileType::QuickerPlayer;
		}
		else {
			newType = TileType::NoEvent;
		}

		//tile visual shape creation
		sf::Vector2<float>size = m_TileDimensions - sf::Vector2f(2.0f * Tile::OUTLINE_THICKNESS, 2.0f * Tile::OUTLINE_THICKNESS);

		//sf::CircleShape* ballNotSharedPtr = new sf::CircleShape(0.0f);
		//std::shared_ptr<sf::CircleShape> ball_Visual = std::make_shared<sf::CircleShape>(ballNotSharedPtr);


		sf::RectangleShape* rect = new sf::RectangleShape(size);
		std::shared_ptr<sf::RectangleShape>rect_shared = std::make_shared<sf::RectangleShape>(*rect);
		rect->setSize(size);

		rect_shared->setOrigin(m_TileDimensions.x / 2.0f, m_TileDimensions.y / 2.0f);
		rect_shared->setFillColor(tileTypeToColor[newType]);
		rect_shared->setOutlineColor(sf::Color::White);
		rect_shared->setOutlineThickness(Tile::OUTLINE_THICKNESS);
		rect_shared->setPosition(position);
		/*	tileShapes[i] = rect;*/
		int visID = renderManager->AddShape(rect_shared);
		Tile* newTile = new Tile(position, newType, tileTypeToColor[newType], visID);
		tiles[i] = newTile;
		
		
	}
	return true;
}

sf::Vector2f Get2DPositionWithIndex(int index) {
	sf::Vector2f position;

	position.x = index % (WINDOW_SEGMENTS_WIDTH);
	position.y = std::floor((float)index/(WINDOW_SEGMENTS_WIDTH));
	return position;
}

bool CheckForInput(Ball& ball, Tile* gameTiles[], sf::RenderWindow& window) {

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
		m_PlayerPositionChanges.x -= BASE_MOVEMENT_SPEED;
		
		return true;
	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
	{
		// left key is pressed: move our character
		m_PlayerPositionChanges.x += BASE_MOVEMENT_SPEED;
		
		return true;
	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::P) && inputAllowed[0]) {
		
		std::cout <<"Ball Pos:" << ball.ballPosition.x <<" ," << ball.ballPosition.y <<std::endl;
		inputAllowed[0] = false;
	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::O) && inputAllowed[7]) {

		std::cout << "Player Pos:" << m_PlayerPosition.x << " ," << m_PlayerPosition.y << std::endl;
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
		m_PlayerSpeedHeightenedTimer = m_PlayerSpeedHeightenedDuration;
		m_PlayerSpeedIsHeightened = true;
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




//F ball.h
bool CheckForCollisionWithPlayer(Ball& ball, sf::Vector2f playerTilePos, sf::Vector2f* bounceDirection, sf::Vector2f* nextBallPosition)
{
	
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

	float minPlayerX = playerTilePos.x - (m_PlayerDimensions.x /2.0f);
	float maxPlayerX = playerTilePos.x + (m_PlayerDimensions.x /2.0f);
	float minPlayerY = playerTilePos.y - (m_PlayerDimensions.y / 2.0f);
	float maxPlayerY = playerTilePos.y + (m_PlayerDimensions.y / 2.0f);
	
	//      |
	//		*-------*
	//		|       |  rightX
	//		*-------*
	//      |
	//
	//bools to check for sector position of the ball
	bool leftX	= false;
	bool rightX = false;
	bool upY	= false;
	bool downY	= false;

	if(futureBallPosition.x < minPlayerX){ testingX			= minPlayerX; leftX		= true;}
	else if(futureBallPosition.x > maxPlayerX){ testingX	= maxPlayerX; rightX	= true;}
	
	if (futureBallPosition.y < minPlayerY) {testingY		= minPlayerY; upY		= true;}
	else if (futureBallPosition.y > maxPlayerY) { testingY	= maxPlayerY; downY		= true;}
	
	float distanceX = futureBallPosition.x - testingX;
	float distanceY = futureBallPosition.y - testingY;
	float overallDistance = sqrt((distanceX * distanceX)+ (distanceY* distanceY));

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
		else if (downY &&leftX) {
			leftLowerCorner = true;
			cornerWasHit = true;
		}
		else if (upY && rightX) {
			rightUpperCorner = true;
			cornerWasHit = true;
		}
		else if (downY &&rightX) {
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
		*bounceDirection = sf::Vector2f(ball.ballDirection.x * -1.0f, ball.ballDirection.y* -1.0f);
		return true;
	}
	if (playerCollideVertical) {
		*bounceDirection = CalculateBounceVector(futureBallPosition, CollisionType::VerticalCollision, ball);
		return true;
	}

	if (playerCollideHorizontal) {
		*bounceDirection = sf::Vector2f(ball.ballDirection.x * -1.0f, ball.ballDirection.y);
		return true;
	}


	

	
	return false;
}

void UpdateCollisionDebugDrawings(sf::Vector2f leftRimNormal, sf::Vector2f rightRimNormal, sf::Vector2f ballToPlayerVector, sf::Vector2f newBallDirVector) {

	sf::Vector2f leftPos = sf::Vector2f(m_PlayerPosition.x - (m_PlayerDimensions.x * PLAYER_RIM_PERCENTAGE), m_PlayerPosition.y);
	sf::Vector2f rightPos = sf::Vector2f(m_PlayerPosition.x + (m_PlayerDimensions.x * PLAYER_RIM_PERCENTAGE), m_PlayerPosition.y);
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
	m_PlayerNormalLineCoords[4].position = m_PlayerPosition - (ballToPlayerVector * 1000.0f);
	m_PlayerNormalLineCoords[4].color = sf::Color::Red;

	m_PlayerNormalLineCoords[5].position = m_PlayerPosition;
	m_PlayerNormalLineCoords[5].color = sf::Color::Red;

	m_PlayerNormalLineCoords[6].position = m_PlayerPosition;
	m_PlayerNormalLineCoords[6].color = sf::Color::Red;

	m_PlayerNormalLineCoords[7].position = m_PlayerPosition + (newBallDirVector * 1000.0f);
	m_PlayerNormalLineCoords[7].color = sf::Color::Red;
}

sf::Vector2f CalculateBounceVector(sf::Vector2f futureBallPosition, CollisionType type, Ball& ball) {

	sf::Vector2f newBallDirectionVector; 
	float deviationAngle = 5.0f;
	float leftAngleInRadianX = cos(((double)270.0f + (double)deviationAngle)  * (M_PI / 180.0f));
	float leftAngleInRadianY = sin(((double)270.0f + (double)deviationAngle)  * (M_PI / 180.0f));
	float rightAngleInRadianX = cos(((double)270.0f - (double)deviationAngle) * (M_PI / 180.0f));
	float rightAngleInRadianY = sin(((double)270.0f - (double)deviationAngle) * (M_PI / 180.0f));

	
	sf::Vector2f leftRimNormal = sf::Vector2f(leftAngleInRadianX , leftAngleInRadianY);
	sf::Vector2f rightRimNormal =  sf::Vector2f(rightAngleInRadianX , rightAngleInRadianY);

	/*m_PlayerNormalLineCoords[0].position = m_PlayerPosition;
	m_PlayerNormalLineCoords[0].color = sf::Color::Red;

	m_PlayerNormalLineCoords[1].position = m_PlayerPosition + (rightRimNormal * 100.0f);
	m_PlayerNormalLineCoords[1].color = sf::Color::Red;*/


	//player collision ball bounce vector calculation based on where on the player it lands
	//1) where on the player did we land? (percentage areas)
	if (type == CollisionType::VerticalCollision) {
		float proportionalXPos = futureBallPosition.x - (m_PlayerPosition.x - (m_PlayerDimensions.x /2.0f));
		float percentageOnXAxis =proportionalXPos/m_PlayerDimensions.x;
		if (percentageOnXAxis < PLAYER_RIM_PERCENTAGE) {
			//cos-1 [ (a � b) / (|a| |b|) ]
			
			float ballDirTimesNormal = (ball.ballDirection.x * leftRimNormal.x) + (ball.ballDirection.y * leftRimNormal.y);
			float absoluteBallDir = sqrt((ball.ballDirection.x * ball.ballDirection.x) + (ball.ballDirection.y * ball.ballDirection.y));
			float absoluteNormal = sqrt((leftRimNormal.x * leftRimNormal.x) + (leftRimNormal.y * leftRimNormal.y));
			float angleBetweenBallDirectionAndNormal = acos(ballDirTimesNormal/(absoluteBallDir * absoluteNormal)) / (M_PI / 180.0f);
			float newVectorAngle = angleBetweenBallDirectionAndNormal * -2.0f;
			
			newBallDirectionVector = sf::Vector2f(ball.ballDirection.x * cos((newVectorAngle) * (M_PI / 180.0f)) - ball.ballDirection.y * sin(newVectorAngle * (M_PI / 180.0f)), ball.ballDirection.x * sin(newVectorAngle * (M_PI / 180.0f)) + ball.ballDirection.y * cos(newVectorAngle * (M_PI / 180.0f)));
			std::cout<< "old: " << angleBetweenBallDirectionAndNormal <<", new: " << newVectorAngle << std::endl;
		}
		if (percentageOnXAxis > 1.0f - PLAYER_RIM_PERCENTAGE ) {

			float ballDirTimesNormal = (ball.ballDirection.x * rightRimNormal.x) + (ball.ballDirection.y * rightRimNormal.y);
			float absoluteBallDir = sqrt((ball.ballDirection.x * ball.ballDirection.x) + (ball.ballDirection.y * ball.ballDirection.y));
			float absoluteNormal = sqrt((rightRimNormal.x * rightRimNormal.x) + (rightRimNormal.y * rightRimNormal.y));
			float angleBetweenBallDirectionAndNormal = acos(ballDirTimesNormal / (absoluteBallDir * absoluteNormal)) / (M_PI / 180.0f);
			float newVectorAngle = angleBetweenBallDirectionAndNormal * -2.0f;

			newBallDirectionVector = sf::Vector2f(ball.ballDirection.x * cos((newVectorAngle) * (M_PI / 180.0f)) - ball.ballDirection.y * sin(newVectorAngle * (M_PI / 180.0f)), ball.ballDirection.x * sin(newVectorAngle * (M_PI / 180.0f)) + ball.ballDirection.y * cos(newVectorAngle * (M_PI / 180.0f)));
			std::cout<< "old: " << angleBetweenBallDirectionAndNormal <<", new: " << newVectorAngle << std::endl;
		}
		else {
			newBallDirectionVector = sf::Vector2f(ball.ballDirection.x, ball.ballDirection.y* -1.0f);
		}
		UpdateCollisionDebugDrawings(leftRimNormal, rightRimNormal, ball.ballDirection, newBallDirectionVector);
	}

	//2) calculate the normal for the player area (make it customizable based on initial deviation degree for normal)
	//3) calculate the angle between the normal and the ball direction vector
	//4) give new direction vector based on the angle 

	return newBallDirectionVector;
}



bool CheckForBallTileCollisionAndMovementChanges(Ball& ball,  Tile** gametiles, sf::Vector2f* bounceDirection) {


	bool playerCollideVertical = false;
	bool playerCollideHorizontal = false;


	sf::Vector2f futureBallPosition = (ball.ballPosition + sf::Vector2f(ball.ballRadius, ball.ballRadius)) + ball.ballDirection;

	

	for (int i = 0; i < TILE_ARRAY_LENGTH; i++) {
		bool alive = gametiles[i]->isAlive;
		if (!alive) {
			continue;
		}
		sf::Vector2f currentTilePosition = gametiles[i]->position;
		float testingX = futureBallPosition.x;
		float testingY = futureBallPosition.y;
		
		float minTileX = currentTilePosition.x - (m_TileDimensions.x /2.0f);
		float maxTileX = currentTilePosition.x + (m_TileDimensions.x /2.0f);
		float minTileY = currentTilePosition.y - (m_TileDimensions.y / 2.0f);
		float maxTileY = currentTilePosition.y + (m_TileDimensions.y / 2.0f);

		//bools to check for section position of the ball
		bool leftX	= false;
		bool rightX = false;
		bool upY	= false;
		bool downY	= false;

		if(futureBallPosition.x < minTileX){ testingX			= minTileX; leftX		= true;}
		else if(futureBallPosition.x > maxTileX){ testingX	= maxTileX; rightX	= true;}

		if (futureBallPosition.y < minTileY) {testingY		= minTileY; upY		= true;}
		else if (futureBallPosition.y > maxTileY) { testingY	= maxTileY; downY		= true;}

		float distanceX = futureBallPosition.x - testingX;
		float distanceY = futureBallPosition.y - testingY;
		float overallDistance = sqrt((distanceX * distanceX)+ (distanceY* distanceY));

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
			
			*bounceDirection = sf::Vector2f(ball.ballDirection.x, ball.ballDirection.y* -1.0f);
			gametiles[i]->hitCount -= 1;
			int currentHitCount = gametiles[i]->hitCount;
			if (currentHitCount <= 0) {
				gametiles[i]->isAlive = false;
				DropTileEffect(*gametiles[i]);
			}
			return true;
		}

		if (playerCollideHorizontal) {
			*bounceDirection = sf::Vector2f(ball.ballDirection.x * -1.0f, ball.ballDirection.y);
			gametiles[i]->hitCount -= 1;
			int currentHitCount = gametiles[i]->hitCount;
			if (currentHitCount <= 0) {
				gametiles[i]->isAlive = false;
				DropTileEffect(*gametiles[i]);
			}
			return true;
		}
	}

	return false;
	
}

bool DropTileEffect(Tile& tile) {
	sf::RectangleShape effectRect = sf::RectangleShape(sf::Vector2f(10.0f, 10.0f));
	effectRect.setSize(sf::Vector2f(10.0f, 10.0f));
	effectRect.setPosition(tile.position);
	effectRect.setFillColor(tileTypeToColor[tile.tileType]);
	DroppingEffect newEffect = DroppingEffect(tile.tileType, effectRect);
	m_DroppingEffects.push_back(newEffect);
	
	//DroppingEffect& addedElement = m_DroppingEffects.emplace_back();
	//addedElement.visual->setSize()

	//for (auto* effect : m_DroppingEffects)
	//{
	//	delete effect;
	//	effect = nullptr;
	//}
	//m_DroppingEffects.clear();
	return true;
}

bool DrawTileEffects(sf::RenderWindow& window) {
	//go through all tile effects in the list and update their position
	//then draw them
	float downwardMovementIncrement = 0.02f;
	int amount = m_DroppingEffects.size();
	for (int i = 0; i < amount; i++) {
		sf::Vector2f pos = m_DroppingEffects[i].visual.getPosition();
		pos = sf::Vector2f(pos.x, pos.y + downwardMovementIncrement);
		m_DroppingEffects[i].visual.setPosition(pos);
		sf::RectangleShape currentVisual = m_DroppingEffects[i].visual;
		window.draw(currentVisual);
	}

	return true;
}

TileType CheckEffectWithPlayerCollision(bool* collisionBool) {

	TileType foundType = TileType::NoEvent;
	int length = m_DroppingEffects.size();
	for (int i = 0; i < length; i++) {
		//check if tile is colliding with player
		sf::Vector2f effectPos = m_DroppingEffects[i].visual.getPosition();
		float distanceEffectPlayerY = abs(effectPos.y - m_PlayerPosition.y);
		if (distanceEffectPlayerY < 2.0f) {
			if (effectPos.x < (m_PlayerPosition.x + (m_PlayerDimensions.x / 2.0f)) && effectPos.x >(m_PlayerPosition.x - (m_PlayerDimensions.x / 2.0f))) {
				foundType = m_DroppingEffects[i].effectType;
				*collisionBool = true;
				m_DroppingEffects.erase(m_DroppingEffects.begin()+ i);
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
	TileType effectType = CheckEffectWithPlayerCollision(&collisionPlayerWithEffect);
	if (collisionPlayerWithEffect) {
		switch (effectType) {
		case TileType::AddedBall:
			std::cout << "Added Ball!" << std::endl;
			SpawnBall();
			break;
		case TileType::QuickerPlayer:
			std::cout << "Quicker Player!" << std::endl;
			m_PlayerSpeedHeightenedTimer = m_PlayerSpeedHeightenedDuration;
			m_PlayerSpeedIsHeightened = true;
			break;
		case TileType::NoEvent:
			std::cout << "No Event!" << std::endl;
			break;

		}
		return true;
	}
	return false;
}

