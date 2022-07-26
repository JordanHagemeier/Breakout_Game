#include <SFML/Graphics.hpp>
#include <iostream>
#include "Tile.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include "Main_Fuctions.h"
#include <algorithm>
#include <chrono>
#include "Ball.h"
#include <list>
#include <random>



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


const int WINDOW_SEGMENTS_WIDTH = 8;
const int WINDOW_SEGMENTS_HEIGHT = 6;
const int tileArrayLength = WINDOW_SEGMENTS_WIDTH*WINDOW_SEGMENTS_HEIGHT;

const int WINDOW_WIDTH = 200 * scalingFactor;
const int WINDOW_HEIGHT = 200 * scalingFactor;

bool m_PauseGame = false;
bool m_CalculateOneFrame = false;

const sf::Color NO_HIT_COLOR = sf::Color::Blue;
const sf::Color ONE_HIT_COLOR = sf::Color::Red;

//Player Variables
bool				m_CurrentInputToWorkWith = false;
Tile*				m_PlayerTile; 
const sf::Vector2f	m_PlayerDimensions		= sf::Vector2f(55.0f, 10.0f) * scalingFactor;
//sf::Vector2f		m_PlayerPosition		= sf::Vector2f((WINDOW_WIDTH / 2.0f) - (m_PlayerDimensions.x / 2.0f), WINDOW_HEIGHT * 0.66f);
sf::Vector2f		m_PlayerPosition		= sf::Vector2f(198.798f, 396.0f);
sf::Vector2f		m_PlayerPositionChanges = sf::Vector2f(0.0f, 0.0f);
float				m_PMovementIncrements	= 0.09f;
float				m_PlayerRimPercentage	= 0.1f;

//Ball Variables
//float m_BallRadius = 2.0f * scalingFactor;
//sf::Vector2f m_BallPosition; 
//sf::Vector2f m_BallDirection;
//float m_BallAngle = 315.0f;
//float m_BallSpeed = 0.05f;
//float m_RepellantDistance = m_BallRadius/2.0f;

bool	m_BlockMovementChanges = false;
int		m_BM_Timer = 0;
float	m_BlockedMovementFrameTime = 10;

bool	m_PLAYER_BlockMovementChanges = false;
int		m_PLAYER_BM_Timer = 0;
float	m_PLAYER_BlockedMovementFrameTime = 20;



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

std::list<Ball*> Balls_In_Game;
std::vector<DroppingEffect> m_DroppingEffects;
std::map<TileType, sf::Color> tileTypeToColor;

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
	sf::CircleShape shape(10.f);
	shape.setFillColor(sf::Color::Green);
	shape.setPosition((WINDOW_WIDTH/2.0f)-10.0f, (WINDOW_HEIGHT/2.0f)-10.0f);

	
	Tile* gametiles[tileArrayLength];
	FillTileArrayWithData(gametiles, tileArrayLength);

	sf::RectangleShape tileShapes[tileArrayLength];


	
	for (int i = 0; i < tileArrayLength; i++) {
		float outlineThickness = 0.5f;
		sf::Vector2<float>size = m_TileDimensions - sf::Vector2f(2.0f* outlineThickness,2.0f* outlineThickness);
		sf::Vector2<float> position = sf::Vector2<float>((gametiles[i]->position.x * (WINDOW_WIDTH/WINDOW_SEGMENTS_WIDTH)) + outlineThickness + (m_TileDimensions.x /2.0f), gametiles[i]->position.y * (WINDOW_HEIGHT/WINDOW_SEGMENTS_HEIGHT)/3.0f + (m_TileDimensions.y /2.0f));
		sf::RectangleShape rect(size);

		
		
		rect.setOrigin(m_TileDimensions.x/2.0f, m_TileDimensions.y/2.0f);
		rect.setFillColor(gametiles[i]->color);
		rect.setOutlineColor(sf::Color::White);
		rect.setOutlineThickness(outlineThickness);
		rect.setPosition(position);
		tileShapes[i] = rect;
		
		
	}

	//Create player tile
	m_PlayerTile = new Tile();
	m_PlayerTile->position = m_PlayerPosition;
	
	//create ball
	sf::CircleShape ball_Visual = sf::CircleShape(0.0f);
	Ball ball = Ball(2.0f * scalingFactor, m_BallStarterPosition_UNALTERED, 315.0f, 0.05f, scalingFactor, ball_Visual);
	ball.ballPosition = m_BallStarterPosition_UNALTERED;
	ball.ballDirection = sf::Vector2f(std::cos(ball.ballAngle) * ball.ballSpeed, std::sin(ball.ballAngle) * ball.ballSpeed);
	ball.ballVisual->setRadius(ball.ballRadius);
	ball.ballVisual->setFillColor(sf::Color::Red);
	ball.ballVisual->setPosition(ball.ballPosition);

	Balls_In_Game = std::list<Ball*>();
	Balls_In_Game.push_back(&ball);

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
			m_CurrentInputToWorkWith = CheckForInput(ball, tileShapes, gametiles, window);
		}

		if (m_CalculateOneFrame) {
			DoGameLoopCalculations(ball, tileShapes, gametiles);
			//std::cout << "Calculate one frame" << std::endl;
			m_CalculateOneFrame = false;
		}

		if (m_PauseGame) {
			MoveBallThroughInput(ball);
			RenderGameData(window,ball, tileShapes, gametiles);
			
			continue;
		}

		
		DoGameLoopCalculations(ball, tileShapes, gametiles);
		RenderGameData(window, ball, tileShapes, gametiles);
	}

	return 0;
}


bool MoveBallThroughInput(Ball& ball) {

	bool inputGiven = false;

	sf::Vector2f oldDirection = ball.ballDirection;
	if (moveUp)		{ball.ballDirection = sf::Vector2f(0.0f, -1.0f);		moveUp    = false; inputGiven = true; }
	if (moveDown)	{ball.ballDirection = sf::Vector2f(0.0f, 1.0f);		moveDown  = false; inputGiven = true;}
	if (moveLeft)	{ball.ballDirection = sf::Vector2f(-1.0f, 0.0f);	moveLeft  = false; inputGiven = true;}
	if (moveRight)	{ball.ballDirection = sf::Vector2f(1.0f, 0.0f);	moveRight = false; inputGiven = true;}

	if (inputGiven) {
		ball.ballPosition = ball.ballPosition + (ball.ballDirection * 2.0f);
		ball.ballVisual->setPosition(ball.ballPosition);
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

bool DoGameLoopCalculations(Ball& ball, sf::RectangleShape tileShapes[], Tile* gametiles[]) {
	//Update Player Position

	if (m_CurrentInputToWorkWith) {
		m_PlayerPosition = sf::Vector2f(clip((m_PlayerPositionChanges.x + m_PlayerPosition.x), 0.0f + (m_PlayerDimensions.x / 2.0f), WINDOW_WIDTH - (m_PlayerDimensions.x /2.0f)), m_PlayerPosition.y);
		m_PlayerPositionChanges = sf::Vector2f(0.0f, 0.0f);
		m_CurrentInputToWorkWith = false;
	}
	//Update Ball 
	//0) check if ball is colliding with walls
	float wallDistanceXMin = ball.ballPosition.x;
	float wallDistanceXMax = std::abs(WINDOW_WIDTH - ball.ballPosition.x);

	if (wallDistanceXMin < ball.repellantDistance || wallDistanceXMax < ball.repellantDistance) {
		std::cout << "Wall Collide!" << std::endl;
		ball.ballDirection = sf::Vector2f(ball.ballDirection.x * -1.0f, ball.ballDirection.y);
	}

	float wallDistanceYMin = ball.ballPosition.y;
	float wallDistanceYMax = std::abs(WINDOW_HEIGHT - ball.ballPosition.y);

	if (wallDistanceYMin < ball.repellantDistance || wallDistanceYMax < ball.repellantDistance) {
		std::cout << "Wall Collide!" << std::endl;
		ball.ballDirection = sf::Vector2f(ball.ballDirection.x, ball.ballDirection.y* -1.0f);

	}

	sf::Vector2f originalBallDirection = ball.ballDirection;
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

		bool collisionWithPlayer = CheckForCollisionWithPlayer(ball,m_PlayerPosition, &playerCollisionBounceDirection, ptrToNewDirectionVector);
		bool collisionWithTile = CheckForBallTileCollisionAndMovementChanges(ball,tileShapes, gametiles, &tileCollisionBounceDirection);

		
		if (collisionWithPlayer) {
			ball.ballDirection = playerCollisionBounceDirection;
		}

		if (collisionWithTile) {
			ball.ballDirection = tileCollisionBounceDirection;
		}
	/*}*/
	

	
	ball.ballPosition = ball.ballPosition + ball.ballDirection;
	ball.ballVisual->setPosition(ball.ballPosition);

	return true;
}

bool RenderGameData(sf::RenderWindow& window, Ball& ball, sf::RectangleShape tileShapes[], Tile* gametiles[]) {

	//Update Tiles 

	window.clear();


	//draw tiles
	for (int j = 0; j < tileArrayLength; j++) {
		sf::RectangleShape currentTile = tileShapes[j];
		float currentHitPercentage = 1.0f - (float)gametiles[j]->hitCount / gametiles[j]->allowedHits;
		sf::Uint8 redPercentage = lerp(gametiles[j]->color.r, m_FinalHitColor.r, currentHitPercentage);
		sf::Uint8 greenPercentage = lerp(gametiles[j]->color.g, m_FinalHitColor.g, currentHitPercentage);
		sf::Uint8 bluePercentage = lerp(gametiles[j]->color.b, m_FinalHitColor.b, currentHitPercentage);
		sf::Uint8 alphaPercentage = 255;

		sf::Color color = sf::Color(redPercentage, greenPercentage, bluePercentage, alphaPercentage);
		currentTile.setFillColor(color);
		if (gametiles[j]->isAlive) {
			window.draw(currentTile);
		}
	}
	//draw tile effects which are falling down
	DrawTileEffects(window);

	//draw player tile
	sf::RectangleShape playerTileMiddle			= sf::RectangleShape(sf::Vector2f(m_PlayerDimensions.x * (1.0f - m_PlayerRimPercentage), m_PlayerDimensions.y));
	sf::RectangleShape playerTileLeft		= sf::RectangleShape(sf::Vector2f(m_PlayerDimensions.x * m_PlayerRimPercentage, m_PlayerDimensions.y));
	sf::RectangleShape playerTileRight		= sf::RectangleShape(sf::Vector2f(m_PlayerDimensions.x * m_PlayerRimPercentage, m_PlayerDimensions.y));


	playerTileMiddle.setOrigin(sf::Vector2f(m_PlayerDimensions.x/2.0f, m_PlayerDimensions.y/2.0f));
	playerTileLeft.setOrigin(sf::Vector2f((m_PlayerDimensions.x/2.0f), m_PlayerDimensions.y/2.0f));
	playerTileRight.setOrigin(sf::Vector2f((m_PlayerDimensions.x/2.0f), m_PlayerDimensions.y/2.0f));
	/*playerTileLeft.setOrigin(sf::Vector2f((m_PlayerDimensions.x / 2.0f) * m_PlayerRimPercentage, m_PlayerDimensions.y / 2.0f));
	playerTileRight.setOrigin(sf::Vector2f((m_PlayerDimensions.x / 2.0f) + ((m_PlayerDimensions.x / 2.0f) * m_PlayerRimPercentage), m_PlayerDimensions.y / 2.0f));*/

	

	sf::Vector2f drawingPos = sf::Vector2f(m_PlayerPosition.x + (m_PlayerDimensions.x * (m_PlayerRimPercentage)), m_PlayerPosition.y);
	sf::Vector2f leftPos = sf::Vector2f(m_PlayerPosition.x + (m_PlayerDimensions.x * (1.0f - m_PlayerRimPercentage)), m_PlayerPosition.y);
	sf::Vector2f rightPos = sf::Vector2f(m_PlayerPosition.x, m_PlayerPosition.y);
	playerTileMiddle.setPosition(drawingPos);
	playerTileLeft.setPosition(leftPos);
	playerTileRight.setPosition(rightPos);

	playerTileMiddle.setFillColor(sf::Color::Green);
	playerTileLeft.setFillColor(sf::Color::Red);
	playerTileRight.setFillColor(sf::Color::Blue);
	playerTileMiddle.setOutlineColor(sf::Color::Yellow);
	window.draw(playerTileMiddle);
	window.draw(playerTileLeft);
	window.draw(playerTileRight);


	ball.ballVisual->setPosition(ball.ballPosition);



	//draw ball
	window.draw(*ball.ballVisual);

	window.display();

	return true;
}
bool FillTileArrayWithData(Tile* tiles[], int tileArrayLength) {


	
	//get tile type by chance
	std::random_device rd; // obtain a random number from hardware
	std::mt19937 gen(rd()); // seed the generator
	std::uniform_int_distribution<> distr(0, 100); // define the range
	
	
	
	for (int i = 0; i < tileArrayLength; i++) {
		
		sf::Vector2f newPos = Get2DPositionWithIndex(i);
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
		Tile* tile = new Tile(newPos, newType, tileTypeToColor[newType]);
		tiles[i] = tile;
		
		
	}
	return true;
}

sf::Vector2f Get2DPositionWithIndex(int index) {
	sf::Vector2f position;

	position.x = index % (WINDOW_SEGMENTS_WIDTH);
	position.y = std::floor((float)index/(WINDOW_SEGMENTS_WIDTH));
	return position;
}

bool CheckForInput(Ball& ball,sf::RectangleShape tileShapes[], Tile* gameTiles[], sf::RenderWindow& window) {

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
		m_PlayerPositionChanges.x -= m_PMovementIncrements;
		
		return true;
	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
	{
		// left key is pressed: move our character
		m_PlayerPositionChanges.x += m_PMovementIncrements;
		
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
		m_PauseGame = !m_PauseGame;
		std::cout <<"Game Pause: " << m_PauseGame <<std::endl;
		inputAllowed[1] = false;
	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Enter) && m_PauseGame && inputAllowed[2]) {
		m_CalculateOneFrame = true;
		
		inputAllowed[2] = false;
	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::T) && inputAllowed[8]) {
		DropTileEffect(*gameTiles[10], &tileShapes[10]);
		DrawTileEffects(window);

		inputAllowed[8] = false;
	}

	if (m_PauseGame) {
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



float clip(float n, float lower, float upper) {
	return std::max(lower, std::min(n, upper));
}


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

		/*bool verticalIsClosest = (distanceY < distanceX);
		if (verticalIsClosest) {
			playerCollideVertical = true;
		}
		else {
			playerCollideHorizontal = true;
		}*/
		
	}

	
	ball.ballVisual->setFillColor(sf::Color::Red);

	if (cornerWasHit) {
		*bounceDirection = sf::Vector2f(ball.ballDirection.x * -1.0f, ball.ballDirection.y* -1.0f);
		return true;
	}
	if (playerCollideVertical) {
		ball.ballVisual->setFillColor(sf::Color::Yellow);
		/**bounceDirection = sf::Vector2f(m_BallDirection.x, m_BallDirection.y* -1.0f);*/
		*bounceDirection = CalculateBounceVector(futureBallPosition, CollisionType::VerticalCollision, ball);
		return true;
	}

	if (playerCollideHorizontal) {
		ball.ballVisual->setFillColor(sf::Color::Magenta);
		*bounceDirection = sf::Vector2f(ball.ballDirection.x * -1.0f, ball.ballDirection.y);
		return true;
	}


	

	
	return false;
}


sf::Vector2f CalculateBounceVector(sf::Vector2f futureBallPosition, CollisionType type, Ball& ball) {

	sf::Vector2f newBallDirectionVector; 
	float deviationAngle = 1.0f;
	float leftAngleInRadianX = cos((90.0f + deviationAngle)  * (M_PI / 180.0f));
	float leftAngleInRadianY = sin((90.0f + deviationAngle)  * (M_PI / 180.0f));
	float rightAngleInRadianX = cos((90.0f - deviationAngle) * (M_PI / 180.0f));
	float rightAngleInRadianY = sin((90.0f - deviationAngle) * (M_PI / 180.0f));

	
	sf::Vector2f leftRimNormal = sf::Vector2f(leftAngleInRadianX , leftAngleInRadianY);
	sf::Vector2f rightRimNormal =  sf::Vector2f(rightAngleInRadianX , rightAngleInRadianY);
	//player collision ball bounce vector calculation based on where on the player it lands
	//1) where on the player did we land? (percentage areas)
	if (type == CollisionType::VerticalCollision) {
		float proportionalXPos = futureBallPosition.x - (m_PlayerPosition.x - (m_PlayerDimensions.x /2.0f));
		float percentageOnXAxis =proportionalXPos/m_PlayerDimensions.x;
		if (percentageOnXAxis < m_PlayerRimPercentage) {
			//cos-1 [ (a · b) / (|a| |b|) ]
			
			float ballDirTimesNormal = (ball.ballDirection.x * leftRimNormal.x) + (ball.ballDirection.y * leftRimNormal.y);
			float absoluteBallDir = sqrt((ball.ballDirection.x * ball.ballDirection.x) + (ball.ballDirection.y * ball.ballDirection.y));
			float absoluteNormal = sqrt((leftRimNormal.x * leftRimNormal.x) + (leftRimNormal.y * leftRimNormal.y));
			float angleBetweenBallDirectionAndNormal = acos(ballDirTimesNormal/(absoluteBallDir * absoluteNormal)) / (M_PI / 180.0f);
			float newVectorAngle = angleBetweenBallDirectionAndNormal * -2.0f;
			
			newBallDirectionVector = sf::Vector2f(ball.ballDirection.x * cos((newVectorAngle) * (M_PI / 180.0f)) - ball.ballDirection.y * sin(newVectorAngle * (M_PI / 180.0f)), ball.ballDirection.x * sin(newVectorAngle * (M_PI / 180.0f)) + ball.ballDirection.y * cos(newVectorAngle * (M_PI / 180.0f)));
			std::cout<< "old: " << angleBetweenBallDirectionAndNormal <<", new: " << newVectorAngle << std::endl;
		}
		if (percentageOnXAxis > m_PlayerRimPercentage * 2.0f) {

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
	}

	//2) calculate the normal for the player area (make it customizable based on initial deviation degree for normal)
	//3) calculate the angle between the normal and the ball direction vector
	//4) give new direction vector based on the angle 

	return newBallDirectionVector;
}

bool CornerCollisionCheck(sf::Vector2f circleDistance, Ball& ball) {
	if (circleDistance.y > (m_PlayerDimensions.y / 2 + ball.ballRadius)) { return false; }
	if (circleDistance.x > (m_PlayerDimensions.x / 2 + ball.ballRadius)) { return false; }
	
	float cornerDistance_sq = ((circleDistance.x - m_PlayerDimensions.x / 2) * (circleDistance.x - m_PlayerDimensions.x / 2)) +
		((circleDistance.y - m_PlayerDimensions.y / 2) * (circleDistance.y - m_PlayerDimensions.y / 2));

	if ((cornerDistance_sq <= (ball.ballRadius * ball.ballRadius))) { return true; }
	return false;
}
bool VerticalCollisionCheck(sf::Vector2f circleDistance, Ball& ball) {
	if (circleDistance.y > (m_PlayerDimensions.y / 2 + ball.ballRadius)) { return false; }
	if (circleDistance.x > (m_PlayerDimensions.x / 2 + ball.ballRadius)) { return false;}
	/*if (circleDistance.x < (m_PlayerDimensions.x / 2 + m_BallDiametre)) {*/
		if (circleDistance.y <= ((m_PlayerDimensions.y / 2) + ball.ballRadius)) { return true; }
	/*}*/
	
	return false;
	
}

bool HorizontalCollisionCheck(sf::Vector2f circleDistance, Ball& ball) {
	if (circleDistance.y > (m_PlayerDimensions.y / 2 + ball.ballRadius)) { return false; }
	if (circleDistance.x > (m_PlayerDimensions.x / 2 + ball.ballRadius)) { return false; }

	/*if (circleDistance.y < (m_PlayerDimensions.y / 2 + m_BallDiametre))
	{*/
		if (circleDistance.x <= (m_PlayerDimensions.x / 2) + ball.ballRadius) { return true;}
	/*}*/
	
	return false;
	
}

bool CheckForBallTileCollisionAndMovementChanges(Ball& ball, sf::RectangleShape* tileShapes,  Tile** gametiles, sf::Vector2f* bounceDirection) {


	bool playerCollideVertical = false;
	bool playerCollideHorizontal = false;


	sf::Vector2f futureBallPosition = (ball.ballPosition + sf::Vector2f(ball.ballRadius, ball.ballRadius)) + ball.ballDirection;

	

	for (int i = 0; i < tileArrayLength; i++) {
		bool alive = gametiles[i]->isAlive;
		if (!alive) {
			continue;
		}
		sf::Vector2f currentTilePosition = tileShapes[i].getPosition();
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


		ball.ballVisual->setFillColor(sf::Color::Red);
		if (playerCollideVertical) {
			ball.ballVisual->setFillColor(sf::Color::Yellow);
			*bounceDirection = sf::Vector2f(ball.ballDirection.x, ball.ballDirection.y* -1.0f);
			gametiles[i]->hitCount -= 1;
			int currentHitCount = gametiles[i]->hitCount;
			if (currentHitCount <= 0) {
				gametiles[i]->isAlive = false;
				DropTileEffect(*gametiles[i], &tileShapes[i]);
			}
			return true;
		}

		if (playerCollideHorizontal) {
			ball.ballVisual->setFillColor(sf::Color::Magenta);
			*bounceDirection = sf::Vector2f(ball.ballDirection.x * -1.0f, ball.ballDirection.y);
			gametiles[i]->hitCount -= 1;
			int currentHitCount = gametiles[i]->hitCount;
			if (currentHitCount <= 0) {
				gametiles[i]->isAlive = false;
				DropTileEffect(*gametiles[i], &tileShapes[i]);
			}
			return true;
		}
	}

	return false;
	
}

bool DropTileEffect(Tile& tile, sf::RectangleShape* drawnTile) {
	sf::RectangleShape effectRect = sf::RectangleShape(sf::Vector2f(10.0f, 10.0f));
	effectRect.setSize(sf::Vector2f(10.0f, 10.0f));
	effectRect.setPosition(drawnTile->getPosition());
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

float lerp(float a, float b, float f)
{
	return a + f * (b - a);
}