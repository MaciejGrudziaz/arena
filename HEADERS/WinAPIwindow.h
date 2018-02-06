#pragma once
#include "GAMElogic.h"

//definicja rozmiarów okna dla wybranych trybów wyœwietlania
const unsigned int fullscreenWidth = 1920;
const unsigned int fullscreenHeight = 1080;
const unsigned int windowscreenWidth = 1280;
const unsigned int windowscreenHeight = 720;

//-------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------OKNO APLIKACJI WINAPI-----------------------------------------------                                                                                              
//-----[klasa przechowuj¹ca struktury okreœlaj¹ce dan¹ instancjê okna oraz metody umo¿liwij¹ce jego stworzenie]------
//-------------------------------------------------------------------------------------------------------------------

class WinAPIwindow {
	WinAPIwindow() {}

	static void initializeWindowClassEx();							//iniclaizacja klasy okna WinAPI
	static DWORD getWindowStyle();									//okreœlenie podstawowych sty³ów okna WinAPI w zale¿noœci od trybu wyœwietlania [pe³noekranowy/okienkowy]
	static DWORD getWindowExStyle();								//okreœlenie dodatkowych stylów okna WinAPI w zale¿noœci od trybu wyœwietlania [pe³noekranowy/okienkowy]
	static int createWindowEx();									//stworzenie okna WinAPI w zale¿noœci od trybu wyœwietlania [pe³noekranowy/okienkowy]
	static void SetupFullscreenSettings();							//ustawianie w³aœciwoœci okna dla trybu pe³noekranowego
	static void SetupPixelFormat();									//funkcja okreœlaj¹ca foramt pikseli dla OpenGL

public:
	static WNDCLASSEX windowClass;									//klasa okna Windows
	static HWND hwnd;												//uchwyt okna WinAPI
	static MSG msg;													//komunikat pochodz¹cy z aplikacji WinAPI
	static HDC hDC;													//kontekst urz¹dzenia [u¿ywany do tworzenia grafiki w oknie]
	static HGLRC hRC;												//kontekst grafiki OpenGL
	static HINSTANCE hInstance;										//obecna instancja okna aplikacji
	static double FOV;												//obecna wartoœæ FOV

	static bool fullscreen;											//tryb pe³noekranowy
	static int screenWidth;											//szerokoœæ i wysokoœæ ekranu
	static int screenHeight;

	static int SetupWindowSettings(HINSTANCE hInstance);			//stworzenie ca³ego okna WinAPI [klasa okna, uchwyt, kontekst urz¹dzenia]
	static int CreateNewWindow();									//stworzenie nowego okna [stworzenie samego nowego uchwytu i kontekstu urz¹dzenia bez nowej klasy]
	static int CreateGraphicContext(HWND hwnd);						//utworzenie kontekstów grafiki dla okna WinAPI
	static void ReleaseGraphicContext();							//funkcja s³u¿¹ca do zwolnienia kontekstu grafiki OpenGL dla kontekstu urz¹dzenia WinAPI
	static void ShowWinAPIwindow();									//funkcja wyœwitalj¹ca okno apliakcji WinAPI
	static void MessageProcessing();								//przetwarzanie komunikatów okna WinAPI
	static int SetupOpenGLScreenSettings();							//funkcja ustawiaj¹ca dane opcje dla ekranu apliakcji OpenGL
	
	static void WinAPImainLoop();									//g³ówna pêtla aplikacji okna WinAPI
};