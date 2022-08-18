#pragma once
#include <map>
#include <SFML/Graphics.hpp>
#include "ManagerInterface.h"
#include <iostream>
//- map <id, sf::Shape*>
//->GetShape(id);
//->id AddShape(Shape*)
//->DeleteShape(id);
//->Render()-- > rendert alle yoinker
//->jedes Object was rendern will, holt sich vom RManager ne id für sein rendering dings
//-- > Render& Gamelogic mehr trennen

class RenderManager : public ManagerInterface/*<ManagerType::renderManager_T>*/{

	std::map<int, std::shared_ptr<sf::Shape>> m_ShapesToRender;
	sf::RenderWindow* m_CurrentWindow; 
	int m_IdCounter = 0;
	bool m_InitializationIsDone = false;

	public: 
		sf::Vector2f m_WindowDimensions_px;
		sf::Vector2f m_WindowSegmentDimensions_px; 
		float m_ScalingFactor;

		RenderManager() = default;
		RenderManager(sf::RenderWindow& window, sf::Vector2f windowDimensions_px, sf::Vector2f windowSegmentDimensions_px, float scalingFactor) {
			m_CurrentWindow = &window;
			m_WindowDimensions_px = windowDimensions_px;
			m_WindowSegmentDimensions_px = windowSegmentDimensions_px;
			m_ScalingFactor = scalingFactor;
			m_InitializationIsDone = true;
		}

		virtual ManagerType GetManagerType(){return ManagerType::renderManager_T;}
		virtual void TickBeforeStart(){}
		virtual bool HasFinishedInitialization() { return m_InitializationIsDone; }
		virtual void Tick() {
			//std::cout<< "RenderManager Tick!" << std::endl;
		}
		virtual void TerminateManager(){};

		bool Render();
		void SetRenderWindow(sf::RenderWindow& window) { m_CurrentWindow = &window; }
		std::shared_ptr<sf::Shape> GetShape(int id);
		int AddShape(std::shared_ptr<sf::Shape> shape);
		bool DeleteShape(int id);
};