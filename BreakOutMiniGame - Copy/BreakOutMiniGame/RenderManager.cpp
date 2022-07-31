#include "RenderManager.h"

bool RenderManager::Render() {
	//iterate over all shapes in m_ShapesToRender
	//Draw to Window
	std::map<int, sf::Shape*>::iterator it = m_ShapesToRender.begin();
	while (it != m_ShapesToRender.end()) {
		sf::Shape* currentShape = m_ShapesToRender[it->first];
		m_CurrentWindow->draw(*currentShape);
		it++;
	}
	//for (int i = 1; i < m_IdCounter + 1; i++) {
	//}
	return true;
}

sf::Shape* RenderManager::GetShape(int id){

	return m_ShapesToRender[id];
}
int RenderManager::AddShape(sf::Shape* shape) {
	m_IdCounter++;
	m_ShapesToRender[m_IdCounter] = shape;
	return m_IdCounter;
}
bool RenderManager::DeleteShape(int id) {
	m_ShapesToRender.erase(id);

	return true;
}