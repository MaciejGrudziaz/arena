#pragma once
#include <thread>
#include "GameObjects.h"
#include "CollisionDetection.h"
#include "GamePhysicsStruct.h"

#include "PlayerInput.h"
#include "BotsActions.h"

//------------------------------------------------------------------------------------------------
//-------------------------------------------FIZYKA GRY-------------------------------------------
//--------------------klasa odpowiadaj¹ca za przetwarzanie 'fizyki' gry m.in.:--------------------
//-----------------------------------WYKONYWANIE AKCJI POSTACI------------------------------------
//------------------------------------------------------------------------------------------------

class GamePhysics {
	GamePhysics() {}																							//prywatny konstruktor domyœlny [singleton]
 
	static void PlayerActions();																				//przetwarzanie akcji gracza

	static void UpdatePlayerCollisionBox();																		//aktualizacja bry³y kolizji gracza

	static void CheckCollisions();																				//sprawdzenie kolizji

	static void CheckPlayersCollisions();																		//sprawdzenie kolizji dla postaci gracza z obiektami aktualnej mapy
	static void CheckBotsCollisions();																			//sprawdzenie kolziji dla botów z obiektami aktualnej mapy

	//-------------------------------------------------------------CheckCharacterCollisions()--------------------------------------------------------------
	static GAMEPHYSICS::CollisionStatus CheckCharacterCollisions(Map& map, DynamicCharacter& character);		//sprawdzenie kolizji dla danej postaci
	//static GAMEPHYSICS::AllCollisionsTypesVec LoadAllCollisionTypesVec(Map& map, DynamicCharacter& character, GAMEPHYSICS::specialNormals standardColVec)
	static GAMEPHYSICS::specialNormals LoadCharacterCollisionNormals(Map& map, DynamicCharacter& character);	//pobranie normalnych z obiektami z którymi zasz³a kolizja
	static GAMEPHYSICS::ColContain_AllColTypeVec CreateDotProductVectors(Map& map, DynamicCharacter& character, 
		GAMEPHYSICS::specialNormals& normals);																	//rozdzielenie wekotra normalnych kolizji na podstawie iloczynu skalarnego z [0,1,0]
	static void SortVector(GAMEPHYSICS::specialNormals& colVec);												//posortowanie wektora normalnych ze wzglêdu na param. 'linesCrossed'
	static void NoCollisionState(DynamicCharacter& character, Map& map, double yMapVal, 
		const GAMEPHYSICS::CollisionVectorCheck& mapBoundriesCol);												//funkcja stanu dla braku kolizji
	static GAMEPHYSICS::CollisionVectorCheck StandardColState(GAMEPHYSICS::specialNormals& standardCol, DynamicCharacter& character);	//obliczenie nowej pozycji dla standardowej kolizji
	static GAMEPHYSICS::CollisionVectorCheck NegativeColState(GAMEPHYSICS::specialNormals& negativeCol, DynamicCharacter& character);	//obliczenie nowej pozycji dla 'negatywnej' kolziji
	static GAMEPHYSICS::CollisionVectorCheck GroundColState(GAMEPHYSICS::specialNormals& groundCol, DynamicCharacter& character);		//obliczenie nowej pozycji dla pozycji z obiektem pod³o¿a
	static Vector AdjustNextPos(const GAMEPHYSICS::ColContain_AllColTypeVec& allColstruct, DynamicCharacter& character, Map& map, double yMapVal);														//dosotosowanie obliczonego wektora nowej pozycji ze wzglêdu na obecne kolizje
	static Vector AdjustNextPos_AdjustGroundCollisionAndFallAction(DynamicCharacter& character, const Vector& adjustedNextPos,
		double yMapVal, const GAMEPHYSICS::ColContain_AllColTypeVec& allColstruct);								//dostosowanie obliczonego wektora nowej pozycji ze wzglêdu na kolzijê z pod³o¿em
																												//i akcji spadania gracza
	static GAMEPHYSICS::CollisionVectorCheck CheckMapBoundries(Map& map, DynamicCharacter& character);			//sprawdzenie czy postaæ nie wyjdzie poza granice mapy
	static GAMEPHYSICS::CollisionStatus LoadCollisionStatusRetVal(const GAMEPHYSICS::ColContain_AllColTypeVec& allColStruct);			//za³adowanie statusu zwracanego dla badanej kolizji

public:
	static void ProccessPhysics();																				//przetwarzanie 'fizyki' œwiata gry

};