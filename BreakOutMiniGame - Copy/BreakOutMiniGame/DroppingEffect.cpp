#include "DroppingEffect.h"

void DroppingEffectManager::DrawTileEffects(RenderManager& renderManager) {
	//go through all tile effects in the list and update their position
	//then draw them
	float downwardMovementIncrement = 0.02f;
	int amount = m_CurrentlyShownEffects.size();
	for (int i = 0; i < amount; i++) {
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