#pragma once
#include <memory>
#include "Character.h"
#include "Action.h"
#include "Quaternion.h"
#include "TIMER.h"
#include "Camera.h"
#include "LOGFILE.h"
#include "PhysicsModuleConst.h"
#include "GamePhysicsConst.h"

//-------------------------------------------------------------------------------------
//------------------------------KLASA POSTACI DYNAMICZNEJ------------------------------
//----------------------------------[klasa wirtualna]----------------------------------
//-------------------------------------------------------------------------------------

typedef std::shared_ptr<ACTION> ActionPtr;

class DynamicCharacter:public Character{
protected:
	double speed;																				//prêdkoœæ przemieszczenia postaci
	double turnSensitivity;																		//czu³oœæ obrotu i rozgl¹dania siê postaci	
	//double G;																					//wartoœæ przyspieszenia ziemskiego dla postaci
	double weight;
	double jumpStrength;																		//wartoœæ si³y wyskoku

	double lookAngle;																			//k¹t 'widzenia' postaci [odchylenie kamery wzd³u¿ osi 'x' uk³ady lokalnego modelu]

	std::vector<ActionPtr> actions;																//wektor przechowuj¹cy wszystkie akcje które maj¹ zostaæ wykonane przez postaæ

public:
	//konstruktory
	DynamicCharacter(const std::string& filename);
	DynamicCharacter(const std::string& filename, const std::string& Name, const Vector& Position, const Vector& Orientation);

	void SetDefaultValues();																	//ustawienie domyœlnych wartoœci dla sk³adowych klasy

	void AddAction(ACTION& action);																//dodanie akcji

	bool ActionsAvailable();																	//sprawdzenie czy w wekotrze 'actions' znajduj¹ siê akcje do wykonania

	bool CheckAirAction();																		//sprawdzenie czy istnieje akcja z subklasy 'AIR'

	bool RemoveAirAction();																		//usuniêcie istniej¹cej akcji z subklasy 'AIR'

	void PerformAction(ActionPtr Action);														//funkcja wykonywuj¹ca dan¹ akcjê 'Action' postaci
	virtual void PerformAdditionalAction(ActionPtr Action) = 0;
	void PerformAllActions();																	//wykonanie wszystkich akcji z wekotra 'actions' postaci

	void UpdateTransformMatrix();																//aktualizacja macierzy przekszta³ceñ postaci

	void AdjustMoveAction(Vector k);															//dostosowanie akcji przemieszczenia

	void UpdatePosition();																		//aktualizacja pozycji
	void UpdateOrientation();																	//aktualizacja orientacji

	void Update();

	//pobranie wartoœci sk³adowych klasy
	double GetSpeed() { return speed; }															//pobranie wartoœci prêdkoœci postaci gracza
	double GetTurnSensitivity() { return turnSensitivity; }										//pobranie wartoœci czu³oœci obrotu postaci gracza
	double GetWeight() { return weight; }
	double GetJumpStrength() { return jumpStrength; }
	//IdxPair GetSectorIdx() { return sectorIdx; }
	double GetLookAngle() { return lookAngle; }

	//ustwienie odpowiednich wartoœci sk³adowych klasy
	void SetSpeed(double Speed) { speed = Speed; }												//ustawienie wartoœci prêdkoœci postaci gracza
	void SetTurnSensitivity(double Sensitivity) { turnSensitivity = Sensitivity; }				//ustawienie wartoœci czu³oœci obrotu gracza
	void SetWeight(double _weight) { weight=_weight; }
	void SetJumpStrength(double JumpStrength) { jumpStrength = JumpStrength; }
	void SetSectorIdx(const IdxPair& newIdx) { sectorIdx = newIdx; }
};