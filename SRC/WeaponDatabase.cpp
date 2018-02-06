#include "WeaponDatabase.h"

std::vector<WeaponDatabaseRecord> WeaponDatabase::database;

//inicjalizacja bazy danych
void WeaponDatabase::Initialize() {
	std::fstream file("RESOURCES/WeaponDatabase.txt", std::ios::in);													//otworzenie pliku opisuj¹cego bazê danych broni

	WeaponDatabaseRecord newRecord;																			//nowy rekord do bazy
	char tmp='\0';
	int eof = 0;
	while (tmp != '\n')																						//pominiêcie pierwszej linii pliku
		file.read(&tmp, 1);
	while (eof!=EOF) {																						//dopóki nie odczytano znaku koñca pliku
		std::string filename;
		file >> newRecord.name;																				//wczytanie nazwy broni					

		file >> filename;																					//wczytanie nazwy pliku opisuj¹cego model broni
		for (unsigned i = 0; i < filename.size(); i++)
			newRecord.filename[i] = filename[i];															//przypisanie go do nazwy pliku w strukturze rekordu
		newRecord.filename[filename.size()] = NULL;
		//wczytanie statystyk broni
		file >> newRecord.ROF;
		file >> newRecord.damage;
		file >> newRecord.bulletSpeed;
		file >> newRecord.magazineCapacity;
		file >> newRecord.reloadTime;
		database.push_back(newRecord);																		//dodanie nowego rekordu do bazy danych
		eof=file.peek();																					//sprawdzenie czy nastêpnym znakiem nie jest znak koñca pliku
	}
}

//pobranie rekordu z bazy danych [indeksuj¹c nazw¹ broni]
WeaponDatabaseRecord WeaponDatabase::GetWeaponStats(const std::string& weaponName) {
	for (WeaponDatabaseRecord record : database) {
		if (record.name == weaponName)																		//jeœli nazwa broni rekordu zgadza siê z szukan¹ nazw¹
			return record;																					//zwróæ dany rekord
	}
	return WeaponDatabaseRecord();																			//jeœli nie znaleziono poszukiwanego rekordu zwróæ pusty rekord
}