#include "BotsActions.h"

//obliczenie nowych zachowañ botów ['MainLogic()']
void BotsActions::CalculateNewMoves() {
	for (unsigned i = 0; i < CharacterRegister::GetBotCount(); i++) {													//dla wszystkich zarejestrowanych botów
		Bot* bot = CharacterRegister::GetBot(i);																		//pobranie wskaŸnika na obiekt bota
		if (bot->IsEnabled())																							//jeœli jest odblokowany
			bot->MainLogic();																							//wykonaj g³ówn¹ logikê bota
	}
}

//wykonanie wszystkich oczekuj¹cych akcji botów
void BotsActions::PerformAllActions() {
	for(unsigned i=0;i<CharacterRegister::GetBotCount();i++){															//dla wszystkich zarejestroanych botów
		Bot* bot = CharacterRegister::GetBot(i);																		//pobranie wskaŸnika na obiekt bota
		if (bot->IsEnabled())																							//jeœli jest odblokowany
			bot->PerformAllActions();																					//wykonaj wszystkie oczekuj¹ce akcje bota
	}
}

//aktualizacja bry³ kolizji botów
void BotsActions::UpdateCollisionBox() {
	for(unsigned i=0;i<CharacterRegister::GetBotCount();i++){															//dla wszystkich zarejestrowanych botów
		Bot* bot = CharacterRegister::GetBot(i);																		//pobranie wskaŸnika na obiekt bota

		Vector pos, prevPos, orient, prevOrient;
		pos = bot->getPosition();																						//wczytanie obecnej pozycji bota
		prevPos = bot->GetPreviousPosition();																			//wczytanie poprzedniej pozycji bota
		orient = bot->getOrientation();																					//wczytanie orientacji bota
		prevOrient = bot->GetPreviousOrientation();																		//wczytanie poprzedniej orientacji bota

		if (pos != prevPos || orient != prevOrient) {																	//jeœli poz. lub orient. bota nie zgadzaj¹ siê z poprzedni¹ poz. lub orient.
			bot->UpdateTransformMatrix();																				//aktualizuj macierz przekszta³cenia bota
			bot->UpdateCollisionBoxVertices();																			//aktualizuj wspó³rzêdne wierzcho³ków bry³ kolizji bota
			bot->UpdateCollisionBoxNormals();																			//aktualizuj normalne bry³ kolizji bota
		}
	}
}

//wykonanie ca³ej logiki botów
void BotsActions::PerformBotLogic() {
	CalculateNewMoves();																								//obliczenie nowych zachowañ botów
	PerformAllActions();																								//wykonanie wszystkich oczekuj¹cych akcji
	UpdateCollisionBox();																								//aktulizacja bry³ kolizji
}