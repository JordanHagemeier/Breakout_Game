#pragma once
#include <SFML/Graphics.hpp>
#include "RenderManager.h"
#include "MathHelper.h"

class Player {
	public:
	
	int m_VisualID [3]; 
	sf::Vector2f		m_Dimensions;
	sf::Vector2f		m_Position;
	sf::Vector2f		m_MovementToApply = sf::Vector2f(0.0f, 0.0f); 
	static const float		RIM_PERCENTAGE;

	//movement
	static const sf::Color	BASE_COLOR;
	static const sf::Color	BUFF_COLOR;
	static const float		BASE_MOVEMENT_SPEED;

	bool	m_SpeedIsIncreased = false; //F buffed, increased
	int		m_RemainingSpeedBuffTime = 0; //F unclear: could be time since buff start or time until buff end -> m_RemainingSpeedBuffTime | m_SpeedBuffRunningSince
	
	//this belongs to the buff class:
	float	m_IncreasedSpeedMultiplier = 3; //F static constexpr --> could be part of buff class (does not really belong to player)
	float	m_SpeedIncreasedDuration = 5000.0f; //F static constexpr --> could be part of buff class (does not really belong to player)

	Player(sf::Vector2f dim, sf::Vector2f pos, RenderManager& renderManager) {
		m_Dimensions = dim;
		m_Position = pos;
		InitializeVisuals(renderManager);
	}

	void InitializeVisuals(RenderManager& renderManager);
	void UpdatePlayerVisuals(RenderManager& renderManager, sf::Vector2f windowDimensions);

};