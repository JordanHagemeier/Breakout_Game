#include "DroppingEffectManager.h"
#include "GameManager.h"
#include "TileManager.h"
#include "PlayerManager.h"
#include "BallManager.h"
#include <cassert>

void DroppingEffectManager::SetupDroppingEffects() {
	ManagerInterface* ptrToRenderManager = GameManager::GetManagerByType(ManagerType::renderManager_T);
	if (ptrToRenderManager == nullptr) {
		std::cout << "RenderManager not yet initialized!" << std::endl;
		return;
	}
	RenderManager& renderManager = static_cast<RenderManager&>(*ptrToRenderManager);
	assert(renderManager.HasFinishedInitialization(), "Render Manager not yet fully initialized! See initialization process & order.");

	TileManager& tileManager = static_cast<TileManager&>(*GameManager::m_ManagerMap[ManagerType::tileManager_T]);
	assert(tileManager.HasFinishedInitialization(), "Tile Manager not yet fully initialized! See initialization process & order.");
	for (int i = 0; i < tileManager.m_Tiles.size(); i++) {

		/*if (!tileManager.HasFinishedInitialization()) {
			std::cout << "Tile Manager not yet fully initialized! See initialization process & order." <<std::endl;
			
		}*/

		//setup effects for each tile
		// add delete in unint
		// -> raii "resource allocation is initialization"

		//memory leak: instead use make_shared(constructor arguments)
		std::shared_ptr<sf::RectangleShape>rect_shared = std::make_shared<sf::RectangleShape>(sf::Vector2f(10.0f, 10.0f));
		// ....
		rect_shared->setPosition(tileManager.m_Tiles[i]->position);
		rect_shared->setFillColor(tileManager.m_Tiles[i]->baseColor);


		DroppingEffect* newEffect = new DroppingEffect();
		newEffect->m_TileEffectType = tileManager.m_Tiles[i]->tileType;
		newEffect->m_VisualID = renderManager.AddShape(rect_shared);
		newEffect->m_Position = rect_shared->getPosition();
		newEffect->m_BaseColor = rect_shared->getFillColor();
		m_CurrentlyShownEffects.push_back(*newEffect);


	}
}
void DroppingEffectManager::UpdateTileEffectVisuals() {
	//go through all tile effects in the list and update their position
	//then draw them
	float downwardMovementIncrement = 0.02f; //constexpr oder const
	int amount = m_CurrentlyShownEffects.size();
	for (int i = 0; i < amount; i++) {
	// im game manager:
	// Step 1:
	// ManagerInterface* GetManager(ManagerType type);
	// Step 2:
	// T* GetManager<T>();
	// m_ManagerMap[T::MType]
	// RenderManager* renderManager = GameManager::GetManager<RenderManager>();
		RenderManager& renderManager = static_cast<RenderManager&>(*GameManager::m_ManagerMap[ManagerType::renderManager_T]);

		int currentID = m_CurrentlyShownEffects[i].m_VisualID;
		std::shared_ptr<sf::RectangleShape> currentEffect_Visual = std::dynamic_pointer_cast<sf::RectangleShape>(renderManager.GetShape(currentID));

		if (!m_CurrentlyShownEffects[i].m_IsActive) {
			currentEffect_Visual->setFillColor(sf::Color::Transparent);
			// return;
		}

		if (m_CurrentlyShownEffects[i].m_IsActive) {

			if (currentEffect_Visual->getFillColor() == sf::Color::Transparent) {
				currentEffect_Visual->setFillColor(m_CurrentlyShownEffects[i].m_BaseColor);
			}
			sf::Vector2f pos = m_CurrentlyShownEffects[i].m_Position;
			pos = sf::Vector2f(pos.x, pos.y + downwardMovementIncrement);
			currentEffect_Visual->setPosition(pos);
			m_CurrentlyShownEffects[i].m_Position = pos;

		}


	}

	//for (const DroppingEffect& effect : m_CurrentlyShownEffects)
	//{

	//}

}

void DroppingEffectManager::ActivateTileEffect(int arrayPosition) {
	m_CurrentlyShownEffects[arrayPosition].m_IsActive = true;
}

TileType DroppingEffectManager::CheckEffectWithPlayerCollision(bool* collisionBool, Player& player) {

	TileType foundType = TileType::NoEvent;
	int length = m_CurrentlyShownEffects.size();
	for (int i = 0; i < length; i++) {
		//check if tile is colliding with player
		if (!m_CurrentlyShownEffects[i].m_IsActive) {
			continue;
		}
		sf::Vector2f effectPos = m_CurrentlyShownEffects[i].m_Position;

		float distanceEffectPlayerY = abs(effectPos.y - player.m_Position.y);
		if (distanceEffectPlayerY < 2.0f) {
			if (effectPos.x < (player.m_Position.x + (player.m_Dimensions.x / 2.0f)) && effectPos.x >(player.m_Position.x - (player.m_Dimensions.x / 2.0f))) {
				foundType = m_CurrentlyShownEffects[i].m_TileEffectType;
				m_CurrentlyShownEffects[i].m_IsActive = false;
				*collisionBool = true;

				return foundType;
			}
		}


		//if yes, return 
	}

	*collisionBool = false;
	return foundType;
}

bool DroppingEffectManager::CheckForEffectUsage() {
	bool collisionPlayerWithEffect = false;
	PlayerManager& playerManager = static_cast<PlayerManager&>(*GameManager::m_ManagerMap[ManagerType::playerManager_T]);
	BallManager& ballManager = static_cast<BallManager&>(*GameManager::m_ManagerMap[ManagerType::ballManager_T]);
	DroppingEffectManager& droppingEffectManager = static_cast<DroppingEffectManager&>(*GameManager::m_ManagerMap[ManagerType::droppingEffectManager_T]);

	for (int i = 0; i < playerManager.m_Players.size(); i++) {
		TileType effectType = droppingEffectManager.CheckEffectWithPlayerCollision(&collisionPlayerWithEffect, *playerManager.m_Players[i]);
		if (collisionPlayerWithEffect) {
			switch (effectType) {
			case TileType::AddedBall:
				std::cout << "Added Ball!" << std::endl;
				ballManager.CreateNewBall();
				break;
			case TileType::QuickerPlayer:
				std::cout << "Quicker Player!" << std::endl;
				playerManager.m_Players[i]->m_RemainingSpeedBuffTime = playerManager.m_Players[i]->m_SpeedIncreasedDuration;
				playerManager.m_Players[i]->m_SpeedIsIncreased = true;
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

void DroppingEffectManager::TerminateManager() {
	ManagerInterface* ptrToRenderManager = GameManager::GetManagerByType(ManagerType::renderManager_T);
	if (ptrToRenderManager == nullptr) {
		std::cout << "RenderManager not yet initialized!" << std::endl;
		return;
	}
	RenderManager& renderManager = static_cast<RenderManager&>(*ptrToRenderManager);
	assert(renderManager.HasFinishedInitialization(), "Render Manager not yet fully initialized! See initialization process & order.");

	for (DroppingEffect effect : m_CurrentlyShownEffects) {
		renderManager.DeleteShape(effect.m_VisualID);

	}
}