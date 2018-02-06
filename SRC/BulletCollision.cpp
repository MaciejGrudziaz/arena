#include "BulletCollision.h"

BulletCollision* BulletCollision::instance = NULL;
std::vector<Gun*> BulletCollision::gunRegister;

//pobranie instancji modelu
BulletCollision* BulletCollision::GetInstance() {
	if (instance == NULL)																								//jeœli isntancja modelu nie zosta³a jeszcze stworzona	
		instance = new BulletCollision();																				//stwórz j¹

	return instance;																									//zwróæ instancjê modelu
}

//wyrejestrowanie obiektu broni
void BulletCollision::UnregisterGun(Gun* _gun) {
	for (auto it = gunRegister.begin(); it != gunRegister.end(); it++) {												
		if (*it == _gun) {																								//jeœli wskaŸnik broni w rejestrze jest równy danemu wskŸnikowi '_gun'
			gunRegister.erase(it);																						//usuñ dany element z wektora
		}
	}
}

//przetworzenie modelu fizyki
void BulletCollision::Process() {
	AnalyzeShotsFired();																								//analiza wszystkich wystrzelonych pocisków [z zarejestrowanych broni]
}

//funkcja analizuj¹ca wszystkie wystrzelone pociski
void BulletCollision::AnalyzeShotsFired() {
	for (Gun* gun : gunRegister) {																						//z rejestr zarejestrowanych broni
		if (gun->GetShotsFiredCount() > 0) {																			//jeœli dana broñ posiada wystrzelone pociski
			for (unsigned i = 0; i < gun->GetShotsFiredCount(); i++) {
				if (!gun->GetBullet(i).hit)																				//jeœli pocisk jeszcze nie zderzy³ siê z ¿adnym obiektem
					BulletRayCastAlgorithm(gun->GetBullet(i), BULLET::DYNAMIC);											//wykonaj dla niego algorytm RayCast [w wersji dynamicznej]
			}
			gun->UpdateBullets();																						//aktualizuj wszytkie wystrzelone pociski dla danej broni
		}
	}
}

//wykonanie algorytmu RayCast dla danego pocisku
void BulletCollision::BulletRayCastAlgorithm(Bullet& bullet,BULLET::RayCastVer version) {
	Map* map = CharacterRegister::GetMap();																				//pobierz obiekt mapy z g³ównego rejestru

	if (map->IsEnabled()) {																								//jeœli mapa jest odblokowana
		switch (version) {																								//wybierz dan¹ wersjê algirytmu RayCast
		case BULLET::INSTANT:																							//INSTANT - kolizja natychmiastowa badana w tej samej klatce co wystrzelono pocisk
			BulletRayCastInstantHit(*map, bullet);																		//wywo³anie alogorytmu RayCAst w wersji INSTANT
			break;
		case BULLET::DYNAMIC:																							//DYNAMIC - pocisk jest przesuwany w ka¿dej kl., kolzija badana jest dla jego akt. poz.
			BulletRayCastDynamic(*map, bullet);																			//wywo³anie algorytmu RayCast w wersji DYNAMIC
			break;
		}
	}
}

//g³ówne cia³o algorytmu RayCast w wersji INSTANT [kolizja pocisku badana jest w tej samej klatce w której zosta³ wystrzelony, dla wszystkich pocisków na jego torze lotu] 
void BulletCollision::BulletRayCastInstantHit(Map& map, Bullet& bullet) {
	IdxPair mainSectorIdx = map.GetSectorIdx(bullet.startPos);															//indeks sekotra podstawowego [w którym aktualnie znajduje siê pocisk]
	bool collisionOccured = false;																						//zmienna okreœlaj¹ca czy kolizja zasz³a
	BULLET::newSector newSectorIdxAndPositon;																			//struktura przechowuj¹ca dane o nowym sektorze i pozycji pocisku															

	double shift;																										//wart. przesuniêcia dla pocisku w kolejnych iteracjach algorytmu
	std::pair<double, double> sectorShift = map.GetSectorShift();														//pobranie wymiarów (X,Z) sektora mapy
	if (bullet.direction.x > bullet.direction.z) shift = sectorShift.first;												//przypisanie do zmiennej 'shift' wymiaru dla którego pocisk przebywa wiêksz¹ drogê 
	else shift = sectorShift.second;

	while (!collisionOccured) {																							//dopóki nie zasz³a kolizja pocisku
		std::vector<IdxPair> neighboursIdx;																	
		neighboursIdx = BulletRayCastInstantHit_GetSectorNeighbours(map, bullet, mainSectorIdx);						//pobierz indeksy s¹siadów dla aktualnego sektora podstawowego

		if (BulletRayCastInstantHit_CheckCollision(bullet, map, mainSectorIdx, neighboursIdx))							//sprawdzenie czy zachodzi kolizja w sektorze podstawowym lub w sek. s¹siadów
			collisionOccured = true;
											
		newSectorIdxAndPositon = BulletRayCastInstantHit_GetNewMainSectorIdx(mainSectorIdx, bullet, shift, map);		//pobranie nowego sektora podstawowego i pozycji pocisku
		if (!newSectorIdxAndPositon.insideMap) {																		//jeœli nowowyznaczona pozycja pocisku znajduje siê poza granicami mapy
			bullet.hitPos = newSectorIdxAndPositon.position;															//przypisz j¹ jako pozycjê kolziji pocisku
			bullet.hit = true;																				
			collisionOccured = true;																					//zakoñcz wykonywanie algorytmu
		} 
		else if (BulletRayCastInstantHit_CheckGroundCol(map, bullet, newSectorIdxAndPositon.position))					//jeœli zachodzi kolizja z pod³o¿em mapy
			collisionOccured = true;																					//zakoñcz wykonywanie algorytmu

		mainSectorIdx = newSectorIdxAndPositon.sectorIdx;																//przypisz now¹ wart. indeksu podstawowego sektora
		bullet.actualPos = newSectorIdxAndPositon.position;																//przypisz now¹ wart. pozycji pocisku
	}
}

//funkcja sprawdzaj¹ca czy zasz³a kolizja pocisku z innym obiektem
bool BulletCollision::BulletRayCastInstantHit_CheckCollision(Bullet& bullet,Map& map, IdxPair mainSectorIdx, std::vector<IdxPair>& neighboursIdx) {
	BULLET::BulletColStruct bulletCol;																					//struktura przechowuj¹ca dane na temat kolizji pocisku
	bulletCol = CheckSectorBulletCollision(mainSectorIdx, bullet);														//sprawdzenie kolizji pocisku w sekotrze podstawowym
	if (bulletCol.collisionOccured) {																					//jeœli kolizja zasz³a
		if (bulletCol.charHitPtr != NULL)																				
			bulletCol.charHitPtr->disableCharacter();																	//wy³¹cz obiekt z którym zasz³a kolizja		
		bullet.hitPos = bulletCol.collisionPos;																			//przypisz wart. wsp. kolizji pocisku
		bullet.hit = true;																								//ustaw potwierdzenie kolizji dla pocisku

		return true;
	}
	else {																												//jeœli nie wykryto kolizji w sektorze podstawowym
		for (IdxPair sectorIdx : neighboursIdx) {																		//sprawdŸ sekotry s¹siaduj¹ce
			bulletCol = CheckSectorBulletCollision(sectorIdx, bullet);													//sprawdzenie kolizji w danym sekotrze s¹siednim
			if (bulletCol.collisionOccured) {																			//jesli kolzija zasz³a
				if (bulletCol.charHitPtr != NULL)	
					bulletCol.charHitPtr->disableCharacter();															//wy³¹cz obiekt z któym zasz³a kolizja
				bullet.hitPos = bulletCol.collisionPos;																	//przypisz wart. wsp. kolizji pocsiku
				bullet.hit = true;																						//ustaw potwierdzenie kolizji dla pocisku

				return true;
			}
		}
	}

	return false;
}

//funkcja sprawdzaj¹ca czy zasz³a kolizja pocisku z pod³o¿em
bool BulletCollision::BulletRayCastInstantHit_CheckGroundCol(Map& map, Bullet& bullet, Vector newPos) {
	Vector mapSize = map.GetMaxPoint() - map.GetMinPoint();																//wielkoœæ mapy
	double dist;																	
	if (mapSize.x < mapSize.z)																							//zdefiniwoanie zmiennej 'dist' jako najmniejszego wymiaru sektora mapy wsp. Y
		dist = mapSize.x / MAP::stdMapDiv;
	else
		dist = mapSize.z / MAP::stdMapDiv;

	Vector actNewVec = newPos - bullet.actualPos;																		//wektor przesuniêcia miêdzy now¹, a poprzedni¹ pozycj¹ pocisku
	Vector checkPos = bullet.actualPos + bullet.direction*dist;															//nowa pozycja do sprawdzenia kolizji z pod³o¿em
	Vector shift = newPos - checkPos;																					//wektor przesuniêcia miêdzy now¹ pozycj¹, a pozycj¹ sprawdzan¹ pocisku

	while (actNewVec.DotProduct(shift) >= 0) {																			//'checkPos' nie minê³a jeszcze 'newPos'
		if (checkPos.y < map.Get_Y_Pos(checkPos)) {																		//jeœli wart. wsp. y dla sprawdzanej pozycji jest mniejsza ni¿ wart. wsp. Y dla mapy
			bullet.hitPos = checkPos;																					//przypisz t¹ wsp. jako wsp. kolizji pocisku
			bullet.hit = true;																							//ustaw potwierdzenie kolizji pocisku
			return true;
		}
		checkPos += bullet.direction*dist;																				//w przeciwnhym wypadku wyznacz now¹ wart. wsp. pozycji do sprawdzenia
		shift = newPos - checkPos;																						//oblicz now¹ wart. wektora przesuniêcia od pomiêdzy sprawdzan¹, a now¹ pozycj¹ pocisku
	}

	return false;
}

//pobranie indeksów sektorów s¹siadów do aktualnego sektora
std::vector<IdxPair> BulletCollision::BulletRayCastInstantHit_GetSectorNeighbours(Map& map, const Bullet& bullet, const IdxPair& sectorIdx) {
	std::vector<IdxPair> neighboursIdx;																					//wektor indeksów sektorów s¹siadów
	IdxPair neighbour;																									//indeks sektora
	if (bullet.direction.x >= 0 && bullet.direction.z >= 0) {															//jeœli pocisk skierowany jest wzd³u¿ dodatnich osi X i Z
		neighbour = map.GetSectorNeighbourIdx(sectorIdx, MAP::TOP);														//pobranie 'górnego' s¹siada dla danego sektora
		if (neighbour != sectorIdx) neighboursIdx.push_back(neighbour);													//jeœli s¹siad istnieje dodaj go do wektora
		neighbour = map.GetSectorNeighbourIdx(sectorIdx, MAP::TOP_RIGHT);												//pobranie 'prawego-górnego' s¹siada dla danego sektora
		if (neighbour != sectorIdx) neighboursIdx.push_back(neighbour);
		neighbour = map.GetSectorNeighbourIdx(sectorIdx, MAP::RIGHT);													//pobranie 'prawego' s¹siada dla danego sektora
		if (neighbour != sectorIdx) neighboursIdx.push_back(neighbour);
	}
	if (bullet.direction.x >= 0 && bullet.direction.z < 0) {															//jesli pocisk jest skierowany wzd³uz dodatniej osi X i ujemnej Z
		neighbour = map.GetSectorNeighbourIdx(sectorIdx, MAP::RIGHT);													//pobranie 'prawego' s¹siedniego sektora
		if (neighbour != sectorIdx) neighboursIdx.push_back(neighbour);
		neighbour = map.GetSectorNeighbourIdx(sectorIdx, MAP::BOTTOM_RIGHT);											//pobranie 'prawego-dolnego' s¹siedniego sektora
		if (neighbour != sectorIdx) neighboursIdx.push_back(neighbour);
		neighbour = map.GetSectorNeighbourIdx(sectorIdx, MAP::BOTTOM);													//pobranie 'dolnego' sektora
		if (neighbour != sectorIdx) neighboursIdx.push_back(neighbour);
	}
	if (bullet.direction.x < 0 && bullet.direction.z < 0) {																//jeœli pocisk skierowany jest wzd³u¿ ujemnych osi X i Z
		neighbour = map.GetSectorNeighbourIdx(sectorIdx, MAP::BOTTOM);													//pobranie 'dolnego' s¹siedniego sektora
		if (neighbour != sectorIdx) neighboursIdx.push_back(neighbour);
		neighbour = map.GetSectorNeighbourIdx(sectorIdx, MAP::BOTTOM_LEFT);												//pobranie 'lewego-dolnego' s¹siedniego sektora
		if (neighbour != sectorIdx) neighboursIdx.push_back(neighbour);
		neighbour = map.GetSectorNeighbourIdx(sectorIdx, MAP::LEFT);													//pobranie 'lewego' s¹siedniego sektora
		if (neighbour != sectorIdx) neighboursIdx.push_back(neighbour);
	}
	if (bullet.direction.x < 0 && bullet.direction.z >= 0) {															//jesli pocisk jest skierowany wzd³u¿ ujemnej osi X i dodatniej osi Z
		neighbour = map.GetSectorNeighbourIdx(sectorIdx, MAP::LEFT);													//pobranie 'lewego' s¹siedniego sektora
		if (neighbour != sectorIdx) neighboursIdx.push_back(neighbour);
		neighbour = map.GetSectorNeighbourIdx(sectorIdx, MAP::TOP_LEFT);												//pobranie 'górnego-lewego' s¹siedniego sektora
		if (neighbour != sectorIdx) neighboursIdx.push_back(neighbour);
		neighbour = map.GetSectorNeighbourIdx(sectorIdx, MAP::TOP);														//pobranie 'górnego' s¹siedniego sektora
		if (neighbour != sectorIdx) neighboursIdx.push_back(neighbour);
	}

	return neighboursIdx;
}

//pobranie nowego sektora do algorytmu
BULLET::newSector BulletCollision::BulletRayCastInstantHit_GetNewMainSectorIdx(const IdxPair& mainSectorIdx, const Bullet& bullet, const double shift, Map& map) {
	BULLET::newSector retVal;																							//wart. zwracana nowego sektora
	IdxPair newMainSectorIdx = mainSectorIdx;																			//indeks nowego sektora podstawowego
	Vector newPos = bullet.actualPos;																					//nowa pozycja pocisku
	Vector minMap = map.GetMinPoint(), maxMap = map.GetMaxPoint();														//min. i maks. wspó³rzêdne mapy

	while (newMainSectorIdx == mainSectorIdx) {																			//dopóki nowo wyznaczony sektor jest identyczny z aktulanym sektorem
		newPos += bullet.direction*shift;																				//wyznacz now¹ pozycje pocisku
		retVal.position = newPos;
		if (newPos.x > maxMap.x || newPos.z > maxMap.z || newPos.y>1000.0)												//jesli nowowyznaczona pozycja jest poza granicami mapy
			return retVal;																								//zakoñcz poszuiwanie nowego punktu
		if (newPos.x < minMap.x || newPos.z < minMap.z || newPos.y<-1000.0)
			return retVal;
		newMainSectorIdx = map.GetSectorIdx(newPos);																	//w przecienym wypadku wyznacz indeks sektora dla nowowyznaczonej pozycji
	}

	retVal.sectorIdx = newMainSectorIdx;																				//przypisz indeks sektora do wart. zwracanej
	retVal.insideMap = true;																							//ustaw potweirdzenie ¿e nowa pozycja pocisku znajduje siê wewn¹trz mapy

	return retVal;
}

//g³ówne cia³o algorytmu RayCast w wersji DYNAMIC [w ka¿dej kolejnej klatce pocisk jest przesuwany i badana jest kolizja w stosunku do jego aktualnej pozycji]
void BulletCollision::BulletRayCastDynamic(Map& map, Bullet& bullet) {
	if (!BulletRayCastDynamic_CheckMapBoundries(map,bullet)){
		IdxPair mainSectorIdx = map.GetSectorIdx(bullet.actualPos);														//indeks sektora dla aktualnej pozycji
		IdxPair prevPosSecIdx = map.GetSectorIdx(bullet.previousPos);													//indeks sektora dla poprzedniej pozycji
			
		BulletRayCastDynamic_CheckCollision(map, bullet, mainSectorIdx, bullet.actualPos);								//sprawdŸ kolizjê pocisku w aktualnym sektorze

		if (prevPosSecIdx != mainSectorIdx && !bullet.hit)																//jeœli indeksy poprz.o i akt. sektora nie s¹ sobie równe i nie zasz³al kolizja
			BulletRayCastDynamic_CheckCollision(map, bullet, prevPosSecIdx, bullet.actualPos);							//sprawdŸ kolizjê pocisku w poprzednim sektorze

		BulletRayCastDynamic_CheckGroundCol(map, bullet);																//sprawdŸ kolizjê pocisku z pod³o¿em
	}
}

//funkcja sprawdzaj¹ca czy zasz³a kolizja pocisku z innym obiektem
bool BulletCollision::BulletRayCastDynamic_CheckCollision(Map& map, Bullet& bullet,MAP::IdxPair sectorIdx, Vector bulletPos) {
	BULLET::BulletColStruct bulletCol = CheckSectorBulletCollision(sectorIdx, bullet);									//sprawdzenie kolizji pocisku w danym sektorze
	Vector startActShift, actColShift, prevColShift;																					

	if (bulletCol.collisionOccured) {
		startActShift = bullet.actualPos - bullet.startPos;																//wektor od pozycji startowej do aktualnej pocisku
		actColShift = bulletCol.collisionPos - bullet.actualPos;														//wektor od aktualnej pozycji pocisku do pozycji w której zasz³a kolizja

		if (startActShift.DotProduct(actColShift) < 0 ) {																//jeœli punkt kolizji znajduje siê 'przed' aktualn¹ pozycj¹ pocisku															
			if (bulletCol.charHitPtr != NULL) {																			//jeœli wyznaczono wskaŸnik na obiekt z którym zasz³a kolizja
				//bulletCol.charHitPtr->disableCharacter();																//'wy³¹cz' dany obiekt
				if (bulletCol.charHitPtr->IsEnabled())
					bulletCol.charHitPtr->SubstractHealth(static_cast<unsigned>(bullet.damage));
			}
			
			bullet.hitPos = bulletCol.collisionPos;																		//przypisz wart. do poz. kolizji pocisku
			bullet.hit = true;																							//przypisz potwierdzenie trafienia do pocisku
			return true;
		}
	}
	return false;
}

//funkcja sprawdzaj¹ca czy pocisk wyszed³ poza granice mapy
bool BulletCollision::BulletRayCastDynamic_CheckMapBoundries(Map& map, Bullet& bullet) {
	Vector minMap = map.GetMinPoint(), maxMap = map.GetMaxPoint();														//pobrnie min. i max. punktu mapy
	if (bullet.actualPos.x > maxMap.x || bullet.actualPos.z > maxMap.z || bullet.actualPos.y > 1000.0 ||				//sprawdzenie czy aktualna pozycja pocisku nie mieœci siê w granicach mapy
		bullet.actualPos.x < minMap.x || bullet.actualPos.z < minMap.z || bullet.actualPos.y < -1000.0) {
		bullet.hitPos = bullet.actualPos;																				//jeœli tak to przypisz aktualn¹ pozycjê pocisku jako pozycjê kolizji 
		bullet.hit = true;																								//przypisz potwierdzenie trafienia dla pocisku

		return true;

	}
	else return false;
}

//funkcja sprawdzaj¹ca czy zasz³a kolizja pocisku z pod³o¿em
bool BulletCollision::BulletRayCastDynamic_CheckGroundCol(Map& map, Bullet& bullet) {
	if (bullet.actualPos.y < map.Get_Y_Pos(bullet.actualPos)-0.1) {														//jeœli aktulana pozycja pocisku znajduje siê pod powierzchni¹ mapy
		bullet.hitPos = bullet.actualPos;																				//przypisz pozycjê kolizji pocisku jako aktualn¹ pozycjê
		bullet.hit = true;																								//przypisz potwierdzenie trafienia dla pocisku
		return true;
	}
	
	//w przeciwnym wypadku
	Vector shift = bullet.actualPos - bullet.previousPos;																//oblicz wektor przesuniêcia od poprzedniej do aktualnej pozycji pocisku
	Vector newPos = bullet.actualPos - bullet.direction*(shift.MOD() / 3.0);											//oblicz now¹ poz. do testu znajduj¹c¹ siê w 1/3 pomiêdzy akt. a poprz. poz.

	if (newPos.y < map.Get_Y_Pos(newPos)-0.1) {																			//jeœli pozycja testowa znajduje siê pod powierzchni¹ mapy
		bullet.hitPos = newPos;																							//przypisz pozycjê kolizji pocisku jako pozycjê testow¹
		bullet.hit = true;																								//przypisz potwierdzenie trafienia dla pocisku
		return true;
	}

	newPos = bullet.actualPos - bullet.direction*(2.0*shift.MOD() / 3.0);												//oblicz now¹ poz. do testu znajduj¹c¹ siê w 2/3 pomiêdzy akt. a poprz. poz.

	if (newPos.y < map.Get_Y_Pos(newPos)-0.1) {																			//jeœli pozycja testowa znajduje siê pod powierzchni¹ mapy
		bullet.hitPos = newPos;																							//przypisz pozycjê kolizji pocsiku jako pozycjê testow¹
		bullet.hit = true;																								//przypisz potwierdzenie trafienia dla pocisku
		return true;
	}

	return false;
}

//sprawdzenie kolziji pocisku w danym sektorze
BULLET::BulletColStruct BulletCollision::CheckSectorBulletCollision(IdxPair sectorIdx, Bullet& bullet) {
	BULLET::BulletColStruct mapColStruct, charactersColStruct;															//struktury kolizji dla kolizji z obiektami mapy oraz pozosta³ymi obiektami
	Vector mapShift, charactersShift;																					//wektory przesuniêcia dla wykrytych kolizji
		
	mapColStruct = CheckSectorBulletCollision_Map(sectorIdx, bullet);													//sprawdzenie kolizji z obiektami mapy
	charactersColStruct = CheckSectorBulletCollision_Characters(sectorIdx, bullet);										//sprawdzenie kolziji z pozosta³ymi obiektami

	if (mapColStruct.collisionOccured) {																				//jeœli zasz³a kolizja z obeiktem mapy
		if (charactersColStruct.collisionOccured) {																		//jeœli zasz³a kolizja z pozosta³ymi obiektami œwiata gry
			mapShift = mapColStruct.collisionPos - bullet.startPos;														//oblicz wektory przesuniêcia dla wykrytych kolizji							
			charactersShift = charactersColStruct.collisionPos - bullet.startPos;
			if (mapShift.MOD() < charactersShift.MOD())																	//wybierz kolizjê która zasz³a bli¿ej od punktu startowego pocisku
				return mapColStruct;
			else return charactersColStruct;
		}
		else return mapColStruct;																						//jeœli nie wykryto kolizji z innymi obiektami, zwróæ kolizjê z ob. mapy
	}
	else {																												//jeœli nie zasz³a kolizja z obiektem mapy
		if (charactersColStruct.collisionOccured)																		//ale zasz³a kolizja z innym obiektem œwiata gry
			return charactersColStruct;																					//zwróæ strukturê tej kolizji
		else return BULLET::BulletColStruct();																			//w przeciwnym wypadku zwróæ pust¹ strukturê kolizji
	}
}

//sprawdzenie kolizji pocisku z obiektami umieszczonymi na mapie
BULLET::BulletColStruct BulletCollision::CheckSectorBulletCollision_Map(IdxPair sectorIdx, Bullet& bullet) {
	Map* map = CharacterRegister::GetMap();																				//pobranie wskaŸnika na aktualn¹ mapê
	std::vector<unsigned> charactersIdx = map->GetSectorCharactersIdx(sectorIdx);										//pobranie indeksów obiektów 'Character' w danym sektorze mapy
	std::vector<unsigned> objIdx = map->GetSectorObjectsIdx(sectorIdx);													//pobranie indeksów podst. obiektów w danym sektorze mapy
	BULLET::BulletColStruct bulletCol;																					//struktury oisuj¹ce kolizjê pocisku
	BULLET::BulletColStruct bulletColRet;
	double dist = -1;																									//mininmalny dystans od punktu startowego pocisku do punktu kolizji																								

	for (unsigned idx : charactersIdx) {																				//sprawdzenie wszystkich obiektów 'Character' w danym sektorze
		if (map->GetCharacter(idx).IsEnabled()) {																		//jeœli obiekt jest 'odblokowany'
			bulletCol = CheckBulletCollision(map->GetCharacter(idx), bullet);											//sprawdŸ kolizjê pocisku z danym obiektem
			if (bulletCol.collisionOccured)																				//jeœli kolizja zasz³a
				if (CheckSectorBulletCollision_CheckNewCol(bulletCol, dist, bullet))									//sprawdŸ czy jest 'lepsza' od poprzedniej 'najlepszej' wykrytej kolizji
					bulletColRet = bulletCol;
		}
	}

	for (unsigned idx : objIdx) {																						//sprawdzenie wszystkich podstawowych obiektów w danym sektorze
		bulletCol = CheckBulletCollision(*map, idx, bullet);															//sprawdŸ kolizjê pocisku z danym obiektem
		if (bulletCol.collisionOccured)																					//jeœlli kolizja zasz³a
			if (CheckSectorBulletCollision_CheckNewCol(bulletCol, dist, bullet))										//sprawdŸ czy jest 'lepsza' od poprzedniej 'najlepszej' wykrytej kolizji
				bulletColRet = bulletCol;
	}

	return bulletColRet;																								//zwróæ strukturê kolizji dla obiektów mapy w danym sektorze
}

//sprawdzenie kolizji pocisku z pozosta³ymi obiektami œwiata gry
BULLET::BulletColStruct BulletCollision::CheckSectorBulletCollision_Characters(IdxPair sectorIdx, Bullet& bullet) {
	Map* map = CharacterRegister::GetMap();																				//pobierz wskaŸnik na aktualn¹ mapê
	BULLET::BulletColStruct bulletCol, bulletColRet;																	//struktury opisuj¹ce kolizjê pocisku
	double dist = -1;																									//minimalny dystans od punktu startowego pocisku do punktu kolizji
	std::vector<Character*> allObjectsVec;																				//wektor wszystkich obiektów do sprawdzenia w sektorze
	Character* _char;																									//wskaŸnik na dany obiekt

	for (unsigned i = 0; i < CharacterRegister::GetStaticCharacterCount(); i++) {
		_char = CharacterRegister::GetStaticCharacter(i);																//pobranie statycznego obiektu
		if (_char->IsEnabled() && _char->GetSectorIdx() == sectorIdx)													//sprawdzenie czy obiekt jest odbl. i czy znajduje siê w aktualnym sektorze
			allObjectsVec.push_back(_char);																				//jeœli tak - dodaj obiekt do kontenera 
	}

	for (unsigned i = 0; i < CharacterRegister::GetDynamicCharacterCount(); i++) {
		_char = static_cast<Character*>(CharacterRegister::GetDynamicCharacter(i));										//pobranie dynamicznego obiektu
		if (_char->IsEnabled() && _char->GetSectorIdx() == sectorIdx)													//sprawdzenie czy obiekt jest odbl. i czy znajduje siê w aktualnym sektorze
			allObjectsVec.push_back(_char);																				//jeœli tak - dodaj obiekt do kontenera
	}

	for (unsigned i = 0; i < CharacterRegister::GetBotCount(); i++) {	
		_char = static_cast<Character*>(CharacterRegister::GetBot(i));													//pobranie obiektu bota
		if (_char->IsEnabled() && _char->GetSectorIdx() == sectorIdx)													//sprawdzenie czy obiekt jest odbl. i czy znajduje siê w aktualnym sektorze
			allObjectsVec.push_back(_char);																				//jeœli tak - dodaj obiekt do kontenera
	}

	_char = static_cast<Character*>(CharacterRegister::GetPlayer());													//pobranie obiektu postaci gracza
	if (_char->IsEnabled() && _char->GetSectorIdx() == sectorIdx)														//sprawdzenie czy obiekt jest odbl. i czy znajduje siê w aktualnym sektorze
		allObjectsVec.push_back(_char);																					//jeœli tak - dodaj obiekt do kontenera

	for (Character* _char : allObjectsVec) {																			//dla wszystkich obiektów z kontenera
		bulletCol= CheckBulletCollision(*_char, bullet);																//sprawdŸ kolizjê dla danego obiektu
		if (bulletCol.collisionOccured)																					//jeœli kolizja zachodzi
			if (CheckSectorBulletCollision_CheckNewCol(bulletCol, dist, bullet))										//sprawdzenie czy nowa kolizja jest 'lepsza' od poprzedniej 'najlepszej'
				bulletColRet = bulletCol;																				//jeœli tak - przypisz dan¹ kolizjê jako now¹ 'najlepsz¹'
	}

	return bulletColRet;																								//zwróæ strukturê kolizji dla pozosta³ych obiektów w danym sektorze
}

//sprawdzenie nowo wykrytej kolizji [czy jest 'lepsza' od wczeœniejszej]
bool BulletCollision::CheckSectorBulletCollision_CheckNewCol(BULLET::BulletColStruct &bulletCol, double& distance, Bullet& bullet) {
	BULLET::BulletColStruct bulletColRet;																				//zwracana struktura kolizji
	Vector shift;

	if (bulletCol.collisionOccured) {																					//jeœli dana kolzija zachodzi
		shift = bulletCol.collisionPos - bullet.startPos;																//oblicz wektor przesuniêcia od poz. startowej pocisku do poz. kolizji
		if (distance == -1) {																							//jeœli min. dystans = -1 [nie wykryto jeszcze wczeœniej kolizji]																				
			distance = shift.MOD();																						//przypisz dyst. pomiêdzy pkt. kol. a pkt. start. pocisku jako min. dyst.
			return true;
		}
		else {																											//jeœli wczeœniej wykryo kolizjê
			if (shift.MOD() < distance) {																				//sprawdŸ czy odl. do poz. akt. kolizji jest mniejsza ni¿ poprz. min. dyst.	
				distance = shift.MOD();																					//przypisz j¹ jako minimalna odleg³oœæ do pozycji kolizji
				return true;
			}
		}
	}

	return false;
}

//sprawdzenie kolizji pocisku z obiektem 'character'
BULLET::BulletColStruct BulletCollision::CheckBulletCollision(Character& character, Bullet& bullet) {
	Vector charPos = character.getPosition();																			//aktualna pozycja danego obiektu
	Vector charMidPt = character.getCollisionBox().midPoint+charPos;													//ppzycja œrodka obiektu
	double charR = character.getCollisionBox().radius;																	//promieñ sfery ograniczaj¹cej obiekt
	BULLET::BulletColStruct retColStruct;																				//struktura kolizji z obiektem

	Vector PpPi = charMidPt - bullet.startPos;																			//wektor od pkt. start. poc. do pkt. œrodkowego obiektu
	PpPi.NORM();																										//normalizacja wektora
	double dotProdPpPiDir = PpPi.DotProduct(bullet.direction);															//iloczyn sklarany obliczonego wektora z kierunkiem toru pocisku

	if (dotProdPpPiDir >= 0) {																							//jeœli pocisk leci w kierunku obiektu
		Vector r = PpPi.CrossProduct(Vector(0, 1, 0));																	//wektor prostopad³y do wek. wskazuj¹cego od pkt. start. poc. do œr. obiektu
		r.NORM();																										//normalizacja wektora
		Vector Pir = charMidPt + r*charR;																				//obliczenie pkt. oddalonego o R od pkt. œr. postaci w kierunku wek. 'r'
		Vector PpPir = Pir - bullet.startPos;																			//obl. wek. od pkt. start. poc. do pkt. znajduj¹cego siê na sf. ogr. obiekt
		PpPir.NORM();																									//normalizacja wektora
		double dotProdPpPiPpPir = PpPi.DotProduct(PpPir);																//iloczyn skalarny pomiêdzy wekotrami PpPi oraz PpPir

		unsigned objColBoxCount = 0;																					//iloœæ podst. bry³ kolziji wchodz¹cych w sk³ad pojedynczego obiektu modelu
		if (dotProdPpPiDir >= dotProdPpPiPpPir) {																		//jeœli wek. kierunku toru pocisku wskazuje na dany obiekt
			for (unsigned i = 0; i < character.getObjectsCount(); i++) {												//dla wszystkich podstawowych obiektów wchodz¹cych w sk³ad modelu
				objColBoxCount += character.getObjectCollisionBoxCount(i);												//dodaj iloœæ podst. bry³ kolizji wchodz¹cych w sk³ad danego obiektu
				retColStruct=CheckBulletCharacterCollision_CheckObject(character, bullet, i);							//sprawdŸ kolzijê pocisku z danym obiektem podstawowym
				if (retColStruct.collisionOccured)																		//jeœli kolzija zachodzi
					return retColStruct;																				//zwróæ otrzymany obiekt kolizji
			}
			if (objColBoxCount == 0) {																					//jeœli w sk³ad modelu nie wchodz¹ ¿adne podst. bry³y kolizji
				retColStruct = CheckBulletCharacterCollision_CheckObject(character, bullet, -1);						//sprawdŸ kolizjê z g³ówn¹ bry³¹ kolizji obiektu
				if (retColStruct.collisionOccured)																		//jeœli kolizja zachodzi
					return retColStruct;																				//zwróæ otrzymany obiekt kolizji
			}
		}
		else return BULLET::BulletColStruct();																			//jeœli wektor toru pocisku nie jest skierowany do obiektu																
	}																													//zwróæ pusty obiekt kolizji
	else return BULLET::BulletColStruct();																				//jeœli wektor toru pocisku jest skierowany w kierunku odwrotnym ni¿ obiekt
																														//zwróæ pusty obiekt kolizji	
	return BULLET::BulletColStruct();																					//zwróæ pusty obiekt kolizji
}

//sprawdzenie kolizji pocisku z obiektem modelu 'character'
BULLET::BulletColStruct BulletCollision::CheckBulletCharacterCollision_CheckObject(Character& character, const Bullet& bullet, int objIdx) {
	CollisionBox charColBox;																							//struktura opisuj¹ca bry³ê kolizji
	BULLET::BulletColStruct retColStruct;																				//struktura opisuj¹ca kolizjê
	if (objIdx == -1) {																									//jeœli arg. indeksu obiektu jest -1
		charColBox = character.getCollisionBox();																		//wczytaj g³ówna bry³ê kolizji obiektu
		retColStruct = CheckBulletCharacterCollision_CheckObjectColBox(character, bullet, charColBox);					//sprawdŸ kolizjê z tym hitboxem
		if (retColStruct.collisionOccured)																				//jeœli kolizja zachodzi
			return retColStruct;																						//zwróæ obiekt kolizji
	}
	else if(objIdx >= 0 && static_cast<unsigned>(objIdx) < character.getObjectsCount()) {								//jeœli podany indeks okreœla któryœ z obiektów podstawowych
		unsigned charColBoxCount = character.getObjectCollisionBoxCount(objIdx);										//wczytaj liczbê bry³ kolizji dla takiego obiektu
		for (unsigned i = 0; i < charColBoxCount; i++) {																//dla wszystkich bry³ kolizji
			charColBox = character.getObjectCollisionBox(objIdx,i);														//pobierz dan¹ bry³e kolizji
			retColStruct = CheckBulletCharacterCollision_CheckObjectColBox(character, bullet, charColBox);				//sprawdŸ kolizjê dla wybranego hitboxa
			if (retColStruct.collisionOccured)																			//jeœli zachodzi kolizja
				return retColStruct;																					//zwróæ obiekt kolizji
		}
	}

	return BULLET::BulletColStruct();																					//jeœli ni wykryto kolizji - zwróæ pusty obiekt kolizji
}

//sprawdzenie kolizji pocisku z hitboxem obiektu modelu 'character'
BULLET::BulletColStruct BulletCollision::CheckBulletCharacterCollision_CheckObjectColBox(Character& character, const Bullet& bullet, CollisionBox& objColBox) {
	Face face;																											//struktura opisuj¹ca powierzchniê [trójk¹t]
	Vector vertices[3];																									//tablica wierzcho³ków wchodz¹cych w sk³ad danej powierzchni
	Vector normal;																										//normalna do powierzchni
	Vector pointProjection;																								//rzut punktu na powierzchniê
	unsigned faceCount = objColBox.facesIdx.size();																		//liczba poweirzchni wchodz¹cych w sk³ad hitboxa
	for (unsigned i = 0; i < faceCount; i++) {																			//dla wszystkich powierzchni
		face = character.getFace(objColBox.facesIdx[i]);																//pobierz obiekt powierzchni
		normal = character.getTransformedNormal(face.NormalIdx);														//pobierz normaln¹
		for (unsigned j = 0; j < 3; j++)											
			vertices[j] = character.getTransformedVertice(face.PointsIdx[j]);											//pobierz wspó³rzêdne wierzcho³ków powierzchni

		if (normal.DotProduct(bullet.direction) < 0) {																	//jeœli normalna do powierzchni jest skierowana odwrotnie do kierunku pocisku
			pointProjection = PointSurfaceProjection(vertices, normal, bullet.startPos, bullet.direction);				//wyznacz rzut pozycji start. pocisku na powierzchniê wzd³u¿ jego toru lotu

			if (CheckPointInTriangle(pointProjection, vertices, normal))												//jeœli rzutowany punkt znajduje sie wewn¹trz powierzchni
				return BULLET::BulletColStruct(true, pointProjection,&character);										//zwróæ obiekt kolizji
		}
	}

	return BULLET::BulletColStruct();																					//jeœli nie wykryto kolizji - zwróæ pusty obiekt kolizji
}

//sprawdzenie kolizji pocisku z obiektem 'objIdx' mapy
BULLET::BulletColStruct BulletCollision::CheckBulletCollision(Map& map, unsigned objIdx, Bullet& bullet) {
	CollisionBox objColBox;																								//struktura opisuj¹ca bry³ê kolizji
	BULLET::BulletColStruct bulletCol;																					//struktura opisuj¹ca kolizjê pocisku

	if (map.getObjectCollisionBoxCount(objIdx) > 0) {																	//jeœli obiekt posiada sk³adowe bry³y kolizji
		for (unsigned i = 0; i < map.getObjectCollisionBoxCount(objIdx); i++) {											//dla ka¿dej takiej bry³y kolizji
			objColBox = map.getObjectCollisionBox(objIdx, i);															//pobierz dan¹ bry³ kolizji

			bulletCol = CheckBulletObjectCollision_CheckObjectReach(map, bullet, objColBox);							//sprawdŸ czy zachodzi kolizja

			if (bulletCol.collisionOccured)																				//jeœli tak
				return bulletCol;																						//zwróæ pobrany obiekt kolizji
		}
		return BULLET::BulletColStruct();																				//jesli nie wykryto kolizji - zwróæ pusty obiekt kolizji
	}
	else {																												//jeœli obiekt nie posiada sk¹³dowych bry³ kolizji
		objColBox = map.getObjectMainCollisionBox(objIdx);																//pobierz g³ówny hitbox obiektu

		return CheckBulletObjectCollision_CheckObjectReach(map, bullet, objColBox);										//sprawdŸ czy zachodzi kolizja i zwróc obiekt kolizji [pusty jeœli nie zachodzi]
	}
}

//sprawdzenie czy obiekt znajduje siê wewn¹trz zasiêgu pocisku 
BULLET::BulletColStruct BulletCollision::CheckBulletObjectCollision_CheckObjectReach(Map& map, Bullet& bullet, CollisionBox& objColBox) {
	BULLET::BulletColStruct retColStruct;																				//struktura opisuj¹ca kolizjê pocisku
	//Vector mapPos = map.getPosition();																				//pobranie pozycji mapy
	Vector objMidPt = objColBox.midPoint;// +mapPos;																	//pobranie punktu œrodkowego hitboxa obiektu	
	double objR = objColBox.radius;																						//pobrnaie promienia ograniczaj¹cego hitboxa

	Vector PpPi = objMidPt - bullet.startPos;																			//wektor opisuj¹ca przemieszczenie z pkt. start. pocisku do œrodka hitboxa
	PpPi.NORM();																										//normalizacja wektora
	double dotProdPpPiDir = PpPi.DotProduct(bullet.direction);															//iloczyn skalarny wektora PpPi z wekotrem kierunku lotu pocisku

	if (dotProdPpPiDir >= 0) {																							//jeœli obiekt znajduje siê wzgl. pocisku w tym samym kierunku co jego tor lotu
		Vector r = PpPi.CrossProduct(Vector(0, 1, 0));																	//wektor prostopad³y do PpPi - wskazuj¹cy na zewn¹trz obiektu od jego œrodka
		r.NORM();																										//normalizacja
		Vector Pir = objMidPt + r*objR;																					//obliczenie punktu znajduj¹cego siê na granicy sfery ograniczaj¹cej obiekt
		Vector PpPir = Pir - bullet.startPos;																			//obliczenie wekotra rzesuniêcia od œr. pocisku do tego punktu
		PpPir.NORM();																									//normalizacja
		double dotProdPpPiPpPir = PpPi.DotProduct(PpPir);																//iloczyn skalrny PpPi oraz PpPir

		if (dotProdPpPiDir >= dotProdPpPiPpPir) {																		//jeœli wektor toru lotu pocisku jest pomiêdzy wektorem PpPi oraz PpPir
			retColStruct = CheckBulletObjectCollision_CheckColBox(map, bullet, objColBox);								//sprawdŸ kolizjê z danym hitboxem
			if (retColStruct.collisionOccured)																			//jeœli kolizja zachodzi 
				return retColStruct;																					//zwróæ pobrany obiekt kolizji
		}
	}

	return BULLET::BulletColStruct();																					//jeœli nie weykryto kolizji - zwróæ pusty obiekt kolizji
}

//sprawdzenie bry³y kolziji danego obiektu mapy
BULLET::BulletColStruct BulletCollision::CheckBulletObjectCollision_CheckColBox(Map& map, Bullet& bullet, CollisionBox& objColBox) {
	Face face;																											//struktura opisuj¹ca powierzchniê [trójk¹t]
	Vector vertices[3];																									//wspó³rzêdne wierzcho³ków powierzchni
	Vector normal;																										//normalna do powierzchni
	Vector pointProjection;																								//rzut punktu na powierzchniê
	unsigned faceCount = objColBox.facesIdx.size();																		//liczba poweirzchni wchodz¹cych w sk³ad obiektu
	for (unsigned i = 0; i < faceCount; i++) {																			//dla ka¿dej powierzchni
		face = map.getFace(objColBox.facesIdx[i]);																		//pobierz obiekt powierzchni
		normal = map.getTransformedNormal(face.NormalIdx);																//pobierz normaln¹
		for (unsigned j = 0; j < 3; j++)	
			vertices[j] = map.getTransformedVertice(face.PointsIdx[j]);													//pobierz wspó³zêdne wierzcho³ków

		if (normal.DotProduct(bullet.direction) < 0) {																	//jeœli normalna do pwoerzchni jest skierowana przeciwnie do toru lotu pocisku
			pointProjection = PointSurfaceProjection(vertices, normal, bullet.startPos, bullet.direction);				//wyznacz rzut pikt. start. pocisku na dan¹ powierzchniê wzd³u¿ jego toru lotu 
			if (CheckPointInTriangle(pointProjection, vertices, normal))												//jeœli wyznaczony punk znajduje siê wewn¹trz powierzchni [kolizja zachodzi]
				return BULLET::BulletColStruct(true, pointProjection);													//zwróæ obiekt kolizji
		}
	}

	return BULLET::BulletColStruct();																					//jeœli nie wykryto kolizji - zwróæ pusty obiekt kolizji
}