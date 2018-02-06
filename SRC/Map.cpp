#include "Map.h"

//kontruktor - za³¹dowanie mapy z pliku
Map::Map(const std::string& filename, unsigned _sectorDiv, unsigned _mapDiv):Character(filename) {
	std::string name;
	name = "MAP " + std::to_string(GameObjects::maps.size() + 1);
	setName(name);

	mapDiv = _mapDiv;																		//przypisaniew artoœci dla dzielnika macierzy wspó³rzêdnych mapy
	sectorDiv = _sectorDiv;																	//przypisanie wartoœci dla dzielnika sektorów mapy
}

//konstruktor - za³adowanie mapy z pliku i nadanie jej nazy
Map::Map(const std::string& filename, char* mapName, unsigned _sectorDiv, unsigned _mapDiv):Character(filename) {
	setName(mapName);

	mapDiv = _mapDiv;																		//przypisaniew artoœci dla dzielnika macierzy wspó³rzêdnych mapy
	sectorDiv = _sectorDiv;																	//przypisanie wartoœci dla dzielnika sektorów mapy
}

//dodanie postaci do wektora postaci [Character] przechowywanego w obiekcie mapy
void Map::LoadCharacter(const Character& character) {
	characters.push_back(character);
}

//inicjalizacja mapy
void Map::Initialize() {
	for (unsigned int i = 0; i < objects.size(); i++) {
		if (objects[i].name == "GROUND") {													//znalezienie indeksu obiektu tworz¹cego pod³o¿e mapy [p³aszczyzna (X,Z)]				
			groundIdx = i;
			break;
		}
	}
	//wyznaczenie punktu max i min dla ca³ej mapy na podstawie obiektu pod³o¿a ["GROUND"]
	unsigned int n = objects[groundIdx].verticesIdx.size();
	maxPt = minPt = vertices[objects[groundIdx].verticesIdx[0]];
	for (unsigned int i = 1; i < n; i++) {
		unsigned int idx = objects[groundIdx].verticesIdx[i];
		for (unsigned int j = 0; j < 3; j++) {
			if (vertices[idx][j] < minPt[j])
				minPt[j] = vertices[idx][j];
			else if (vertices[idx][j] > maxPt[j])
				maxPt[j] = vertices[idx][j];
		}
	}
	
	sectors.Initialize(sectorDiv,minPt,maxPt);												//inicjalizacja macierzy sektorów
	y_valMap.Initialize(mapDiv, minPt, maxPt);												//inicjalizacja macierzy wspó³rzêdnych

	LoadSectorsData();																		//za³adowanie danych do sektorów
		
	Load_Y_valMapSectorsData();																//podzia³ macierzy wspó³rzênych na sektory i za³adwoanie podstawowych danych
	Load_Y_valMapData();																	//za³¹dwoanie wart. wsp. Y do macierzy wpó³rzêdnych
}

//pobranie wart. wsp. Y dla danego punktu ['position'] w p³aszczyŸnie (X,Z)
double Map::Get_Y_Pos(Vector position) {
	std::pair<int, int> idx = y_valMap.GetSector(position.x, position.z);					//pobranie indeksów sektora macierzy wsp. dla danego punktu ['position'] w p³aszczyŸnie (X,Z)
	std::pair<double, unsigned int> d[4];													//tablica odleg³oœci badanego punktu do ka¿dego wierzcho³ka danego sektora
	Vector sectPt[4];																		//tablica przechowuj¹ca wierzcho³ki danego sektora
	Vector midPt = y_valMap[idx.first][idx.second].midPt;									//punkt œrodkowy danego sektora	

	Get_Y_Pos_LoadDistVertTab(d, sectPt, position, idx);									//za³adowanie wart. tablicy odleg³oœci ['d'] oraz tablicy wierzcho³ków ['sectPt']
	
	Get_Y_Pos_SortDistTab(d);																//posortowanie tablicy odleg³oœci ['d']

	return  Get_Y_Pos_Count_Y_val(sectPt, midPt, position, d);								//obliczenie i zwrócenie wart. wsp. Y dla badanej pozycji na podstawie tablicy odleg³oœci ['d']
}

//pobrnaie wektora indeksów postaci [Character] dla danego sektora ['sectors']
std::vector<unsigned int> Map::GetSectorCharactersIdx(Vector position, double R) {
	IdxPair idx;																			//indeksy sektora wewn¹trz któego znajduje sie punkt 'position'
	//obliczenie wart. indeksu w zale¿noœci od pozycji w p³aszczyŸnie (X,Z)
	idx.first = static_cast<unsigned>((position.x - sectors.GetMinPoint().x) / sectors.GetXshift());				
	idx.second = static_cast<unsigned>((sectors.GetMaxPoint().z - position.z) / sectors.GetZshift());

	//jeœli podano wartoœæ dla promienia
	if (R>0) {
		Vector shift;
		//znajdŸ najmniejsz¹ odleg³oœæ punktu 'position' do wierzcho³ka oraz pobierz indeks tego wierzcho³ka
		shift = position - sectors[idx.first][idx.second][0];								
		unsigned int minIdx = 0;
		double dist = shift.MOD();
		for (unsigned i = 1; i < 4; i++) {
			shift = position - sectors[idx.first][idx.second][i];
			if (shift.MOD() < dist) {
				minIdx = i;
				dist = shift.MOD();
			}
		}
		if (dist < R * 1.41)																//jeœli obliczona odleg³oœæ jest mniejsza od przek¹tnej kwadratu któego boki okreœlone sa przez promieñ 'R'
			return GetSectorsCharacters_GetIdx(idx, minIdx);								//zwróæ indeksy postaci aktualnego sektora oraz s¹siednich sektorów [w zaleznoœci od wybranego wierz.]
		else return sectors[idx.first][idx.second].charactersIdx;							//w przeciwnym wypadku zwróc tylko indeksy postaci aktualnego sektora
	}
	else return sectors[idx.first][idx.second].charactersIdx;								//jeœli nie podano wart. 'R' zwróæ tylko idneksy postaci aktualnego sektora
}

//pobranie wektora indeksów postaci ['Character'] dla danego sektora okreœlonego indeksem 'sectorIdx'
std::vector<unsigned> Map::GetSectorCharactersIdx(IdxPair sectorIdx) {
	return sectors[sectorIdx.first][sectorIdx.second].charactersIdx;
}

//pobranie wektora indeksów obiektów dla danego sektora ['sectors']
std::vector<unsigned int> Map::GetSectorObjectsIdx(Vector position, double R) {
	IdxPair idx;																			//indeksy sektora wewn¹trz któego znajduje sie punkt 'position'
	//obliczenie wart. indeksu w zale¿noœci od pozycji w p³aszczyŸnie (X,Z)
	idx.first = static_cast<unsigned>((position.x - sectors.GetMinPoint().x) / sectors.GetXshift());				
	idx.second = static_cast<unsigned>((sectors.GetMaxPoint().z - position.z) / sectors.GetZshift());

	//jeœli podano wartoœæ dla promienia
	if (R>0) {
		Vector shift;
		//znajdŸ najmniejsz¹ odleg³oœæ punktu 'position' do wierzcho³ka oraz pobierz indeks tego wierzcho³ka
		shift = position - sectors[idx.first][idx.second][0];
		unsigned int minIdx = 0;
		double dist = shift.MOD();
		for (unsigned i = 1; i < 4; i++) {
			shift = position - sectors[idx.first][idx.second][i];
			if (shift.MOD() < dist) {
				minIdx = i;
				dist = shift.MOD();
			}
		}
		if (dist < R * 1.41)																//jeœli obliczona odleg³oœæ jest mniejsza od przek¹tnej kwadratu któego boki okreœlone sa przez promieñ 'R'
			return GetSectorsObjects_GetIdx(idx, minIdx);									//zwróæ indeksy obiektów aktualnego sektora oraz s¹siednich sektorów [w zaleznoœci od wybranego wierz.]
		else return sectors[idx.first][idx.second].objectsIdx;								//w przeciwnym wypadku zwróc tylko indeksy obiektów aktualnego sektora
	}
	else return sectors[idx.first][idx.second].objectsIdx;									//jeœli nie podano wart. 'R' zwróæ tylko idneksy obiektów aktualnego sektora
}

//pobranie wektora obiektów ['objects'] dla danego sektora okreœlonego indeksem 'sectorIdx'
std::vector<unsigned> Map::GetSectorObjectsIdx(IdxPair sectorIdx) {
	return sectors[sectorIdx.first][sectorIdx.second].objectsIdx;
}

//pobranie indeksów sektora dla danej pozycji
IdxPair Map::GetSectorIdx(const Vector& position) {
	return sectors.GetSector(position.x, position.z);
}

//pobranie indeksów s¹siaduj¹cego sektora
IdxPair Map::GetSectorNeighbourIdx(IdxPair sectorIdx, MAP::sectorNeigbours neighbour) {
	switch (neighbour) {
		case MAP::TOP: {
			if (sectorIdx.second > 0) return IdxPair(sectorIdx.first, sectorIdx.second - 1);
			else return sectorIdx;
			break;
		}
		case MAP::BOTTOM: {
			if (sectorIdx.second < sectorDiv-1) return IdxPair(sectorIdx.first, sectorIdx.second + 1);
			else return sectorIdx;
			break;
		}
		case MAP::LEFT: {
			if (sectorIdx.first > 0) return IdxPair(sectorIdx.first - 1, sectorIdx.second);
			else return sectorIdx;
			break;
		}
		case MAP::RIGHT: {
			if (sectorIdx.first < sectorDiv-1) return IdxPair(sectorIdx.first + 1, sectorIdx.second);
			else return sectorIdx;
			break;
		}
		case MAP::TOP_RIGHT: {
			if (sectorIdx.first < sectorDiv-1 && sectorIdx.second>0) return IdxPair(sectorIdx.first + 1, sectorIdx.second - 1);
			else return sectorIdx;
			break;
		}
		case MAP::TOP_LEFT: {
			if (sectorIdx.first > 0 && sectorIdx.second > 0) return IdxPair(sectorIdx.first - 1, sectorIdx.second - 1);
			else return sectorIdx;
			break;
		}
		case MAP::BOTTOM_RIGHT: {
			if (sectorIdx.first < sectorDiv-1 && sectorIdx.second < sectorDiv-1) return IdxPair(sectorIdx.first + 1, sectorIdx.second + 1);
			else return sectorIdx;
			break;
		}
		case MAP::BOTTOM_LEFT: {
			if (sectorIdx.first > 0 && sectorIdx.second < sectorDiv-1) return IdxPair(sectorIdx.first - 1, sectorIdx.second + 1);
			else return sectorIdx;
			break;
		}
		default:
			return IdxPair(0, 0);
			break;
	}
}

//funkcja rysuj¹ca mapê
void Map::Draw() {
	DrawModel();																			//wywo³anie funkcji rysuj¹cej dla modelu mapy

	for (unsigned int i = 0; i < characters.size(); i++) {
		if(characters[i].IsEnabled())
			characters[i].DrawModel();															//wywo³anie funkcji rysuj¹cych dla ka¿dego obiektu wchodz¹cego w sk³ad mapy ['Character']
	}
}

//funkcja rysuj¹ca bry³y kolizji
void Map::DrawCollisionBox() {
	OBJmodel::DrawCollisionBox();															//wywo³anie funkcji rysuj¹cej bry³y kolizji dla modelu mapy

	for (Character i : characters)															//wywo³anie funkcji rysuj¹cej bry³y kolizji dla ka¿dego obiektu 'Character' wchodz¹cego w sk³ad mapy
		if(i.IsEnabled())
			i.DrawCollisionBox();
}

//za³adwoanie danych do sektorów
void Map::LoadSectorsData() {
	double xShift = sectors.GetXshift(), zShift = sectors.GetZshift();						//pobranie wart. wsp. przesuniêcia dla osi X i Z
	for (unsigned int i = 0; i < sectors.GetDimension(); i++)
		for (unsigned int j = 0; j < sectors.GetDimension(); j++) {
			SECTOR newSec; Vector midPt; Vector shift;										
			//obliczenie wart. wsp. dla pierwszego wierzcho³ka ['0']
			newSec[0].x = minPt.x + (i + 1)*xShift; newSec[0].y = 0.0; newSec[0].z = maxPt.z - j*zShift;
			midPt += newSec[0];
			//obliczenie wart. wsp. dla drugiego wierzcho³ka [1']
			newSec[1].x = minPt.x + i*xShift; newSec[1].y = 0.0; newSec[1].z = maxPt.z - j*zShift;
			midPt += newSec[1];
			//obliczenie wart. wsp. dla trzeciego wierzcho³ka ['2']
			newSec[2].x = minPt.x + i*xShift; newSec[2].y = 0.0; newSec[2].z = maxPt.z - (j + 1)*zShift;
			midPt += newSec[2];
			//obliczenie wart. wsp. dla czwartego wierzcho³ka ['3']
			newSec[3].x = minPt.x + (i + 1)*xShift; newSec[3].y = 0.0; newSec[3].z = maxPt.z - (j + 1)*zShift;
			midPt += newSec[3];
			midPt /= 4.0;																	//obliczenie wart. wsp. dla punktu œrodkowego sektora

			shift = newSec[0] - midPt;														//obliczenie przesuniêcia miêdzy pierwszym wierzcho³kiem sektora, a jego œrodkiem
			newSec.midPt = midPt;															//wczytanie puntku œrodkowego do nowego sektora
			newSec.radius = shift.MOD();													//wczytanie wartoœci promienia okrêgu opisanego na sektorze
			sectors[i][j] = newSec;															//dodanie nowego sektora do macierzy

			LoadSectorsData_LoadCharactersAndObjects(i, j);									//za³adowanie indeksów postaci ['Character'] dla danego sektora
		}
}

//za³adowanie indeksów postaci ['Character'] oraz obiektów mapy do danego sektora
void Map::LoadSectorsData_LoadCharactersAndObjects(unsigned rowIdx, unsigned columnIdx) {
	Vector shift, charMidPt, sectorMidPt=sectors[rowIdx][columnIdx].midPt;					//wczytanie wart. wsp. punktu œrodkowego dla badanego sektora
	double charR, sectorR=sectors[rowIdx][columnIdx].radius;								//wczytanie wart. promienia dla badanego sektora
	//wczytanie indeksów obiektów postaci ['Character']
	for (unsigned int i = 0; i < characters.size(); i++) {
		charMidPt = characters[i].getCollisionBox().midPoint+characters[i].getPosition();	//wczytanie wart. wsp. punktu œrodkowego dla danego obeiktu postaci
		charR = characters[i].getCollisionBox().radius;										//wczytanie wart. promienia dla danego obiektu postaci

		shift = charMidPt - sectorMidPt;													//obliczenie przesuniêcia pomiêdzy punktami œrodkowymi sektora oraz danego obektu postaci
		if (shift.MOD() <= charR + sectorR)													//jeœli wartoœæ ta jest mniejsza/równa ni¿ suma promieni
			sectors[rowIdx][columnIdx].charactersIdx.push_back(i);							//dodaj indeks postaci do wektora dla badanego sektora
	}
	//wczytanie indeksów obiektó mapy
	for (unsigned int i = 0; i < objects.size(); i++) {
		if (objects[i].name != "GROUND") {													//jeœli obiektem nie jest obiekt pod³o¿a
			charMidPt = objects[i].mainColBox.midPoint;										//za³adowanie wart. wsp. punktu œrodkowego dla danego obiektu
			charR = objects[i].mainColBox.radius;											//za³adowanie wart. promienia dla danego obiektu

			shift = charMidPt - sectorMidPt;												//obliczenie przesuniecia pomiêdzy œrodkiem danego obeiktu, a sektora
			if (shift.MOD() <= charR + sectorR)												//jeœli wart. jest ta mniejsza/równa ni¿ suma promieni
				sectors[rowIdx][columnIdx].objectsIdx.push_back(i);							//dodaj indeks obiektu mapy do wekotra badanego sektora
		}
	}
}

//podzielenie macierzy wspó³rzêdnych na sektory
void Map::Load_Y_valMapSectorsData() {
	double xShift = y_valMap.GetXshift(), zShift = y_valMap.GetZshift();
	for (unsigned int i = 0; i < y_valMap.GetDimension(); i++)
		for (unsigned int j = 0; j < y_valMap.GetDimension(); j++) {
			SECTOR newSec; Vector midPt; Vector shift;
			//obliczenie wart. wsp. dla pierwszego wierzcho³ka ['0']
			newSec[0].x = minPt.x + (i + 1)*xShift; newSec[0].y = 0.0; newSec[0].z = maxPt.z - j*zShift;
			midPt += newSec[0];
			//obliczenie wart. wsp. dla drugiego wierzcho³ka ['1']
			newSec[1].x = minPt.x + i*xShift; newSec[1].y = 0.0; newSec[1].z = maxPt.z - j*zShift;
			midPt += newSec[1];
			//obliczenie wart. wsp. dla trzeciego wierzcho³ka ['2']
			newSec[2].x = minPt.x + i*xShift; newSec[2].y = 0.0; newSec[2].z = maxPt.z - (j + 1)*zShift;
			midPt += newSec[2];
			//obliczenie wart. wsp. dla czwartego wierzcho³ka ['3']
			newSec[3].x = minPt.x + (i + 1)*xShift; newSec[3].y = 0.0; newSec[3].z = maxPt.z - (j + 1)*zShift;
			midPt += newSec[3];
			midPt /= 4.0;																	//obliczenie wart. wsp. punktu œrodkowego sektora

			shift = newSec[0] - midPt;														//obliczenie wektora przesuniêcia od punktu œrodkowego do pierwszego wierzcho³ka ['0']
			newSec.midPt = midPt;															
			newSec.radius = shift.MOD();													//obliczenie wart. promienia okrêgu opisanego na sektorze
			y_valMap[i][j] = newSec;														//dodanie nowego sektora do amcierzy wspó³rzêdnych
		}
}

//za³adowanie danych dotycz¹cych wsp. Y dla wczesniej stworzonych sekotrów macierzy
void Map::Load_Y_valMapData() {
	for (unsigned int i = 0; i < objects[groundIdx].facesIdx.size(); ++i) {					//dla danej powierzchni obiektu pod³o¿a ['GROUND']
		Vector minPt, maxPt;
		std::pair<Vector, Vector> minMaxPair;
		IdxPair minIdx, maxIdx;
		Vector triangle[3];
		Vector normal;
		double Surface[4];
		unsigned int fIdx = objects[groundIdx].facesIdx[i];									//pobranie indeksu dla powierzchni wchodz¹cej w sk³ad obiektu pod³o¿a

		for(unsigned int j=0;j<3;j++)
			triangle[j]= vertices[faces[fIdx].PointsIdx[j]];								//przypisanie wart. wsp. wierzcho³ka danej powierzchni do tablicy wierzcho³ków
		normal = normals[faces[fIdx].NormalIdx];											//pobranie normalnej do danej powierzchni

		//obliczenie prametrów równania p³aszczyzny dla danej poweirzchni [A,B,C,D]
		for (unsigned int j = 0; j < 3; j++)
			Surface[j] = normal[j];												
		Surface[3] = -Surface[0] * triangle[0].x - Surface[1] * triangle[0].y - Surface[2] * triangle[0].z;

		minMaxPair = Load_Y_valMapData_GetMinMaxPt(fIdx);									//obliczenie najmniejszych i najwiêkszych wart. wsp. punktów dla powierzchni
		minPt = minMaxPair.first; maxPt = minMaxPair.second;								

		maxIdx=Load_Y_valMapData_GetMaxIdx(minPt, maxPt);									//pobranie wart. indeksu sektora najbardziej oddalonego od pocz¹tku macierzy [0.0]
		minIdx = Load_Y_valMapData_GetMinIdx(minPt, maxPt);									//pobranie wart. indeksu sekotra najbli¿ej pocz¹tku macierzy [0,0]

		Load_Y_valMapData_LoadData(minIdx, maxIdx, Surface, triangle, normal);				//za³adowanie wart. wsp. Y do macierzy wspó³rzêdnych
	}
}

//pobranie wsp. punktu o najwiêkszych i najmniejszych wartoœciach dla aktualnie rozwa¿anej powierzchni	
std::pair<Vector, Vector> Map::Load_Y_valMapData_GetMinMaxPt(unsigned int faceIdx) {		
	Vector minPt, maxPt;																	//wsp. punktów min i max
	minPt = maxPt = vertices[faces[faceIdx].PointsIdx[0]];									
	for (unsigned int j = 1; j < 3; j++) {													//wyszukanie najmniejszych i najwiêkszych wart. wspó³r¿êdnych dla punktów danej powierzchni
		for (unsigned int k = 0; k < 3; k++) {
			if (vertices[faces[faceIdx].PointsIdx[j]][k] < minPt[k])
				minPt[k] = vertices[faces[faceIdx].PointsIdx[j]][k];
			else if (vertices[faces[faceIdx].PointsIdx[j]][k] > maxPt[k])
				maxPt[k] = vertices[faces[faceIdx].PointsIdx[j]][k];
		}
	}

	std::pair<Vector, Vector> retVal;														//utworzenie pary zwrotnej punktów min i max
	retVal.first = minPt;
	retVal.second = maxPt;

	return retVal;
}

//pobranie indeksów sektora macierzy dla punku o najwiêkszych wart. wsp.
IdxPair Map::Load_Y_valMapData_GetMaxIdx(Vector minPt, Vector maxPt) {
	IdxPair maxIdx;																			//maks. wart. indeksów dla macierzy wspó³rzêdnych dla danych punktów 'minPt' i 'maxPt' 
	maxIdx = y_valMap.GetSector(maxPt.x, minPt.z);											//pobranie indeksów sektora dla danego punktu
	if (maxIdx.first < y_valMap.GetDimension() - 2) maxIdx.first += 2;						//jeœli to mo¿liwe pobierz indeks sektorów o 2 wiêkszych [zwiêksza obszar przeszukwianych sektorów]
	else if (maxIdx.first < y_valMap.GetDimension() - 1) maxIdx.first++;					//jeœli nie, to zwiêksz o 2
	if (maxIdx.second < y_valMap.GetDimension() - 2) maxIdx.second += 2;
	else if (maxIdx.second < y_valMap.GetDimension() - 1) maxIdx.second++;

	return maxIdx;
}

//pobranie indeksów sekotra macierzy dla punktu o najmniejszych wart. wsp.
IdxPair Map::Load_Y_valMapData_GetMinIdx(Vector minPt,Vector maxPt) {
	IdxPair minIdx;																			//min. wart. indeksów macierzy wspó³rzêdnych dla danych punktów 'minPt' i 'maxPt'
	minIdx = y_valMap.GetSector(minPt.x, maxPt.z);											//pobranie indeksów sektora dla danego punktu
	if (minIdx.first > 1) minIdx.first -= 2;												//jeœli to mo¿liwe pobierz indeks sektorów o 2 mniejszych [zwiêksza obszar przesukwianych sektorów]
	else if (minIdx.first > 0) minIdx.first--;												//jeœli nie to zmniejsz o 1
	if (minIdx.second > 1) minIdx.second -= 2;
	else if (minIdx.second > 0) minIdx.second--;

	return minIdx;
}

//za³adowanie wart. wsp. Y dla danych sektorów
//'minIdx', 'maxIdx' - min. i maks. badanych indeksy sektorów
//'Surface' - wsp. równania dla badanej powierzchni [A,B,C,D], 'traingle' - wierzcho³ki powierzchni, 'normal' - normalna do powierzchni 
void Map::Load_Y_valMapData_LoadData(IdxPair minIdx, IdxPair maxIdx,double Surface[],Vector triangle[],Vector normal) {
	Vector y_norm(0, 1, 0);																	//wektor normalny skierowany wzd³u¿ osi Y
	for (unsigned int j = minIdx.first; j < maxIdx.first; j++)								
		for (unsigned int k = minIdx.second; k < maxIdx.second; k++) {						//dla wszystkich sektorów ograniczonych przez indeksy 'minIdx' i 'maxIdx'
			double t;																		//parametr 't' dla parametrycznego równania prostej
			Vector mid = y_valMap[j][k].midPt;												//punkt œrodkowy danego sektora
			//obliczenie licznika i mianownika dla parametru 't' [rzutowanie punktu 'mid' wzd³u¿ wektora 'y_norm' na badan¹ powierzchniê]
			double licz = -Surface[3] - Surface[0] * mid.x - Surface[1] * mid.y - Surface[2] * mid.z;														
			double mian = Surface[0] * y_norm.x + Surface[1] * y_norm.y + Surface[2] * y_norm.z;
			if (mian == 0.0) t = 0.0;														//jeœli mianownik nie jest równy 0 - oblicz wart. 't'
			else t = licz / mian;

			mid = y_norm*t + mid;															//oblicz wart. punktu mid po zrzutowaniu na powierzchniê

			if (CheckPointInTriangle(mid, triangle, normal))								//jeœli punkt ten znajduje siê wewn¹trz badanej powierzchni
				y_valMap[j][k].midPt = mid;													//przypisz go do sektora

			for (unsigned int l = 0; l < 4; l++) {											//dla ka¿dego wierzcho³ka sektora
				Vector point = y_valMap[j][k][l];											//pobierz wart. wsp. wierzcho³ka 
				//obliczenie licznika dla parametru 't' [rzutowanie wierzcho³ka wzd³u¿ wektora 'y_norm' na badan¹ powierzchniê], mianownik niezale¿ny od badanego punktu
				licz = -Surface[3] - Surface[0] * point.x - Surface[1] * point.y -
					Surface[2] * point.z;
				if (mian == 0.0) t = 0.0;													//jeœli mianownik nie jest równy 0 - oblicz wart. 't'
				else t = licz / mian;

				point = y_norm*t + point;													//oblicz wart. wierzcho³ka po zrzutowaniu na poweirzchniê

				if (CheckPointInTriangle(point, triangle, normal))							//jeœli punkt ten znajduje siê wewn¹trz badanej powierzchni
					y_valMap[j][k][l] = point;												//przypisz go do sektora
			}
		}
}

//obliczenie wart. dla tablicy dystansów [odl. badanego punktu do ka¿dego wierzcho³ka sektora]
//'distTab' - ³adowana tablica dystansów, 'sectPt' - ³adowana tablica wierzcho³ków sektora, 'mainPt' - badany punkt, 'idx' - indeksy sektora 
void Map::Get_Y_Pos_LoadDistVertTab(std::pair<double,unsigned int> distTab[],Vector sectPt[],Vector mainPt,IdxPair idx) {
	Vector tmp1;
	for (unsigned int i = 0; i < 4; i++) {													//dla ka¿dego wierzcho³ka sektora
		sectPt[i] = y_valMap[idx.first][idx.second][i];										//wczytaj wart. wierzcho³ków sektora		
		tmp1 = Vector(mainPt - sectPt[i]);													//pomocniczy wektor przesuniêcia pomiêdzy danym wierzcho³kiem, a badanym punktem
		distTab[i].first = tmp1.MOD();														//obliczenie dystansu pomiêdzy danym wierzcho³kiem, a badanym punktem
		distTab[i].second = i;																//przypisanie indeksu wierzcho³ka do danego dystansu
	}
}

//posortowanie tablicy dystansów
void Map::Get_Y_Pos_SortDistTab(std::pair<double,unsigned int> distTab[]) {
	for (unsigned int i = 0; i<3; ++i)
		for (unsigned int j = i + 1; j < 4; ++j)											//posortowanie tablicy dystansów [od najmniejszego dystansu do najwiêkszego]
		{
			if (distTab[i] > distTab[j]) {
				std::pair<double, unsigned int> tmpD = distTab[i];
				distTab[i] = distTab[j];
				distTab[j] = tmpD;
			}
		}
	//usuniêcie artefaktów z posortowanej tablicy dystansów
	switch (distTab[0].second) {															
	case 0:																					//jeœli pierwszym indeksem jest 0 - to drugim indeksem powinien byæ albo 1 albo 3
		if (distTab[1].second != 1 || distTab[1].second != 3)
			if (distTab[2].second == 1) distTab[1].second = 1;								//jeœli wierzcho³ek '1' znajduje siê bli¿ej to drugim indeksem jest 1
			else distTab[1].second = 3;														//w przeciwnym wypadku 3
			break;
	case 1:																					//jeœli pierwszym indeksem jest 1 - to drugim indeksem powinien byæ albo 0 albo 2
		if (distTab[1].second != 0 || distTab[1].second != 2)
			if (distTab[2].second == 0) distTab[1].second = 0;								//jeœli wierzcho³ek '0' znajduje siê bli¿ej to drugim indeksem jest 0	
			else distTab[1].second = 2;														//w przeciwnym wypadku 2
			break;
	case 2:																					//jeœli pierwszym indeksem jest 2 - to drugim indeksem powinien byæ albo 1 albo 3
		if (distTab[1].second != 1 || distTab[1].second != 3)								
			if (distTab[2].second == 1) distTab[1].second = 1;								//jeœli wierzcho³ek '1' znajduje siê bli¿ej to drugim indeksem jest 1
			else distTab[1].second = 3;														//w przeciwnym wypadku 3
			break;
	case 3:																					//jeœli pierwszym indeksem jest 3 - to drugim indeksem powinien byæ albo 2 albo 0
		if (distTab[1].second != 2 || distTab[1].second != 0)
			if (distTab[2].second == 2) distTab[1].second = 2;								//jeœli wierzcho³ek '2' znajduje siê bli¿ej to drugim indeksem jest 2
			else distTab[1].second = 0;														//w preciwnym wypadku 0
			break;
	}
}

//obliczenie wart. wsp. Y dla badanego punktu w p³¹szczyŸnie (X,Z)
//'sectPt' - wierzcho³ki sektorów, 'mainPt' - badany punkt, 'distTab' - tablica dystansów
double Map::Get_Y_Pos_Count_Y_val(Vector sectPt[],Vector midPt,Vector mainPt,std::pair<double,unsigned int> distTab[]) {
	Vector normal,tmp1,tmp2;																//normal - normalna trójk¹ta wyznaczonego przez punkt œrodkowy sektora oraz dwa jego wierzcho³ki
	Vector yNorm(0, 1, 0);																	//wektor jednostkowy wzd³u¿ osi Y
	double Surface[4];																		//parametry równania powierzchni 
	double t, t_licz, t_mian;																//parametr 't' parametrycznego równania prostej

	tmp1 = Vector(sectPt[distTab[0].second] - midPt);										//wektor przesuniêcia pomiêdzy pierwszym wierz. z tablicy dyst. a punktem œr. sektora
	tmp2 = Vector(sectPt[distTab[1].second] - midPt);										//wektor przesuniêcia pomiêdzy drugim wierz. z tablicy dyst. a punktem œr. sektora
	normal = tmp1.CrossProduct(tmp2);														//obliczenie normalnej
	//za³adowanie wart. param. równania powierzchni
	for (unsigned int i = 0; i < 3; i++)
		Surface[i] = normal[i];
	Surface[3] = -Surface[0] * sectPt[1].x - Surface[1] * sectPt[1].y - Surface[2] * sectPt[1].z;
	//obliczenie licznika i minaownika param. 't' [rzut punktu 'mainPt' na poweirzchniê okreslon¹ przez wktor 'normal']
	t_licz = -Surface[3] - Surface[0] * mainPt.x - Surface[1] * mainPt.y - Surface[2] * mainPt.z;
	t_mian = Surface[0] * yNorm.x + Surface[1] * yNorm.y + Surface[2] * yNorm.z;

	if (t_mian != 0.0) t = t_licz / t_mian;													//jeœli mianownik nie jest równy 0 - oblicz wart. param. 't'
	else return mainPt.y;

	tmp1 = (yNorm*t) + mainPt;																//obliczenie wart. wsp. rzutowanego punktu 'mainPt' na badan¹ powierzchniê 

	return tmp1.y;																			//zwrócenie wsp. Y rzutowanego punktu
}

//pobranie indeksów postaci 'Character' znajduj¹cych siê w danym sektorze
//'mainIdx' - indeksy podstawowego sektora, 'vertexIdx' - indeks wybranego wierzcho³ka sektora [przy jego pomocy ustalani s¹ s¹siedzi sektora]
std::vector<unsigned> Map::GetSectorsCharacters_GetIdx(IdxPair mainIdx, unsigned vertexIdx) {
	std::vector<unsigned> retVec;															//zwracany wektor indeksów
	IdxPair neighbours[3];																	//indeksy sektorów s¹siadów sektora podstawowego
	MAP::edgeCheckFun edgeVal[2];

	GetSectorsCharacters_Objects_LoadNeighboursIdx(neighbours, mainIdx, vertexIdx);			//pobranie indeksów dla s¹siadów aktualnego sektora

	GetSectorsCharacters_Objects_LoadEdgeCheckFun(edgeVal, mainIdx, vertexIdx);				//pobranie funkcji testuj¹cych czy aktualny sektor znajduje siê na granicy macierzy

	retVec.insert(retVec.end(), sectors[mainIdx.first][mainIdx.second].charactersIdx.begin(),
		sectors[mainIdx.first][mainIdx.second].charactersIdx.end());						//dodanie indeksów z podstawowego sektora

	if (edgeVal[0](mainIdx, sectorDiv)) {													//jeœli aktualny sektor nie jest na granicy macierzy w osi X
		retVec.insert(retVec.end(), sectors[neighbours[0].first][neighbours[0].second].charactersIdx.begin(),
			sectors[neighbours[0].first][neighbours[0].second].charactersIdx.end());		//dodaj indeksy z s¹siedniego sektora

		if (edgeVal[1](mainIdx, sectorDiv))													//jeœli aktualny sektor nie jest na granicy macierzy w osi Z
			retVec.insert(retVec.end(), sectors[neighbours[1].first][neighbours[1].second].charactersIdx.begin(),
				sectors[neighbours[1].first][neighbours[1].second].charactersIdx.end());	//dodaj indeksy z s¹siedniego sektora
	}
	if (edgeVal[1](mainIdx, sectorDiv))														//jeœli aktualny sektor nie jest na granicy macierzy w osi Z
		retVec.insert(retVec.end(), sectors[neighbours[2].first][neighbours[2].second].charactersIdx.begin(),
			sectors[neighbours[2].first][neighbours[2].second].charactersIdx.end());		//dodaj indeksy z s¹siedniego sektora

	std::sort(retVec.begin(), retVec.end());												//posortowanie pobranych indeksów [dla funkcji 'erase']
	retVec.erase(std::unique(retVec.begin(), retVec.end()), retVec.end());					//usuniêcie powtórzeñ indeksów

	return retVec;
}

//pobranie indeksów obiektó 'objects' zmajduj¹cych siê w danym sektorze
//'mainIdx' - indeksy podstawowego sektora, 'vertexIdx' - indeks wybranego wierzcho³ka sektora [przy jego pomocy ustalani s¹ s¹siedzi sektora]
std::vector<unsigned> Map::GetSectorsObjects_GetIdx(IdxPair mainIdx, unsigned vertexIdx) {
	std::vector<unsigned> retVec;															//zwracany wektor indeksów
	IdxPair neighbours[3];																	//indeksy sektorów s¹siadó sektora podstawowego
	MAP::edgeCheckFun edgeVal[2];

	GetSectorsCharacters_Objects_LoadNeighboursIdx(neighbours, mainIdx, vertexIdx);			//pobranie indeksów dla s¹siadów aktualnego sektora

	GetSectorsCharacters_Objects_LoadEdgeCheckFun(edgeVal,mainIdx, vertexIdx);				//pobranie funkcji testuj¹cych czy aktualny sektor znajduje siê na granicy macierzy

	retVec.insert(retVec.end(), sectors[mainIdx.first][mainIdx.second].objectsIdx.begin(),
		sectors[mainIdx.first][mainIdx.second].objectsIdx.end());							//dodanie indeksów z podstawowego sektora

	if (edgeVal[0](mainIdx,sectorDiv)) {													//jeœli aktualny sektor nie jest na granicy macierzy w osi X
		retVec.insert(retVec.end(), sectors[neighbours[0].first][neighbours[0].second].objectsIdx.begin(),
			sectors[neighbours[0].first][neighbours[0].second].objectsIdx.end());			//dodaj indeksy z s¹siedniego sektora

		if (edgeVal[1](mainIdx,sectorDiv))													//jeœli aktualny sektor nie jest na granicy macierzy w osi Z
			retVec.insert(retVec.end(), sectors[neighbours[1].first][neighbours[1].second].objectsIdx.begin(),
				sectors[neighbours[1].first][neighbours[1].second].objectsIdx.end());		//dodaj indeksy z s¹siedneigo sektora
	}
	if (edgeVal[1](mainIdx,sectorDiv))														//jeœli aktualny sektor nie jest na granicy macierzy w osi Z
		retVec.insert(retVec.end(), sectors[neighbours[2].first][neighbours[2].second].objectsIdx.begin(),
			sectors[neighbours[2].first][neighbours[2].second].objectsIdx.end());			//dodaj indeksy z s¹siedniego sektora

	std::sort(retVec.begin(), retVec.end());												//posortowanie pobranych indeksów [dla funkcji 'erase']
	retVec.erase(std::unique(retVec.begin(), retVec.end()), retVec.end());					//usuniêcie powtórzeñ indeksów

	return retVec;
}

//za³adowanie wart. indeksów s¹siadów sektora ['neighbours'] w zaleznoœci od wybranego wierzcho³ka
void Map::GetSectorsCharacters_Objects_LoadNeighboursIdx(IdxPair neighbours[], IdxPair mainIdx, unsigned vertexIdx) {
	switch (vertexIdx) {																	//w zale¿noœci od wybranego wierzcho³ka sektora
		//za³aduj wartoœci indeksów dla s¹siadów aktualnego sektora w okolicy danego wierzcho³ka
	case 0:
		neighbours[0].first = mainIdx.first + 1;  neighbours[0].second = mainIdx.second;
		neighbours[1].first = mainIdx.first + 1; neighbours[1].second = mainIdx.second - 1;
		neighbours[2].first = mainIdx.first; neighbours[2].second = mainIdx.second - 1;
		break;
	case 1:
		neighbours[0].first = mainIdx.first - 1;  neighbours[0].second = mainIdx.second;
		neighbours[1].first = mainIdx.first - 1; neighbours[1].second = mainIdx.second - 1;
		neighbours[2].first = mainIdx.first; neighbours[2].second = mainIdx.second - 1;
		break;
	case 2:
		neighbours[0].first = mainIdx.first - 1;  neighbours[0].second = mainIdx.second;
		neighbours[1].first = mainIdx.first - 1; neighbours[1].second = mainIdx.second + 1;
		neighbours[2].first = mainIdx.first; neighbours[2].second = mainIdx.second + 1;
		break;
	case 3:
		neighbours[0].first = mainIdx.first + 1;  neighbours[0].second = mainIdx.second;
		neighbours[1].first = mainIdx.first + 1; neighbours[1].second = mainIdx.second + 1;
		neighbours[2].first = mainIdx.first; neighbours[2].second = mainIdx.second + 1;
		break;
	}
}

//za³adowanie funkcji pomocniczych  ['fun'] w zale¿noœci od wybranego wierzcho³ka
void Map::GetSectorsCharacters_Objects_LoadEdgeCheckFun(MAP::edgeCheckFun fun[],IdxPair mainIdx,unsigned vertexIdx) {
	switch (vertexIdx) {																	//w zaleznoœci od wybranego wierzcho³ka sektora
		//utwórz odpowiednie funkcje sprawdzaj¹ce czy dany sektor jest na granicy macierzy
		//sprawdzenie to s³u¿y podjêciu decyzji czy mozna wczytaæ indeksy s¹siadów dla aktualnego sektora
	case 0:
		fun[0] = [](IdxPair idx, unsigned sectorDiv)->bool {if (idx.first < sectorDiv - 1) return true; else return false; };
		fun[1] = [](IdxPair idx, unsigned sectorDiv)->bool {if (idx.second > 0)return true; else return false; };
		break;
	case 1:
		fun[0] = [](IdxPair idx, unsigned sectorDiv)->bool {if (idx.first > 0) return true; else return false; };
		fun[1] = [](IdxPair idx, unsigned sectorDiv)->bool {if (idx.second > 0) return true; else return false; };
		break;
	case 2:
		fun[0] = [](IdxPair idx, unsigned sectorDiv)->bool {if (idx.first > 0) return true; else return false; };
		fun[1] = [](IdxPair idx, unsigned sectorDiv)->bool {if (idx.second < sectorDiv - 1) return true; else return false; };
		break;
	case 3:
		fun[0] = [](IdxPair idx, unsigned sectorDiv)->bool {if (idx.first < sectorDiv - 1) return true; else return false; };
		fun[1] = [](IdxPair idx, unsigned sectorDiv)->bool {if (idx.second < sectorDiv - 1) return true; else return false; };
		break;
	}
}

//funkcja sprawdzaj¹ca czy dany punkt znajduje siê wewn¹trz trójk¹ta (w rzucie X,Z)
bool Map::CheckPointInTriangle(Vector point, Vector triangle[], Vector normal) {			//trójk¹t opisany jest przy pomocy 3 kolejno podanych punktów ['triangle'] oraz normalnej
	Vector n[3], AB, BC, CA;
	AB = triangle[1] - triangle[0]; BC = triangle[2] - triangle[1];
	CA = triangle[0] - triangle[2];															//inicjalizacja wektorów opisuj¹cych kolejne ramiona trójk¹ta
	n[0] = AB.CrossProduct(normal);															//obliczenie normalnych do ka¿dego boku trójk¹ta (wskazuj¹ one na zewn¹trz trójk¹ta)
	n[1] = BC.CrossProduct(normal);
	n[2] = CA.CrossProduct(normal);

	for (unsigned int i = 0; i < 3; i++) {													//dla ka¿dego boku
		double A, B, C, D;
		A = n[i].x; B = n[i].y; C = n[i].z;													//wyzanczamy parametry powierzchni opisuj¹cej dany bok	
		D = -A*triangle[i].x - B*triangle[i].y - C*triangle[i].z;							//[przy pomocy normalnej do danego boku oraz punktów wchodz¹cych w jego sk³ad]

		double t = -D - A*point.x - B*point.y - C*point.z;									//obliczenie parametru 't' dla parametrycznego równania prostej	
		t /= A*n[i].x + B*n[i].y + C*n[i].z;												//zaczynaj¹cej siê w punkcie 'point' i skierowanej zgodnie z wektorem normalnym

		if (t < 0) return false;															//jeœli wartoœc ta jest ujemna - punkt nie zanjduje siê wewn¹trz trójk¹ta - zwróæ FALSE
	}

	return true;																			//jeœli wszystkie wartoœci parametru 't' by³y dodatnie - punkt znajduje siê wewn¹trz trójk¹ta
}

//konstruktor sektora
SECTOR::SECTOR() {
	for (unsigned int i = 0; i < 4; i++)
		vertices[i] = Vector();																//za³aduj wierzcho³ki wartoœciami domyœlnymi
}

//konstruktor macierzy sektorów
SECTOR_MATRIX::SECTOR_MATRIX() {

}

//inicjalizacja macierzy sektorów
void SECTOR_MATRIX::Initialize(unsigned int _dimension,Vector _minPt, Vector _maxPt) {
	dimension = _dimension;																	//wczytanie wymiaru macierzy
	matrix = new SECTOR*[dimension];														//stworzenie macierzy sektorów o podanym wymiarze
	for (unsigned int i = 0; i < dimension; i++)
		matrix[i] = new SECTOR[dimension];

	minPoint = _minPt;																		//wczytanie wart. punktu min.
	maxPoint = _maxPt;																		//wczytanie wart. punktu max.

	xShift= (maxPoint.x - minPoint.x) / dimension;											//obliczenie d³ugoœci pojedynczego sektora w osi X 
	zShift = (maxPoint.z - minPoint.z) / dimension;											//obliczenie d³ugoœci pojedynczego sektora w osi Z
}

//pobranie indeksów sektora dla danego punktu w p³aszczyŸnie (X,Z)
IdxPair SECTOR_MATRIX::GetSector(double x, double z) {
	IdxPair idx;

	idx.first = static_cast<unsigned>((x - minPoint.x) / xShift);
	idx.second = static_cast<unsigned>((maxPoint.z - z) / zShift);

	return idx;
}

//przeci¹¿enie operatora '[]' [umo¿liwia to odwo³ywanie siê do konkretnrych sektorów przy u¿yciu sk³adni: MATRIX_SECTOR[0][0] ] 
SECTOR* SECTOR_MATRIX::operator[](unsigned int a) {
	if (a < dimension)
		return matrix[a];
	else return NULL;
}
