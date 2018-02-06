#include "GAMEINPUT.h"

//inicajlizacja systemu wejœcia
void GAMEINPUT::InitalizeInputSystem() { 
	//inicajlizacja systemu wejœcia DirectInput
	if (WinAPIwindow::fullscreen) DIRECTINPUT::Initialize();								//jeœli apliakcja znajduje siê w trybie pe³noekranowym to wywo³¹j inicjalizacjê z trybem 'ekskluzywnym'
	else DIRECTINPUT::Initialize(false);													//jesli nie to wy³¹cz tryb 'ekskluzywny'
	/*TRYB EKSKLUZYWNY:
		tryb w któym nie widaæ myszy na ekranie oraz jest ona na wy³¹cznoœæ obecnej apliakcji*/
}

//uaktualnienie sygna³ów z systemu wejœcia
void GAMEINPUT::UpdateInputSystem() { DIRECTINPUT::Update(); }

//zamkniêcie i zwolnienie danego systemu wejœcia
void GAMEINPUT::ShutdownInputSystem() { DIRECTINPUT::Shutdown(); }

//sprawdzenie czy przycisk dany kodem 'key' jest wciœniêty
bool GAMEINPUT::KeyDown(int key) { return(DIRECTINPUT::KeyDown(key)); }

//sprawdzenie czy przycisk dany kodem 'key' jest puszczony
bool GAMEINPUT::KeyUp(int key) { return(DIRECTINPUT::KeyUp(key)); }

//sprawdzenie czy przycisk na myszy dany kodem 'key' jest wciœniety
bool GAMEINPUT::MouseButtonDown(int key) { return(DIRECTINPUT::ButtonDown(key)); }

//sprawdzenie czy przycisk na myszy dany kodem 'key' jest puszczony
bool GAMEINPUT::MouseButtonUp(int key) { return(DIRECTINPUT::ButtonUp(key)); }

//pobranie bezwzglêdnej pozycji kursora myszy na ekranie apliakcji
POSITION GAMEINPUT::GetMouseAbsPos() { return(WinAPImessages::GetMousePos()); }

//pobranie przemieszczenia [pozycji wzglêdnej] myszy
POSITION GAMEINPUT::GetMouseMove() {
	POSITION retPos;

	DIRECTINPUT::GetMouseMovement(retPos.x, retPos.y);										//wczytanie wartoœci x i y dla struktury zwracaj¹cej wartoœæ przesuniêcia myszy

	return retPos;
}