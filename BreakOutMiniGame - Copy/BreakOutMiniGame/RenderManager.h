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

struct RenderInfo {
	sf::RenderWindow* window;
	sf::Vector2f windowDimensions_px;
	sf::Vector2f windowSegmentAmountPerDimension;
	float scalingFactor;
};

class RenderManager : public ManagerInterface/*<ManagerType::renderManager_T>*/{

	std::map<int, std::shared_ptr<sf::Shape>> m_ShapesToRender;
	sf::RenderWindow* m_CurrentWindow; 
	int m_IdCounter = 0;
	bool m_InitializationIsDone = false;

	public: 
		sf::Vector2f m_WindowDimensions_px;
		sf::Vector2f m_SegmentCountPerWindowDimension; 
		sf::Vector2f m_WindowPercentageSegmentPerDimension;
		float m_ScalingFactor;

		RenderManager() = default;
		RenderManager(RenderInfo info) {
			m_CurrentWindow = info.window;
			m_WindowDimensions_px = info.windowDimensions_px;
			m_SegmentCountPerWindowDimension = info.windowSegmentAmountPerDimension;
			m_ScalingFactor = info.scalingFactor;
			m_InitializationIsDone = true;
			m_WindowPercentageSegmentPerDimension = sf::Vector2f(m_WindowDimensions_px.x / m_SegmentCountPerWindowDimension.x,
				                                                 m_WindowDimensions_px.y / m_SegmentCountPerWindowDimension.y);
		}

		
		virtual ManagerType GetManagerType(){return ManagerType::renderManager_T;}
		virtual void TickBeforeStart(){}
		virtual bool HasFinishedInitialization() const { return m_InitializationIsDone; }
		virtual void Tick() {
			//std::cout<< "RenderManager Tick!" << std::endl;
		}
		virtual void TerminateManager(){ DeleteAllShapes(); };

		bool Render();
		void SetRenderWindow(sf::RenderWindow& window) { m_CurrentWindow = &window; }
		const sf::RenderWindow* GetRenderWindow() {return m_CurrentWindow;}
		std::shared_ptr<sf::Shape> GetShape(int id);
		int AddShape(std::shared_ptr<sf::Shape> shape);
		
		bool DeleteShape(int id);
		bool DeleteAllShapes();
};