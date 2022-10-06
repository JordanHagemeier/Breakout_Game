#pragma once
#include <SFML/Graphics.hpp>
class ParticleEffect {
public:
	int m_VisualID;
	sf::Vector2f m_Position;
	sf::Vector2f m_Direction;
	float m_Speed;
	bool m_IsAlive = false;
	int m_FramesTillDeath;
	int m_CurrentFrame; 

	void ActivateParticle(sf::Vector2f pos);
	void UpdateParticleEffect();
	void DeactivateParticle();
};