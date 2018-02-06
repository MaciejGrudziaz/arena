#pragma once
#include <vector>
#include <string>
#include <fstream>

//------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------LOGFILES-----------------------------------------------------
//--------------------klasa u¿ywana do tworzenia plików komunikatów [logów] wysy³anych przez grê--------------------
//-----------------------------------mo¿e tworzyæ i pisaæ do wielu plików na raz------------------------------------
//------------------------------------------------------------------------------------------------------------------

//struktura u¿ywana do przechowywania nazwy logu [nazwa poprzez któ¹ u¿ytkownik odnosi siê do konkrentego pliku]
//oraz nazwy pliku przechowuj¹cego dane komuniaktu
struct FileHeader {
	std::string name;																//nazwa logu
	std::string filename;															//nazwa pliku
	std::fstream fileHandler;

	//konstruktor wczytuj¹cy nazwê logu i nazwê pliku
	FileHeader(std::string Name, std::string FileName) { 
		name = Name; filename = "LOGS/"+FileName;
		fileHandler.open(filename);
		if (!fileHandler.is_open())														//sprawdzenie czy  plik o danej nazwie ju¿ istnieje
			fileHandler.open(filename, std::ios::out);
	}

	FileHeader(const FileHeader& header) {
		name = header.name;
		filename = header.filename;
		fileHandler.open(filename);
		if (!fileHandler.is_open())														//sprawdzenie czy  plik o danej nazwie ju¿ istnieje
			fileHandler.open(filename, std::ios::out);
	}
};

class LOGFILE {
	LOGFILE();																		

	static std::vector<FileHeader> logs;											//wektor przechowuj¹cy adresy nag³ówki logów [nazwy logów oraz zwi¹zane z nimi nazwy plików]

public:
	static void CreateLogFile(std::string Name, std::string FileName);				//stworzenie pliku logów
	static int ClearLogFile(std::string Name);										//wyczyszczenie istniej¹cego pliku logów
	static int WriteToLogFile(std::string Name, std::string message);				//zapis wiadomoœci do pliku
	static void OpenAllFiles();
	static void CloseAllFiles();													//zamkniêcie wszystkich plików logów przed zakoñczeniem pracy programu
};