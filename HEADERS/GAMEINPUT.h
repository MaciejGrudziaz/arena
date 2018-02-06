#pragma once
#include "DIRECTINPUT.h"
#include "WinAPImessages.h"
#include "Vector.h"

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------GAMEINPUT-----------------------------------------------------------------------------------
//--------------------------------------------------------klasa wysokiego poziomu s³u¿¹ca za system wejœcia dla aplikacji-------------------------------------------------------
//-----------------------------------------zadekalrowane w niej s¹ funkcje za pomoc¹ których u¿ytkownik odwo³uje siê do systemu wejœcia-----------------------------------------
//--------------------definiecje tych funkcji s¹ odwo³aniem do konkretnych funkcji klas ni¿szego poziomu pochodz¹cych z wybranego API [Direct Input, WinAPI]--------------------
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

class GAMEINPUT {
	GAMEINPUT() {}

public:
	static void InitalizeInputSystem();										//inicjalizacja systemu wejœcia

	static void UpdateInputSystem();										//uaktualnienie sygna³ów z systemu wejœcia

	static void ShutdownInputSystem();										//zamkniêcie i zwolnienie danego systemu wejœcia

	//---------------------------------------------------------------------------------------------------------------------------------

	static bool KeyDown(int key);											//sprawdzenie czy przycisk dany kodem 'key' jest wciœniêty

	static bool KeyUp(int key);												//sprawdzenie czy przycisk dany kodem 'key' jest puszczony

	static bool MouseButtonDown(int key);									//sprawdzenie czy przycisk na myszy dany kodem 'key' jest wciœniety

	static bool MouseButtonUp(int key);										//sprawdzenie czy przycisk na myszy dany kodem 'key' jest puszczony

	static POSITION GetMouseAbsPos();										//pobranie bezwzglêdnej pozycji kursora myszy na ekranie apliakcji

	static POSITION GetMouseMove();											//pobranie przemieszczenia [pozycji wzglêdnej] myszy
};