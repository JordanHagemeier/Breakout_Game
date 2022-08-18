#pragma once
#include <SFML/Graphics.hpp>

struct BallInfo {
	float radius;
	sf::Vector2f position;
	float angle;
	float scalingFactor;
	int visualID;
};

class Ball {
	public:
		static float BALL_BASE_SPEED;
		int m_VisualID = -1;
		float ballRadius;
		sf::Vector2f ballPosition;
		sf::Vector2f ballDirection;
		float ballAngle;
		float repellantDistance = ballRadius / 2.0f;

		Ball(float scalingFactor) {
			ballRadius = 2.0f * scalingFactor;
		}
		Ball(BallInfo info) {
			ballRadius = info.radius;
			ballPosition = info.position;
			ballAngle = info.angle;
			repellantDistance = ballRadius / 2.0f;
			m_VisualID = info.visualID;
			ballDirection = sf::Vector2f(std::cos(ballAngle) * BALL_BASE_SPEED, std::sin(ballAngle) * BALL_BASE_SPEED);
		}
		

		
};
