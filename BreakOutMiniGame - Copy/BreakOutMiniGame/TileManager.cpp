#include "TileManager.h"
#include "ParticleEffectManager.h"
#include <cassert>

sf::Vector2f TileManager::TILE_DIMENSIONS	= sf::Vector2f(0.0f, 0.0f);
int TileManager::TILE_ARRAY_LENGTH			= 0;
const float TileManager::OUTLINE_THICKNESS	= 2.5f;



bool TileManager::InitializeTileVector() {

	RenderManager& renderManager = static_cast<RenderManager&>(*GameManager::m_ManagerMap[ManagerType::renderManager_T]);
	assert(renderManager.HasFinishedInitialization(), "Render Manager not yet fully initialized! See initialization process & order.");

	sf::Vector2<float>tileSize = TILE_DIMENSIONS - sf::Vector2f(2.0f * OUTLINE_THICKNESS, 2.0f * OUTLINE_THICKNESS);
	for (int i = 0; i < TILE_ARRAY_LENGTH; i++) {


		sf::RectangleShape* rect						= new sf::RectangleShape(tileSize);
		std::shared_ptr<sf::RectangleShape>rect_shared	= std::make_shared<sf::RectangleShape>(*rect);

		//set tile position
		sf::Vector2<float> position = GetTilePosition(i, renderManager);
		rect_shared->setPosition(position);

		//set size and origin
		rect->setSize(tileSize);
		rect_shared->setOrigin(TILE_DIMENSIONS.x / 2.0f, TILE_DIMENSIONS.y / 2.0f);

		//Get Tiletype for new tile, then set colors according to type
		TileType newType = CreateTileType();
		rect_shared->setFillColor(GameManager::m_TileTypeToColorMap[newType]);
		rect_shared->setOutlineColor(sf::Color::White);
		rect_shared->setOutlineThickness(OUTLINE_THICKNESS);

		//Add tile visual to rendermanager
		//Create tile data for collisions and calculations
		int visID = renderManager.AddShape(rect_shared);
		Tile* newTile = new Tile(position, newType, GameManager::m_TileTypeToColorMap[newType], visID);
		m_Tiles.push_back(newTile);


	}
	return true;
}

sf::Vector2<float> TileManager::GetTilePosition(int i, RenderManager& renderManager) {

	sf::Vector2f index2DPosition = MathHelper::Get2DPositionWithIndex(i, (int)renderManager.m_SegmentCountPerWindowDimension.x);

	sf::Vector2<float> position = sf::Vector2<float>((index2DPosition.x * renderManager.m_WindowPercentageSegmentPerDimension.x) + OUTLINE_THICKNESS + (TILE_DIMENSIONS.x / 2.0f),
													 (index2DPosition.y * renderManager.m_WindowPercentageSegmentPerDimension.y) * coveredWindowPercentage + OUTLINE_THICKNESS + (TILE_DIMENSIONS.y / 2.0f));

	return position;
}

TileType TileManager::CreateTileType() {

	//get tile type by chance
	std::random_device rd; // obtain a random number from hardware
	std::mt19937 gen(rd()); // seed the generator
	std::uniform_int_distribution<> distr(0, 100); // define the range


	//what sort of tile is this? -> gets randomly generated for each tile per game
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
	return newType;
}


void TileManager::UpdateTileAfterCollision(int tileID) {
	Tile& currentTile = *m_Tiles[tileID];
	currentTile.hitCount -= 1;
	int currentHitCount = currentTile.hitCount;
	if (currentHitCount <= 0) {
		currentTile.isAlive = false;
		
		//drop a small square for the player to catch, starting from the tile position
		NotifyDroppingEffectManager(tileID);

		//notify particle effects manager to sprinkle in some particles
		NotifyParticleEffectManager(currentTile);
	}
}

void TileManager::NotifyDroppingEffectManager(int tileID) {
	DroppingEffectManager* droppingEffectsManager = static_cast<DroppingEffectManager*>(GameManager::m_ManagerMap[ManagerType::droppingEffectManager_T]);
	droppingEffectsManager->ActivateTileEffect(tileID);

}

void TileManager::NotifyParticleEffectManager(Tile& tile) {
	ManagerInterface* ptrToParticleManager = GameManager::GetManagerByType(ManagerType::particleManager_T);
	ParticleEffectManager& particleManager = static_cast<ParticleEffectManager&>(*ptrToParticleManager);
	assert(particleManager.HasFinishedInitialization(), "Render Manager not yet fully initialized! See initialization process & order.");
	particleManager.AddParticleEventAtPosition(tile.position);
}

void TileManager::TerminateManager() {

	m_Tiles.clear();
	
}