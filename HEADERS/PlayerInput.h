#pragma once
#include <vector>
#include "GAMEINPUT.h"

const int TerminalReadDelayVal = 150000;							//opóŸnienie pomiêdzy wczytwaniem kolejnych znaków z terminala [150 ms]

class PlayerInput {
	PlayerInput() {}

	static std::vector<int> playerKeyboardInputs;					//wekotr przechowuj¹cy sygna³y wejœciowe z kalwiatury u¿ywane do sterowania postaci¹ gracza

	static std::vector<int> playerMouseInputs;						//wektor przechowuj¹cy sygna³y wejœciowe z myszy u¿ywane do sterowania postaci¹ gracza

	static unsigned long delay;										//czas trwania aplikacji
	//tablice przechowuj¹ce cza w którym zosta³ wciœniêty dany klawisz
	static unsigned long terminalDelayChar[25];						
	static unsigned long terminalDelayNum[2];
	static unsigned long terminalDelaySpecial[4];

	static void InitializePlayerKeyboardInputs();					//inicjalizacja wekotra sygna³ów wejœciowych z klawiatury
	static bool CheckPlayerKeyboardInputsVector(int val);			//sprawdzenie czy dany sygna³ wejsciowy ['key'] zosta³ ju¿ dodany do wektora sygna³ów wejœciowych dla postaci gracza
	static void CheckAllKeyboardInputs();							//sprawdzenie wszystkich sygna³ów wejœciowych z klawiatury umieszczonych w wekotrze 'playerKeyboardInputs'

	static void InitilizePlayerMouseInputs();						//inicjalizacja wektora sygna³ów wejœciowych z myszy
	static bool CheckPlayerMouseInputsVector(int val);				//sprawdzenie czy dany sygna³ wejsciowy ['key'] zosta³ ju¿ dodany do wektora sygna³ów wejœciowych dla postaci gracza
	static bool CheckMouseInput(int val);							//sprawdzenie czy akcja opisana sygna³em wejœciowym 'val' mia³a miejsce
	static int GetMouseInputParam(int val);							//zwrócenie parametru przesuniecia myszy dla sygna³u wejœciowego 'val'
	static void CheckAllMouseInputs();								//sprawdzenie wszystkich sygna³ów wejœciowych z myszy umieszczonych w wektorze 'playerMouseInputs'

	static void CheckConsoleInput();								//sprawdzenie sygna³ów wejœciowych w terminalu konsoli

public:

	static void InitializePlayerInputs();							//inicjalizacja wszystkich sygna³ów wejœciowych [mysz + klawiatura]

	static void ProccessInput();									//przetwarzanie sygna³ów wejœciowych
};