#pragma once
#include <windows.h>
#include <vector>

//-------------------------------------------------------------------------
//----------------------------------TIMER----------------------------------
//-------------------klasa u¿ywana do odmierzania czasu--------------------
//----------mo¿e odmierzaæ czas dla wielu znaczników jednoczeœnie----------
//-------------------------------------------------------------------------

//struktura u¿ywana do inicjalizowania znaczników czasowych
struct TIME_STAMPS {
	std::string name;															//nazwa znacznika
	LARGE_INTEGER start, stop;													//zmienne okreœlaj¹ce pocz¹tek i koniec odliczania czasu
	unsigned long elapsedTime;													//zmierzony czas [w mikrosekundach]
	std::vector<unsigned long> allTimeMeasurments;								//wszystkie pomiary czasowe

	//konstruktor
	TIME_STAMPS(std::string Name) {									
		name = Name; start = { 0 }; stop = { 0 }; elapsedTime = 0;				//pobiera nazwê znacznika, a dla pozosta³ych pól struktury ustawia wartoœci domyœlneS
	}
};

class TIMER {
	TIMER();																	//zablokowany [prywatny] konstruktor domyœlny - klasa singletonowa

	static LARGE_INTEGER frequency;												//czêstotliwoœæ performance countera

	static std::vector <TIME_STAMPS> markers;									//wektor znaczników czasowych

public:
	static void GetFrequency();													//pobranie czêstotliwoœci performance countera
	static void AddTimeMarker(std::string Name);								//dodanie nowego znacznika czasowego
	static int DeleteTimeMarker(std::string Name);								//usuniêcie znacznika czasowego
	static int StartTimeMeasurment(std::string Name);							//rozpoczêcie odmierzania czasu dla znacznika o danej nazwie
	static int QuickStartTimeMeasurment(int idx = -1);							//szybkie rozpoczêcie odmierzania czasu dla znacznika o danym indeksie
	static int StopTimeMeasurment(std::string Name);							//zakoñczenie odmierzania czasu dla znacznika o danej nazwie
	static int QuickStopTimeMeasurment(int idx = -1);							//szybkie zakoñczenie odmierzania czasu dla znacznika o danym indeksie
	static unsigned long GetTime(std::string Name);								//pobranie zmierzonego czasu dla danego znacznika
	static std::vector<unsigned long> GetAllTimeMeasurments(std::string Name);	//pobranie wszystkich zmierzonych i zapisanych wartoœci czasu dla danego znacznika
};