#pragma once
#include <time.h>
#include "DynamicCharacter.h"
#include "GamePhysicsStruct.h"

//--------------------------------------------------------------------------------------------
//-----------------------------------KLASA PROTOTYPOWA BOTA-----------------------------------
//--------------------------------------------------------------------------------------------

class Bot :public DynamicCharacter {
	std::vector<GAMEPHYSICS::CollisionStatus> lastColStatus;														//wektor ostatnich, niezbadanych statusów kolizji

	double makeTurn;																								//wartoœæ k¹ta obrotu [makeTurn > 0 - pozosta³a wartoœæ obrotu do wykonania]
	unsigned turnOrient;																							//orientacja obrotu [LEWO, PRAWO]

	void MakeTurnAction(double yAngle);																				//wykonanie akcji obrotu
	void MakeLookAction(double xAngle);																				//wykonanie akcji zmainy widoku

	void RandomMoveAction();																						//wykonanie losowego ruchu

public:
	Bot(const std::string& filename) : DynamicCharacter(filename),makeTurn(0) {}
	Bot(const std::string& filename, const std::string& name, const Vector& position, const Vector& orientation) :
		DynamicCharacter(filename, name, position, orientation),makeTurn(0) {}

	void PerformAdditionalAction(ActionPtr Action) {}																//dodatkowe akcje bota do wykonania [brak]

	void LoadLastColStatus(GAMEPHYSICS::CollisionStatus newLastCol);												//za³adowanie nowego obiektu kolizji do wektora niezbadanych statusów kolizji

	void MainLogic();																								//g³ówna logika bota

	void Update();																									//funkcja uaktualniaj¹ca stan bota
};