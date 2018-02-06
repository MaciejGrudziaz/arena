#pragma once
#include "Weapon.h"

//--------------------------------------------------------------------------------------
//------------------------------KLASA OPISUJ¥CA BROÑ PALN¥------------------------------
//--------------------------------------------------------------------------------------

//struktura opisuj¹ca pocisk
struct Bullet {
	std::string parentName;																							//nazwa obiektu rodzica który wystrzeli³ pocisk
	bool hit;																										//zmienna sprawdzaj¹ca czy pocisk uderzy³ w obiekt
	double damage;																									//obra¿enia zadawane przez pocisk
	double speed;																									//prêdkoœæ pocisku

	Vector direction;																								//kierunek lotu pocisku
	Vector startPos;																								//pozycja startowa pocisku
	Vector hitPos;																									//pozycja dla kolizji pocisku
	Vector actualPos;																								//aktualna pozycja pocisku
	Vector previousPos;																								//poprzednia pozycja pocisku
	
	unsigned long lifetime;																							//czas ¿ycia pocisku

	Bullet() :parentName(""), damage(0), speed(0), direction(Vector(0, 0, 0)), startPos(Vector(0, 0, 0)), hitPos(startPos),lifetime(0),actualPos(startPos),previousPos(startPos),hit(false) {}
	Bullet(const std::string& _parentName, double _damage, double _speed, const Vector& _direction, const Vector& _startPos) :
		parentName(_parentName), damage(_damage), speed(_speed), direction(_direction), startPos(_startPos), hitPos(_startPos),lifetime(0),actualPos(startPos),previousPos(startPos),hit(false) {}
};

class Gun :public Weapon {
	std::string shotTimerName;																						//nazwa TIMERA dla odliczania czasu pomiêdzy wystrza³ami
	std::string reloadTimerName;

	unsigned ROF;																									//szybkostrzelnoœæ broni
	double shotTimeDif;																								//ró¿nica czasowa pomiêdzy strza³ami - sta³a [w mikrosekundach]
	double timeFromLastShot;																						//czas który up³yn¹³ od ostatniego strza³u [w mikrosekundach]
	double damage;																									//obra¿enia zadawane przez broñ
	double bulletSpeed;																								//prêdkoœæ pocisków
	unsigned magazineCapacity;																						//pojemnoœæ magazynka
	unsigned bulletsCount;																							//³¹czna liczba pocisków
	unsigned bulletsInMagLeft;																						//³¹czna liczba pocisków pozosta³ych w magazynku

	unsigned reloadState;
	unsigned long reloadTime;
	unsigned long actReloadTime;

	std::vector<Bullet> shotsFired;																					//wektor wystrzelonych pocików

public:

	Gun(const std::string& _name, const std::string& _parentName, const Vector& _parentMidPoint, const Vector& parentPos, const Vector& parentOrient);

	void PerformAdditionalAction(ActionPtr Action);																	//dodatkowe akcje

	void Shoot();																									//funkcja wykonuj¹ca strza³ z broni

	void ClearAllBullets() { shotsFired.clear(); }																	//usuniêcie wszystkich pocisków z wektora 'shotsFired'

	void UpdateBullets();																							//aktualizacja stanu pocisków

	void DrawModel();																								//narysowanie modelu
	void DrawCrossHair();																							//narysowanie celownika

	//funkcej zwracaj¹ce sk³adowe klasy
	unsigned GetShotsFiredCount() { return shotsFired.size(); }
	Bullet& GetBullet(unsigned idx) { return shotsFired[idx]; }
	unsigned GetBulletsLeftInMag() { return bulletsInMagLeft; }
	unsigned GetBulletsCount() { return bulletsCount; }
	unsigned GetReloadState() { return reloadState; }
	unsigned GetMagazineCapacity() { return magazineCapacity; }

	void SetBulletsLeftInMag(unsigned n) { bulletsInMagLeft = n; }
	void SetBulletsCount(unsigned n) { bulletsCount = n; }
	void SetReloadState(unsigned n) { 
		if (n > 100)
			reloadState = 100;
		else reloadState = n;
	}

	void AddBulletsToMag(unsigned n) { bulletsInMagLeft += n; }
	void AddBullets(unsigned n) { bulletsCount += n; }

	void Update();
};