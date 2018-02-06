#include "Character.h"

//podstawowy konstruktor postaci okreœlaj¹cy tylko model tworzonego obiektu
Character::Character(const std::string& filename) {
	LoadModel(filename);																	//wczytanie modelu

	name = "";																				//ustawienie domyœlnych wartoœci dla odpowiednich pól klasy
	position = { 0,0,0 };
	orientation = { 0,0,0 };
	sectorIdx = IdxPair(0, 0);
	enable = false;
	health = 100;

	InitilizeTransformMatrix();
	InitilizeTransformedVertices();
	InitilizeTransformedNormals();
	UpdateCollisionBoxVertices();
}

//konstruktur okreœlaj¹cy od razu nazwê, po³o¿enie i oreintacjê postaci
Character::Character(const std::string& filename, const std::string& Name, const Vector& Position, const Vector& Orientation) {
	LoadModel(filename);																	//wczytanie modelu

	name = Name;																			//ustwienie wartoœci dla odpowiednich pól klasy

	position = Position;
	orientation = Orientation;
	sectorIdx = IdxPair(0, 0);
	enable = false;
	health = 100;

	InitilizeTransformMatrix();
	InitilizeTransformedVertices();
	InitilizeTransformedNormals();
	UpdateCollisionBoxVertices();
}

//inicjalizacja postaci
void Character::Initialize(std::string Name, Vector Position, Vector Orientation) {
	name = Name;																			//ustwienie wartoœci dla odpowiednich pól klasy
	position = Position;
	orientation = Orientation;
	sectorIdx = IdxPair(0, 0);

	InitilizeTransformMatrix();
	InitilizeTransformedVertices();
	InitilizeTransformedNormals();
	UpdateCollisionBoxVertices();
}

//inicjalizacja macierzy przekszta³ceñ [macierz przekszta³cenia jednorodnego]
//w celu wygenerowania macierzy przekszta³ceñ wykorzystywana jest pozycja postaci oraz jej orientacja
void Character::InitilizeTransformMatrix() {
	Vector angle(orientation); angle = angle / 360.0 * 2.0 * 3.1415;						//wartoœci k¹tów orientacji postaci w radianach
	//wpisanie odpowiednich wartoœci do macierzy przekszat³ceñ
	transform[0][0] = cos(angle.y)*cos(angle.z); transform[0][1] = cos(angle.z)*sin(angle.x)*sin(angle.y) - cos(angle.x)*sin(angle.z);
	transform[0][2] = sin(angle.x)*sin(angle.z) + cos(angle.x)*cos(angle.z)*sin(angle.y); transform[0][3] = position.x;

	transform[1][0] = cos(angle.y)*sin(angle.z); transform[1][1] = cos(angle.x)*cos(angle.z) + sin(angle.x)*sin(angle.y)*sin(angle.z);
	transform[1][2] = cos(angle.x)*sin(angle.y)*sin(angle.z) - cos(angle.z)*sin(angle.x); transform[1][3] = position.y;

	transform[2][0] = -sin(angle.y); transform[2][1] = cos(angle.y)*sin(angle.x); transform[2][2] = cos(angle.x)*cos(angle.y); transform[2][3] = position.z;

	transform[3][0] = transform[3][1] = transform[3][2] = 0.0; transform[3][3] = 1.0;
}

//inicjalizacja wekotra przekszta³conych wspó³rzêdnych wierzcho³ków modelu
void Character::InitilizeTransformedVertices() {
	for (unsigned int i = 0; i < vertices.size(); i++)
		transformedVertices.push_back(transform*vertices[i]);
}

void Character::InitilizeTransformedNormals() {
	for (unsigned int i = 0; i < normals.size(); i++)
		transformedNormals.push_back(transform*normals[i]);
}

//aktualizacja wierzcho³ków bry³y kolizji [dla wszystkich bry³ kolizji wchodz¹cych w sk³ad postaci]
void Character::UpdateCollisionBoxVertices() {
	for (unsigned int i = 0; i < objects.size(); i++) {										//sprawdzenie wszystkich obiektów wchodz¹cych w sk³ad modelu
		unsigned int colBoxSize = objects[i].collisionBox.size();
		for (unsigned int j = 0; j < colBoxSize; j++) {										//sprawdzenie wszystkich bry³ kolizji dla ka¿dego obiektu
			unsigned int vertSize = objects[i].collisionBox[j].verticesIdx.size();
			for (unsigned int k = 0; k < vertSize; k++) {
				unsigned int idx = objects[i].collisionBox[j].verticesIdx[k];				//wczytanie indeksu wierzcho³ka bry³y kolizji
				transformedVertices[idx] = transform*vertices[idx];							//aktualizacja wspó³rzêdnych wierzcho³ka bry³y kolizji dla pojedynczych obiektów modelu
			}
		}
	}

	//aktualizacja wierzcho³ków dla bry³y kolizji ca³ego modelu							
	for (unsigned int i = 0; i < collisionBox.verticesIdx.size(); i++) {					//sprawdzenie wszystkich wierzcho³ków z bry³y kolizji
		unsigned int idx = collisionBox.verticesIdx[i];										//wczytanie indeksu wierzcho³ka bry³y kolizji
		transformedVertices[idx] = transform*vertices[idx];									//aktualizacja wspó³rzêdnych bry³y kolizji dla ca³ego modelu
	}
}

void Character::UpdateCollisionBoxNormals() {
	for (unsigned int i = 0; i < objects.size(); i++) {										//sprawdzenie wszystkich obiektów wchodz¹cych w sk³ad modelu
		unsigned int colBoxSize = objects[i].collisionBox.size();
		for (unsigned int j = 0; j < colBoxSize; j++) {										//sprawdzenie wszystkich bry³ kolizji dla ka¿dego obiektu
			unsigned int faceSize = objects[i].collisionBox[j].normalsIdx.size();
			for (unsigned int k = 0; k < faceSize; k++) {
				unsigned int idx = objects[i].collisionBox[j].normalsIdx[k];				//wczytanie indeksu wierzcho³ka bry³y kolizji
				transformedNormals[idx] = transform*normals[idx];						//aktualizacja wspó³rzêdnych wierzcho³ka bry³y kolizji dla pojedynczych obiektów modelu
			}
		}
	}

	//aktualizacja wierzcho³ków dla bry³y kolizji ca³ego modelu							
	for (unsigned int i = 0; i < collisionBox.normalsIdx.size(); i++) {					//sprawdzenie wszystkich wierzcho³ków z bry³y kolizji
		unsigned int idx = collisionBox.normalsIdx[i];										//wczytanie indeksu wierzcho³ka bry³y kolizji
		transformedNormals[idx] = transform*normals[idx];									//aktualizacja wspó³rzêdnych bry³y kolizji dla ca³ego modelu
	}
}