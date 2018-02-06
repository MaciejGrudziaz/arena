#pragma once
#include "CharacterRegister.h"

//-----------------------------------------------------------------------------------------------------
//------------------------------KLASA WYKONUJ¥CA AKCJE DLA OBIEKTÓW BOTÓW------------------------------
//-----------------------------------------------------------------------------------------------------

class BotsActions {
	static void CalculateNewMoves();																					//obliczenie nowych zachowañ botów ['MainLogic()']
			
	static void PerformAllActions();																					//wykonanie wszystkich oczekuj¹cych akcji botów

	static void UpdateCollisionBox();																					//aktualizacja bry³ kolizji botów

public:
	static void PerformBotLogic();																						//wykonanie ca³ej logiki botów
};