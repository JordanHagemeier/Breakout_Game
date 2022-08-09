#include "DroppingEffectManager.h"
#include "GameManager.h"
#include "TileManager.h"

void DroppingEffectManager::SetupDroppingEffects() {
	for (int i = 0; i < m_TileArrayLength; i++) {

		RenderManager& renderManager = static_cast<RenderManager&>(*GameManager::m_ManagerMap[ManagerType::renderManager_T]);
		TileManager& tileManager = static_cast<TileManager&>(*GameManager::m_ManagerMap[ManagerType::tileManager_T]);
		//setup effects for each tile
		DroppingEffect* newEffect = new DroppingEffect();

		sf::RectangleShape* rect = new sf::RectangleShape(sf::Vector2f(10.0f, 10.0f));
		rect->setPosition(tileManager.m_Tiles[i]->position);
		rect->setFillColor(tileManager.m_Tiles[i]->baseColor);
		std::shared_ptr<sf::RectangleShape>rect_shared = std::make_shared<sf::RectangleShape>(*rect);

		newEffect->m_TileEffectType = tileManager.m_Tiles[i]->tileType;
		newEffect->m_VisualID = renderManager.AddShape(rect_shared);
		newEffect->m_Position = rect->getPosition();
		newEffect->m_BaseColor = rect->getFillColor();
		m_CurrentlyShownEffects.push_back(*newEffect);


	}
}
void DroppingEffectManager::UpdateTileEffectVisuals() {
	//go through all tile effects in the list and update their position
	//then draw them
	float downwardMovementIncrement = 0.02f;
	int amount = m_CurrentlyShownEffects.size();
	for (int i = 0; i < amount; i++) {
		RenderManager& renderManager = static_cast<RenderManager&>(*GameManager::m_ManagerMap[ManagerType::renderManager_T]);

		int currentID = m_CurrentlyShownEffects[i].m_VisualID;
		std::shared_ptr<sf::RectangleShape> currentEffect_Visual = std::dynamic_pointer_cast<sf::RectangleShape>(renderManager.GetShape(currentID));

		if (!m_CurrentlyShownEffects[i].m_IsActive) {
			currentEffect_Visual->setFillColor(sf::Color::Transparent);
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

}

void DroppingEffectManager::ActivateTileEffect(int arrayPosition) {
	m_CurrentlyShownEffects[arrayPosition].m_IsActive = true;
}