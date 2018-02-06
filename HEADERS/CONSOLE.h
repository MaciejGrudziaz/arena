#pragma once
#include "WinAPIwindow.h"
#include "FONT.h"

//----------------------------------FUNKCJE KONSOLI----------------------------------
//----------definicja funkcji u¿ywanych w konsoli jako sta³ych numerycznych----------

const int ShowTerminal_FUNC = 0x01;
const int FPS_FUNC = 0x02;
const int CollisionBox_FUNC = 0x04;
const int InputProccesTime_FUNC = 0x08;
const int PhysicsProccesTime_FUNC = 0x10;
const int GraphicsProccesTime_FUNC = 0x20;
const int Timer_FUNC = 0x40;
const int PlayerPos_FUNC = 0x80;

const int maxTerminalTextSize = 44;

const Vector CONSOLE_white(1.0, 1.0, 1.0);
const Vector CONSOLE_green(0.0, 1.0, 0.0);
const Vector CONSOLE_red(1.0, 0.0, 0.0);
const Vector CONSOLE_yellow(1.0, 1.0, 0.0);

enum alignment { LEFT, RIGHT };																		

//----------------------------------------------------------------------------------------------------------------
//--------------------------------------------------KONSOLA GRY---------------------------------------------------
//----------konsola u¿ywana do wprowadzania komend przez gracza oraz do wyœwietlania danych na temat gry----------
//----------------------------------------------------------------------------------------------------------------

class CONSOLE {
	CONSOLE() {}

	static FONT consoleFont;																//czcionka u¿ywana do wypisywania danych

	static unsigned int enabledFunc;														//zmienna okreœlaj¹ca które funkcje konsoli s¹ w³¹czone [wart. domyslna = 0]

	static Vector terminalSize;																//wielkoœæ terminala w OpenGL [zale¿na od wspó³rzêdnej po³o¿enia z oraz wielkoœci 'terminalPixelSize']
	static Vector terminalPixelSize;														//wielkoœæ terminala w pikselach [const]

	static std::string terminalCommand;														//komenda wpisana do terminala [tekst zatweirdzony przyciskiem enter]
	static unsigned int terminalTextSize;															//wielkoœæ tekstu wpisanego do terminala
	static char terminalText[maxTerminalTextSize+1];										//tekst wpisany do terminala			

	//parametry opisuj¹ce wymiary ekranu w OpenGL
	static double textScreenWidth;															//PO£OWA szerokoœci ekranu
	static double textScreenHeight;															//PO£OWA wysokoœci ekranu
	static double textScreenRatio;															//stosunek rozdzielczoœci ekranu [szerokoœæ/wysokoœæ]
	static double textScreenAdjust;															//ró¿nica pomiêdzy wysokoœci¹ 't³a' terminalu, a wielkoœci¹ tekstu

	static unsigned long timer;																//mierzona wartoœæ czasu przez funkcjê 'ShowTimer'

	static std::vector<unsigned> FPSval;

	static void CheckCommand();																//sprawdzenie wprowadzonej komendy do terminala

	static void ShowConsoleTerminal();														//wyœwietlenie terminalu konsoli
	static void DrawTerminalBackground();													//narysowanie 't³a' terminalu konsoli
	static Vector GetTerminalBackgroundPosition();											//pobranie pozycji 't³a' terminalu konsoli
	static void TerminalText();																//wyœwietlenie tekstu w terminalu
	static Vector GetTerminalTextPosition();												//pobranie pozycji tekstu w terminalu

	static void ShowFPS();																	//wyœwietlenie aktualnej liczby FPS
	static unsigned int GetFPS();															//pobranie aktulanej wartoœci FPS

	static void DrawCollisionBoxes();														//wyœwietalnie bry³ kolizji postaci [klasy 'Character' i 'Player']

	static void ShowInputProccessTime();													//wyœwietlenie czasu wykonywania operacji pobrania sygna³ów wejœciwoych
	static void ShowPhysicsProcessTime();													//wyœwietlenie czasu wykonania operacji obliczenia 'fizyki' gry
	static void ShowGraphicsProccessTime();													//wyœwietlenie czasu wykonania operacji wyœwietlenia grafiki

	static void ShowTimer();																//wyœwietlanie pomiaru czasu
	static void GetTimerVal(char* buffer);													//zapis wartoœci zmierzonego czasu do bufora
	static void ResetTimer();																//zresetowanie timera

	static void ShowPlayerPos();

	static bool IsFuncEnabled(int func) { return (enabledFunc & func); }					//sprawdzenie czy dana funkcja jest 'w³¹czona'
	static void EnableFunc(int func) { enabledFunc |= func; }								//w³¹czenie funkcji
	static void DisableFunc(int func) { enabledFunc &= ~func; }								//wy³¹czenie funkcji

	static void CountTextScreenParams(double z);											//obliczenie parametrów opisuj¹cych wymiary okna OpenGL

	static Vector GetTextScreenPosition(alignment alin, unsigned int line);
	static void WriteTextOnScreen(Vector position, char buffer[], Vector color);

public:
	static void Initialize();																//inicjalizacja konsoli

	static void ShowConsoleOutput();														//wyœwietlenie aktywnych 'funkcji' konsoli

	static void GetTerminalInput(char a);													//wczytanie znaku do terminala

	static void ShowTerminal() { enabledFunc |= ShowTerminal_FUNC; }						//w³¹czenie terminala

	static void HideTerminal() { enabledFunc &= ~ShowTerminal_FUNC; }						//wy³¹czenie terminala

	static unsigned int GetTerminalStatus() { return (enabledFunc & ShowTerminal_FUNC); }	//sprawdzenie stanu terminala
};