#include "ParticleEffect.h"
#include "RenderManager.h"
#include "GameManager.h"
#include <cassert>
#include <random>

void ParticleEffect::ActivateParticle(sf::Vector2f pos) {
	
	m_IsAlive = true;
	m_Position = pos;
	m_CurrentFrame = m_FramesTillDeath;

	ManagerInterface* ptrToRenderManager = GameManager::GetManagerByType(ManagerType::renderManager_T);
	if (!ptrToRenderManager) {
		std::cout << "RenderManager not yet initialized!" << std::endl;
		return;
	}
	RenderManager& renderManager = static_cast<RenderManager&>(*ptrToRenderManager);
	assert(renderManager.HasFinishedInitialization(), "Render Manager not yet fully initialized! See initialization process & order.");
	std::shared_ptr<sf::RectangleShape> currentEffectVisual = std::static_pointer_cast<sf::RectangleShape>(renderManager.GetShape(m_VisualID));
	currentEffectVisual->setFillColor(sf::Color::White);


	//create randomized direction vector
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<> dist(0, 360);
	float randomAngle = dist(gen); 
	sf::Vector2f direction = sf::Vector2f(std::cos(randomAngle), std::sin(randomAngle));
	m_Direction = direction;

}

void ParticleEffect::UpdateParticleEffect() {
	//calculate new position
	float lifeSpan = (float)m_CurrentFrame / (float)m_FramesTillDeath;
	sf::Vector2f newPosition = m_Position + (m_Direction * lifeSpan * m_Speed);
	m_CurrentFrame--;


	//set new position
	RenderManager& renderManager = static_cast<RenderManager&>(*GameManager::m_ManagerMap[ManagerType::renderManager_T]);
	std::shared_ptr<sf::RectangleShape> currentEffectVisual = std::static_pointer_cast<sf::RectangleShape>(renderManager.GetShape(m_VisualID));
	currentEffectVisual->setPosition(newPosition);
	m_Position = newPosition;
	
}

void ParticleEffect::DeactivateParticle() {
	RenderManager& renderManager = static_cast<RenderManager&>(*GameManager::m_ManagerMap[ManagerType::renderManager_T]);
	std::shared_ptr<sf::RectangleShape> currentEffectVisual = std::static_pointer_cast<sf::RectangleShape>(renderManager.GetShape(m_VisualID));
	currentEffectVisual->setFillColor(sf::Color::Transparent);
}