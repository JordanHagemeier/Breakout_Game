#include "TileManager.h"
#include "ParticleEffectManager.h"
#include <cassert>

sf::Vector2f TileManager::TILE_DIMENSIONS = sf::Vector2f(0.0f, 0.0f);
int TileManager::TILE_ARRAY_LENGTH = 0;


bool TileManager::InitializeTileVector() {


	
	//get tile type by chance
	std::random_device rd; // obtain a random number from hardware
	std::mt19937 gen(rd()); // seed the generator
	std::uniform_int_distribution<> distr(0, 100); // define the range



	for (int i = 0; i < TILE_ARRAY_LENGTH; i++) {

		RenderManager& renderManager = static_cast<RenderManager&>(*GameManager::m_ManagerMap[ManagerType::renderManager_T]);
		assert(renderManager.HasFinishedInitialization(), "Render Manager not yet fully initialized! See initialization process & order.");

		//two dimensional "array position" 
		sf::Vector2f index2DPosition = MathHelper::Get2DPositionWithIndex(i, (int)renderManager.m_WindowSegmentDimensions_px.x);
		sf::Vector2<float> position = sf::Vector2<float>((index2DPosition.x * (renderManager.m_WindowDimensions_px.x / renderManager.m_WindowSegmentDimensions_px.x)) + Tile::OUTLINE_THICKNESS + (TILE_DIMENSIONS.x / 2.0f),
			(index2DPosition.y * (renderManager.m_WindowDimensions_px.y / renderManager.m_WindowSegmentDimensions_px.y)) / 3.0f + (TILE_DIMENSIONS.y / 2.0f));

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
		sf::Vector2<float>size = TILE_DIMENSIONS - sf::Vector2f(2.0f * Tile::OUTLINE_THICKNESS, 2.0f * Tile::OUTLINE_THICKNESS);

		sf::RectangleShape* rect = new sf::RectangleShape(size);
		std::shared_ptr<sf::RectangleShape>rect_shared = std::make_shared<sf::RectangleShape>(*rect);
		rect->setSize(size);

		rect_shared->setOrigin(TILE_DIMENSIONS.x / 2.0f, TILE_DIMENSIONS.y / 2.0f);
		rect_shared->setFillColor(GameManager::m_TileTypeToColorMap[newType]);
		rect_shared->setOutlineColor(sf::Color::White);
		rect_shared->setOutlineThickness(Tile::OUTLINE_THICKNESS);
		rect_shared->setPosition(position);
		int visID = renderManager.AddShape(rect_shared);
		Tile* newTile = new Tile(position, newType, GameManager::m_TileTypeToColorMap[newType], visID);
		m_Tiles.push_back(newTile);


	}
	return true;
}

void TileManager::UpdateTileAfterCollision(int tileID) {
	Tile& currentTile = *m_Tiles[tileID];
	currentTile.hitCount -= 1;
	int currentHitCount = currentTile.hitCount;
	if (currentHitCount <= 0) {
		currentTile.isAlive = false;
		
		//drop a small square, starting from the tile position
		DroppingEffectManager* droppingEffectsManager = static_cast<DroppingEffectManager*>(GameManager::m_ManagerMap[ManagerType::droppingEffectManager_T]);
		droppingEffectsManager->ActivateTileEffect(tileID);


		//notify particle effects manager to sprinkle in some particles
		ManagerInterface* ptrToParticleManager = GameManager::GetManagerByType(ManagerType::particleManager_T);
		ParticleEffectManager& particleManager = static_cast<ParticleEffectManager&>(*ptrToParticleManager);
		assert(particleManager.HasFinishedInitialization(), "Render Manager not yet fully initialized! See initialization process & order.");
		particleManager.AddParticleEventAtPosition(currentTile.position);
	}
}

void TileManager::TerminateManager() {
	ManagerInterface* ptrToRenderManager = GameManager::GetManagerByType(ManagerType::renderManager_T);
	if (ptrToRenderManager == nullptr) {
		std::cout << "RenderManager not yet initialized!" << std::endl;
		return;
	}
	RenderManager& renderManager = static_cast<RenderManager&>(*ptrToRenderManager);
	assert(renderManager.HasFinishedInitialization(), "Render Manager not yet fully initialized! See initialization process & order.");

	for (Tile* tile : m_Tiles) {
		renderManager.DeleteShape(tile->m_VisualID);
		delete tile;
	}
}