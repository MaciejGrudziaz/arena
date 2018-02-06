#define WIN32_LEAN_AND_MEAN		//'odchudzanie' aplikacji Windows
#include "WinAPIwindow.h"

//-----------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------FUNKCJA MAIN APLIKACJI WINAPI------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInsatnce, LPSTR lpCmdLine, int nShowCmd) {
	if (!WinAPIwindow::SetupWindowSettings(hInstance))		//stworzenie okna aplikacji WinAPI
		return 0;

	WinAPIwindow::ShowWinAPIwindow();						//wyœwietlenie okna apliakcji WinAPI

	GAMElogic::InitializeGameLogic();						//inicjalizacja logiki gry

	WinAPIwindow::WinAPImainLoop();							//g³ówna pêtla aplikacji WinAPI

	return (WinAPIwindow::msg).wParam;
}