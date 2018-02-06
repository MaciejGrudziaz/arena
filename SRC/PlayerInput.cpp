#include "PlayerInput.h"

//inicjalizacja obiektów statycznych klasy
std::vector<int> PlayerInput::playerKeyboardInputs;
std::vector<int> PlayerInput::playerMouseInputs;
unsigned long PlayerInput::terminalDelayChar[25] = { 0 };
unsigned long PlayerInput::terminalDelayNum[2]={ 0 };
unsigned long PlayerInput::terminalDelaySpecial[4] = { 0 };
unsigned long PlayerInput::delay = TerminalReadDelayVal;

//inicjalizacja wszystkich sygna³ów wejœciowych [mysz + klawiatura]
void PlayerInput::InitializePlayerInputs() {
	InitializePlayerKeyboardInputs();												//inicjalizacja sygna³ów wejœciowych z klawiatury
	InitilizePlayerMouseInputs();													//inicjalizacja sygna³ów wejœciowych z myszy
}

//przetwarzanie sygna³ów wejœciowych
void PlayerInput::ProccessInput() {
	GAMEINPUT::UpdateInputSystem();													//aktualizacja sygna³ów z wejœcia

	if (!CONSOLE::GetTerminalStatus()) {
		CheckAllKeyboardInputs();														//sprawdzenie wszystkich sygna³ów wejœciowych z klawiatury umieszczonych w wektorze 'playerKeyboardInputs'

		CheckAllMouseInputs();															//sprawdzenie wszystkich sygna³ów wejœciowych z myszy umieszczonych w wektorze 'playerMouseInputs'
	}
	else CheckConsoleInput();
}

//inicjalizacja sygna³ów wejœciowych z klawiatury dla postaci sterowanej przez gracza
void PlayerInput::InitializePlayerKeyboardInputs() {
	for (unsigned int i = 0; i < GameObjects::players.size(); i++) {				//sprawdzenie wszystkich postaci z wektora postaci
		//pobranie wekotra relacji akcji i sygna³ów wejœciowych dla danej postaci
		std::vector<ACTION_INPUT> playerActionVec = GameObjects::players[i].GetActionKeyboardRelations();
		unsigned int n = playerActionVec.size();
		for (unsigned int j = 0; j<n; j++) {										//sprwadzenie wszystkich sygna³ów wejœciowych zwekotra relacji akcja - wejœcie
			int val = playerActionVec[j].input;										//pobranie wartoœci sygna³u wejœciowego z wektora realcji akcja - wejœcie
			if (!CheckPlayerKeyboardInputsVector(val))								//jeœli dany sygna³ wejœciowy nie zosta³ jeszcze dodany do wektora sygna³ów wejœciowych dla postaci
				playerKeyboardInputs.push_back(val);								//dodaj sygna³ do wektora
		}
	}
}

//sprawdzenie czy dany sygna³ ['key'] ZANJDUJE SIÊ w wektorze sygna³ów wejœciowych z klawaitury
bool PlayerInput::CheckPlayerKeyboardInputsVector(int val) {
	for (unsigned int i = 0; i < playerKeyboardInputs.size(); i++) {
		if (playerKeyboardInputs[i] == val) return true;							//jeœli TAK zwróæ TRUE
	}
	return false;																	//w przciwnym wypadku - FALSE
}

//sprwadzenie wszystkich sygna³ów wejœciowych z klawiatury dodanych do wektora 'playerKeyboardInputs'
void PlayerInput::CheckAllKeyboardInputs() {
	for (unsigned int i = 0; i < playerKeyboardInputs.size(); i++) {				//sprawdzenie wszystkich elementó wektora 'playerKeyboardInputs'		
		int input = playerKeyboardInputs[i];										//pobranie sygna³u wejœciowego
		if (GAMEINPUT::KeyDown(input)) {											//jesli sygna³ jest aktywny [dany przycisk zosta³ wciœniêty]
			unsigned int n = GameObjects::players.size();							
			for (unsigned j = 0; j < n; j++) {										//przejrzenie wszystkich dostepnych postaci gracza
				if (GameObjects::players[j].IsEnabled() &&
					GameObjects::players[j].CheckKeyboardInputVal(input)) {			//jeœli postaæ jest 'odblokowana' i obs³uguje ona dany sygna³ wejœciowy
					GameObjects::players[j].AddKeyboardAction(input);				//dodaj akcjê powi¹zan¹ z tym sygna³em do wekotra akcji postaci gracza
				}
			}
		}
	}
}

//inicjalizacja sygna³ów wejœciowych z myszy dla postaci sterowanej przez gracza
void PlayerInput::InitilizePlayerMouseInputs() {
	for (unsigned int i = 0; i < GameObjects::players.size(); i++) {				//sprawdzenie wszystkich postaci z wektora postaci
		//pobranie wekotra relacji akcji i sygna³ów wejœciowych dla danej postaci
		std::vector<ACTION_INPUT> playerActionVec = GameObjects::players[i].GetActionMouseRelations();
		unsigned int n = playerActionVec.size();
		for (unsigned int j = 0; j<n; j++) {										//sprawdzenie wszystkich sygna³ów wejœciowych z wekotra akacja - wejœcie
			int val = playerActionVec[j].input;										//pobranie wartoœci sygna³u wejsciowego z wekotra akcja - wejœcie
			if (!CheckPlayerMouseInputsVector(val))									//jeœli dany sygna³ wejœciowy nie zosta³ jeszcze dodany do wektora sygna³ów wejœciowych z myszy dla postaci
				playerMouseInputs.push_back(val);									//dodaj sygna³ wejœciwoy do wektora
		}
	}
}

//sprawdzenie czy dany sygna³ ['val'] znajduje siê w wekotrze sygna³ów wejœciowych z myszy
bool PlayerInput::CheckPlayerMouseInputsVector(int val) {
	for (unsigned int i = 0; i < playerMouseInputs.size(); i++) {
		if (playerMouseInputs[i] == val) return true;								//jeœli TAK zwróæ TRUE
	}
	return false;																	//w przeciwnym wypadku - FALSE
}

//sprwadzenie czy dany sygna³ z myszy ['val'] wydarzy³ siê
bool PlayerInput::CheckMouseInput(int val) {
	POSITION mouseMove = GAMEINPUT::GetMouseMove();									//pobranie przesuniecia wzglêdnego myszy

	switch (val) {
	case INPUTCONST::MOUSE_RIGHT:													//przesuniêcie w PRAWO
		if (mouseMove.x > 0) return true;											//jesli wartoœæ przesuniêcia w osi x > 0 zwróæ TRUE
		else return false;															//w przeciwnym wypadu - FALSE
		break;

	case INPUTCONST::MOUSE_LEFT:													//przesuniêcie w LEWO
		if (mouseMove.x < 0) return true;											//jeœli wartoœæ przesuniêcia w osi x < 0 zwróæ TRUE
		else return false;															//w przeciwnym wypadku - FALSE
		break;

	case INPUTCONST::MOUSE_UP:														//przesuniêcie do GÓRY				
		if (mouseMove.y > 0) return true;											//jesli wartoœæ przesuniêcia w osi y > 0 zwróæ TRUE
		else return false;															//w przeciwnym wypadku - FALSE
		break;
		
	case INPUTCONST::MOUSE_DOWN:													//przesuniêcie w DÓ£
		if (mouseMove.y < 0) return true;											//jeœli wartoœæ przesuniêcia w osi y < 0 zwróæ TRUE
		else return false;															//w przeciwnym wypadku - FALSE	
		break;

	case INPUTCONST::MOUSE_LEFT_BUT:
		return GAMEINPUT::MouseButtonDown(INPUTCONST::MOUSE_LEFT_BUT);
		break;

	case INPUTCONST::MOUSE_RIGHT_BUT:
		return GAMEINPUT::MouseButtonDown(INPUTCONST::MOUSE_RIGHT_BUT);
		break;

	default:																		//w ka¿dym innym przypadku zwracana jest wartoœæ FALSE
		return false;
		break;
	}
}

//funkcja zwracja¹ca parametr przesuniêcia myszy w danej osi dla danego sygna³u wejsciowego 'val'
int PlayerInput::GetMouseInputParam(int val) {
	POSITION mouseMove = GAMEINPUT::GetMouseMove();									//pobranie przesuniêcia wzglêdnego myszy

	switch (val) {
	case INPUTCONST::MOUSE_RIGHT:													//przesuniecie w PRAWO
		return mouseMove.x;															//wartoœæ przesuniêcia myszy w osi X
		break;

	case INPUTCONST::MOUSE_LEFT:													//przesuniecie w LEWO
		return -mouseMove.x;														//wartoœæ przesuniecia myszy w osi X
		break;

	case INPUTCONST::MOUSE_UP:														//przesuniecie do GÓRY
		return mouseMove.y;															//wartoœæ przesuniecia myszy w osi Y
		break;

	case INPUTCONST::MOUSE_DOWN:													//przesuniecie w DÓ£
		return -mouseMove.y;														//wartoœæ przesuniêcia myszy w osi Y
		break;

	default:
		return 0;																	//w ka¿dym innym przypadku zwracana jest wartoœæ 0
		break;
	}

}

//sprawdzenie wszystkich sygna³ów z myszy zapisanych w wekotrze 'playerMouseInputs'
void PlayerInput::CheckAllMouseInputs() {
	POSITION mouseMove = GAMEINPUT::GetMouseMove();									//pobranie przesuniêcia wzglêdnego myszy
	for (unsigned int i = 0; i < playerMouseInputs.size(); i++) {				//sprawdŸ wszystkie sygna³y wejœciowe z myszy
		int input = playerMouseInputs[i];										//pobranie sygna³u wejœciowego
		if (CheckMouseInput(input)) {											//sprawdzenie czy dany sygna³ wejœciowy zaszed³
			int param = GetMouseInputParam(input);								//pobranie parametru przesuniêcia myszy
			unsigned int n = GameObjects::players.size();
			for (unsigned int j = 0; j < n; j++) {								//sprawdzenie wekotra postaci sterowanych przez gracza
				if (GameObjects::players[j].IsEnabled() &&
					GameObjects::players[j].CheckMouseInputVal(input)) {		//jesli dana postaæ jest 'odblokowana' i dany sygna³ wejœciowy znajduje siê w jej wektorze relacji akcja - wejœcie
					GameObjects::players[j].AddMouseAction(input, param);		//dodaj akcjê powi¹zan¹ z danym sygna³em wejœciwoym do wekotra akcji
				}
			}
		}
	}
}

//sprawdzenie sygna³ów wejœciowych w terminalu konsoli
void PlayerInput::CheckConsoleInput() {
	delay += static_cast<unsigned long>(PHYSICSMODULE::targetFrameTime);		//dodanie czasu trwania jednej klatki do licznika opóŸnienia wczytywania znaków							

	//wczytywanie znaków alfabetycznych
	if (GAMEINPUT::KeyDown(INPUTCONST::A))
		if(delay-terminalDelayChar[0] > TerminalReadDelayVal) {
		CONSOLE::GetTerminalInput('a');  terminalDelayChar[0] = delay;
	}
	if (GAMEINPUT::KeyDown(INPUTCONST::B))
		if(delay-terminalDelayChar[1] > TerminalReadDelayVal) {
		CONSOLE::GetTerminalInput('b'); terminalDelayChar[1] = delay;
	}
	if (GAMEINPUT::KeyDown(INPUTCONST::C))
		if(delay-terminalDelayChar[2] > TerminalReadDelayVal) {
		CONSOLE::GetTerminalInput('c'); terminalDelayChar[2] = delay;
	}
	if (GAMEINPUT::KeyDown(INPUTCONST::D))
		if(delay - terminalDelayChar[3] > TerminalReadDelayVal) {
		CONSOLE::GetTerminalInput('d'); terminalDelayChar[3] = delay;
	}
	if (GAMEINPUT::KeyDown(INPUTCONST::E))
		if(delay - terminalDelayChar[3] > TerminalReadDelayVal) {
		CONSOLE::GetTerminalInput('e'); terminalDelayChar[3] = delay;
	}
	if (GAMEINPUT::KeyDown(INPUTCONST::F))
		if(delay - terminalDelayChar[4] > TerminalReadDelayVal){
		CONSOLE::GetTerminalInput('f'); terminalDelayChar[4] = delay;
	}
	if (GAMEINPUT::KeyDown(INPUTCONST::G))
		if(delay - terminalDelayChar[5] > TerminalReadDelayVal) {
		CONSOLE::GetTerminalInput('g'); terminalDelayChar[5] = delay;
	}
	if (GAMEINPUT::KeyDown(INPUTCONST::H))
		if(delay - terminalDelayChar[6] > TerminalReadDelayVal) {
		CONSOLE::GetTerminalInput('h'); terminalDelayChar[6] = delay;
	}
	if (GAMEINPUT::KeyDown(INPUTCONST::I))
		if(delay - terminalDelayChar[7] > TerminalReadDelayVal) {
		CONSOLE::GetTerminalInput('i'); terminalDelayChar[7] = delay;
	}
	if (GAMEINPUT::KeyDown(INPUTCONST::J))
		if(delay - terminalDelayChar[8] > TerminalReadDelayVal) {
		CONSOLE::GetTerminalInput('j'); terminalDelayChar[8] = delay;
	}
	if (GAMEINPUT::KeyDown(INPUTCONST::K))
		if(delay - terminalDelayChar[9] > TerminalReadDelayVal) {
		CONSOLE::GetTerminalInput('k'); terminalDelayChar[9] = delay;
	}
	if (GAMEINPUT::KeyDown(INPUTCONST::L))
		if(delay - terminalDelayChar[10] > TerminalReadDelayVal) {
		CONSOLE::GetTerminalInput('l'); terminalDelayChar[10] = delay;
	}
	if (GAMEINPUT::KeyDown(INPUTCONST::M))
		if(delay - terminalDelayChar[11] > TerminalReadDelayVal) {
		CONSOLE::GetTerminalInput('m'); terminalDelayChar[11] = delay;
	}
	if (GAMEINPUT::KeyDown(INPUTCONST::N))
		if(delay - terminalDelayChar[12] > TerminalReadDelayVal) {
		CONSOLE::GetTerminalInput('n'); terminalDelayChar[12] = delay;
	}
	if (GAMEINPUT::KeyDown(INPUTCONST::O))
		if(delay - terminalDelayChar[13] > TerminalReadDelayVal) {
		CONSOLE::GetTerminalInput('o'); terminalDelayChar[13] = delay;
	}
	if (GAMEINPUT::KeyDown(INPUTCONST::P))
		if(delay - terminalDelayChar[14] > TerminalReadDelayVal) {
		CONSOLE::GetTerminalInput('p'); terminalDelayChar[14] = delay;
	}
	if (GAMEINPUT::KeyDown(INPUTCONST::R))
		if(delay - terminalDelayChar[15] > TerminalReadDelayVal) {
		CONSOLE::GetTerminalInput('r'); terminalDelayChar[15] = delay;
	}
	if (GAMEINPUT::KeyDown(INPUTCONST::S))
		if(delay - terminalDelayChar[16] > TerminalReadDelayVal) {
		CONSOLE::GetTerminalInput('s'); terminalDelayChar[16] = delay;
	}
	if (GAMEINPUT::KeyDown(INPUTCONST::T))
		if(delay - terminalDelayChar[17] > TerminalReadDelayVal) {
		CONSOLE::GetTerminalInput('t'); terminalDelayChar[17] = delay;
	}
	if (GAMEINPUT::KeyDown(INPUTCONST::U))
		if(delay - terminalDelayChar[18] > TerminalReadDelayVal) {
		CONSOLE::GetTerminalInput('u'); terminalDelayChar[18] = delay;
	}
	if (GAMEINPUT::KeyDown(INPUTCONST::V))
		if(delay - terminalDelayChar[19] > TerminalReadDelayVal) {
		CONSOLE::GetTerminalInput('v'); terminalDelayChar[19] = delay;
	}
	if (GAMEINPUT::KeyDown(INPUTCONST::W))
		if(delay - terminalDelayChar[20] > TerminalReadDelayVal) {
		CONSOLE::GetTerminalInput('w'); terminalDelayChar[20] = delay;
	}
	if (GAMEINPUT::KeyDown(INPUTCONST::X))
		if(delay - terminalDelayChar[21] > TerminalReadDelayVal) {
		CONSOLE::GetTerminalInput('x'); terminalDelayChar[21] = delay;
	}
	if (GAMEINPUT::KeyDown(INPUTCONST::Y))
		if(delay - terminalDelayChar[22] > TerminalReadDelayVal) {
		CONSOLE::GetTerminalInput('y'); terminalDelayChar[22] = delay;
	}
	if (GAMEINPUT::KeyDown(INPUTCONST::Z))
		if(delay - terminalDelayChar[23] > TerminalReadDelayVal) {
		CONSOLE::GetTerminalInput('z'); terminalDelayChar[23] = delay;
	}

	//wczytywanie cyfr
	if (GAMEINPUT::KeyDown(INPUTCONST::_0))
		if(delay - terminalDelayNum[0] > TerminalReadDelayVal) {
		CONSOLE::GetTerminalInput('0'); terminalDelayNum[0] = delay;
	}
	if (GAMEINPUT::KeyDown(INPUTCONST::_1))
		if(delay - terminalDelayNum[1] > TerminalReadDelayVal) {
		CONSOLE::GetTerminalInput('1'); terminalDelayNum[1] = delay;
	}

	//wczytywanie znaków specjalnych
	if (GAMEINPUT::KeyDown(INPUTCONST::BACKSPACE))
		if(delay - terminalDelaySpecial[0] > TerminalReadDelayVal) {
		CONSOLE::GetTerminalInput(0x08); terminalDelaySpecial[0] = delay;
	}
	if (GAMEINPUT::KeyDown(INPUTCONST::DECIMAL_POINT))
		if(delay - terminalDelaySpecial[1] > TerminalReadDelayVal) {
		CONSOLE::GetTerminalInput('.'); terminalDelaySpecial[1] = delay;
	}
	if (GAMEINPUT::KeyDown(INPUTCONST::EQUALS))
		if(delay - terminalDelaySpecial[2] > TerminalReadDelayVal) {
		CONSOLE::GetTerminalInput('='); terminalDelaySpecial[2] = delay;
	}
	if (GAMEINPUT::KeyDown(INPUTCONST::ENTER))
		if(delay - terminalDelaySpecial[3] > TerminalReadDelayVal) {
		CONSOLE::GetTerminalInput(0x0A); terminalDelaySpecial[3] = delay;
	}
}