#pragma once
#include "Vector.h"

//--------------------------------------------------------------------------------------------------------------------
//------------------------------PLIK OPISUJ¥CY STRUKTURY DLA AKCJI OBIEKTÓW DYNAMICZNYCH------------------------------
//---------------------------------------------[akcje gracza, botów itp.]---------------------------------------------
//--------------------------------------------------------------------------------------------------------------------

//typ wyliczeniowy okreœlaj¹cy akcje wykonywane przez postacie
enum ACTION_NAME {
	MOVE_RIGHT, MOVE_LEFT, MOVE_FORWARD, MOVE_BACKWARD,
	TURN_RIGHT, TURN_LEFT, LOOK_UP, LOOK_DOWN,
	FALL, JUMP,
	SHOOT, RELOAD
};

//typ wyliczeniowy okreœlaj¹cy kategorie akcji wykonywanych przez posatcie
enum SUB_ACTION_NAME { MOVEMENT, ORIENTATION, AIR, OTHER };

//struktura s³u¿¹ca do definiowania akcji wykonywanych przez postaæ gracza
//akcja jest definiowana poprzez jej nazwê okreœlon¹ poprzez typ wyliczeniowy ACTION_NAME oraz jej kategoriê opisana przez SUB_ACTION_NAME
class ACTION {
public:
	ACTION_NAME name;																			//nazwa akcji
	SUB_ACTION_NAME subActionName;																//nazwa kategorii

	//KONSTRUKTORY
	ACTION() {}
	ACTION(ACTION_NAME Name, SUB_ACTION_NAME subAction) {
		name = Name;
		subActionName = subAction;
	}
};

//akcja ruchu
class MOVE_ACTION : public ACTION {
public:
	MOVE_ACTION(ACTION_NAME name) :ACTION(name, MOVEMENT) {}
};

//akcja zmiany orientacji
class LOOK_ACTION : public ACTION {
public:
	double mouseMove;																			//parametr przemieszczenia myszy	

	LOOK_ACTION(ACTION_NAME name,double _mouseMove) :ACTION(name, ORIENTATION), mouseMove(_mouseMove) {}
};

//akcja spadania
class FALL_ACTION : public ACTION {
public:
	Vector position;																			//pozycja podczas rozpoczêcia spadania														
	Vector shift;																				//orientacja podczas rozpoczêcia spadania
	double time;																				//³aczny czas trwania spadania

	FALL_ACTION(Vector& _position, Vector& _shift) : ACTION(FALL, AIR), position(_position), shift(_shift), time(0) {}

	void UpdateTime(double dt) {																//aktualizacja czasu spadania
		time += dt;
	}
};

//akcja skoku
class JUMP_ACTION : public ACTION {
public:
	Vector position;																			//pozycja startowa akcji skoku
	Vector shift;																				//orientacja podczas rozpoczêcia skoku
	double time;																				//³¹czny czas trwania skoku

	JUMP_ACTION(Vector& _position, Vector& _shift, double jumpStrength) : ACTION(JUMP, AIR), position(_position), shift(_shift), time(0) {
		shift.y = jumpStrength;
	}

	void UpdateTime(double dt) {																//aktualizcja czasu skoku
		time += dt;
	}
};

//struntura ³¹cz¹ca akcje postaci wraz z sygna³em wejœciowym
struct ACTION_INPUT {
	ACTION_NAME action;																			//nazwa akcji
	SUB_ACTION_NAME subActionName;																//nazwa kategorii akcji
	int input;																					//sygna³ wejœciowy

																								//KONSTRUKTORY
	ACTION_INPUT() {}
	ACTION_INPUT(ACTION_NAME Action, SUB_ACTION_NAME subAction, int Input) {
		action = Action;
		subActionName = subAction;
		input = Input;
	}
};