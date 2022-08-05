#pragma once
#include <SFML/Graphics.hpp>


class Ball {
	public:
		int ballVisualID = -1;
		float ballRadius;
		sf::Vector2f ballPosition;
		sf::Vector2f ballDirection;
		float ballAngle;
		static float BALL_BASE_SPEED;
		float repellantDistance = ballRadius / 2.0f;

		Ball(float scalingFactor) {
			ballRadius = 2.0f * scalingFactor;
		}
		Ball(float radius, sf::Vector2f pos, float angle, float scalingFactor, sf::CircleShape& visual, int ballVisID) {
			ballRadius = radius;
			ballPosition = pos;
			ballAngle = angle;
			repellantDistance = ballRadius / 2.0f;
			ballVisualID = ballVisID;
		}
		

		
};
