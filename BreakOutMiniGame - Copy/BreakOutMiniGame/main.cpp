#include <SFML/Graphics.hpp>
#include <iostream>
#include "Tile.h"
#include <math.h>
#include "Main_Fuctions.h"
#include <algorithm>
#include <chrono>



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

const float scalingFactor = 3.0f;
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
const sf::Vector2f	m_PlayerDimensions		= sf::Vector2f(25.0f, 10.0f) * scalingFactor;
//sf::Vector2f		m_PlayerPosition		= sf::Vector2f((WINDOW_WIDTH / 2.0f) - (m_PlayerDimensions.x / 2.0f), WINDOW_HEIGHT * 0.66f);
sf::Vector2f m_PlayerPosition				= sf::Vector2f(146.345f, 396.0f);
sf::Vector2f		m_PlayerPositionChanges = sf::Vector2f(0.0f, 0.0f);
float				m_PMovementIncrements	= 0.05f;

//Ball Variables
float m_BallRadius = 2.0f * scalingFactor;
sf::Vector2f m_BallPosition; 
sf::Vector2f m_BallDirection;
float m_BallAngle = 315.0f;
float m_BallSpeed = 0.02f;
float m_RepellantDistance = m_BallRadius/2.0f;

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
		sf::Vector2<float> position = sf::Vector2<float>((gametiles[i]->position.x * (WINDOW_WIDTH/WINDOW_SEGMENTS_WIDTH)) + outlineThickness, gametiles[i]->position.y * (WINDOW_HEIGHT/WINDOW_SEGMENTS_HEIGHT)/3.0f);
		sf::RectangleShape rect(size);
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
	sf::CircleShape ball = sf::CircleShape(m_BallRadius);
	m_BallPosition = m_BallStarterPosition_UNALTERED;
	ball.setFillColor(sf::Color::Red);
	ball.setPosition(m_BallPosition);
	m_BallDirection = sf::Vector2f(std::cos(m_BallAngle) * m_BallSpeed, std::sin(m_BallAngle) * m_BallSpeed);


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
			m_CurrentInputToWorkWith = CheckForInput();
		}

		if (m_CalculateOneFrame) {
			DoGameLoopCalculations(ball, tileShapes, gametiles);
			std::cout << "Calculate one frame" << std::endl;
			m_CalculateOneFrame = false;
		}

		if (m_PauseGame) {
			MoveBallThroughInput(ball);
			RenderGameData(window, ball, tileShapes, gametiles);
			
			continue;
		}

		
		DoGameLoopCalculations(ball, tileShapes, gametiles);
		RenderGameData(window, ball, tileShapes, gametiles);
	}

	return 0;
}


bool MoveBallThroughInput(sf::CircleShape& ball) {

	bool inputGiven = false;

	sf::Vector2f oldDirection = m_BallDirection;
	if (moveUp) {m_BallDirection = sf::Vector2f(0.0f, -1.0f);		moveUp    = false; inputGiven = true; }
	if (moveDown) { m_BallDirection = sf::Vector2f(0.0f, 1.0f);		moveDown  = false; inputGiven = true;}
	if (moveLeft) { m_BallDirection = sf::Vector2f(-1.0f, 0.0f);	moveLeft  = false; inputGiven = true;}
	if (moveRight) { m_BallDirection = sf::Vector2f(1.0f, 0.0f);	moveRight = false; inputGiven = true;}

	if (inputGiven) {
		m_BallPosition = m_BallPosition + (m_BallDirection * 2.0f);
		ball.setPosition(m_BallPosition);
	}
	
	m_BallDirection = oldDirection;
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

bool DoGameLoopCalculations(sf::CircleShape& ball, sf::RectangleShape tileShapes[], Tile* gametiles[]) {
	//Update Player Position

	if (m_CurrentInputToWorkWith) {
		m_PlayerPosition = sf::Vector2f(clip((m_PlayerPositionChanges.x + m_PlayerPosition.x), 0.0f, WINDOW_WIDTH - m_PlayerDimensions.x), m_PlayerPosition.y);
		m_PlayerPositionChanges = sf::Vector2f(0.0f, 0.0f);
		m_CurrentInputToWorkWith = false;
	}
	//Update Ball 
	//0) check if ball is colliding with walls
	float wallDistanceXMin = m_BallPosition.x;
	float wallDistanceXMax = std::abs(WINDOW_WIDTH - m_BallPosition.x);

	if (wallDistanceXMin < m_RepellantDistance || wallDistanceXMax < m_RepellantDistance) {
		std::cout << "Wall Collide!" << std::endl;
		m_BallDirection = sf::Vector2f(m_BallDirection.x * -1.0f, m_BallDirection.y);
	}

	float wallDistanceYMin = m_BallPosition.y;
	float wallDistanceYMax = std::abs(WINDOW_HEIGHT - m_BallPosition.y);

	if (wallDistanceYMin < m_RepellantDistance || wallDistanceYMax < m_RepellantDistance) {
		std::cout << "Wall Collide!" << std::endl;
		m_BallDirection = sf::Vector2f(m_BallDirection.x, m_BallDirection.y* -1.0f);

	}

	sf::Vector2f playerCollisionBounceDirection;
	sf::Vector2f tileCollisionBounceDirection;
	bool collisionWithPlayer = CheckForCollisionWithPlayer(m_PlayerPosition, &playerCollisionBounceDirection, ball);
	bool collisionWithTile = CheckForBallTileCollisionAndMovementChanges(tileShapes, gametiles);

	//3) move ball
	if (collisionWithPlayer) {
		m_BallDirection = playerCollisionBounceDirection;
	}

	
	m_BallPosition = m_BallPosition + m_BallDirection;
	ball.setPosition(m_BallPosition);

	return true;
}

bool RenderGameData(sf::RenderWindow& window, sf::CircleShape& ball, sf::RectangleShape tileShapes[], Tile* gametiles[]) {

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
	window.draw(ball);

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
		std::cout << tile->position.x << ":x, " << tile->position.y << ":y" << std::endl;
	}
	return true;
}

sf::Vector2f Get2DPositionWithIndex(int index) {
	sf::Vector2f position;

	position.x = index % (WINDOW_SEGMENTS_WIDTH);
	position.y = std::floor((float)index/(WINDOW_SEGMENTS_WIDTH));
	return position;
}

bool CheckForInput() {

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
		
		std::cout <<"Ball Pos:" << m_BallPosition.x <<" ," << m_BallPosition.y <<std::endl;
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


bool CheckForCollisionWithPlayer(sf::Vector2f playerTilePos, sf::Vector2f* bounceDirection, sf::CircleShape& ball)
{
	
	bool playerCollideVertical = false;
	bool playerCollideHorizontal = false;

	
	sf::Vector2f futureBallPosition = m_BallPosition + m_BallDirection;

	float testingX = futureBallPosition.x;
	float testingY = futureBallPosition.y;

	float minPlayerX = playerTilePos.x - (m_PlayerDimensions.x /2.0f);
	float maxPlayerX = playerTilePos.x + (m_PlayerDimensions.x /2.0f);
	float minPlayerY = playerTilePos.y - (m_PlayerDimensions.y / 2.0f);
	float maxPlayerY = playerTilePos.y + (m_PlayerDimensions.y / 2.0f);
	
	bool leftX = false;
	bool rightX = false;
	bool upY = false;
	bool downY = false;

	if(futureBallPosition.x < minPlayerX){ testingX = minPlayerX; leftX = true;}
	else if(futureBallPosition.x > maxPlayerX){ testingX = maxPlayerX; rightX = true;}
	
	if (futureBallPosition.y < minPlayerY) { testingY = minPlayerY; upY = true;}
	else if (futureBallPosition.y > maxPlayerY) { testingY = maxPlayerY; downY = true;}
	
	float distanceX = futureBallPosition.x - testingX;
	float distanceY = futureBallPosition.y - testingY;
	float overallDistance = sqrt((distanceX * distanceX)+ (distanceY* distanceY));

	if (overallDistance <= m_BallRadius) {

		//test for which quadrant the ball is in 

		if (leftX || rightX) {
			playerCollideHorizontal = true;
		}

		if (upY || downY) {
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

	//sf::Vector2f circleDistance;
	//circleDistance.x = abs(futureBallPosition.x - (playerTilePos.x - m_BallDiametre));
	//circleDistance.y = abs(futureBallPosition.y - (playerTilePos.y - m_BallDiametre));

	//bool circleWithinVerticalArea = (circleDistance.y < ((m_PlayerDimensions.y / 2) + m_BallDiametre));
	////check if the ball is within the area of (playerdimensions/2 + the ball diametre)
	//if (circleWithinVerticalArea && (circleDistance.x < ((m_PlayerDimensions.x / 2) + m_BallDiametre))) {
	//	std::cout << "Within area!" << std::endl;
	//	bool verticalIsClosest = (circleDistance.y < circleDistance.x);
	//	if (verticalIsClosest) {
	//		playerCollideVertical = true;
	//	}
	//	else {
	//		playerCollideHorizontal = true;
	//	}
	//}
	//
	///*playerCollideVertical = VerticalCollisionCheck(circleDistance);
	//playerCollideHorizontal = HorizontalCollisionCheck(circleDistance);*/

	//if (CornerCollisionCheck(circleDistance)) {
	//	*bounceDirection = sf::Vector2f(m_BallDirection.x* -1.0f, m_BallDirection.y* -1.0f);
	//	return true;
	//}

	/*if (playerCollideHorizontal && playerCollideVertical) {

		bool verticalIsClosest = (circleDistance.y < circleDistance.x);
		if (verticalIsClosest) {
			playerCollideHorizontal = false;
		}
		else {
			playerCollideVertical = false;
		}
	}
	*/
	ball.setFillColor(sf::Color::Red);
	if (playerCollideVertical) {
		ball.setFillColor(sf::Color::Yellow);
		*bounceDirection = sf::Vector2f(m_BallDirection.x, m_BallDirection.y* -1.0f);
		return true;
	}

	if (playerCollideHorizontal) {
		ball.setFillColor(sf::Color::Magenta);
		*bounceDirection = sf::Vector2f(m_BallDirection.x * -1.0f, m_BallDirection.y);
		return true;
	}

	
	return false;
}

bool CornerCollisionCheck(sf::Vector2f circleDistance) {
	if (circleDistance.y > (m_PlayerDimensions.y / 2 + m_BallRadius)) { return false; }
	if (circleDistance.x > (m_PlayerDimensions.x / 2 + m_BallRadius)) { return false; }
	
	float cornerDistance_sq = ((circleDistance.x - m_PlayerDimensions.x / 2) * (circleDistance.x - m_PlayerDimensions.x / 2)) +
		((circleDistance.y - m_PlayerDimensions.y / 2) * (circleDistance.y - m_PlayerDimensions.y / 2));

	if ((cornerDistance_sq <= (m_BallRadius * m_BallRadius))) { return true; }
	return false;
}
bool VerticalCollisionCheck(sf::Vector2f circleDistance) {
	if (circleDistance.y > (m_PlayerDimensions.y / 2 + m_BallRadius)) { return false; }
	if (circleDistance.x > (m_PlayerDimensions.x / 2 + m_BallRadius)) { return false;}
	/*if (circleDistance.x < (m_PlayerDimensions.x / 2 + m_BallDiametre)) {*/
		if (circleDistance.y <= ((m_PlayerDimensions.y / 2) + m_BallRadius)) { return true; }
	/*}*/
	
	return false;
	
}

bool HorizontalCollisionCheck(sf::Vector2f circleDistance) {
	if (circleDistance.y > (m_PlayerDimensions.y / 2 + m_BallRadius)) { return false; }
	if (circleDistance.x > (m_PlayerDimensions.x / 2 + m_BallRadius)) { return false; }

	/*if (circleDistance.y < (m_PlayerDimensions.y / 2 + m_BallDiametre))
	{*/
		if (circleDistance.x <= (m_PlayerDimensions.x / 2) + m_BallRadius) { return true;}
	/*}*/
	
	return false;
	
}

bool CheckForBallTileCollisionAndMovementChanges(sf::RectangleShape* tileShapes,  Tile** gametiles) {
	if (m_BlockMovementChanges) {

		if (m_BM_Timer < m_BlockedMovementFrameTime) {
			m_BM_Timer++;

			return false;
		}
		if (m_BM_Timer > m_BlockedMovementFrameTime ) {
			m_BlockMovementChanges = false;
			m_BM_Timer = 0;
		}
	}
	


	for (int i = 0; i < tileArrayLength; i++) {

		//check for every tile if the ball is colliding with one of its walls 

		bool horizontalCollision = false;

		bool verticalCollision = false;

		bool alive = gametiles[i]->isAlive;
		if (!alive) {
			continue;
		}
		sf::Vector2f currentTilePosition = tileShapes[i].getPosition();

		//downcollide = if the ball is under the tile, as well as within the width position of the tile, as well as within a certain distance to the tile

		if (m_BallPosition.x < (currentTilePosition.x + m_TileDimensions.x)
			&& m_BallPosition.x >(currentTilePosition.x - m_TileDimensions.x)) {
			float distanceOnYAxis_Bottom = (currentTilePosition.y +  m_TileDimensions.y) - m_BallPosition.y;
			float distanceOnYAxis_Top = (currentTilePosition.y - m_TileDimensions.y) - m_BallPosition.y;

			// i think i need bounce rules for the top and the bottom separately
			if ((std::abs(distanceOnYAxis_Bottom) < m_RepellantDistance | std::abs(distanceOnYAxis_Top) < m_RepellantDistance) && (distanceOnYAxis_Bottom > 0.0f | distanceOnYAxis_Top < 0.0f)) {
				/*std::cout << distanceOnYAxis_Bottom << ": is distance on bottom" << std::endl;
				std::cout << distanceOnYAxis_Top << ": is distance on top" << std::endl;*/

				gametiles[i]->hitCount += 1;
				int currentHitCount = gametiles[i]->hitCount;
				if (currentHitCount >= m_TileHitsAllowed) {
					gametiles[i]->isAlive = false;
				}
				
				verticalCollision = true;
			}


		}

		if (m_BallPosition.y < (currentTilePosition.y + m_TileDimensions.y)
			&& m_BallPosition.x >(currentTilePosition.y - m_TileDimensions.y)) {
			float distanceOnXAxis_Right = (currentTilePosition.y + m_TileDimensions.x) - m_BallPosition.x;
			float distanceOnXAxis_Left = (currentTilePosition.y - m_TileDimensions.x) - m_BallPosition.x;


			float playerRepellantDistance = 0.5f;
			// i think i need bounce rules for the top and the bottom separately
			if ((std::abs(distanceOnXAxis_Right) < m_RepellantDistance | std::abs(distanceOnXAxis_Left) < m_RepellantDistance) && (distanceOnXAxis_Right > 0.0f | distanceOnXAxis_Left < 0.0f)) {
				/*std::cout << distanceOnXAxis_Right << ": is distance on bottom" << std::endl;
				std::cout << distanceOnXAxis_Left << ": is distance on top" << std::endl;*/
				gametiles[i]->isAlive = false;
				horizontalCollision = true;
			}


		}

		if (horizontalCollision) {
			m_BallDirection = sf::Vector2f(m_BallDirection.x * -1.0f, m_BallDirection.y);
			m_BlockMovementChanges = true;
			return true;
		}


		if (verticalCollision) {
			m_BallDirection =  sf::Vector2f(m_BallDirection.x, m_BallDirection.y* -1.0f);
			m_BlockMovementChanges = true;
			return true;
		}
		
	}
	return true;
}

float lerp(float a, float b, float f)
{
	return a + f * (b - a);
}