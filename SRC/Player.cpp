#include "Player.h"

//konstruktor pobieraj¹cy jedynie nazwê pliku przechowuj¹cego model postaci [reszcie parametrów przypisuje siê wartoœci domyœlne]
Player::Player(const std::string& filename) : DynamicCharacter(filename) {									//wywo³anie odpowiedniego konstruktora klasy Character
	//wype³nienie wektora akcja - wejœcie wartoœciami domyœlnymi
	//------------------------------KLAWIATURA------------------------------
	action_keyboard_relations.push_back(ACTION_INPUT(MOVE_LEFT, MOVEMENT, INPUTCONST::A));
	action_keyboard_relations.push_back(ACTION_INPUT(MOVE_RIGHT, MOVEMENT, INPUTCONST::D));
	action_keyboard_relations.push_back(ACTION_INPUT(MOVE_FORWARD, MOVEMENT, INPUTCONST::W));
	action_keyboard_relations.push_back(ACTION_INPUT(MOVE_BACKWARD, MOVEMENT, INPUTCONST::S));
	action_keyboard_relations.push_back(ACTION_INPUT(JUMP, AIR, INPUTCONST::SPACE));
	action_keyboard_relations.push_back(ACTION_INPUT(RELOAD, OTHER, INPUTCONST::R));
	//---------------------------------MYSZ---------------------------------
	action_mouse_relations.push_back(ACTION_INPUT(TURN_RIGHT, ORIENTATION, INPUTCONST::MOUSE_RIGHT));
	action_mouse_relations.push_back(ACTION_INPUT(TURN_LEFT, ORIENTATION, INPUTCONST::MOUSE_LEFT));
	action_mouse_relations.push_back(ACTION_INPUT(LOOK_UP, ORIENTATION, INPUTCONST::MOUSE_UP));
	action_mouse_relations.push_back(ACTION_INPUT(LOOK_DOWN, ORIENTATION, INPUTCONST::MOUSE_DOWN));
	action_mouse_relations.push_back(ACTION_INPUT(SHOOT, OTHER, INPUTCONST::MOUSE_LEFT_BUT));

	lookAngle = Camera::GetOrientation().x;
}

//konstruktor pobieraj¹cy nazwê pliku, nazwê, pozycjê oraz orientacjê postaci
Player::Player(const std::string& filename, const std::string& Name, const Vector& Position, const Vector& Orientation) :
	DynamicCharacter(filename, Name, Position, Orientation) {									//wywo³anie odpwiedniego konstrukotra klasy DynamicCharacter
	//wype³nienie wekotra akcja - wejœcie wartoœciami domyœlnymi
	//------------------------------KLAWIATURA------------------------------
	action_keyboard_relations.push_back(ACTION_INPUT(MOVE_LEFT, MOVEMENT, INPUTCONST::A));
	action_keyboard_relations.push_back(ACTION_INPUT(MOVE_RIGHT, MOVEMENT, INPUTCONST::D));
	action_keyboard_relations.push_back(ACTION_INPUT(MOVE_FORWARD, MOVEMENT, INPUTCONST::W));
	action_keyboard_relations.push_back(ACTION_INPUT(MOVE_BACKWARD, MOVEMENT, INPUTCONST::S));
	action_keyboard_relations.push_back(ACTION_INPUT(JUMP, AIR, INPUTCONST::SPACE));
	action_keyboard_relations.push_back(ACTION_INPUT(RELOAD, OTHER, INPUTCONST::R));
	//---------------------------------MYSZ---------------------------------
	action_mouse_relations.push_back(ACTION_INPUT(TURN_RIGHT, ORIENTATION, INPUTCONST::MOUSE_RIGHT));
	action_mouse_relations.push_back(ACTION_INPUT(TURN_LEFT, ORIENTATION, INPUTCONST::MOUSE_LEFT));
	action_mouse_relations.push_back(ACTION_INPUT(LOOK_UP, ORIENTATION, INPUTCONST::MOUSE_UP));
	action_mouse_relations.push_back(ACTION_INPUT(LOOK_DOWN, ORIENTATION, INPUTCONST::MOUSE_DOWN));
	action_mouse_relations.push_back(ACTION_INPUT(SHOOT, OTHER, INPUTCONST::MOUSE_LEFT_BUT));

	lookAngle = Camera::GetOrientation().x;
}

//dodanie broni dla postaci
void Player::AddWeapon(const std::string& weaponName) {
	//iniclaizacja sprytnego wskaŸnika broni
	weapon = WeaponPtr(new Gun(weaponName, name, collisionBox.midPoint, position, orientation));														
	weapon->enableCharacter();															//odblokowanie obiektu broni
}

//dodatkowe akcje postaci gracza
void Player::PerformAdditionalAction(ActionPtr Action) {
	double t = PHYSICSMODULE::targetFrameTime;											//predefiniowana sta³a wartoœæ trwania jednej iteracji symulacji fizycznych
	switch (Action->name) {
		case SHOOT: {																		//STRZA£
			if (weapon!=nullptr && weapon->IsEnabled()) {														//jeœli broñ jest odblokowana
				Gun& playerGun = static_cast<Gun&>(*weapon);								
				playerGun.Shoot();															//wykonanie strza³u
				break;
			}
		}

		case RELOAD: {
			if (weapon!=nullptr && weapon->IsEnabled()) {
				Gun& playerGun = static_cast<Gun&>(*weapon);
				playerGun.AddAction(ACTION(RELOAD, OTHER));
				break;
			}
		}

		default:
			break;
	}
}

//funkcja sprawdzaj¹ca czy dany sygna³ wejœciowy 'val' z klawiatury znajduje siê w wekotrze relacji akcja - sygna³ wejœciowy z klawiatury
bool Player::CheckKeyboardInputVal(int val) {
	for (unsigned int i = 0; i < action_keyboard_relations.size(); i++) {
		if (action_keyboard_relations[i].input == val)									//jeœli dany sygna³ wejœciowy ['val'] znajduje siê w wekotrze relacji zwróæ wartoœæ TRUE
			return true;
	}
	return false;																		//w przeciwnym wypadku zwróæ FALSE
}

//dodanie akcji zwi¹zanej z danym sygna³em wejœciowym 'inputVal' z klawiatury do wektora akcji do wykonania przez postaæ gracza
void Player::AddKeyboardAction(int inputVal) {
	for (unsigned int i = 0; i < action_keyboard_relations.size(); i++) {
		if (action_keyboard_relations[i].input == inputVal) {							//jeœli dany sygna³ wejœciowy ['inputVal'] znajduje siê w wektorze relacji
			switch (action_keyboard_relations[i].subActionName) {
			case MOVEMENT: {
					ActionPtr ptr(new MOVE_ACTION(action_keyboard_relations[i].action));
					actions.push_back(ptr);
					break;
				}
			case AIR: {
					if (action_keyboard_relations[i].action == JUMP) {
						if (!(actions.size()>0 && actions[0]->subActionName == AIR)) {
							Vector midPt = collisionBox.midPoint;
							position.y += midPt.y / 4.0;
							ActionPtr ptr(new JUMP_ACTION(position, position - previousPosition, jumpStrength));
							actions.push_back(ptr);
							if (actions.size() > 0) {
								ActionPtr tmp = actions[0];
								actions[0] = actions[actions.size() - 1];
								actions[actions.size() - 1] = tmp;
							}
						}
					}
					break;
				}
			case OTHER: {
					ActionPtr ptr(new ACTION(action_keyboard_relations[i].action,action_keyboard_relations[i].subActionName));
					actions.push_back(ptr);
					break;
				}
			}
			break;
		}
	}
}

//funkcja sprawdzaj¹ca czy dany sygna³ wejœciowy 'val' z klawiatury znajduje siê w wektorze relacji akcja - sygna³ wejœciowy z myszy
bool Player::CheckMouseInputVal(int val) {
	for (unsigned int i = 0; i < action_mouse_relations.size(); i++) {					//jeœli dany sygna³ wejœciowy ['val'] znajduje siê w wektorze relacji zwróæ wartoœæ TRUE
		if (action_mouse_relations[i].input == val)				
			return true;
	}
	return false;																		//w preciwnym wypadku zwróæ FALSE
}

//dodanie akcji zwi¹zanej z danym sygna³em wejœciowym 'inputVal' z myszy do wekotra akcji do wykonania przez postaæ gracza
//parametr MouseMove okreœla wartoœæ przemieszczenia myszy [parametr MousMove zapisywany jest jako wartoœæ param[0] struktury ACTION]
void Player::AddMouseAction(int inputVal,int MouseMove) {
	for (unsigned int i = 0; i < action_mouse_relations.size(); i++) {
		if (action_mouse_relations[i].input == inputVal) {
			switch (action_mouse_relations[i].subActionName) {
			case ORIENTATION: {
					ActionPtr ptr(new LOOK_ACTION(action_mouse_relations[i].action, MouseMove));
					actions.push_back(ptr);
					break;
				}
			case OTHER: {
					ActionPtr ptr(new ACTION(action_mouse_relations[i].action, action_mouse_relations[i].subActionName));
					actions.push_back(ptr);
					break;
				}
			}

			break;
		}
	}
}

//aktualizacja pozycji postaci
void Player::UpdatePosition() {
	DynamicCharacter::UpdatePosition();													//uaktualnienie pozycji postaci						
	if (weapon != nullptr)
		weapon->SetNextPosition(position);													//uaktualnienie pozycji broni postaci
}

//aktualizacja orientacji postaci
void Player::UpdateOrientation() {
	DynamicCharacter::UpdateOrientation();												//uaktualnienie orientacji postaci
	if (weapon != nullptr) {																//uaktualnienie orientacji broni
		Vector weaponOrient = orientation;
		weaponOrient.x = -lookAngle;
		weapon->setOrientation(weaponOrient);
	}
}

void Player::Update() {
	UpdatePosition();
	UpdateOrientation();

	if (weapon != nullptr) {
		weapon->Update();
	}
}

//narysowanie modelu
void Player::DrawModel() {
	OBJmodel::DrawModel();
	if (weapon != NULL)
		weapon->DrawModel();
};