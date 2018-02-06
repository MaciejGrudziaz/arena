#include "PhysicsModule.h"

double PhysicsModule::lastFrameTime = 0.0;
double PhysicsModule::frameResidue = 0.0;

//wykonanie wszystkich akcji i obliczeñ fizyki
void PhysicsModule::ProcessActionsAndPhysics() {
	lastFrameTime = TIMER::GetTime("FPS");																		//pobranie czasu trwania ostatniej klatki
	unsigned physicsIterations = static_cast<unsigned>(lastFrameTime / PHYSICSMODULE::targetFrameTime);			//obliczenie iloœci iteracji obliczeñ fizyki do wykonania w tej klatce
	frameResidue += (lastFrameTime - static_cast<double>(physicsIterations)*PHYSICSMODULE::targetFrameTime);	//obliczenie 'pozosta³oœci' z czasu po wykonaniu obliczeñ fizyki
	if (frameResidue > PHYSICSMODULE::targetFrameTime) {														//jeœli 'pozosta³oœci' czasu sumuj¹ siê do domyœ. czasu trwania jednej klatki
		physicsIterations++;																					//dodaj jeszcze jedn¹ iteracje fizyki
		frameResidue -= PHYSICSMODULE::targetFrameTime;															//aktualizuj 'pozosta³oœci' czasu obliczeñ fizycznych
	}

	for (unsigned i = 0; i < physicsIterations; i++) {															//wykonanie iteracji obliczen fizyki
		TIMER::StartTimeMeasurment("Input");
		PlayerInput::ProccessInput();																			//przetworzenie sygna³ów wejœciowych
		TIMER::StopTimeMeasurment("Input");

		BotsActions::PerformBotLogic();																			//wykonanie logiki botów

		TIMER::StartTimeMeasurment("Physics");
		GamePhysics::ProccessPhysics();																			//wykonanie podstawowych modeli fizyki
		AdditionalPhysics::ProcessAllModels();																	//wykonanie dodatkowych modeli fizyki
		TIMER::StopTimeMeasurment("Physics");

		UpdateCharacters();
	}
}

//rejestracja dodatkowych modeli fizyki
void PhysicsModule::ModelsRegistration() {
	AdditionalPhysics::RegisterModel(BulletCollision::GetInstance(), 0);										//model kolizji pocisków										
}

//aktualizacja stanu postaci œwiata gry
void PhysicsModule::UpdateCharacters() {	
	Map* map = CharacterRegister::GetMap();																		//wskaŸnik na aktualny obiekt mapy
	
	//wywo³anie metody Update [oraz 'SetSectorIdx' dla postaci dynamicznych] dla ka¿dej zarejestrowanej postaci
	for (unsigned i = 0; i < CharacterRegister::GetStaticCharacterCount(); i++)									
		if (CharacterRegister::GetStaticCharacter(i)->IsEnabled())
			CharacterRegister::GetStaticCharacter(i)->Update();

	for (unsigned i = 0; i < CharacterRegister::GetDynamicCharacterCount(); i++)
		if (CharacterRegister::GetDynamicCharacter(i)->IsEnabled()) {
			DynamicCharacter* charPtr = CharacterRegister::GetDynamicCharacter(i);
			charPtr->Update();
			charPtr->SetSectorIdx(map->GetSectorIdx(charPtr->getPosition()));
		}

	for (unsigned i = 0; i < CharacterRegister::GetBotCount(); i++)
		if (CharacterRegister::GetBot(i)->IsEnabled()) {
			Bot* botPtr = CharacterRegister::GetBot(i);
			botPtr->Update();
			botPtr->SetSectorIdx(map->GetSectorIdx(botPtr->getPosition()));
		}

	if (CharacterRegister::GetPlayer()->IsEnabled()) {
		Player* playerPtr = CharacterRegister::GetPlayer();
		playerPtr->Update();
		playerPtr->SetSectorIdx(map->GetSectorIdx(playerPtr->getPosition()));
	}
}