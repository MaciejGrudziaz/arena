#include "GamePhysics.h"

//przetwarzanie 'fizyki' gry
void GamePhysics::ProccessPhysics() {
	PlayerActions();																		//przetwarzanie akcji gracza
	UpdatePlayerCollisionBox();																//aktualizacja bry³y kolizji gracza
	CheckCollisions();
}

//przetwarzanie akcji gracza
void GamePhysics::PlayerActions() {
	for (unsigned int i = 0; i < GameObjects::players.size(); i++) {						//przegl¹danie wektora postaci gracza
		if (GameObjects::players[i].IsEnabled()) {											//jeœli postaæ jest 'odblokowana'
			if (GameObjects::players[i].ActionsAvailable()) {								//sprawdzenie czy dostepne s¹ akcje do wykonania
				GameObjects::players[i].PerformAllActions();								//wykonanie wszystkich akcji postaci
			}
		}
	}
}

//aktualizacja bry³y kolizji gracza
void GamePhysics::UpdatePlayerCollisionBox() {
	for (unsigned int i = 0; i < GameObjects::players.size(); i++) {
		Vector pos,prevPos, orient, prevOrient;
		pos = GameObjects::players[i].getPosition();										//wczytanie obecnej pozycji gracza
		prevPos = GameObjects::players[i].GetPreviousPosition();							//wczytanie poprzedniej pozycji gracza
		orient = GameObjects::players[i].getOrientation();									//wczytanie orientacji gracza
		prevOrient = GameObjects::players[i].GetPreviousOrientation();						//wczytanie poprzedniej orientacji gracza

		if (pos != prevPos || orient != prevOrient) {										//jeœli pozycja lub orientacja gracza nie zgadzaj¹ siê z poprzedni¹ pozycja lub orientacj¹
			GameObjects::players[i].UpdateTransformMatrix();								//aktualizuj macierz przekszta³cenia gracza
			GameObjects::players[i].UpdateCollisionBoxVertices();							//aktualizuj wspó³rzêdne wierzcho³ków bry³ kolizji gracza
			GameObjects::players[i].UpdateCollisionBoxNormals();
		}
	}
}

//sprawdzenie kolizji
void GamePhysics::CheckCollisions() {
	CheckPlayersCollisions();																//sprawdzenie kolizji dla postaci gracza
	CheckBotsCollisions();
}

//sprawdzenie kolizji dla postaci gracza z obiektami aktualnej mapy
void GamePhysics::CheckPlayersCollisions() {
	for (Player& player : GameObjects::players) {
		if (player.IsEnabled()) {																				
			for (Map& map : GameObjects::maps) {	
				if (map.IsEnabled()) {		
					CheckCharacterCollisions(map, player);
					break;
				}																					
			}
		}
	}
}

void GamePhysics::CheckBotsCollisions() {
	unsigned i = 0;
	for (Bot& bot : GameObjects::bots) {
		if (bot.IsEnabled()) {
			for (Map& map : GameObjects::maps) {
				if (map.IsEnabled()) {
					GAMEPHYSICS::CollisionStatus colStatus;
					colStatus=CheckCharacterCollisions(map, bot);
					bot.LoadLastColStatus(colStatus);
					break;
				}
			}
		}
		i++;
	}
}

GAMEPHYSICS::CollisionStatus GamePhysics::CheckCharacterCollisions(Map& map, DynamicCharacter& character) {
	GAMEPHYSICS::CollisionStatus colStatus; 
	if (map.IsEnabled() && character.IsEnabled()) {
		GAMEPHYSICS::specialNormals normals;														//wektor 'specjalnych' normalnych [noramlna+dodatkowe informacje o kolizji]											
		Vector position = character.getPosition();													//pobranie aktualnej pozycji gracza
		double yMapVal = map.Get_Y_Pos(position);													//wczytanie wart. wsp. Y mapy dla danej pozycji w (X,Z)

		normals = LoadCharacterCollisionNormals(map, character);									//wczytanie normalnych do wszystkich œcian z którymi zachodzi kolizja

		if (normals.size() == 0) {
			GAMEPHYSICS::CollisionVectorCheck mapBoundriesCol = CheckMapBoundries(map, character);
			NoCollisionState(character, map, yMapVal,mapBoundriesCol);								//funkcja stanu dla braku kolizji
			colStatus.mapBoundriesCol.collisionOccured = mapBoundriesCol.collisionOccured;
		}
		else {
			Vector newNextPos;																		//nowa wartoœæ nastêpnej pozycji gracza ['nextPosition']

			GAMEPHYSICS::ColContain_AllColTypeVec allColStruct;
			allColStruct = CreateDotProductVectors(map, character, normals);						//inicjalizacja wekotrów dla wszystkich typów kolizji

			newNextPos = AdjustNextPos(allColStruct, character, map, yMapVal);						//obliczenie nowej pozycji gracza na podstawie wczytanych wektorów kolizji

			character.SetNextPosition(newNextPos);													//ustawienie nowej pozycji dla gracza
		
			colStatus = LoadCollisionStatusRetVal(allColStruct);
		}
	}

	return colStatus;
}

//pobranie normalnych z obiektami z którymi zasz³a kolizja
GAMEPHYSICS::specialNormals GamePhysics::LoadCharacterCollisionNormals(Map& map, DynamicCharacter& character) {
	GAMEPHYSICS::specialNormals normals;
	ModelCollision col;																							//struktura reprezentuj¹ca kolizjê
	std::vector<unsigned> charIdx = map.GetSectorCharactersIdx(character.getPosition(), 
		character.getCollisionBox().radius);																		//wczytanie indeksów obiektów 'Character' z danego sektora mapy
	for (unsigned idx : charIdx) {
		col = CollisionDetection::CheckCollision(character, map.GetCharacter(idx));								//sprawdzenie czy zasz³a kolizja z danym obiektem 'Character'
		if (col.status) {																						//jeœli zasz³a kolizja
			CollisionNormals charNormals = CollisionDetection::GetCollisonFaceNormals(character,
				map.GetCharacter(idx), col);																	//wczytaj wszystkie normalne dla kolizji z danym obiektem
			for (std::pair<Vector, unsigned> charN : charNormals) {
				GAMEPHYSICS::SpecialNormalStruct newN;															//stworzenie 'specjanej' struktury dla danej normalnej kolizji
				newN.normal = charN.first;
				newN.linesCrossed = charN.second;
				newN.charIdx = idx;
				newN.objIdx = -1;
				normals.push_back(newN);																		//dodanie 'specjalnej' normalnej do wektora zwracanego
			}
		}
	}
	std::vector<unsigned> objIdx = map.GetSectorObjectsIdx(character.getPosition(),
		character.getCollisionBox().radius);																		//wczytanie indeksów podstawowych obiektów z danego sektora mapy  
	for (unsigned idx : objIdx) {
		col = CollisionDetection::CheckCollision(character, map, idx);												//sprawdenie czy zasz³a kolizja z danym obiektem mapy
		if (col.status) {																						//jeœli zasz³a kolizja
			CollisionNormals objNormals = CollisionDetection::GetCollisonFaceNormals(character, map, col);			//wczytanie wszystkie normalne dla kolizji z danym obiektem
			for (std::pair<Vector, unsigned> charN : objNormals) {
				GAMEPHYSICS::SpecialNormalStruct newN;															//stworzenie 'specjalnej' struktury dla danej normalnej kolizji
				newN.normal = charN.first;
				newN.linesCrossed = charN.second;
				newN.objIdx = idx;
				newN.charIdx = -1;
				normals.push_back(newN);																		//dodanie specjalnej normalnej do wektora zwracanego
			}
		}
	}
	
	return normals;
}

//rozdzielenie wektora normalnych kolizji na podstawie iloczynu skalarnego z [0,1,0]
GAMEPHYSICS::ColContain_AllColTypeVec GamePhysics::CreateDotProductVectors(Map& map, DynamicCharacter& character, GAMEPHYSICS::specialNormals& normals) {
	GAMEPHYSICS::ColContain_AllColTypeVec colRetStruct;
	//colRetStruct.allColCont.standardCol.clear(); colRetStruct.allColCont.negativeCol.clear(); colRetStruct.allColCont.groundCol.clear();
	Vector Y(0, 1, 0);
	for (GAMEPHYSICS::SpecialNormalStruct& colN : normals) {
		colN.dotY = colN.normal.DotProduct(Y);																	//obliczenie iloczynu skalarnego normalnej z wektorem [0,1,0]
		
		if (colN.dotY < 0) colRetStruct.allColVec.negativeCol.push_back(colN);															//przydzielenie wekotra do odpowiedniej grupy 																				
		else if (colN.dotY <= 0.5) colRetStruct.allColVec.standardCol.push_back(colN);													//na podstawie obliczonego iloczynu sklaranego
		else colRetStruct.allColVec.groundCol.push_back(colN);
	}

	//posortowanie wekotrów normalnych dla ka¿dego typu w zaleznoœci od wyniku testu RayCast
	SortVector(colRetStruct.allColVec.standardCol);
	SortVector(colRetStruct.allColVec.negativeCol);
	SortVector(colRetStruct.allColVec.groundCol);

	colRetStruct.allColType.standardCol = StandardColState(colRetStruct.allColVec.standardCol, character);
	colRetStruct.allColType.negativeCol = NegativeColState(colRetStruct.allColVec.negativeCol, character);
	colRetStruct.allColType.groundCol = GroundColState(colRetStruct.allColVec.groundCol, character);
	colRetStruct.allColType.mapBoundriesCol = CheckMapBoundries(map, character);

	return colRetStruct;
}

//posortowanie wektora normalnych ze wzglêdu na param. 'linesCrossed'
void GamePhysics::SortVector(GAMEPHYSICS::specialNormals& colVec) {
	if(colVec.size()>1)
		sort(colVec.begin(), colVec.end(), 
			[](GAMEPHYSICS::SpecialNormalStruct& a, GAMEPHYSICS::SpecialNormalStruct& b)->bool {return a.linesCrossed > b.linesCrossed; });
}

//funkcja stanu dla braku kolizji
void GamePhysics::NoCollisionState(DynamicCharacter& character, Map& map, double yMapVal, const GAMEPHYSICS::CollisionVectorCheck& mapBoundriesCol) {
	Vector position = character.getPosition(), nextPosition = character.GetNextPosition();
	//Vector midPoint = character.getCollisionBox().midPoint;
	Vector minPoint = character.getCollisionBox().minPoint;
	if (position.y+minPoint.y >= yMapVal+0.1) {																	//jeœli gracz znajduje siê nad poweirzchni¹ mapy
		if (!character.CheckAirAction()) {																		//jeœli dla psotaci gracza nie ma aktualnie akcji spadania
			FALL_ACTION fall(position, nextPosition-position);													//stwórz akcjê spadania
			character.AddAction(fall);																				//dodaj j¹ do akcji gracza
		}
	}
	else {																										//w przeciwnym wypadku 
		nextPosition.y = yMapVal - minPoint.y;																	//wyznacz nastêpn¹ pozycjê Y gracza								
		character.RemoveAirAction();																				//usuñ akcjê spadania gracza
	}
	
	if (mapBoundriesCol.collisionOccured) character.SetNextPosition(mapBoundriesCol.nextPos);
	else character.SetNextPosition(nextPosition);																		//ustalenie nowej pozycji gracza
}

//obliczenie nowej pozycji dla standardowej kolizji
GAMEPHYSICS::CollisionVectorCheck GamePhysics::StandardColState(GAMEPHYSICS::specialNormals& standardCol, DynamicCharacter& character) {
	Vector position = character.getPosition(), nextPosition = character.GetNextPosition();
	GAMEPHYSICS::CollisionVectorCheck standardColPair;															//struktura przechowuj¹ca now¹ pozycjê gracza dla danej kolizji 
																												//oraz inf. czy kolizja na pewno zasz³a
	standardColPair.collisionOccured = false;
	GAMEPHYSICS::specialNormals newStandardCol;																	//wektor pomocniczy

	if (standardCol.size() > 0) {
		//przefiltrowanie wektora 'specjalnych' normalnych [usuniêcie wekotrów o podobnym zwrocie]
		for (GAMEPHYSICS::SpecialNormalStruct& n : standardCol) {
			if (!CollisionDetection::GettingOutOfCollision(character, n.normal)) {								//jeœli postaæ nie 'wychodzi' z kolizji
				if (newStandardCol.size() > 0) {																//jesli zosta³ ju¿ dodany obiekt do wektora pomocniczego
					double dotProduct = newStandardCol[0].normal.DotProduct(n.normal);							//oblicz iloczyn skalarny normalnej z wekotra pomocniczego z aktualnie badan¹ normaln¹  
					if (dotProduct < 0.9) {																		//jesli jest on mniejszy od 0.9 [normaln¹ rozbiegaj¹ siê]
						newStandardCol.push_back(n);															//dodaj dan¹ normaln¹ do wektora pomocniczego
					}
				}
				else newStandardCol.push_back(n);																//dodanie pierwsego el. do wektora pomocniczego
			}
		}
		if (newStandardCol.size()==1) {																			//jeœli w wyniku przefiltrowania zosta³ tylko jeden wektor
			Vector move = CollisionDetection::GettingIntoCollision(character, newStandardCol[0].normal);		//na jego podstawie oblicz now¹ pozycjê gracza dla kolizji standardowej
			nextPosition.x = position.x + move.x;
			nextPosition.z = position.z + move.z;
			standardColPair.collisionOccured = true;															//potwierdŸ ¿e zasz³a kolizja
			standardColPair.charIdx = newStandardCol[0].charIdx;
			standardColPair.objIdx = newStandardCol[0].objIdx;
		}
		else {																									//w przeciwnym wypadku
			if(newStandardCol.size()>1) nextPosition = position;												//zablokuj postaæ gracza [zderzenie ze œcianami w ró¿nych p³aszczyznach]
			standardColPair.collisionOccured = true;															//potwierdŸ ¿e zasz³a kolizja
		}
	}

	standardColPair.nextPos = nextPosition;																		//przypisanie wart. nowej pozycji gracza dla kolizji standardowej
	return standardColPair;
}

//obliczenie nowej pozycji dla 'negatywnej' kolziji
GAMEPHYSICS::CollisionVectorCheck GamePhysics::NegativeColState(GAMEPHYSICS::specialNormals& negativeCol, DynamicCharacter& character) {
	Vector position = character.getPosition(), nextPosition = character.GetNextPosition();
	GAMEPHYSICS::CollisionVectorCheck negativeColPair;															//struktura przechowuj¹ca now¹ pozycjê gracza dla danej kolizji 
																												//oraz inf. czy kolizja na pewno zasz³a
	negativeColPair.collisionOccured = false;													
	if (negativeCol.size() > 0) {																				
		if (!CollisionDetection::GettingOutOfCollision(character, negativeCol[0].normal)) {						//jeœli postaæ nie wychodzi z kolizji
			nextPosition.y = position.y;																		//zablokowanie postaci w aktualnej pozycji Y
			Vector adjustedNormal = negativeCol[0].normal;														//pobranie normalnej z najwiekszym wynikiem dla testu RayCast
			adjustedNormal.y = 0;																				//wyzerowanie wsp. Y [interesuje nas tylko przesuniecie w (X,Z)]
			Vector move = CollisionDetection::GettingIntoCollision(character, adjustedNormal);						//obliczenie nowego wektora przesuniêcia dla kolizji
			nextPosition.x = position.x + move.x;																//obliczeni nowej pozycji gracza na podsatwie kolizji
			nextPosition.z = position.z + move.z;

			negativeColPair.collisionOccured = true;															//potwierdŸ ¿e kolizja zasz³a
		}
	}
		
	negativeColPair.nextPos = nextPosition;																		//przypisanie nowej pozycji gracza dla danej kolizji																
	return negativeColPair;
}

//obliczenie nowej pozycji dla pozycji z obiektem pod³o¿a
GAMEPHYSICS::CollisionVectorCheck GamePhysics::GroundColState(GAMEPHYSICS::specialNormals& groundCol, DynamicCharacter& character) {
	Vector position = character.getPosition(), nextPosition = character.GetNextPosition();
	GAMEPHYSICS::CollisionVectorCheck groundColPair;															//struktura przechowuj¹ca now¹ pozycjê gracza dla danej kolizji
																												//oraz inf. czy kolizja na pewno zasz³a
	groundColPair.collisionOccured = false;
	if (groundCol.size() > 0) {
		if (!CollisionDetection::GettingOutOfCollision(character, groundCol[0].normal)) {							//jeœli postaæ nie wychodzi z kolizji
			Vector move = CollisionDetection::GettingIntoCollision(character, groundCol[0].normal);				//obliczenie nowego wektora przesuniêcia dla kolizji
			nextPosition.x = position.x + move.x;
			nextPosition.z = position.z + move.z;
			nextPosition.y = position.y + move.y;

			groundColPair.collisionOccured = true;																//potwierdŸ ¿e kolizja zasz³a
		}
	}

	groundColPair.nextPos = nextPosition;																		//przypisanie nowej pozycji gracza dla danej kolizji
	return groundColPair;
}

//dosotosowanie obliczonego wektora nowej pozycji ze wzglêdu na obecne kolizje
Vector GamePhysics::AdjustNextPos(const GAMEPHYSICS::ColContain_AllColTypeVec& allColstruct, DynamicCharacter& character, Map& map, double yMapVal) {
	Vector position = character.getPosition(), nextPosition = character.GetNextPosition();
	Vector newNextPos;
	if (allColstruct.allColType.mapBoundriesCol.collisionOccured)
		newNextPos = allColstruct.allColType.mapBoundriesCol.nextPos;
	else {

		//----------------------------------------------------------
		bool colChecked = false;
		if (allColstruct.allColType.standardCol.charIdx != -1 || allColstruct.allColType.standardCol.objIdx != -1) {
			if (allColstruct.allColType.standardCol.charIdx != -1) {
				Vector colCharMax = map.GetCharacter(allColstruct.allColType.standardCol.charIdx).getCollisionBox().maxPoint;
				Vector colCharPos = map.GetCharacter(allColstruct.allColType.standardCol.charIdx).getPosition();
				Vector colCharMaxTransform = colCharMax + colCharPos;
				Vector charSize = character.getCollisionBox().size;
				Vector charMin = character.getCollisionBox().minPoint;

				if (position.y + charSize.y / 10.0 > colCharMaxTransform.y) {
					newNextPos = nextPosition;
					newNextPos.y = colCharMaxTransform.y - charMin.y;
					colChecked = true;
				}
			}
			else {
				Vector colObjMax = map.getObjectMainCollisionBox(allColstruct.allColType.standardCol.objIdx).maxPoint;
				Vector charSize = character.getCollisionBox().size;
				Vector charMin = character.getCollisionBox().minPoint;

				if (position.y + charSize.y / 10.0 > colObjMax.y) {
					newNextPos = nextPosition;
					newNextPos.y = colObjMax.y - charMin.y;
					colChecked = true;
				}
			}
		}
		//----------------------------------------------------------

		if (!colChecked) {
			if (allColstruct.allColType.standardCol.collisionOccured) {													//jeœli zasz³a standardowa kolizja
				if (allColstruct.allColType.negativeCol.collisionOccured) {												//jeœli zasz³a 'negatywna' kolizja
					newNextPos = position;																				//zablokuj ruch postaci gracza
				}
				else if (allColstruct.allColType.groundCol.collisionOccured) {											//jeœli zasz³a kolizja z obiektem z obiektem pod³o¿a
					newNextPos = position;																				//pozostaw pozyche w (X,Z) bez zmian
					newNextPos.y = allColstruct.allColType.groundCol.nextPos.y;											//zmieñ pozycjê w Y zgodnie z groundCol
				}
				else {
					newNextPos = allColstruct.allColType.standardCol.nextPos;											//jeœli zasz³a kolizja tylko z obiektem pod³o¿a, wyznacz na jej podstawie now¹ poz.	
				}
			}
			else if (allColstruct.allColType.negativeCol.collisionOccured) {											//jeœli nie szasz³a standardowa kolizja, ale zasz³a koliza 'negatywna'
				if (allColstruct.allColType.groundCol.collisionOccured)													//jeœli zasz³a kolizja z obiektem pod³o¿a
					newNextPos = position;																				//zablokuj ruch postaci gracza
				else newNextPos = allColstruct.allColType.negativeCol.nextPos;											//jeœli zasz³a tylko 'negatywna' kolizja - wyznacz na jej podst. now¹ poz.
			}
			else if (allColstruct.allColType.groundCol.collisionOccured)												//jeœli zasz³a tylko kolzizja z obiektem pod³o¿a
				newNextPos = allColstruct.allColType.groundCol.nextPos;													//wyznacz na jej podstawie now¹ pozycjê postaci
			else newNextPos = nextPosition;																				//jeœli nie zasz³a ¿adna kolizja pozostaw now¹ postaæ gracza bez zmian

		}
	}

	newNextPos = AdjustNextPos_AdjustGroundCollisionAndFallAction(character, newNextPos, yMapVal,allColstruct);		//dostosuj wyznaczon¹ now¹ postaæ gracza ze wzglêdu na akcjê spadania i kolizjê
																													//z obiektem pod³o¿a
	return newNextPos;
}

//dostosowanie obliczonego wektora nowej pozycji ze wzglêdu na kolzijê z pod³o¿em i akcji spadania gracza
Vector GamePhysics::AdjustNextPos_AdjustGroundCollisionAndFallAction(DynamicCharacter& character, const Vector& adjustedNextPos, double yMapVal, 
	const GAMEPHYSICS::ColContain_AllColTypeVec& allColStruct) {
	unsigned groundColVecSize = allColStruct.allColVec.groundCol.size();
	Vector position = character.getPosition(), nextPosition = character.GetNextPosition();
	Vector minPt = character.getCollisionBox().minPoint;
	Vector newNextPos = adjustedNextPos;
	if (position.y + minPt.y > yMapVal + 0.1) {																	//jeœli postaæ gracza znajduje siê nad pod³o¿em mapy
		if (groundColVecSize==0) {																				//jeœli nie znalezniono ¿adnego wektora kolziji z obiektem pod³o¿a
			if (character.CheckAirAction()) {																	//jeœli dla postaci istnieje akacja 'powietrzna'
				if(!allColStruct.allColType.negativeCol.collisionOccured)										//jeœli nie zasz³a 'ujemna' kolizja
					newNextPos.y = nextPosition.y;																//nowa pozycja Y postaci gracza pochodzi z akcji spadania
				else {
					character.RemoveAirAction();																//usuniêcie aktualnej akcji 'powietrznej'
					FALL_ACTION fall(position, (nextPosition - position));										//stwórz akcjê spadania
					character.AddAction(fall);																	//dodaj j¹ do akcji postaci
				}
			}
			else {																								//w przeciwnym wypadku
				FALL_ACTION fall(position, (nextPosition - position));											//stwórz akcjê spadania
				character.AddAction(fall);																		//dodaj j¹ do akcji postaci
			}
		}
		else {																									//jeœli zachodzi jakakolwiek kolzija z obiektem pod³o¿a [nawet 'wyjœcie' z kolizji]
			character.RemoveAirAction();																		//usuñ akcjê spadania postaci
		}
	}

	if (position.y + minPt.y <= yMapVal) {																		//jeœli postaæ znajduje siê pod poweirzchni¹ mapy
		if (groundColVecSize == 0) {																			//jeœli nie zachodzi jakakolwiek kolizja z obiektem pod³o¿a
			newNextPos.y = yMapVal - minPt.y;																	//wczytaj now¹ pozycjê Y postaci przy pomocy mapy
			character.RemoveAirAction();																		//usuñ akcjê spadania dla postaci
		}
	}

	return newNextPos;
}

//sprawdzenie czy postaæ nie wyjdzie poza granice mapy
GAMEPHYSICS::CollisionVectorCheck GamePhysics::CheckMapBoundries(Map& map, DynamicCharacter& character) {
	Vector nextPosition = character.GetNextPosition();
	GAMEPHYSICS::CollisionVectorCheck retVal;
	retVal.nextPos = nextPosition;
	Vector mapMaxPt = map.GetMaxPoint();
	Vector mapMinPt = map.GetMinPoint();
	double r = character.getCollisionBox().radius;
	for (unsigned int i = 0; i < 3; i++) {																		//dla wspó³rzênych (X,Z)
		if (i != 1 && nextPosition[i] + r >= mapMaxPt[i]) {														//jeœli nastêpna pozycja gracza bêdzie poza granicami mapy (z b³êdem równym R postaci)
			retVal.nextPos = character.getPosition();															//zablokuj ruch postaci
			retVal.collisionOccured = true;
		}
		if (i != 1 && nextPosition[i] - r <= mapMinPt[i]){
			retVal.nextPos = character.getPosition();
			retVal.collisionOccured = true;
		}
	}

	return retVal;
}

//za³adowanie statusu zwracanego dla badanej kolizji
GAMEPHYSICS::CollisionStatus GamePhysics::LoadCollisionStatusRetVal(const GAMEPHYSICS::ColContain_AllColTypeVec& allColStruct) {
	GAMEPHYSICS::CollisionStatus colStatus;

	colStatus.groundCol.collisionOccured = allColStruct.allColType.groundCol.collisionOccured;				//status dla kolizji z obiektem 'pod³o¿a'
	if (allColStruct.allColVec.groundCol.size() > 0) colStatus.groundCol.collisionNormal = allColStruct.allColVec.groundCol[0].normal;
	colStatus.negativeCol.collisionOccured = allColStruct.allColType.negativeCol.collisionOccured;			//status dla 'neagtywnej' kolizji
	if (allColStruct.allColVec.negativeCol.size() > 0) colStatus.negativeCol.collisionNormal = allColStruct.allColVec.negativeCol[0].normal;
	colStatus.standardCol.collisionOccured = allColStruct.allColType.standardCol.collisionOccured;			//status dla standardowej kolizji
	if (allColStruct.allColVec.standardCol.size() > 0) colStatus.standardCol.collisionNormal = allColStruct.allColVec.standardCol[0].normal;
	colStatus.mapBoundriesCol.collisionOccured = allColStruct.allColType.mapBoundriesCol.collisionOccured;	//status dla kolizji z granic¹ mapy

	return colStatus;
}
