#include "Bot.h"

//za³adowanie nowego obiektu kolizji do wektora niezbadanych statusów kolizji
void Bot::LoadLastColStatus(GAMEPHYSICS::CollisionStatus newLastCol) {
	if (newLastCol.groundCol.collisionOccured || newLastCol.mapBoundriesCol.collisionOccured ||
		newLastCol.negativeCol.collisionOccured || newLastCol.standardCol.collisionOccured)
		lastColStatus.push_back(newLastCol);																				//jeœli zasz³a jakakolwiek kolizja - dodaj j¹ do wektora
}

//g³ówna logika bota
void Bot::MainLogic() {
	RandomMoveAction();
}

//funkcja uaktualniaj¹ca stan bota
void Bot::Update() {
	DynamicCharacter::Update();

	if (health <= 0) {
		health = 0;
		disableCharacter();
	}
}

//wykonanie akcji obrotu
void Bot::MakeTurnAction(double yAngle) {
	double t = PHYSICSMODULE::targetFrameTime/1000000.0;																	//standardowy czas trwania jednej klatki [w sek.]

	if (yAngle >= 0) {																										//jeœli wykonujemy obrót w prawo
		double turnVal = yAngle / (static_cast<double>(t)*turnSensitivity*360.0);											//oblicz wart. dla akcji obrotu o k¹t 'yAngle'
		LOOK_ACTION turn(TURN_RIGHT, turnVal);																				//stworzenie nowej akcji obrotu
		AddAction(turn);																									//dodanie jej do wektora akcji
	}
	else {
		yAngle = -yAngle;																									//jeœli wykonujemy obrót w lewo
		double turnVal = yAngle / (static_cast<double>(t)*turnSensitivity*360.0);											//oblicz wart. dla akcji obrotu o k¹t 'yAngle'
		LOOK_ACTION turn(TURN_LEFT, turnVal);																				//stworzenie nowej akcji obrotu
		AddAction(turn);																									//dodanie jej do wektora akcji
	}
}

//wykonanie akcji zmiany widoku
void Bot::MakeLookAction(double xAngle) {

}

//wykonanie losowego ruchu
void Bot::RandomMoveAction() {
	double t = PHYSICSMODULE::targetFrameTime;

	MOVE_ACTION newAction(MOVE_FORWARD);
	AddAction(newAction);																									//dodanie akcji ruchu do przodu

	if (lastColStatus.size() > 0 && makeTurn == 0) {																		//jeœli zasz³a kolizja  i bot nie wykonywa³ ¿adnego obrotu
		if (lastColStatus[0].standardCol.collisionOccured || lastColStatus[0].mapBoundriesCol.collisionOccured) {			//jeœli zasz³a kolizja standardowa lub z granic¹ mapy
			makeTurn = rand() % 180;																						//wykonaj obrót o maks. 180 stopni
			turnOrient = rand() % 2;																						//w losowym kierunku
		}
		lastColStatus.clear();																								//wyczyœæ wektor niezbadanych kolizji
	}

	if (makeTurn > 0) {																										//jeœli postaæ nie skoñczy³a jeszcze obrotu
		if (turnOrient == 0) {																								//jeœli wykonujemy obrót w lewo
			MakeTurnAction(-1);																								//wykonaj obrót o 1 stopieñ
			makeTurn -= 1;																									//aktualizuj wartoœæ pozosta³¹ obrotu
			if (makeTurn < 0) makeTurn = 0.0;
		}
		else {																												//jeœli wykonujemy obrót w prawo
			MakeTurnAction(1);																								//wykonaj obrót o 1 stopieñ
			makeTurn -= 1;																									//aktualizuj wartoœæ pozosta³¹ obrotu
			if (makeTurn < 0) makeTurn = 0.0;
		}
	}
}