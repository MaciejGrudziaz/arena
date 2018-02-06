#include "TIMER.h"

LARGE_INTEGER TIMER::frequency = { 0 };									//inicjalizacja czêstotliwoœci performance countera

std::vector <TIME_STAMPS> TIMER::markers;								//inicjalizacja wektora zanczników czasowych

//pobranie czêstotlwioœci perfomrance countera
void TIMER::GetFrequency() {
	QueryPerformanceFrequency(&frequency);
}

//dodanie nowego znacznika czasowego
void TIMER::AddTimeMarker(std::string Name) {
	TIME_STAMPS newMarker(Name);
	markers.push_back(newMarker);
}

//usuniêcie zancznika czasowego
int TIMER::DeleteTimeMarker(std::string Name) {
	//bool search = true;
	unsigned int i = 0;

	while (i<markers.size()) {											//dopóki nie przekoroczono zakresu wektora znaczników											
		if (markers[i].name == Name) {									//sprawdŸ zgodnoœæ nazwy z nazw¹ kolejnego znacznika
			markers.erase(markers.begin() + i);							//jeœli s¹ zgodne - usuñ znacznik
			return 1;													//pomyœlne ukoñczenie usuwania
		}
		i++;															//inkrementacja indeksu wetora znaczników
	}

	return 0;															//usuniêcie nie powiod³o sie
}

//rozpoczêcie odmierzania czasu dla danego znacznika
int TIMER::StartTimeMeasurment(std::string Name) {
	unsigned int i = 0;

	while (i<markers.size()) {											//dopóki nie przekroczono zakresu wektora znaczników
		if (markers[i].name == Name) {									//sprawdŸ zgodnoœæ nazwy z nazw¹ aktualnego znacznika
			QueryPerformanceCounter(&(markers[i].start));				//jeœli s¹ zgodne - przeka¿ odpowiedni¹ wartoœæ do zmiennej mierz¹cej pocz¹tek odliczania
			return 1;													//pomyœlne ukoñczenie operacji
		}
		i++;															//inkrementacja indeksu wektora znaczników
	}

	return 0;															//operacja nie powiod³a siê
}

//szybkie rozpoczêcie odmierzania czasu dla o danym indeksie w wekotrze znaczników
int TIMER::QuickStartTimeMeasurment(int idx) {
	if (idx >= 0 && idx < (int)markers.size()) {								//sprawdzenie poprawnoœci podanego indeksu [wart. domyœlna = -1]
		QueryPerformanceCounter(&(markers[idx].start));					//przekazanie odpowiedniej wartoœci do zmiennej mierz¹cej pocz¹tek odliczania
		return 1;														//operacja powiod³a siê
	}
	else return 0;														//operacja nie powiod³a siê
}

//zakoñczenie odmierzania czasu dla danego znacznika
int TIMER::StopTimeMeasurment(std::string Name) {
	unsigned int i = 0;

	while (i<markers.size()) {											//dopóki nie przekroczono zakresu wektora znaczników
		if (markers[i].name == Name) {									//sprawdŸ zgodnoœæ nazwy z nazw¹ aktualnego znacznika
			QueryPerformanceCounter(&(markers[i].stop));				//jeœli s¹ zgodne - przeka¿ odpowiednia wartoœæ do zmiennej mierz¹cej koniec odliczania
			//zmierz up³yw czasu przy pomocy zmiennych 'start' i 'stop'
			//ró¿nica miêdzy zmiennymi 'stop' i 'start' dla danego znacznika czasowego okreœla iloœæ cyklów performance countera
			//ró¿nica ta podzielona przez czêstotliwoœæ performance countera pozwala uzyskaæ czas w sek.
			//pomno¿enie wyniku przez 1,000,000 pozwala uzyskaæ wynik w mikrosekundach, co przenosi siê na wiêksz¹ dok³adnoœæ w pomiarze
			markers[i].elapsedTime = (unsigned long) ( (markers[i].stop.QuadPart - markers[i].start.QuadPart) * 1000000.0 / frequency.QuadPart );
			markers[i].allTimeMeasurments.push_back(markers[i].elapsedTime);
			return 1;													//operacja powiod³a siê
		}
		i++;															//inkrementacja indeksu wektora znacnzików
	}

	return 0;															//operacja nie powiod³a siê
}

//szybkie zakoñczenie pomiaru czasu dla znacznika o danym indeksie w wektorze znaczników
int TIMER::QuickStopTimeMeasurment(int idx) {
	if (idx >= 0 && idx < (int)markers.size()) {								//sprawdzenie poprawnoœci podanego indeksu [wart. domyœlna = -1]
		QueryPerformanceCounter(&(markers[idx].stop));					//przekazanie odpoweidniej wartoœci do zmiennej mierz¹cej koniec odliczania
		//zmierz up³yw czasu przy pomocy zmiennych 'start' i 'stop'
		//ró¿nica miêdzy zmiennymi 'stop' i 'start' dla danego znacznika czasowego okreœla iloœæ cyklów performance countera
		//ró¿nica ta podzielona przez czêstotliwoœæ performance countera pozwala uzyskaæ czas w sek.
		//pomno¿enie wyniku przez 1,000,000 pozwala uzyskaæ wynik w mikrosekundach, co przenosi siê na wiêksz¹ dok³adnoœæ w pomiarze
		markers[idx].elapsedTime = (unsigned long) ( (markers[idx].stop.QuadPart - markers[idx].start.QuadPart) * 1000000.0 / frequency.QuadPart );
		return 1;														//operacja powiod³a siê
	}
	else return 0;														//operacja nie powiod³a siê
}

//pobranie odmierzonego czasu dla danego znacznika
unsigned long TIMER::GetTime(std::string Name) {
	unsigned int i = 0;
	unsigned int retTime=0;												//wartoœæ zwracana odmierzonego czasu

	while (i < markers.size()) {										//dopóki nie przekroczono zakresu wektora znaczników
			if (markers[i].name == Name) {								//sprawdŸ zgodnoœæ nazwy z nazw¹ aktualnego znacznika
				retTime = markers[i].elapsedTime;						//jeœli s¹ zgodne - przeka¿ zmierzon¹ wartoœæ czasu do zwrócenia
				break;													//zakoñcz wykonywanie pêtli
			}
			i++;														//inkrementacja indeksu wektora znaczników
	}

	return retTime;														
}

//pobranie ca³ego wekotra pomiarów czasu dla danego znacznika
std::vector<unsigned long> TIMER::GetAllTimeMeasurments(std::string Name) {
	unsigned int i = 0;
	std::vector<unsigned long> retTimes;								//wartoœæ zwracana odmierzonego czasu

	while (i < markers.size()) {										//dopóki nie przekroczono zakresu wektora znaczników
		if (markers[i].name == Name) {									//sprawdŸ zgodnoœæ nazwy z nazw¹ aktualnego znacznika
			retTimes = markers[i].allTimeMeasurments;					//jeœli s¹ zgodne - przeka¿ zmierzon¹ wartoœæ czasu do zwrócenia
			break;														//zakoñcz wykonywanie pêtli
		}
		i++;															//inkrementacja indeksu wektora znaczników
	}

	return retTimes;
}