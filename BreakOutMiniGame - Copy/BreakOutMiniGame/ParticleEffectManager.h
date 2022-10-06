#pragma once
#include "ManagerInterface.h"
#include "ParticleEffect.h"

#include <SFML/Graphics.hpp>
#include <vector>
//whenever a tile is removed, I want this manager to spawn some temporary particle effects that fall down 
//will need a math function for the falling behavior




class ParticleEffectManager : public ManagerInterface {

	int m_AmountOfParticleEffects = 54;
	int m_ParticleEffectsPerEvent = 6;
	int m_FramesTillEffectsDeath = 700;
	float m_ParticleSpeed = 0.3f;

	sf::RectangleShape m_BaseShape;
	std::vector<ParticleEffect*> m_ParticleEffects;
	bool m_HasFinishedInitialization = false;
	public:
		ParticleEffectManager() = default;
		virtual bool HasFinishedInitialization() const { return m_HasFinishedInitialization;};
		virtual ManagerType GetManagerType() { return particleManager_T; };
		virtual void TickBeforeStart() {
			SetUpParticleEffects();
			m_HasFinishedInitialization = true;
		};
		virtual void Tick() { UpdateParticles(); };
		virtual void TerminateManager() {};

		void SetUpParticleEffects();
		void AddParticleEventAtPosition(sf::Vector2f pos);
		void UpdateParticles();
		void DeleteParticleEffects();
};