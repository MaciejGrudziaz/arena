#include "DynamicCharacter.h"

DynamicCharacter::DynamicCharacter(const std::string& filename):Character(filename) {
	SetDefaultValues();
}

DynamicCharacter::DynamicCharacter(const std::string& filename, const std::string& Name, const Vector& Position, const Vector& Orientation):
	Character(filename,Name,Position,Orientation){
	SetDefaultValues();
}

//ustawienie domyœlnych wartoœci dla sk³adowych klasy
void DynamicCharacter::SetDefaultValues() {
	position.y += collisionBox.midPoint.y;
	speed = 1.0;
	turnSensitivity = 1.0;
	previousPosition = position;
	nextPosition = position;
	previousOrientation = orientation;
	weight = 1;
	jumpStrength = 25.0;
	lookAngle = 0.0;
}

//dodanie akcji
void DynamicCharacter::AddAction(ACTION& action) {
	switch (action.subActionName) {
	case AIR:																				//jeœli akcja pochodzi z subklasy 'AIR'
		if (action.name == FALL) {															//jeœli jest to akcja spadania
			if (!(actions.size() > 0 && actions[0]->subActionName == AIR)) {				//jeœli nie ma aktualnie akcji spadania obecnej dla postaci
				ActionPtr ptr(new FALL_ACTION(static_cast<FALL_ACTION&>(action).position, static_cast<FALL_ACTION&>(action).shift));
				actions.push_back(ptr);														//stwórz tak¹ akcjê i dodaj j¹ do wektora akcji

				if (actions.size() > 1) {													//umieœæ nowostworzon¹ akcjê spadania na pocz¹tku wektora akcji
					ActionPtr tmp = actions[0];
					actions[0] = actions[actions.size() - 1];
					actions[actions.size() - 1] = tmp;
				}
			}
		}
		break;
	case ORIENTATION: {																		//jeœli akcja pochodzi z subklay 'ORIENTATION'
		ActionPtr ptr(new LOOK_ACTION(static_cast<LOOK_ACTION&>(action).name, static_cast<LOOK_ACTION&>(action).mouseMove));
		actions.push_back(ptr);																//wykonaj rzutowanie akcji w górê hierarchi i dodaj j¹ do wektora wszystkich akcji
		}
		break;

	default:																				//domyœlnie
		ActionPtr ptr(new ACTION(action));													//dodaj akcjê do wektora akcji
		actions.push_back(ptr);
		break;
	}
}

//sprawdzenie czy w wekotrze akcji zanjduj¹ siê akcje do wykonania
bool DynamicCharacter::ActionsAvailable() {
	if (actions.size() > 0) return true;
	else return false;
}

//sprawdzenie czy w wektorze akcji znajduje siê akcja z subklasy 'AIR'
bool DynamicCharacter::CheckAirAction() {
	if (actions.size() > 0) {
		if (actions[0]->subActionName == AIR)
			return true;
		else return false;
	}
	else return false;
}

//usuniêcie, jesli istneije akcji z subklay 'AIR'
bool DynamicCharacter::RemoveAirAction() {
	if (actions.size() > 0) {
		if (actions[0]->subActionName == AIR) {
			actions.erase(actions.begin());
			return true;
		}
		else return false;
	}
	else return false;
}

//wykonanie wszystkich akcji umieszczonych w wektorze
void DynamicCharacter::PerformAllActions() {
	if (actions.size() > 0) {
		unsigned moveCount = 0;
		if (actions.size() >= 2) {																//sprawdzenie czy istniej¹ wiêcej ni¿ dwie akcje przemieszczenia do wykonania
			for (ActionPtr ptr : actions) {
				if (ptr->subActionName == MOVEMENT)
					moveCount++;
			}
		}

		double playerSpeed = speed;
		if (moveCount >= 2)																		//jeœli tak zmniejsz prêdkoœæ postaci o po³owê
			speed /= 2.0;

		if (actions[0]->subActionName == AIR) {													//jeœli istnieje akcja z subklasy 'AIR'
			for (ActionPtr ptr : actions) {
				if (ptr->subActionName != MOVEMENT)												//nie wykonuj akcji zwi¹zanych z ruchem
					PerformAction(ptr);
			}
			actions.erase(actions.begin() + 1, actions.end());									//po wykonaniu wszystkich akci usuñ je z wektora oprócz akcji z subklasy 'AIR'
		}
		else {
			for (ActionPtr ptr : actions) {														//jeœli nie istnieje akcja z subklasy 'AIR'
				PerformAction(ptr);																//wykonaj wszystkie akcje
			}
			actions.clear();																	//wyczyœæ ca³y wektor akcji
		}

		speed = playerSpeed;																	//przywróæ pierwotn¹ wartoœæ prêdkoœci postaci
	}
}

//aktualizacja macierzy przekszta³ceñ postaci
void DynamicCharacter::UpdateTransformMatrix() {
	Vector angle(orientation); angle = angle / 360.0 * 2.0 * 3.1415;						//zamiana wartoœci k¹tów obrotu wokó³ danych osi na radiany
	//wyzanczenie kwaternionów dla ka¿dej osi obrotu
	Quaternion q_x(cos(angle.x / 2.0), Vector(1, 0, 0)*sin(angle.x / 2.0));
	Quaternion q_y(cos(angle.y / 2.0), Vector(0, 1, 0)*sin(angle.y / 2.0));
	Quaternion q_z(cos(angle.z / 2.0), Vector(0, 0, 1)*sin(angle.z / 2.0));
	//z³o¿enie wszystkich obrotów i wyznaczenie kwaternionu orientacji
	Quaternion Q = q_z.CrossProduct(q_y);
	Q.CrossProduct(q_x);

	transform = Q.CreateMatrix();															//wyznacznie macierzy jednorodnej z kwaternionu orientacji

	for (unsigned int i = 0; i < 3; i++)
		transform[i][3] = position[i];														//dodanie do macierzy jednorodnej wektor przesuniêcia
}

//----------------------------------------------------WYKONANIE AKCJI----------------------------------------------------
//---------------------------------------funkcja wykonuj¹ca dan¹ akcjê ['Action']----------------------------------------
//----------wszystkie akcje zdefiniowane w typie wyliczeniowym ACTION_NAME musz¹ byæ zdefiniowane w tej funkcji----------
void DynamicCharacter::PerformAction(ActionPtr Action) {
	double t = PHYSICSMODULE::targetFrameTime /1000000.0;									//predefiniowana sta³a wartoœæ trwania jednej iteracji symulacji fizycznych
	double angle = orientation.y / 360.0 * 2.0 * 3.1415;									//wartoœæ k¹ta obrotu postaci wokó³ osi y [w radianach]
																							//2.0 * 3.1415 -> 2 * PI
	double G = GamePhysicsConst::GetInstance()->GetG();
	
	switch (Action->name) {

	case MOVE_RIGHT: {																		//przemieszczenie w prawo
																							//previousPosition = position;
		double s = speed*t;																	//s - wartoœæ przemieszczenia, speed - prêdkoœæ postaci, t/1000000.0 - czas trwania poprzedniej klatki w sek.
		nextPosition.x -= s*cos(-angle);
		nextPosition.z -= s*sin(-angle);
	}
					 break;

	case MOVE_LEFT: {																		//przemieszczenie w lewo
																							//previousPosition = position;
		double s = speed*t;																	//s - wartoœæ przemieszczenia, speed - prêdkoœæ postaci, t/1000000.0 - czas trwania poprzedniej klatki w sek.
		nextPosition.x += s*cos(-angle);
		nextPosition.z += s*sin(-angle);
	}
					break;

	case MOVE_FORWARD: {																	//przemieszczenie do przodu
																							//previousPosition = position;
		double s = speed*t;																	//s - wartoœæ przemieszczenia, speed - prêdkoœæ postaci, t/1000000.0 - czas trwania poprzedniej kaltki w sek.
		nextPosition.x -= s*sin(-angle);
		nextPosition.z += s*cos(-angle);
	}
					   break;

	case MOVE_BACKWARD: {																	//przemieszczenie do ty³u
																							//previousPosition = position;
		double s = speed*t;																	//s - wartoœæ przemieszczenia, speed - prêdkoœæ postaci, t/1000000.0 - czas trwania poprzedniej klatki w sek.
		nextPosition.x += s*sin(-angle);
		nextPosition.z -= s*cos(-angle);
	}
						break;

	case TURN_RIGHT: {																		//obrót w prawo
		LOOK_ACTION& look = static_cast<LOOK_ACTION&>(*Action);
		previousOrientation = orientation;
		double yRot = look.mouseMove * t * turnSensitivity * 360.0;							//yRot - k¹t obrotu, Action.param[0] - wartoœæ przemieszczenia myszy w poprzedniej klatce
																							//t/1000000.0 - czas trwania poprzedniej klatki w sek., turnSensitivity - czu³oœæ obrotu postaci
		orientation.y -= yRot;																//ustawienie nowej wartoœci orientacji postaci gracza w osi Y
		if (orientation.y < 0)																//sprawdzenie czy wartoœæ orientacji przekroczy³a zakres
			orientation.y = 360.0;
	}
					 break;

	case TURN_LEFT: {																		//obrót w lewo
		LOOK_ACTION& look = static_cast<LOOK_ACTION&>(*Action);
		previousOrientation = orientation;
		double yRot = look.mouseMove * t * turnSensitivity * 360.0;							//yRot - k¹t obrotu w osi Y, Action.param[0] - wartoœæ przemieszczenia myszy w poprzedniej klatce w osi X
																							//t/1000000.0 - czas trwania poprzedniej klatki w sek., turnSensitivity - czu³oœæ obrotu postaci
		orientation.y += yRot;																//ustawienie nowej wartoœci orientacji postaci w osi Y
		if (orientation.y > 360.0)															//sprawdzenie czy wartoœæ orientacji przekroczy³a zakres
			orientation.y = 0.0;
	}
					break;

	case LOOK_UP: {																			//spojrzenie w górê
		LOOK_ACTION& look = static_cast<LOOK_ACTION&>(*Action);
		double xRot = look.mouseMove  * t * turnSensitivity * 360.0;						//xRot - k¹t obrotu w osi X, Action.param[0] - wartoœæ przemieszczenia myszy w poprzedniej klatce w osi Y
																							//t/1000000.0 - czas trwania poprzedniej klatki w sek., turnSensitivity - czu³oœæ obrotu postaci
		lookAngle -= xRot;
		if (lookAngle < -90.0) lookAngle = -90.0;
	}
				  break;

	case LOOK_DOWN: {																		//spojrzenie w dó³
		LOOK_ACTION& look = static_cast<LOOK_ACTION&>(*Action);
		double xRot = look.mouseMove * t * turnSensitivity*360.0;							//xRot - k¹t obrotu w osi X, Action.param[0] - wartoœæ przemieszczenia myszy w poprzedniej klatce w osi Y
																							//t/1000000.0 - czas trwania poprzedniej klatki w sek., turnSensitivity - czu³oœæ obrotu postaci
		lookAngle += xRot;
		if (lookAngle > 90) lookAngle = 90.0;
	}
					break;

	case FALL: {																			//spadanie
		FALL_ACTION& fall = static_cast<FALL_ACTION&>(*Action);								//rzutowanie w górê hierarchii w celu uzyskania w³aœciwgo obiektu akcji
		fall.time += t;																		//aktualizacja czasu spadania
		nextPosition.x += fall.shift.x;														//obliczenie nastêpnej pozycji postaci
		nextPosition.z += fall.shift.z;
		nextPosition.y = fall.position.y - 0.5*G*(fall.time*fall.time) * weight;
	}
			   break;

	case JUMP: {																			//skok
		JUMP_ACTION& jump = static_cast<JUMP_ACTION&>(*Action);								//rzutowanie w górê hierarchii w celu uzyskania w³aœciwego obiektu akcji
		jump.time += t;																		//aktualizacja czasu skoku
		nextPosition.x += jump.shift.x / 1.33;												//obliczenie nastêpnej pozycji postaci
		nextPosition.z += jump.shift.z / 1.33;
		nextPosition.y = jump.position.y + jump.shift.y*jump.time - 0.5*G*(jump.time*jump.time)*weight;
	}
			   break;

	default:																				//wartoœæ domyœlna
		PerformAdditionalAction(Action);													//wykonanie akcji niestandardowej
		break;
	}
}

//dostosowanie akcji przemieszczenia
void DynamicCharacter::AdjustMoveAction(Vector k) {
	nextPosition = position + k;
}

//aktualizacja pozycji postaci
void DynamicCharacter::UpdatePosition() {
	previousPosition = position;
	position = nextPosition;
	nextPosition = position;
}

//aktualizacja orientacji postaci
void DynamicCharacter::UpdateOrientation() {
	previousOrientation = orientation;
}

void DynamicCharacter::Update() {
	UpdatePosition();
	UpdateOrientation();
}
