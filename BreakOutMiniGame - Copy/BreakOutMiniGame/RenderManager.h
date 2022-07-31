#pragma once
#include <map>
#include <SFML/Graphics.hpp>
//- map <id, sf::Shape*>
//->GetShape(id);
//->id AddShape(Shape*)
//->DeleteShape(id);
//->Render()-- > rendert alle yoinker
//->jedes Object was rendern will, holt sich vom RManager ne id für sein rendering dings
//-- > Render& Gamelogic mehr trennen

class RenderManager {

	std::map<int, sf::Shape*> m_ShapesToRender;
	sf::RenderWindow* m_CurrentWindow; 
	int m_IdCounter = 0;
	
	public: 
		RenderManager() {

		}
		RenderManager(sf::RenderWindow& window) {
			m_CurrentWindow = &window;
		}

		bool Render();
		void SetRenderWindow(sf::RenderWindow& window) { m_CurrentWindow = &window; }
		sf::Shape* GetShape(int id);
		int AddShape(sf::Shape* shape);
		bool DeleteShape(int id);
};