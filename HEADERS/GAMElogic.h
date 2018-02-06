#pragma once
#include "GAMEINPUT.h"
#include "Camera.h"
#include "GameObjects.h"
#include "TIMER.h"
#include "LOGFILE.h"
#include "CONSOLE.h"
#include "WeaponDatabase.h"
#include "CharacterRegister.h"

#include "PhysicsModule.h"
#include "GameGraphics.h"

//---------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------G£ÓWNA LOGIKA GRY--------------------------------------------------
//--------------------------------------klasa odpowiadaj¹ca za g³ówn¹ logikê gry---------------------------------------
//-----w klasie tej definiuje siê mapy [obiekty statyczne], postacie [obiekty dynamiczne], kamerê oraz oœwietlenie-----
//---------------------------------------------------------------------------------------------------------------------

class GAMElogic {
	GAMElogic();

	static void InitilizeCamera();
	static void InitializeOpenGLProperties();						//inicjalizacja podstawowych w³aœciwoœci okna OpenGL
	static void InitializeLights();									//inicjalizacja Ÿróde³ œwiat³a
	static void InitializeMaterials();								//inicjalizacja materia³ów [wersja prototypowa]
	static void InitializePlayer();									//inicjalizacja postaci gracza [klasa Player]
	static void InitializeBots();									//inicjalizacja botów
	static void InitializeCharacters();								//inicjalizacja postaci [klasa Character]
	static void InitializeMaps();									//inicjalizacja map [klasa Map]
	static void InitilizeLogFiles();								//inicjalizacja plików logów
	static void InitilizeTimers();									//inicjalizacja timerów

	static void AddNewBot(const Vector& position, const Vector& orientation);

public:
	static void InitializeGameLogic();								//inicjalziacja logiki gry

	static void RefreshGameLogicInitialization();					//ponowna inicjalizacja logiki gry [wykonywana w razie 'zniszczenia' okna gry]

	static void ProccessFrame();									//przetworzenie pojedynczej klatki gry

};