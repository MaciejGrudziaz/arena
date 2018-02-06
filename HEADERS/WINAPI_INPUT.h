#pragma once
#include "Vector.h"

//----------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------INPUT DLA WINAPI------------------------------------------------------
//------------------------------klasa interfejsu dla systemu wejœcia zdefiniwoanego przez WinAPI------------------------------
//----------------------------------------------------------------------------------------------------------------------------

class WINAPI_INPUT {
protected:
	static bool keyPressed[256];										//tablica przechowuj¹ca wartoœci sygna³ów wejœciowych z klawiatury

	static POSITION mousePos;											//struktura przechowuj¹ca aktualn¹ pozycjê kursora myszy [bezwzglêdn¹]

public:
	//funkcje zwracaj¹ce stan sygna³ów wejœciowych z klawiatury
	static bool KeyDown(int key) { return keyPressed[key]; }
	static bool KeyUp(int key) { return !(keyPressed[key]); }

	//funkcja zwracaj¹ca aktualn¹ pozycjê bezwzglêdn¹ kursora myszy
	static POSITION GetMousePos() { return mousePos; }

};