#pragma once
#include <windows.h>
#include <gl/GL.h>
#include <fstream>
#include <sstream>
#include "Model3D.h"
#include "Vector.h"

#define vertices_VEC 0
#define transformedVertices_VEC 1

//------------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------TYP PLIKÓW OBJ-----------------------------------------------------
//------------------------------klasa odpowiadaj¹ca za wczytywanie plików 3D w formacie OBJ-------------------------------
//------------------------------[musi mieæ zdefiniowane funkcje LoadModel(..) i DrawModel()]------------------------------
//------------------------------------------------------------------------------------------------------------------------

class OBJmodel: public Model3D {
	//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//-------------------------------------------------------------FUNKCJE POMOCNICZA DLA WCZYTYWANIA DANYCH MODELU------------------------------------------------------------
	//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//funkcje charakterystyczne dla formatu plików OBJ
	int char2int(char* string, int n);									//zamiana tablicy znaków char na liczbê ca³kowit¹ int

	//funkcje przemieszczaj¹ce znacznik w tekœcie do docelowej pozycji [opuszcanie znaków]
	void skipFileBegin(std::fstream& file);								//pominiêcie znaków na pocz¹tku pliku a¿ do pocz¹tku wspó³rzêdnych wierzcho³ków
	std::string readObjectName(std::fstream& file);
	void skipMidFile(std::fstream& file);								//pominiêcie znaków w œrodku pliku od koñca wspó³rzednych normalnych do pocz¹tku danych dla powierzchni

	bool loadObject(std::fstream& file,std::string objectName);			//za³adowanie danych obiektu
	bool loadCollisionBox(std::fstream& file, std::string objectName);	//za³adowanie danych bry³y kolizji

	//funkcje odpowiadaj¹ce za wczytywanie wspó³rzêdnych wierzcho³ków
	Vector VERTEX_getVertex(std::fstream& file);						//pobranie wspó³rzêdnych wierzcho³ka
	void VERTEX_LoadVertices(std::fstream& file);						//za³adowanie danych dla wszystkich wierzcho³ków

	//funkcje odpowiadaj¹ce za wczytywanie wspó³rzêdnych normalnych
	Vector NORMAL_getNormal(std::fstream& file);						//pobranie wspó³rzêdnych normalnej
	void NORMAL_LoadNormals(std::fstream& file);						//za³adowanie danych dla wszystkich normalnych

	//funkcje odpowiadj¹ce za wczytywanie danych dla powierzchni
	int FACE_readPtIdx(std::fstream& file);								//wczytanie indeksu wierzcho³ka [niskopoziomowo]
	int FACE_readNormIdx(std::fstream& file);							//wczytanie indeksu normalnej dla powierzchni	[niskopoziomowo]
	void FACE_skipSpace(std::fstream& file, int idx);					//opuszcanie znaków w wierszu w zaleznoœci od obecnego po³o¿enia znacznika w tekœcie
	int FACE_readFaceVertexIdx(std::fstream& file, int idx);			//wczytanie indeksu wierzcho³ka [wysokopoziomowo]
	int FACE_readFaceNormalIdx(std::fstream& file);						//wczytanie indeksu normalnej [wysokopoziomowo]
	Face FACE_getFace(std::fstream& file);								//wczytanie danych dla danej powierzchni
	int FACE_LoadFaces(std::fstream& file);								//wczytanie danych dla wszystkich powierzchni

	//funkcje odpwoiadajce za wczytywanie danych dla obiektu
	void OBJECT_LoadObject(unsigned int startIdx, 
		unsigned int endIdx,std::string objectName);					//wczytanie indeksów powierzchni dla danego obiektu
	bool OBJECT_CheckVertexInVector(Object obj,unsigned int vertexIdx);	//sprawdzenie czy wierzcho³ek o danym indeksie znajduje siê w wekotrze wirzcho³ków obiektu
	int OBJECT_findObject(std::string objectName);						//sprawdzenie czy dany obiekt ju¿ istnieje

	//funkcje odpowiadaj¹ce za wczytywanie danych dla bry³y kolizji
	void COLLISIONBOX_LoadData(CollisionBox &colBox,
		unsigned int verticesStartIdx, unsigned int facesStartIdx, unsigned int normalsStartIdx);
	void COLLISIONBOX_LoadCollisionToObject(CollisionBox& newCollision, std::string objectName, unsigned int objIdx);
	bool COLLISIONBOX_LoadCollisionToObject_CheckIfMainHitbox(std::string& objectName);
	void COLLISIONBOX_LoadCollisionToObject_LoadObjectMainHitbox(std::string objectName, CollisionBox& newCollision);


	//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//-----------------------------------------------------------------FUNKCJE POMOCNICZE DLA RYSOWANIA MODELU-----------------------------------------------------------------
	//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//pobranie wpó³rzêdnych wszystkich wierzcho³ków oraz wchodz¹cych w sk³ad powierzchni oraz normalnej do tej powierzchni
	//powierzchnia dana jest indeksem 'faceIdx'
	void DRAW_getFaceData(Vector tmpVertices[], Vector& normal, unsigned int faceIdx, int verticesVector=0);
	void ModelTransformation();											//przekszta³cenie macierzy widoku modelu

public:
	OBJmodel();															//konstruktor inicjalizuj¹ce zmienne
	int LoadModel(const std::string& filename);										//³adowanie danych modelu z pliku
	void DrawModel();													//funkcja rysuj¹ca model
	void DrawCollisionBox();
};