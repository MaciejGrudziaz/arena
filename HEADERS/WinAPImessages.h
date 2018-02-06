#pragma once
#include <windowsx.h>
//#include <Windows.h>
#include "WinAPIwindow.h"
#include "WINAPI_INPUT.h"

//------------------------------------------------------------------
//--------------------OBS£UGA KOMUNIKATÓW WINAPI--------------------
//----klasa obs³uguj¹ca komunikaty pochodz¹ce z aplikacji WinAPI----
//----------------[mo¿e byæ u¿yta do obs³ugi wejœcia]---------------
//------------------------------------------------------------------

class WinAPImessages:public WINAPI_INPUT{
	WinAPImessages() {}

	static int ChangeWindowSize();									//funkcja pomocnicza u¿ywana przy zmianie trybu wyœwietlania [pe³noekranowy/okienkowy]

	static void WriteTimeMeasurmentstoFile();

public:
	//g³owna funkcja przetwarzaj¹ca komunikaty WinAPI
	static LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
};