#include "WinAPIwindow.h"

//inicajlizacja zmiennych statycznych dla klasy okna aplikacji WinAPI
WNDCLASSEX WinAPIwindow::windowClass;
HWND WinAPIwindow::hwnd = NULL;					
MSG WinAPIwindow::msg = { 0 };
HDC WinAPIwindow::hDC = NULL;
HGLRC WinAPIwindow::hRC = NULL;
HINSTANCE WinAPIwindow::hInstance = NULL;
double WinAPIwindow::FOV = 90.0;

//inicjalizacja zmiennych okreœalj¹cych tryb wywietlania [okienkowy/pe³noekranowy]
#define fullscreenToggle 0
#if fullscreenToggle == 0
bool WinAPIwindow::fullscreen = false;
int WinAPIwindow::screenWidth = windowscreenWidth;
int WinAPIwindow::screenHeight = windowscreenHeight;
#endif
#if fullscreenToggle == 1
bool WinAPIwindow::fullscreen = true;
int WinAPIwindow::screenWidth = fullscreenWidth;
int WinAPIwindow::screenHeight = fullscreenHeight;
#endif

//ustawienie w³aœciwoœci i stworzenie okna [pobranie uchwytu okna HWND i kontekstu urz¹dzenia HDC]
int WinAPIwindow::SetupWindowSettings(HINSTANCE HInstance) {
	hInstance = HInstance;

	initializeWindowClassEx();				//inicjalizacja klasy okna WinAPI

	if (!RegisterClassEx(&(windowClass)))			//rejestracja klasy okna
		return 0;

	if(!CreateNewWindow())							//stworzenie nowego okna
		return 0;

	return 1;
}

//stworzenie nowego okna [stworzenie samego nowego uchwytu i kontekstu urz¹dzenia bez nowej klasy]
int WinAPIwindow::CreateNewWindow() {
	if (fullscreen)
		SetupFullscreenSettings();					//ustawienie w³aœciwoœci dla trybu pe³noekranowego

	ShowCursor(FALSE);								//ukrycie kursora

	if (!createWindowEx())							//stworzenie okna aplikacji
		return 0;

	SetCapture(WinAPIwindow::hwnd);

	return 1;
}

int WinAPIwindow::CreateGraphicContext(HWND hwnd) {
	hDC = GetDC(hwnd);
	if (!hDC)
		return 0;
	SetupPixelFormat();
	hRC = wglCreateContext(hDC);
	if (!hRC)
		return 0;
	wglMakeCurrent(hDC, hRC);

	return 1;
}

//funkcja s³u¿¹ca do zwolnienia kontekstu grafiki OpenGL dla kontekstu urz¹dzenia WinAPI
void WinAPIwindow::ReleaseGraphicContext() {
	wglMakeCurrent(hDC, NULL);
	wglDeleteContext(hRC);
}

//funkcja wyœwietlaj¹ca i aktualizuj¹ca okno WinAPI
void WinAPIwindow::ShowWinAPIwindow() {
	ShowWindow(hwnd, SW_SHOW);						//wyœwietlenie i aktualizacja okna WinAPI
	UpdateWindow(hwnd);
}

//przetwarzanie komunikatów systemu Windows
void WinAPIwindow::MessageProcessing() {
	TranslateMessage(&msg);							//przet³umaczenie wiadomoœci i przes³anie jej do systemu
	DispatchMessage(&msg);
}

//funkcja ustawiaj¹ca dane opcje dla ekranu apliakcji OpenGL
int WinAPIwindow::SetupOpenGLScreenSettings() {
	//nadanie wymiaru okna OpenGL
	glViewport(0, 0, screenWidth, screenHeight);

	glMatrixMode(GL_PROJECTION);						//okreœlenie macierzy rzutowania
	glLoadIdentity();									//zresetowanie macierz rzutowania

														//wyznaczenie proporcji obrazu i pola widzenia
	gluPerspective(FOV, (GLfloat)screenWidth / (GLfloat)screenHeight, 0.01f, 1000.0f);

	glMatrixMode(GL_MODELVIEW);							//wybranie macierzy widoku modelu
	glLoadIdentity();									//zresetowanie macierzy widoku

	return 1;
}

//g³owna pêtla aplikacji WinAPI
void WinAPIwindow::WinAPImainLoop() {
	bool done = false;								//zmienna pomocnicza okreœlaj¹ca zakoñczenie dzia³ania aplikacji

	while (!done) {
		//sprawdzenie bierz¹cej wiadomoœci systemu Windows [bez jej odczytywania]
		PeekMessage(&msg, NULL, NULL, NULL, PM_REMOVE);

		//jeœli pobrano wiadomoœæ okreœlaj¹c¹ zamkniêcie okna to koñczymy dzia³anie aplikacji
		if (msg.message == WM_QUIT) {		
			done = true;
		}
		//w przeciwnym wypadku
		else {
			GAMElogic::ProccessFrame();				//przetworrzenie klatki gry (wejœcie, fizyka, grafika)
			MessageProcessing();		//przetwarzanie komunikatów Windows	
		}
	}
}

//inicjalizacja klasy okna WNDCLASSEX
void WinAPIwindow::initializeWindowClassEx() {
	windowClass.cbSize = sizeof(WNDCLASSEX);
	windowClass.style = CS_HREDRAW | CS_VREDRAW;
	windowClass.lpfnWndProc = WinAPImessages::WndProc;
	windowClass.cbClsExtra = 0;
	windowClass.cbWndExtra = 0;
	windowClass.hInstance = hInstance;
	windowClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	windowClass.hbrBackground = NULL;
	windowClass.lpszMenuName = NULL;
	windowClass.lpszClassName = "OknoWinAPI";
	windowClass.hIconSm = LoadIcon(NULL, IDI_WINLOGO);
}

//okreslenie podstawowego stylu okna w zale¿noœci od trybu wyœwietlania [pe³noekranowy/okienkowy]
DWORD WinAPIwindow::getWindowStyle() {
	DWORD dwStyle;									//podstawowy styl okna

	if (fullscreen)
		dwStyle = WS_POPUP;
	else
		dwStyle = WS_OVERLAPPED | WS_VISIBLE | WS_SYSMENU;

	return dwStyle;
}

//okreœlenie rozszerzonego trybu okna w zale¿noœci od trybu wyœwietlania [pe³noekranowy/okeinkowy]
DWORD WinAPIwindow::getWindowExStyle() {
	DWORD dwExStyle;								//rozszerzony styl okna

	if (fullscreen)
		dwExStyle = WS_EX_APPWINDOW;
	else
		dwExStyle = NULL;

	return dwExStyle;
}

//stworzenie okna aplikacji oraz pobranie jego uchwytu [HWND]
int WinAPIwindow::createWindowEx() {
	DWORD dwStyle, dwExStyle;						//style okna [podstawowy - dwStyle i rozszerzony - dwExStyle]

	dwStyle = getWindowStyle();						//pobranie podstawowego stylu okna
	dwExStyle = getWindowExStyle();					//pobranie rozszerzonego stylu okna

													//stworzenie okna aplikacji oraz pobranie jego uchwytu
	hwnd = CreateWindowEx(dwExStyle,
		"OknoWinAPI",
		"ARENA v2",
		dwStyle | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
		0, 0,
		screenWidth, screenHeight,
		NULL,
		NULL,
		hInstance,
		NULL);

	if (!hwnd)										//sprawdzenie czy okno zosta³o stworzone
		return 0;
	return 1;
}

//ustawienie w³aœciwoœci okna dla trybu pe³noekranowego [stworzenie struktur DEVMODE i RECT]
void WinAPIwindow::SetupFullscreenSettings() {
	DWORD dwStyle, dwExStyle;						//style okna

	dwStyle = getWindowStyle();						//pobranie stylów okna
	dwExStyle = getWindowExStyle();

	//sktruktura DEVMODE zawieraj¹ca informacjê o inicjalizacji i konfiguracji urz¹dzenia wyjœciowego [ekranu]
	DEVMODE dmScreenSettings;
	memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
	dmScreenSettings.dmSize = sizeof(dmScreenSettings);
	dmScreenSettings.dmPelsWidth = screenWidth;
	dmScreenSettings.dmPelsHeight = screenHeight;
	dmScreenSettings.dmBitsPerPel = 32;
	dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

	//zmiana trybu wyœwietlania na pe³noekranowy
	if (ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL)
		fullscreen = false;

	///struktura RECT okreœlaj¹ca prostok¹t zawartoœci okna w systemie Windows
	RECT windowRect;
	windowRect.top = 0;								//górny, lewy naro¿nik okna
	windowRect.left = 0;
	windowRect.bottom = screenHeight;				//dolny, prawy naro¿nik okna
	windowRect.right = screenWidth;

	//funkcja wyznaczaj¹ca rozmiar okna systemu Windows na podstawie zadanych rozmiarów prostok¹ta zawartoœci [struktura RECT]
	//pozwala to aplikacji OpenGL wykorzytsaæ maksymalnie rozmiar okna systemu
	AdjustWindowRectEx(&windowRect, dwStyle, FALSE, dwExStyle);
}

//wybranie i usatwienie foramtu pikseli
void WinAPIwindow::SetupPixelFormat() {
	int nPixelFormat;									//indeks formatu pikseli

														//struktura okreœlaj¹ca u¿ywany foramt pikseli
	static PIXELFORMATDESCRIPTOR pfd = {
		sizeof(PIXELFORMATDESCRIPTOR),					//rozmiar struktury
		1,												//domyœlna wersja
		PFD_DRAW_TO_WINDOW |							//grafika w oknie
		PFD_SUPPORT_OPENGL |							//grafika OpenGL
		PFD_DOUBLEBUFFER,								//podwójne buforowanie
		PFD_TYPE_RGBA,									//tryb kolorów RGBA
		32,												//32-bitowy opis kolorów
		0, 0, 0, 0, 0, 0,								//nie specyfikuje bitów kolorów
		0,												//bez bufora alfa
		0,												//ignoruje bit przesuniêcia
		0,												//bez bufora akumulacji
		0, 0, 0, 0,										//ignoruje bity akumulacji
		32,												//32-bitowy bufor Z
		0,												//bez bufora powielania
		0,												//bez buforów pomocniczych
		PFD_MAIN_PLANE,									//g³ówna p³aszczyzna rysowania
		0,												//zarezerwowane
		0, 0, 0 };										//ignoruje maski warstw

														//wybiera najodpowiedniejszy format pikseli z dostepnych
	nPixelFormat = ChoosePixelFormat(hDC, &pfd);

	//okreœla format pikseli dla kontekstu urz¹dzenia
	SetPixelFormat(hDC, nPixelFormat, &pfd);
}