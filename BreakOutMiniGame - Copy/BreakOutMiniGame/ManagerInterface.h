#pragma once


enum ManagerType {
	renderManager_T,
	ballManager_T,
	tileManager_T,
	boosterDropManager_T,
	playerManager_T,
	particleManager_T,
	gameManager_T
};

//template <ManagerType Type>
class ManagerInterface {
	
public:
	//using MType = Type;

	
	virtual bool HasFinishedInitialization() const = 0;
	virtual ManagerType GetManagerType() = 0;
	virtual void TickBeforeStart() = 0;
	virtual void Tick() = 0;
	virtual void TerminateManager() = 0;
};
