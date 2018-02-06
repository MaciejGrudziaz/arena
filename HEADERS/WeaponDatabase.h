#pragma once
#include <string>
#include <vector>
#include <fstream>

//-----------------------------------------------------------------------------
//------------------------------BAZA DANYCH BRONI------------------------------
//-----------------------------------------------------------------------------

//struktura opisuj¹ca rekord z bazy danych
struct WeaponDatabaseRecord {
	std::string name;																			//nazwa broni
	char filename[30];																			//nazwa pliku modelu broni
	//statystyki
	double damage;																				
	unsigned ROF;																				
	double bulletSpeed;
	unsigned magazineCapacity;
	unsigned reloadTime;

	WeaponDatabaseRecord() :name(""), filename(""), damage(0), ROF(0), bulletSpeed(0), magazineCapacity(0) {}
};

class WeaponDatabase {
	static std::vector<WeaponDatabaseRecord> database;											//wektor przechowuj¹cy rekordy bazy danych

public:
	static void Initialize();																	//inicjalizacja bazy danych
	static WeaponDatabaseRecord GetWeaponStats(const std::string& weaponName);					//pobranie rekordu z bazy danych [indeksuj¹c nazw¹ broni]
};