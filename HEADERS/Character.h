#pragma once
#include "OBJmodel.h"

//---------------------------------------------------------------------------------
//------------------------------------POSTACIE-------------------------------------
//----------klasa opisuj¹ca grywalne postacie [obiekty dynamiczne] w grze----------
//---------------------------------------------------------------------------------

typedef std::pair<unsigned int, unsigned int> IdxPair;

class Character:public OBJmodel {														//klasa postaci korzytsa z plików OBJ w celu przedstawienia postaci
protected:
	std::string name;																	//nazwa postaci

	unsigned health;

	bool enable;																		//zmienna okreœlaj¹ca czy postaæ jest dostêpna

	Vector nextPosition;																//nastêpna pozycja postaci [w kolejnej klatce]
	Vector previousPosition;															//poprzednia pozycja postaci
	Vector previousOrientation;															//poprzednia orientacja postaci

	IdxPair sectorIdx;																	//indeks sektora w którym aktualnie znajduje siê postaæ

public:
	Character():enable(false) {}
	Character(const std::string& filename);													//konstruktor podstawowy [nieokreslaj¹cy dok³adnej pozycji i orientacji postaci]
	Character(const std::string& filename,const std::string& Name, const Vector& Position, const Vector& Orientation);	//konstruktor wczytuj¹cy od razu nazwê, pozycjê i orientacjê postaci

	void Initialize(std::string Name, Vector Position, Vector Orientation);				//funkcja inicjalizuj¹ca postaæ [wczytuj¹ca nazwê, po³o¿enie i orientacjê postaci]

	void InitilizeTransformMatrix();													//inicjalizacja macierzy przekszta³ceñ
	
	void InitilizeTransformedVertices();												//inicjalizacja wektora przekszta³conych wspó³rzêdnych wierzcho³ków modelu

	void InitilizeTransformedNormals();

	void UpdateCollisionBoxVertices();													//aktualizacja pozycji wierzcho³ków dla bry³y kolizji
	void UpdateCollisionBoxNormals();

	//funkcje ustwiaj¹ce wartoœci dla odpowiednich pól klay
	void SetNextPosition(Vector newPosition) { nextPosition = newPosition; }
	void SetPreviousPosition(Vector newPosition) { previousPosition = newPosition; }
	void SetPreviousOrientation(Vector newOrientation) { previousOrientation = newOrientation; }

	void setName(std::string newName) { name = newName; }					
	void setPosition(Vector newPosition) { position = newPosition; }
	void setOrientation(Vector newOrientation) { orientation = newOrientation; }

	void setPositionX(double newX) { position.x = newX; }
	void setPositionY(double newY) { position.y = newY; }
	void setPositionZ(double newZ) { position.z = newZ; }

	void setOrientationX(double newX) { orientation.x = newX; }
	void setOrientationY(double newY) { orientation.y = newY; }
	void setOrientationZ(double newZ) { orientation.z = newZ; }

	void SetHealth(unsigned newHealth) { health = newHealth; }

	//funkcje zwracaj¹ce wartoœci odpowiednich pól klasy
	std::string getName() { return name; }
	Vector getPosition() { return position; }
	Vector getOrientation() { return orientation; }
	CollisionBox getCollisionBox() { return collisionBox;}
	unsigned int getObjectsCount() { return objects.size(); }
	unsigned int getObjectCollisionBoxCount(unsigned int objIdx) { 
		return objects[objIdx].collisionBox.size();		}
	CollisionBox getObjectCollisionBox(unsigned int objIdx, unsigned int colBoxIdx) {
		return objects[objIdx].collisionBox[colBoxIdx];	}
	CollisionBox getObjectMainCollisionBox(unsigned int objIdx) {
		return objects[objIdx].mainColBox;	}

	//funkcje ustawiaj¹ce wartoœci odpowiednich pól klasy
	Vector getVertice(unsigned int idx) { return vertices[idx]; }
	Vector getTransformedVertice(unsigned int idx) { return transformedVertices[idx]; }
	Vector getNormal(unsigned int idx) { return normals[idx]; }
	Vector getTransformedNormal(unsigned int idx) { return transformedNormals[idx]; }
	Face getFace(unsigned int idx) { return faces[idx]; }
	Matrix getTransformMatrix() { return transform; }

	Vector GetNextPosition() { return nextPosition; }
	Vector GetPreviousPosition() { return previousPosition; }
	Vector GetPreviousOrientation() { return previousOrientation; }
	IdxPair GetSectorIdx() { return sectorIdx; }

	unsigned GetHealth() { return health; }

	//w³¹czanie i wy³¹cznie postaci
	void enableCharacter() { enable = true; }
	void disableCharacter() { enable = false; }

	//sprawdzanie czy postaæ jest uruchomiona
	bool IsEnabled() { return enable; }

	//opeacje na punktach ¿ycia postaci
	void AddHealth(unsigned healthVal) { health += healthVal; }
	void SubstractHealth(unsigned healthVal) { 
		if (healthVal > health)
			health = 0;
		else health -= healthVal;
	}

	virtual void Update() {}
};