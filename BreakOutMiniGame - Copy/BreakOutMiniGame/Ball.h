#pragma once
#include <SFML/Graphics.hpp>
#include "Main_Fuctions.h"


class Ball {
	public:
		sf::CircleShape* ball_Visual;
		float m_BallRadius;
		sf::Vector2f m_BallPosition;
		sf::Vector2f m_BallDirection;
		float m_BallAngle;
		float m_BallSpeed;
		float m_RepellantDistance = m_BallRadius / 2.0f;

		Ball(float scalingFactor) {
			m_BallRadius = 2.0f * scalingFactor;
		}
		Ball(float radius, sf::Vector2f pos, float angle, float speed, float scalingFactor, sf::CircleShape visual) {
			m_BallRadius = radius;
			m_BallPosition = pos;
			m_BallAngle = angle;
			m_BallSpeed = speed;
			ball_Visual = &visual;
		}
		

		
};
