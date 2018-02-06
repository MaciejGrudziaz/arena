#pragma once
#include <vector>
#include "Vector.h"
#include "Matrix.h"

//------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------MODEL 3D--------------------------------------------------------------
//-----------------------------------------------klasa s³u¿¹ca za interfejs modelów 3D------------------------------------------------
//-----ka¿da klasa odpowiadaj¹ca za wczytywanie plików 3D w danym formacie musi zdefiniowaæ funkcje LoadModel(..) i DrawModel(..)-----
//------------------------------------------------------------------------------------------------------------------------------------

//struktura opisuj¹ca powierzchniê (trójk¹t)
struct Face{
	unsigned int PointsIdx[3];							//indeksy wierzcho³ków wchodz¹cych w sk³ad powierzchni [trójk¹ta]
	unsigned int NormalIdx;								//indeks normalnej dla danej poweirzchni
};

//struktura opisuj¹ca bry³ê kolizji
struct CollisionBox {
	std::vector<unsigned int> verticesIdx;				//wektor punktów opisuj¹cych model
	std::vector<unsigned int> facesIdx;					//wektor powierzchni wchdz¹cych w sk³ad modelu
	std::vector<unsigned int> normalsIdx;				//wektor normalnych do danych powierzchni

	Vector midPoint;									//punkt wyznaczaj¹cy geometryczny œrodek bry³y kolizji
	Vector maxPoint;									//punkt o maksymalnych wartoœciach wspó³r¿ednych dla ca³ej bry³y kolizji
	Vector minPoint;

	double radius;										//promieñ koreœlaj¹cy sferê kolizji [midPoint - œrodek sfery]

	Vector size;
};

//struktura opisuj¹ca obiekt
struct Object {
	std::string name;
	std::vector<unsigned int> facesIdx;					//indeksy powierzchni wchodz¹cych w sk³ad obiektu
	std::vector<unsigned int> verticesIdx;				//indeksy wierzcho³ków wchodz¹cych w sk³ad obiektu
	std::vector<CollisionBox> collisionBox;				//bry³y kolizji dla obiektu
	CollisionBox mainColBox;
};


//podstawowa klasa modelu 3D
class Model3D {
protected:
	std::vector<Vector> vertices;						//wektor punktów opisuj¹cych model
	std::vector<Face> faces;							//wektor powierzchni wchdz¹cych w sk³ad modelu
	std::vector<Vector> normals;						//wektor normalnych do danych powierzchni
	std::vector<Object> objects;						//wektor obiektów wchodz¹cych w sk³ad modelu

	std::vector<Vector> transformedVertices;			//wektor przekszta³conych wierzcho³ków modelu
	std::vector<Vector> transformedNormals;

	CollisionBox collisionBox;							//bry³a kolizji dla ca³ego modelu

	Vector position;									//obecna pozycja modelu
	Vector orientation;									//obecna orientacja modelu [przechowuje k¹t obrotu wokó³ osi x, y i z w STOPNIACH]

	Matrix transform;									//macierz przekszta³cenia jednorodnego opisuj¹ca przekszta³cenia wierzcho³ków modelu

public:
	virtual int LoadModel(const std::string& filename) = 0;			//funkcja ³¹duj¹ca model [funkcja wirtualna definiowana dla klasy danego typu plików]
	virtual void DrawModel() = 0;						//funkcja rysuj¹ca model [funkcja wirtualna definiowana dla klasy danego typu plików]

};