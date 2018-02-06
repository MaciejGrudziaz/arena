#pragma once
#include "PhysicsModel.h"
#include "GameObjects.h"
#include "Gun.h"
#include "SpecialMath.h"
#include "CharacterRegister.h"

//-----------------------------------------------------------------------------------------
//------------------------------MODEL FIZYKI KOLIZJI POCISKÓW------------------------------
//-----------------------------------------------------------------------------------------

namespace BULLET {
	//struktura opisuj¹ca kolizjê pocisku
	struct BulletColStruct {
		bool collisionOccured;																									//zmienna okreœlaj¹ca czy kolizja zasz³a
		Vector collisionPos;																									//wspó³rzêdne punktu kolizji
		Character* charHitPtr;																									//wskaŸnik na obiekt z którym zasz³a kolizja

		BulletColStruct() :collisionOccured(false), collisionPos(Vector(0, 0, 0)), charHitPtr(NULL) {}
		BulletColStruct(bool _collisionOccured, Vector _collisionPos) :collisionOccured(_collisionOccured), collisionPos(_collisionPos), charHitPtr(NULL) {}
		BulletColStruct(bool _collisionOccured, Vector _collisionPos, Character* charPtr) :collisionOccured(_collisionOccured), collisionPos(_collisionPos), charHitPtr(charPtr) {}
	};

	//struktura zwracana przez fun. BulletCollision::GetNewMainSectorIdx(..)
	struct newSector {
		bool insideMap;																											//sprawdzenie czy nowa pozycja pocisku znajduje siê w obrêbie mapy
		Vector position;																										//nowa pozycja pocisku
		IdxPair sectorIdx;																										//indeks sektora dla wspó³rzêdnych 'position'

		newSector():insideMap(false), position(Vector(0,0,0)), sectorIdx(0,0){}
	};

	enum RayCastVer{INSTANT,DYNAMIC};																							//typ wyliczeniowy dla rodzajów algorytmu BulletCollision
}

//KOLIZJA POCISKÓW
class BulletCollision :public PhysicsModel {
	BulletCollision() {}
	BulletCollision(const BulletCollision& bulletCol) {}
	BulletCollision& operator=(const BulletCollision& bulletCol) {}

	static BulletCollision* instance;																							//instancja singletonu
	static std::vector<Gun*> gunRegister;																						//rejestr broni których pociski bêd¹ sprawdzane

	void AnalyzeShotsFired();																									//funkcja analizuj¹ca wszystkie wystrzelone pociski

	void BulletRayCastAlgorithm(Bullet& bullet, BULLET::RayCastVer version);													//wykonanie algorytmu RayCast dla danego pocisku

	//algorytm RayCast w trybie INSTANT
	void BulletRayCastInstantHit(Map& map, Bullet& bullet);																		//g³ówne cia³o algorytmu								
	bool BulletRayCastInstantHit_CheckCollision(Bullet& bullet, Map& map, IdxPair mainSectorIdx, 
		std::vector<IdxPair>& neighboursIdx);																					//funkcja sprawdzaj¹ca czy zasz³a kolizja pocisku z innym obiektem
	bool BulletRayCastInstantHit_CheckGroundCol(Map& map, Bullet& bullet, Vector newPos);										//funkcja sprawdzaj¹ca czy zasz³a kolizja pocisku z pod³o¿em
	std::vector<IdxPair> BulletRayCastInstantHit_GetSectorNeighbours(Map& map, const Bullet& bullet, const IdxPair& sectorIdx);	//pobranie indeksów sektorów s¹siadów do aktualnego sektora
	BULLET::newSector BulletRayCastInstantHit_GetNewMainSectorIdx(const IdxPair& mainSectorIdx, const Bullet& bullet, 
		const double shift, Map& map);																							//pobranie nowego sektora do algorytmu

	//algorytm RayCast w trybie DYNAMIC
	void BulletRayCastDynamic(Map& map, Bullet& bullet);																		//g³ówne cia³o algorytmu
	bool BulletRayCastDynamic_CheckCollision(Map& map, Bullet& bullet, MAP::IdxPair sectorIdx, Vector bulletPos);				//funkcja sprawdzaj¹ca czy zasz³a kolizja pocisku z innym obiektem
	bool BulletRayCastDynamic_CheckMapBoundries(Map& map, Bullet& bullet);														//funkcja sprawdzaj¹ca czy pocisk wyszed³ poza granice mapy
	bool BulletRayCastDynamic_CheckGroundCol(Map& map, Bullet& bullet);															//funkcja sprawdzaj¹ca czy zasz³a kolizja pocisku z pod³o¿em

	BULLET::BulletColStruct CheckSectorBulletCollision(IdxPair sectorIdx, Bullet& bullet);										//sprawdzenie kolziji pocisku w danym sektorze

	BULLET::BulletColStruct CheckSectorBulletCollision_Map(IdxPair sectorIdx, Bullet& bullet);									//sprawdzenie kolizji pocisku z obiektami umieszczonymi na mapie
	BULLET::BulletColStruct CheckSectorBulletCollision_Characters(IdxPair sectorIdx, Bullet& bullet);							//sprawdzenie kolizji pocisku z pozosta³ymi obiektami œwiata gry
	bool CheckSectorBulletCollision_CheckNewCol(BULLET::BulletColStruct &bulletCol, double& distance, Bullet& bullet);			//sprawdzenie nowo wykrytej kol. [czy jest 'lepsza' od wczeœniejszej]

	BULLET::BulletColStruct CheckBulletCollision(Character& character, Bullet& bullet);											//sprawdzenie kolizji pocisku z obiektem 'character'
	BULLET::BulletColStruct CheckBulletCharacterCollision_CheckObject(Character& character, const Bullet& bullet, int objIdx);	//sprawdzenie kolizji pocisku z obiektem modelu 'character'
	BULLET::BulletColStruct CheckBulletCharacterCollision_CheckObjectColBox(Character& character, 
		const Bullet& bullet, CollisionBox& objColBox);																			//sprawdzenie kolizji pocisku z hitboxem obiektu modelu 'character'

	BULLET::BulletColStruct CheckBulletCollision(Map& map, unsigned objIdx, Bullet& bullet);									//sprawdzenie kolizji pocisku z obiektem 'objIdx' mapy
	BULLET::BulletColStruct CheckBulletObjectCollision_CheckObjectReach(Map& map, Bullet& bullet, CollisionBox& objColBox);		//sprawdzenie czy obiekt znajduje siê wewn¹trz zasiêgu pocisku 
	BULLET::BulletColStruct CheckBulletObjectCollision_CheckColBox(Map& map, Bullet& bullet, CollisionBox& objColBox);			//sprawdzenie bry³y kolziji danego obiektu mapy

public:
	~BulletCollision() { delete instance; }
	static BulletCollision* GetInstance();																						//pobranie instancji modelu
	
	static void RegisterGun(Gun* _gun) { gunRegister.push_back(_gun); }															//zarejestrowanie obiektu broni
	static void UnregisterGun(Gun* _gun);																						//wyrejestrowanie obiektu broni

	void Process();																												//przetworzenie modelu fizyki
};