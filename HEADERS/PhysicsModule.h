#pragma once
#include "PhysicsModuleConst.h"
#include "TIMER.h"
#include "PlayerInput.h"
#include "BotsActions.h"
#include "GamePhysics.h"
#include "AdditionalPhysics.h"
#include "BulletCollision.h"

//------------------------------------------------------------------------------------------------------
//------------------------------KLASA ODPOWIDAJ¥CA ZA WYKONANIE FIZYKI GRY------------------------------
//------------------------------------------------------------------------------------------------------

class PhysicsModule {
	PhysicsModule() {}
	PhysicsModule(const PhysicsModule& module) {}
	PhysicsModule& operator=(const PhysicsModule& module) {}

	static double lastFrameTime;																		//czas wykonania ostatniej klatki gry
	static double frameResidue;																			//'pozosta³oœæ' z ostatniej klatki gry

	static void UpdateCharacters();																		//aktualizacja stanu postaci œwiata gry

public:

	static void ProcessActionsAndPhysics();																//wykonanie wszystkich akcji i obliczeñ fizyki

	static void ModelsRegistration();																	//rejestracja dodatkowych modeli fizyki
};