#pragma once
#include <memory>
#include "Character.h"
#include "GAMEINPUT_CONSTANTS.h"
#include "TIMER.h"
#include "LOGFILE.h"
#include "Camera.h"
#include "SpecialMath.h"
#include "Quaternion.h"
#include "Action.h"
#include "DynamicCharacter.h"
#include "Gun.h"

//-------------------------------------------------------------------------------------
//---------------------------------------PLAYER----------------------------------------
//--------------------klasa opisuj¹ca postaæ sterowan¹ przez gracza--------------------
//-------------------------------------------------------------------------------------

typedef std::shared_ptr<Weapon> WeaponPtr;

class Player : public DynamicCharacter {														//klasa dziedziczy po obiekcie postaci [elementy niestatyczne w grze]
	std::vector<ACTION_INPUT> action_keyboard_relations;										//wektor przechowuj¹cy wszystkie relacje akcja - sygna³ wejœciowy z klawiatury dla danej postaci
	std::vector<ACTION_INPUT> action_mouse_relations;											//wektor przechowuj¹cy wszystkie relacje akcja - sygna³ wejœciowy z myszy dla danej postaci

	WeaponPtr weapon;

public:
	//konstruktory
	Player(const std::string& filename);
	Player(const std::string& filename, const std::string& Name, const Vector& Position, const Vector& Orientation);

	void AddWeapon(const std::string& weaponName);												//dodanie broni dla postaci gracza

	void PerformAdditionalAction(ActionPtr Action);												//dodatkowe akcje postaci gracza

	bool CheckKeyboardInputVal(int val);														//sprawdzenie czy dany sygna³ wejœciowy znajduje siê w wektorze relacji akcja - sygna³ wejœciowy z klawiatury
	void AddKeyboardAction(int inputVal);														//dodanie nowej akcji do wektora przypisanej do danego sygna³u wejœciowego 'inputVal' z klawiatury

	bool CheckMouseInputVal(int val);															//sprawdzenie czy dany sygna³ wejœciowy znajduje siê w wektorze relacji akcja - sygna³ wejœciowy z myszy
	void AddMouseAction(int inputVal,int Param);												//dodanie nowej akcji do wektora przypisanej do danego sygna³u wejœciowego 'inputVal' z myszy	

	//pobranie wekotra akcja - sygna³ wejœciowy z klawiatury
	std::vector<ACTION_INPUT> GetActionKeyboardRelations() { return action_keyboard_relations; }		

	//pobranie wektora akcja - sygna³ wejœciowy z myszy
	std::vector<ACTION_INPUT> GetActionMouseRelations() { return action_mouse_relations; }

	WeaponPtr GetWeapon() { return weapon; }

	void UpdatePosition();
	void UpdateOrientation();
	void Update();

	void DrawModel();
};
