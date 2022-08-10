#include "Player.h"

const sf::Color Player::BASE_COLOR = sf::Color::Green;
const sf::Color Player::BUFF_COLOR = sf::Color::Cyan;
const float		Player::BASE_MOVEMENT_SPEED = 0.09f;
const float		Player::RIM_PERCENTAGE = 0.3f;

void Player::InitializeVisuals(RenderManager& renderManager) {
	sf::RectangleShape* playerTileMiddle = new sf::RectangleShape(sf::Vector2f(m_Dimensions.x * (1.0f - RIM_PERCENTAGE), m_Dimensions.y));
	sf::RectangleShape* playerTileLeft = new sf::RectangleShape(sf::Vector2f(m_Dimensions.x * RIM_PERCENTAGE, m_Dimensions.y));
	sf::RectangleShape* playerTileRight =new sf::RectangleShape(sf::Vector2f(m_Dimensions.x * RIM_PERCENTAGE, m_Dimensions.y));


	playerTileMiddle->setOrigin(sf::Vector2f(m_Dimensions.x / 2.0f, m_Dimensions.y / 2.0f));
	playerTileLeft->setOrigin(sf::Vector2f((m_Dimensions.x / 2.0f), m_Dimensions.y / 2.0f));
	playerTileRight->setOrigin(sf::Vector2f((m_Dimensions.x / 2.0f), m_Dimensions.y / 2.0f));


	sf::Vector2f drawingPos = sf::Vector2f(m_Position.x + (m_Dimensions.x * (RIM_PERCENTAGE)), m_Position.y);
	sf::Vector2f leftPos = sf::Vector2f(m_Position.x + (m_Dimensions.x * (1.0f - RIM_PERCENTAGE)), m_Position.y);
	sf::Vector2f rightPos = sf::Vector2f(m_Position.x, m_Position.y);
	playerTileMiddle->setPosition(drawingPos);
	playerTileLeft->setPosition(leftPos);
	playerTileRight->setPosition(rightPos);

	playerTileMiddle->setFillColor(BASE_COLOR);
	playerTileLeft->setFillColor(sf::Color::Red);
	playerTileRight->setFillColor(sf::Color::Blue);
	playerTileMiddle->setOutlineColor(sf::Color::Yellow);

	std::shared_ptr<sf::RectangleShape>playerTileMiddle_Shared = std::make_shared<sf::RectangleShape>(*playerTileMiddle);
	std::shared_ptr<sf::RectangleShape>playerTileLeft_Shared = std::make_shared<sf::RectangleShape>(*playerTileLeft);
	std::shared_ptr<sf::RectangleShape>playerTileRight_Shared = std::make_shared<sf::RectangleShape>(*playerTileRight);

	m_VisualID[0] = renderManager.AddShape(playerTileMiddle_Shared);
	m_VisualID[1] = renderManager.AddShape(playerTileLeft_Shared);
	m_VisualID[2] = renderManager.AddShape(playerTileRight_Shared);

}

void Player::UpdatePlayerVisuals(RenderManager& renderManager) {

	//calculate delta position 
	sf::Vector2f oldPos = m_Position;
	m_Position = sf::Vector2f(MathHelper::Clamp((m_MovementToApply.x + m_Position.x), 0.0f + (m_Dimensions.x / 2.0f), renderManager.m_WindowDimensions_px.x - (m_Dimensions.x / 2.0f)),m_Position.y);
	m_MovementToApply = sf::Vector2f(0.0f, 0.0f);
	sf::Vector2f deltaPosition = m_Position - oldPos;

	std::shared_ptr<sf::RectangleShape> playerVisual_Middle = std::static_pointer_cast<sf::RectangleShape>(renderManager.GetShape(m_VisualID[0]));
	std::shared_ptr<sf::RectangleShape> playerVisual_Left = std::static_pointer_cast<sf::RectangleShape>(renderManager.GetShape(m_VisualID[1]));
	std::shared_ptr<sf::RectangleShape> playerVisual_Right = std::static_pointer_cast<sf::RectangleShape>(renderManager.GetShape(m_VisualID[2]));

	//use delta position to offset the original position without much extra calculation
	playerVisual_Middle->setPosition(m_Position);
	playerVisual_Left->setPosition(playerVisual_Left->getPosition() + deltaPosition);
	playerVisual_Right->setPosition(playerVisual_Right->getPosition() + deltaPosition);
}

bool Player::CheckPlayerForBuffEffect(RenderManager& renderManager) {


	if (m_SpeedIsIncreased) {
		m_RemainingSpeedBuffTime--;

		if (m_RemainingSpeedBuffTime < 0.0f) {
			m_SpeedIsIncreased = false;
			std::shared_ptr<sf::RectangleShape> playerVisual_Middle = std::static_pointer_cast<sf::RectangleShape>(renderManager.GetShape(m_VisualID[0]));
			playerVisual_Middle->setFillColor(Player::BASE_COLOR);
			return false;
		}

		m_MovementToApply.x = m_MovementToApply.x * m_IncreasedSpeedMultiplier;
		float effectDurationPercentage = (float)m_RemainingSpeedBuffTime / (float)m_SpeedIncreasedDuration;

		sf::Color effectBasedColor;
 		effectBasedColor.r = MathHelper::Lerp(Player::BUFF_COLOR.r, Player::BASE_COLOR.r, 1.0f - effectDurationPercentage);
		effectBasedColor.g = MathHelper::Lerp(Player::BUFF_COLOR.g, Player::BASE_COLOR.g, 1.0f - effectDurationPercentage);
		effectBasedColor.b = MathHelper::Lerp(Player::BUFF_COLOR.b, Player::BASE_COLOR.b, 1.0f - effectDurationPercentage);

		std::shared_ptr<sf::RectangleShape> playerVisual_Middle = std::static_pointer_cast<sf::RectangleShape>(renderManager.GetShape(m_VisualID[0]));
		playerVisual_Middle->setFillColor(effectBasedColor);

	}
		
	return true;
	
}