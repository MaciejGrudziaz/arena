#include "LOGFILE.h"

std::vector<FileHeader> LOGFILE::logs;														//inicjalizacja wektora przechowuj¹cego nag³ówki logów

//stworzenie nowego pliku logów
void LOGFILE::CreateLogFile(std::string Name, std::string FileName) {
	FileHeader newHeader(Name, FileName);													//stworzenie nowego nag³ówka [jest jednoznaczne z otworzeniem pliku]
	logs.push_back(newHeader);																//dodanie nowego nag³ówka do wektora
}

//zapis wiadomoœci do danego pliku logów
int LOGFILE::WriteToLogFile(std::string Name, std::string message) {
	for (FileHeader& header : logs) {
		if (header.name == Name) {															//sprawdŸ czy nazwa jest zgodna z nazw¹ danego logu
			header.fileHandler << message << "\n";											//wpisanie nowej wiadomoœci do logu
			return 0;																		//operacja powiod³a siê
		}
	}

	return 1;																				//operacja nie powiod³a siê										
}

//wyczyszczenie pliku logów
int LOGFILE::ClearLogFile(std::string Name) {
	for (FileHeader& header : logs) {
		if (header.name == Name) {															//sprawdŸ czy nazwa jest zgodna z nazw¹ danego logu
			if (header.fileHandler.is_open()) header.fileHandler.close();					//jeœli dany plik jest otwarty, zamknij go najpierw
			header.fileHandler.open(header.filename, std::ios::out | std::ios::trunc);		//otwórz dany plik czyszcz¹c jego zawartoœæ
			return 0;																		//operacja powiod³a siê
		}
	}

	return 1;																				//operacja nie powiod³a siê
}

void LOGFILE::OpenAllFiles() {
	for (FileHeader& header : logs)
		header.fileHandler.open(header.filename,std::ios::in | std::ios::app);
}

//zamkniêcie wszystkich plików logów przed zakoñczeniem pracy programu
void LOGFILE::CloseAllFiles() {
	for (FileHeader& header : logs) {
		header.fileHandler.close();															//zamkniêcie danego logu
	}
}