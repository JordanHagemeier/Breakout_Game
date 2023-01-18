#include "TileManager.h"
#include "ParticleEffectManager.h"
#include <cassert>

sf::Vector2f TileManager::TILE_DIMENSIONS	= sf::Vector2f(0.0f, 0.0f);
int TileManager::TILE_ARRAY_LENGTH			= 0;
const float TileManager::OUTLINE_THICKNESS	= 2.5f;



bool TileManager::InitializeTileVector() {

	
	ManagerInterface* ptrToRenderManager = GameManager::GetManagerByType(ManagerType::renderManager_T);
	if (ptrToRenderManager == nullptr) {
		std::cout << "Manager Pointer is null" << std::endl;
		return false;
	}
	RenderManager& renderManager = static_cast<RenderManager&>(*ptrToRenderManager);
	

	assert(renderManager.HasFinishedInitialization(), "Render Manager not yet fully initialized! See initialization process & order.");

	m_Tiles.reserve(TILE_ARRAY_LENGTH);

	const sf::Vector2<float>tileSize = TILE_DIMENSIONS - sf::Vector2f(2.0f * OUTLINE_THICKNESS, 2.0f * OUTLINE_THICKNESS);
	for (int i = 0; i < TILE_ARRAY_LENGTH; i++) {


		//R USE THIS TO NOT LEAK MEMEMORY
		//C for an interview: This should probably be a unique_pointer that is passed with std::move() to the renderManager, which then holds the ptr.
		std::shared_ptr<sf::RectangleShape>rect_shared = std::make_shared<sf::RectangleShape>(tileSize);

		//set tile position
		const sf::Vector2<float> position = GetTilePosition(i, renderManager);
		rect_shared->setPosition(position);

		//set origin
		rect_shared->setOrigin(TILE_DIMENSIONS.x / 2.0f, TILE_DIMENSIONS.y / 2.0f);

		//Get Tiletype for new tile, then set colors according to type
		const TileType newType = CreateTileType();
		rect_shared->setFillColor(GameManager::GetTileColorByType(newType));
		rect_shared->setOutlineColor(sf::Color::White);
		rect_shared->setOutlineThickness(OUTLINE_THICKNESS);

		//Add tile visual to rendermanager
		//Create tile data for collisions and calculations
		const int visID = renderManager.AddShape(rect_shared);
		m_Tiles.emplace_back(position, newType, GameManager::GetTileColorByType(newType), visID, i);

	

	}
	return true;
}

sf::Vector2<float> TileManager::GetTilePosition(int i, RenderManager& renderManager) {

	const sf::Vector2f index2DPosition = MathHelper::Get2DPositionWithIndex(i, renderManager.windowWidthInSegments());

	const sf::Vector2<float> position = sf::Vector2<float>((index2DPosition.x * renderManager.windowPercentagePerWidthSegment()) + OUTLINE_THICKNESS + (TILE_DIMENSIONS.x / 2.0f),
													 (index2DPosition.y * renderManager.windowPercentagePerHeightSegment()) * coveredWindowPercentage + OUTLINE_THICKNESS + (TILE_DIMENSIONS.y / 2.0f));

	return position;
}

TileType TileManager::CreateTileType() {

	//get tile type by chance
	std::random_device randomDevice; 
	std::mt19937 generator(randomDevice());
	std::uniform_int_distribution<> distribution(0, 100); 


	//what sort of tile is this? -> gets randomly generated for each tile per game
	TileType newType = TileType::TileTCount;

	const float randomNumber = distribution(generator) / 100.0f;
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
	
	if (tileID <= 0 || tileID > TILE_ARRAY_LENGTH) {
		std::cout<<"Tile ID out of bounds" << std::endl;
		return;
	}

	m_Tiles[tileID].hitCount -= 1;
	if (m_Tiles[tileID].hitCount <= 0) {
		m_Tiles[tileID].isAlive = false;

		//drop a small square for the player to catch, starting from the tile position
		NotifyBoosterDropManager(m_Tiles[tileID]);

		//notify particle effects manager to sprinkle in some particles
		NotifyParticleEffectManager(m_Tiles[tileID]);
	}
	
}



void TileManager::NotifyBoosterDropManager(Tile& tile) {
	ManagerInterface* ptrToBoosterDropManager = GameManager::GetManagerByType(ManagerType::boosterDropManager_T);
	if (ptrToBoosterDropManager == nullptr) {
		std::cout << "Manager Pointer is null" << std::endl;
		return;
	}
	BoosterDropManager& boosterDropManager = static_cast<BoosterDropManager&>(*ptrToBoosterDropManager);
	boosterDropManager.ActivateTileEffect(tile.m_ObjectID);

}


void TileManager::NotifyParticleEffectManager(Tile& tile) {
	
	ManagerInterface* ptrToParticleManager = GameManager::GetManagerByType(ManagerType::particleManager_T);
	if (ptrToParticleManager == nullptr) {
		std::cout<<"Particle Manager Pointer is null" << std::endl;
		return;
	}
	ParticleEffectManager& particleManager = static_cast<ParticleEffectManager&>(*ptrToParticleManager);
	assert(particleManager.HasFinishedInitialization(), "Render Manager not yet fully initialized! See initialization process & order.");
	particleManager.AddParticleEventAtPosition(tile.position);
}

void TileManager::TerminateManager() {

	m_Tiles.clear();
	m_Tiles.shrink_to_fit();

}