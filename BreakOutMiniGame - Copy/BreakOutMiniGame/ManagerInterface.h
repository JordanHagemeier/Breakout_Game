#pragma once


enum ManagerType {
	renderManager_T,
	ballManager_T,
	tileManager_T,
	droppingEffectManager_T,
	playerManager_T,
	gameManager_T
};

//template <ManagerType Type>
class ManagerInterface {
	
public:
	//using MType = Type;

	virtual bool HasFinishedInitialization() = 0;
	virtual ManagerType GetManagerType() = 0;
	virtual void TickBeforeStart() = 0;
	virtual void Tick() = 0;
	virtual void TerminateManager() = 0;
};
