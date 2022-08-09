#pragma once


enum ManagerType {
	renderManager_T,
	gameManager_T,
	ballManager_T,
	tileManager_T,
	droppingEffectManager_T
};

class ManagerInterface {
	
public:
	virtual bool HasFinishedInitialization() = 0;
	virtual ManagerType GetManagerType() = 0;
	virtual void TickBeforeStart() = 0;
	virtual void Tick() = 0;
};
