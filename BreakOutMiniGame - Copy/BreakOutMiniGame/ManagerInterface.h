#pragma once


enum ManagerType {
	renderManager_T,
	gameManager_T,
	droppingEffectManager_T,
	ballManager_T,
	tileManager_T
};

class ManagerInterface {
	
public:
	virtual ManagerType GetManagerType() = 0;
	virtual void TickBeforeStart() = 0;
	virtual void Tick() = 0;
};
