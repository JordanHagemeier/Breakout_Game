#include <SFML/Graphics.hpp>
#include <iostream>
#include "Tile.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include "Main_Fuctions.h"
#include <algorithm>
#include <chrono>
#include "Ball.h"



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
sf::Vector2f m_PlayerPosition				= sf::Vector2f(198.798f, 396.0f);
sf::Vector2f		m_PlayerPositionChanges = sf::Vector2f(0.0f, 0.0f);
float				m_PMovementIncrements	= 0.09f;

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


int main()
{

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
		rect.setFillColor(NO_HIT_COLOR);
		rect.setOutlineColor(sf::Color::White);
		rect.setOutlineThickness(outlineThickness);
		rect.setPosition(position);
		tileShapes[i] = rect;
		
		
	}

	//Create player tile
	m_PlayerTile = new Tile();
	m_PlayerTile->position = m_PlayerPosition;
	
	//create ball
	sf::CircleShape ball_Visual;
	Ball ball = Ball(2.0f * scalingFactor, m_BallStarterPosition_UNALTERED, 315.0f, 0.05f, scalingFactor, ball_Visual);
	ball_Visual.setRadius(ball.m_BallRadius);
	ball.m_BallPosition = m_BallStarterPosition_UNALTERED;
	ball_Visual.setFillColor(sf::Color::Red);
	ball_Visual.setPosition(ball.m_BallPosition);
	ball.m_BallDirection = sf::Vector2f(std::cos(ball.m_BallAngle) * ball.m_BallSpeed, std::sin(ball.m_BallAngle) * ball.m_BallSpeed);


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
			m_CurrentInputToWorkWith = CheckForInput(ball);
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

	sf::Vector2f oldDirection = ball.m_BallDirection;
	if (moveUp)		{ball.m_BallDirection = sf::Vector2f(0.0f, -1.0f);		moveUp    = false; inputGiven = true; }
	if (moveDown)	{ball.m_BallDirection = sf::Vector2f(0.0f, 1.0f);		moveDown  = false; inputGiven = true;}
	if (moveLeft)	{ball.m_BallDirection = sf::Vector2f(-1.0f, 0.0f);	moveLeft  = false; inputGiven = true;}
	if (moveRight)	{ball.m_BallDirection = sf::Vector2f(1.0f, 0.0f);	moveRight = false; inputGiven = true;}

	if (inputGiven) {
		ball.m_BallPosition = ball.m_BallPosition + (ball.m_BallDirection * 2.0f);
		ball.ball_Visual->setPosition(ball.m_BallPosition);
	}
	
	ball.m_BallDirection = oldDirection;
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
	float wallDistanceXMin = ball.m_BallPosition.x;
	float wallDistanceXMax = std::abs(WINDOW_WIDTH - ball.m_BallPosition.x);

	if (wallDistanceXMin < ball.m_RepellantDistance || wallDistanceXMax < ball.m_RepellantDistance) {
		std::cout << "Wall Collide!" << std::endl;
		ball.m_BallDirection = sf::Vector2f(ball.m_BallDirection.x * -1.0f, ball.m_BallDirection.y);
	}

	float wallDistanceYMin = ball.m_BallPosition.y;
	float wallDistanceYMax = std::abs(WINDOW_HEIGHT - ball.m_BallPosition.y);

	if (wallDistanceYMin < ball.m_RepellantDistance || wallDistanceYMax < ball.m_RepellantDistance) {
		std::cout << "Wall Collide!" << std::endl;
		ball.m_BallDirection = sf::Vector2f(ball.m_BallDirection.x, ball.m_BallDirection.y* -1.0f);

	}

	sf::Vector2f originalBallDirection = ball.m_BallDirection;
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
			ball.m_BallDirection = playerCollisionBounceDirection;
		}

		if (collisionWithTile) {
			ball.m_BallDirection = tileCollisionBounceDirection;
		}
	/*}*/
	

	
	ball.m_BallPosition = ball.m_BallPosition + ball.m_BallDirection;
	ball.ball_Visual->setPosition(ball.m_BallPosition);

	return true;
}

bool RenderGameData(sf::RenderWindow& window, Ball& ball, sf::RectangleShape tileShapes[], Tile* gametiles[]) {

	//Update Tiles 

	window.clear();


	//draw tiles
	for (int j = 0; j < tileArrayLength; j++) {
		sf::RectangleShape currentTile = tileShapes[j];
		float currentHitPercentage = (float)gametiles[j]->hitCount / m_TileHitsAllowed;
		sf::Uint8 redPercentage = lerp(m_NoHitColor.r, m_FinalHitColor.r, currentHitPercentage);
		sf::Uint8 greenPercentage = lerp(m_NoHitColor.g, m_FinalHitColor.g, currentHitPercentage);
		sf::Uint8 bluePercentage = lerp(m_NoHitColor.b, m_FinalHitColor.b, currentHitPercentage);
		sf::Uint8 alphaPercentage = 255;

		sf::Color color = sf::Color(redPercentage, greenPercentage, bluePercentage, alphaPercentage);
		currentTile.setFillColor(color);
		if (gametiles[j]->isAlive) {
			window.draw(currentTile);
		}
	}

	//draw player tile
	sf::RectangleShape playerTile = sf::RectangleShape(m_PlayerDimensions);
	playerTile.setOrigin(sf::Vector2f(m_PlayerDimensions.x/2.0f, m_PlayerDimensions.y/2.0f));
	sf::Vector2f drawingPos = sf::Vector2f(m_PlayerPosition.x , m_PlayerPosition.y );
	playerTile.setPosition(drawingPos);
	playerTile.setFillColor(sf::Color::Green);
	playerTile.setOutlineColor(sf::Color::Yellow);
	window.draw(playerTile);


	//draw ball
	window.draw(*ball.ball_Visual);

	window.display();

	return true;
}
bool FillTileArrayWithData(Tile* tiles[], int tileArrayLength) {
	for (int i = 0; i < tileArrayLength; i++) {
		Tile* tile = new Tile;
		sf::Vector2f newPos = Get2DPositionWithIndex(i);
		tile->position = newPos;
		tiles[i] = tile;
		/*if (i % 2 == 0) {
			tile->isAlive = false;
		}*/
		
	}
	return true;
}

sf::Vector2f Get2DPositionWithIndex(int index) {
	sf::Vector2f position;

	position.x = index % (WINDOW_SEGMENTS_WIDTH);
	position.y = std::floor((float)index/(WINDOW_SEGMENTS_WIDTH));
	return position;
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
		
		std::cout <<"Ball Pos:" << ball.m_BallPosition.x <<" ," << ball.m_BallPosition.y <<std::endl;
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
	futureBallPosition = (ball.m_BallPosition + sf::Vector2f(ball.m_BallRadius, ball.m_BallRadius)) + ball.m_BallDirection;
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

	if (overallDistance <= ball.m_BallRadius) {
	
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

	
	ball.ball_Visual->setFillColor(sf::Color::Red);

	if (cornerWasHit) {
		*bounceDirection = sf::Vector2f(ball.m_BallDirection.x * -1.0f, ball.m_BallDirection.y* -1.0f);
		return true;
	}
	if (playerCollideVertical) {
		ball.ball_Visual->setFillColor(sf::Color::Yellow);
		/**bounceDirection = sf::Vector2f(m_BallDirection.x, m_BallDirection.y* -1.0f);*/
		*bounceDirection = CalculateBounceVector(futureBallPosition, CollisionType::VerticalCollision, ball);
		return true;
	}

	if (playerCollideHorizontal) {
		ball.ball_Visual->setFillColor(sf::Color::Magenta);
		*bounceDirection = sf::Vector2f(ball.m_BallDirection.x * -1.0f, ball.m_BallDirection.y);
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

	float playerRimPercentage = 0.33f;
	sf::Vector2f leftRimNormal = sf::Vector2f(leftAngleInRadianX , leftAngleInRadianY);
	sf::Vector2f rightRimNormal =  sf::Vector2f(rightAngleInRadianX , rightAngleInRadianY);
	//player collision ball bounce vector calculation based on where on the player it lands
	//1) where on the player did we land? (percentage areas)
	if (type == CollisionType::VerticalCollision) {
		float proportionalXPos = futureBallPosition.x - (m_PlayerPosition.x - (m_PlayerDimensions.x /2.0f));
		float percentageOnXAxis =proportionalXPos/m_PlayerDimensions.x;
		if (percentageOnXAxis < playerRimPercentage) {
			//cos-1 [ (a · b) / (|a| |b|) ]
			
			float ballDirTimesNormal = (ball.m_BallDirection.x * leftRimNormal.x) + (ball.m_BallDirection.y * leftRimNormal.y);
			float absoluteBallDir = sqrt((ball.m_BallDirection.x * ball.m_BallDirection.x) + (ball.m_BallDirection.y * ball.m_BallDirection.y));
			float absoluteNormal = sqrt((leftRimNormal.x * leftRimNormal.x) + (leftRimNormal.y * leftRimNormal.y));
			float angleBetweenBallDirectionAndNormal = acos(ballDirTimesNormal/(absoluteBallDir * absoluteNormal)) / (M_PI / 180.0f);
			float newVectorAngle = angleBetweenBallDirectionAndNormal * -2.0f;
			
			newBallDirectionVector = sf::Vector2f(ball.m_BallDirection.x * cos((newVectorAngle) * (M_PI / 180.0f)) - ball.m_BallDirection.y * sin(newVectorAngle * (M_PI / 180.0f)), ball.m_BallDirection.x * sin(newVectorAngle * (M_PI / 180.0f)) + ball.m_BallDirection.y * cos(newVectorAngle * (M_PI / 180.0f)));
			std::cout<< "old: " << angleBetweenBallDirectionAndNormal <<", new: " << newVectorAngle << std::endl;
		}
		if (percentageOnXAxis > playerRimPercentage * 2.0f) {

			float ballDirTimesNormal = (ball.m_BallDirection.x * rightRimNormal.x) + (ball.m_BallDirection.y * rightRimNormal.y);
			float absoluteBallDir = sqrt((ball.m_BallDirection.x * ball.m_BallDirection.x) + (ball.m_BallDirection.y * ball.m_BallDirection.y));
			float absoluteNormal = sqrt((rightRimNormal.x * rightRimNormal.x) + (rightRimNormal.y * rightRimNormal.y));
			float angleBetweenBallDirectionAndNormal = acos(ballDirTimesNormal / (absoluteBallDir * absoluteNormal)) / (M_PI / 180.0f);
			float newVectorAngle = angleBetweenBallDirectionAndNormal * -2.0f;

			newBallDirectionVector = sf::Vector2f(ball.m_BallDirection.x * cos((newVectorAngle) * (M_PI / 180.0f)) - ball.m_BallDirection.y * sin(newVectorAngle * (M_PI / 180.0f)), ball.m_BallDirection.x * sin(newVectorAngle * (M_PI / 180.0f)) + ball.m_BallDirection.y * cos(newVectorAngle * (M_PI / 180.0f)));
			std::cout<< "old: " << angleBetweenBallDirectionAndNormal <<", new: " << newVectorAngle << std::endl;
		}
		else {
			newBallDirectionVector = sf::Vector2f(ball.m_BallDirection.x, ball.m_BallDirection.y* -1.0f);
		}
	}

	//2) calculate the normal for the player area (make it customizable based on initial deviation degree for normal)
	//3) calculate the angle between the normal and the ball direction vector
	//4) give new direction vector based on the angle 

	return newBallDirectionVector;
}

bool CornerCollisionCheck(sf::Vector2f circleDistance, Ball& ball) {
	if (circleDistance.y > (m_PlayerDimensions.y / 2 + ball.m_BallRadius)) { return false; }
	if (circleDistance.x > (m_PlayerDimensions.x / 2 + ball.m_BallRadius)) { return false; }
	
	float cornerDistance_sq = ((circleDistance.x - m_PlayerDimensions.x / 2) * (circleDistance.x - m_PlayerDimensions.x / 2)) +
		((circleDistance.y - m_PlayerDimensions.y / 2) * (circleDistance.y - m_PlayerDimensions.y / 2));

	if ((cornerDistance_sq <= (ball.m_BallRadius * ball.m_BallRadius))) { return true; }
	return false;
}
bool VerticalCollisionCheck(sf::Vector2f circleDistance, Ball& ball) {
	if (circleDistance.y > (m_PlayerDimensions.y / 2 + ball.m_BallRadius)) { return false; }
	if (circleDistance.x > (m_PlayerDimensions.x / 2 + ball.m_BallRadius)) { return false;}
	/*if (circleDistance.x < (m_PlayerDimensions.x / 2 + m_BallDiametre)) {*/
		if (circleDistance.y <= ((m_PlayerDimensions.y / 2) + ball.m_BallRadius)) { return true; }
	/*}*/
	
	return false;
	
}

bool HorizontalCollisionCheck(sf::Vector2f circleDistance, Ball& ball) {
	if (circleDistance.y > (m_PlayerDimensions.y / 2 + ball.m_BallRadius)) { return false; }
	if (circleDistance.x > (m_PlayerDimensions.x / 2 + ball.m_BallRadius)) { return false; }

	/*if (circleDistance.y < (m_PlayerDimensions.y / 2 + m_BallDiametre))
	{*/
		if (circleDistance.x <= (m_PlayerDimensions.x / 2) + ball.m_BallRadius) { return true;}
	/*}*/
	
	return false;
	
}

bool CheckForBallTileCollisionAndMovementChanges(Ball& ball, sf::RectangleShape* tileShapes,  Tile** gametiles, sf::Vector2f* bounceDirection) {


	bool playerCollideVertical = false;
	bool playerCollideHorizontal = false;


	sf::Vector2f futureBallPosition = (ball.m_BallPosition + sf::Vector2f(ball.m_BallRadius, ball.m_BallRadius)) + ball.m_BallDirection;

	

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


		if (overallDistance <= ball.m_BallRadius) {

			//test for which section the ball is in 

			if (leftX || rightX) {
				playerCollideHorizontal = true;
			}

			if (upY || downY) {
				playerCollideVertical = true;
			}
			

		}


		ball.ball_Visual->setFillColor(sf::Color::Red);
		if (playerCollideVertical) {
			ball.ball_Visual->setFillColor(sf::Color::Yellow);
			*bounceDirection = sf::Vector2f(ball.m_BallDirection.x, ball.m_BallDirection.y* -1.0f);
			gametiles[i]->hitCount += 1;
			int currentHitCount = gametiles[i]->hitCount;
			if (currentHitCount >= m_TileHitsAllowed) {
				gametiles[i]->isAlive = false;
			}
			return true;
		}

		if (playerCollideHorizontal) {
			ball.ball_Visual->setFillColor(sf::Color::Magenta);
			*bounceDirection = sf::Vector2f(ball.m_BallDirection.x * -1.0f, ball.m_BallDirection.y);
			gametiles[i]->hitCount += 1;
			int currentHitCount = gametiles[i]->hitCount;
			if (currentHitCount >= m_TileHitsAllowed) {
				gametiles[i]->isAlive = false;
			}
			return true;
		}
	}

	return false;
	
}

float lerp(float a, float b, float f)
{
	return a + f * (b - a);
}