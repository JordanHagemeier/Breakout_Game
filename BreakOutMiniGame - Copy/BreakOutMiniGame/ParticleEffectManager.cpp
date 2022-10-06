#include "ParticleEffectManager.h"
#include "GameManager.h"
#include "RenderManager.h"
#include <cassert>
#include <random>

void ParticleEffectManager::SetUpParticleEffects() {
	ManagerInterface* ptrToRenderManager = GameManager::GetManagerByType(ManagerType::renderManager_T);
	if (!ptrToRenderManager) {
		std::cout << "RenderManager not yet initialized!" << std::endl;
		return;
	}
	RenderManager& renderManager = static_cast<RenderManager&>(*ptrToRenderManager);
	assert(renderManager.HasFinishedInitialization(), "Render Manager not yet fully initialized! See initialization process & order.");

	m_BaseShape = sf::RectangleShape(sf::Vector2f(1.0f * renderManager.m_ScalingFactor, 1.0f * renderManager.m_ScalingFactor));
	m_BaseShape.setFillColor(sf::Color::Transparent);
	for (int i = 0; i < m_AmountOfParticleEffects; i++) {
		ParticleEffect* newEffect = new ParticleEffect();
		std::shared_ptr<sf::RectangleShape>shape_Shared = std::make_shared<sf::RectangleShape>(m_BaseShape);
		newEffect->m_VisualID = renderManager.AddShape(shape_Shared);
		newEffect->m_Position = sf::Vector2f(0.0f, 0.0f);
		newEffect->m_IsAlive = false;
		newEffect->m_FramesTillDeath = m_FramesTillEffectsDeath;
		newEffect->m_Speed = m_ParticleSpeed;
		
		m_ParticleEffects.push_back(newEffect);
	}
}
void ParticleEffectManager::AddParticleEventAtPosition(sf::Vector2f pos) {
	int neededEffects = m_ParticleEffectsPerEvent;

		for (ParticleEffect* effect : m_ParticleEffects) {
			if(neededEffects <= 0){
				break;
			}
			if (!effect->m_IsAlive) {
				effect->ActivateParticle(pos);
				neededEffects--;
			}
			
		}

	
}
void ParticleEffectManager::UpdateParticles() {

	for (ParticleEffect* effect : m_ParticleEffects) {
		if (effect->m_IsAlive) {
			if (effect->m_CurrentFrame > 0) {
			
				
				effect->UpdateParticleEffect();
			}
			else {
				effect->m_IsAlive = false;
				effect->DeactivateParticle();
			}
			
		}
		

	}
}
void ParticleEffectManager::DeleteParticleEffects() {
	// ~ Destructor here
}