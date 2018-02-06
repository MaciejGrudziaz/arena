#include "GamePhysicsConst.h"

GamePhysicsConst* GamePhysicsConst::instance = nullptr;

//pobranie instancji klasy
GamePhysicsConst* GamePhysicsConst::GetInstance() {
	if (instance == nullptr) {																							//jeœli nie utworzono jeszcze instancji klasy
		instance = new GamePhysicsConst();																				//utwórz instancjê klasy
	}

	return instance;																									//zwróæ aktualn¹ instancjê klasy
}

//konstruktor domyœlny
GamePhysicsConst::GamePhysicsConst() {
	G = 10.0;
}