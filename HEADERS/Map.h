#pragma once
#include <algorithm>
#include "GameObjects.h"

//-------------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------MAPA-----------------------------------------------------------
//---------------------------------------------klasa opisuj¹ca mapê rozgrywki----------------------------------------------
//----------[przechowuje obiekty znajduj¹ce siê na mapie, macierz punktów pomagaj¹c¹ wyznaczyæ wartoœci Y postaci----------
//-------------------------------oraz macierz sektorów w celu szybszego sprawdzania kolizji]-------------------------------
//-------------------------------------------------------------------------------------------------------------------------

namespace MAP {
	const unsigned int stdSectorDiv = 20;														//standardowa liczba dzielników dla sektorów wzd³u¿ ka¿dej krawêdzi
	const unsigned int stdMapDiv = 200;															//standardowa liczba dzielników dla macierzy punktów wzd³u¿ ka¿dej krawêdzi

	typedef std::pair<unsigned int, unsigned int> IdxPair;										//struktura opisuj¹ca parê indeksów (X,Z)
	typedef bool(*edgeCheckFun)(IdxPair idx, unsigned sectorDiv);								//wskaŸnik na funkcjê pomocnicz¹ dla funkcji 'GetSectorsCharacters_GetIdx' i 'GetSectorsObjects_GetIdx'

	enum sectorNeigbours { LEFT, RIGHT, TOP, BOTTOM, TOP_RIGHT, TOP_LEFT, BOTTOM_RIGHT, BOTTOM_LEFT };
}
//struktura opisuj¹ca pojedynczy sektor mapy
struct SECTOR {
	Vector vertices[4];																		//wierzcho³ki pojedynczego sektora [CCW]					  1  _______________  0
	Vector midPt;																			//punkt œrodkowy												|				|
	double radius;																			//promieñ sektora												|				|
	std::vector <unsigned int> charactersIdx;												//indeksy obiektów postaci w sektorze [Character]				|				|
	std::vector<unsigned int> objectsIdx;													//indeksy obiektów z modelu Mapy w sektorze [Objects]		  2 |_______________| 3	

	SECTOR();																				//konstruktor domyœlny

	Vector& operator[](unsigned int i) {													//przeci¹¿enie operatora [] - odnoszenie siê do kolejnych wierzcho³ków sekotra
		//if (i < 4)
			return vertices[i];
		//else return vertices[0];
	}
};

//klasa opisuj¹ca macierz punktów
class SECTOR_MATRIX {
	unsigned int dimension;																	//wymiar macierzy
	SECTOR** matrix;																		//tablica przechowuj¹ca wszystkie sektory macierzy
	Vector minPoint, maxPoint;																//punkt o najmniejszych i najwiekszych wspó³rzêdnych
	double xShift, zShift;																	//pojedyncze przesuniêcie wzd³u¿ osi X i Z - d³ugoœci boków sektora

public:
	SECTOR_MATRIX();																		//konstruktor domyœlny
	void Initialize(unsigned int dimension, Vector _minPt, Vector _maxPt);					//inicjalizacja macierzy
	//funkcje zwracaj¹ce kolejne wartoœci sk³adowych macierzy
	unsigned int GetDimension() { return dimension; }								
	IdxPair GetSector(double x, double z);
	double GetXshift() { return xShift; }
	double GetZshift() { return zShift; }
	Vector GetMinPoint() { return minPoint; }
	Vector GetMaxPoint() { return maxPoint; }

	SECTOR* operator[](unsigned int a);														//przeci¹¿enie operatora [][] - odnoszenie siê do kolejnych elementów macierzy
};
//	(minX,maxZ)	 _______________________  (maxX,maxZ)
//				|						|
//				|						|
//				|						|
//				|						|
//				|_______________________|
//	(minX,minZ)							  (maxX,minZ)

//klasa Mapy
class Map:public Character {
	enum ContainerType {CharCont, ObjCont};

	unsigned sectorDiv;
	unsigned mapDiv;
	unsigned int groundIdx;																	//indeks obiektu reprezentuj¹cego pod³o¿e
	Vector minPt;																			//punkt o najmniejszych wartoœciach wspó³rzêdnych
	Vector maxPt;																			//punkt o najwiêkszych wartoœciach wspó³rzêdnych
	std::vector <Character> characters;														//obiekty statyczne [Character] wchodz¹ce w sk³ad mapy

	//----------------------------------------------------------------------------SEKTORY MAPY-----------------------------------------------------------------------------
	//---------------'sectors' - macierz sektorów przechowuj¹ca indeksy postaci 'Character' oraz obiektów mapy 'objects' znajduj¹cych siê w strefie sektora----------------
	//----------'y_valMap' - macierz sektorów przechowuj¹ca wart. wsp. Y dla pod³o¿a mapy 'GROUND' dla danych punktów (X,Z) bêd¹cymi wierzch. oraz p. œr. sektora----------
	//-------------------------------------------------------------------Sposób opisu macierzy sektorów:-------------------------------------------------------------------
	//----------------------------------------------------------------SECTOR_MATRIX[0][0] = [minPt.x, maxPt.z]-------------------------------------------------------------
	//---------------------------------------------------------SECTOR_MATRIX[maxVal][maxVal] = [maxPt.x, minPt.z]----------------------------------------------------------
	//---------------------------------------------------------------------------------------------------------------------------------------------------------------------
	SECTOR_MATRIX sectors;																	//sektory mapy u¿ywane do detekcji kolizji obiektów znajduj¹cych wewn¹trz nich													
	SECTOR_MATRIX y_valMap;																	//macierz wspó³rzênych okreslaj¹ca wartoœci wsp. Y dla p³aszczyzny (X,Z)

	void LoadSectorsData();																	//za³adowanie danych do macierzy sektorów [sectors]
	void LoadSectorsData_LoadCharactersAndObjects(unsigned rowIdx,unsigned columnIdx);		//przypisanie obiektów Character i obeiktów mapy do danego sektora

	void Load_Y_valMapSectorsData();														//podzielenie macierzy wspó³rzêdnych na sektory

	void Load_Y_valMapData();																//za³adowanie danych dotycz¹cych wsp. Y dla wczesniej stworzonych sekotrów macierzy

	//funkcje pomocnicze dla 'Load_Y_valMapData()'
	std::pair<Vector,Vector> Load_Y_valMapData_GetMinMaxPt(unsigned int faceIdx);			//pobranie wsp. punktu o najwiêkszych i najmniejszych wartoœciach dla aktualnie rozwa¿anej powierzchni
	IdxPair Load_Y_valMapData_GetMaxIdx(Vector minPt, Vector maxPt);						//pobranie indeksów sektora macierzy dla punku o najwiêkszych wart. wsp.
	IdxPair Load_Y_valMapData_GetMinIdx(Vector minPt, Vector maxPt);						//pobranie indeksów sekotra macierzy dla punktu o najmniejszych wart. wsp.
	void Load_Y_valMapData_LoadData(IdxPair minIdx, IdxPair maxIdx,							
		double Surface[], Vector triangle[], Vector normal);								//za³adowanie wart. wsp. Y dla danych sektorów

	//funkcje pomocnicze dla 'Get_Y_Pos(..)'
	void Get_Y_Pos_LoadDistVertTab(std::pair<double, unsigned int> distTab[],				//obliczenie wart. dla tablicy dystansów [odl. badanego punktu do ka¿dego wierzcho³ka sektora]	
		Vector sectPt[], const Vector mainPt, IdxPair idx);									//oraz wczytanie wart. wsp. wierzcho³ków dla sektora
	void Get_Y_Pos_SortDistTab(std::pair<double, unsigned int> distTab[]);					//posortowanie tablicy dystansów
	double Get_Y_Pos_Count_Y_val(Vector sectPt[], Vector midPt, Vector mainPt,				
		std::pair<double, unsigned int> distTab[]);											//obliczenie wart. wsp. Y dla badanego punktu w p³¹szczyŸnie (X,Z)

	std::vector<unsigned> GetSectorsCharacters_GetIdx(IdxPair mainIdx, unsigned vertexIdx);	//pobranie indeksów postaci 'Character' znajduj¹cych siê w danym sektorze
	std::vector<unsigned> GetSectorsObjects_GetIdx(IdxPair mainIdx, unsigned vertexIdx);	//pobranie indeksów obiektó 'objects' znajduj¹cych siê w danym sektorze
	void GetSectorsCharacters_Objects_LoadNeighboursIdx(IdxPair neighbours[],	
		IdxPair mainIdx, unsigned vertexIdx);												//za³adowanie wart. indeksów s¹siadów sektora ['neighbours'] w zaleznoœci od wybranego wierzcho³ka
	void GetSectorsCharacters_Objects_LoadEdgeCheckFun(MAP::edgeCheckFun fun[], 
		IdxPair mainIdx, unsigned vertexIdx);												//za³adowanie funkcji pomocniczych  ['fun'] w zale¿noœci od wybranego wierzcho³ka

	static bool CheckPointInTriangle(Vector point, Vector triangle[], Vector normal);		//funkcja sprawdzaj¹ca czy dany punkt znajduje siê wewn¹trz trójk¹ta (w rzucie X,Z)

public:
	//konstruktory
	Map(const std::string& filename,unsigned _sectorDiv=MAP::stdSectorDiv,unsigned _mapDiv=MAP::stdMapDiv);
	Map(const std::string& filename, char* mapName, unsigned _sectorDiv = MAP::stdSectorDiv,
		unsigned _mapDiv = MAP::stdMapDiv);

	void LoadCharacter(const Character& character);											//za³adowanie obiektu Character do mapy
	void Initialize();																		//inicjalizacha mapy

	double Get_Y_Pos(Vector position);														//pobranie pozycji Y dla danego punktu ['position']  w p³aszczyxnie (X,Z)

	std::vector<unsigned> GetSectorCharactersIdx(Vector position, double R = 0.0);		//pobrnaie wektora indeksów postaci [Character] dla danego sektora ['sectors']
	std::vector<unsigned> GetSectorCharactersIdx(IdxPair sectorIdx);					
	std::vector<unsigned> GetSectorObjectsIdx(Vector position,double R=0.0);			//pobranie wektora indeksów obiektów dla danego sektora ['sectors']
	std::vector<unsigned> GetSectorObjectsIdx(IdxPair sectorIdx);

	Character& GetCharacter(unsigned int idx) { return characters[idx]; }					//pobranie postaci o danym indeksie

	Vector GetMaxPoint() { return maxPt; }
	Vector GetMinPoint() { return minPt; }

	IdxPair GetSectorIdx(const Vector& position);											//pobranie indeksów sektora dla danej pozycji
	IdxPair GetSectorNeighbourIdx(IdxPair sectorIdx, MAP::sectorNeigbours neighbour);
	std::pair<double, double> GetSectorShift() { 
		return std::pair<double, double>(sectors.GetXshift(), sectors.GetZshift());
	}

	void Draw();																			//funkcja rysuj¹ca mapê
	void DrawCollisionBox();																//funkcja rysuj¹ca bry³y kolizji
};