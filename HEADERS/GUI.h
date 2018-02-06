#pragma once
#include "WinAPIwindow.h"
#include "FONT.h"
#include "GUICONST.h"

//----------------------------------------------------------------------------------------------------
//------------------------------KLASA INTERFEJSU GRAFICZNEGO U¯YTKOWNIKA------------------------------
//----------------------------------------------------------------------------------------------------

class GUI {
	GUI() {}
	GUI(const GUI& obj) {}
	GUI& operator=(const GUI& obj) {}

	//standardowe rozmiary czcionek
	static unsigned normalFontSize;
	static unsigned smallFontSize;
	static unsigned bigFontSize;
	//rozmiary czcionek w OpenGL
	static double screenNormalFontHeight;
	static double screenSmallFontHeight;
	static double screenBigFontHeight;

	//czcionki
	static FONT guiNormalFont;
	static FONT guiSmallFont;
	static FONT guiBigFont;

	//parametry okna 
	static double screenDepth;
	static double screenWidth;
	static double screenHeight;
	static double screenRatio;

	//funkcje rysuj¹ce poszczególne elementy GUI
	static void DrawAmmoCount();																							//stan amunicji gracza
	static void DrawReloadInfo();																							//informacje dotycz¹ce prze³adowania broni
	static void DrawPlayerHealthInfo();																						//punkty ¿ycia gracza
	static void DrawBotsHealthInfo();																						//punkty ¿ycia botów

	//funkcje pomocnicze
	static void WriteTextOnScreen(Vector position, char buffer[], Vector color, GUICONST::FONTSIZE size=GUICONST::NORMAL);	//wypisanie tekstu na ekranie
	static void DrawLine(Vector begin, Vector end, double thickness, Vector color, bool onScreen=true);						//narysowanie linii
	static void DrawBar(Vector begin, Vector end, double thickness, Vector color, bool onScreen=true);						//narysowanie 'paska' [prostok¹ta o okreœlonej gruboœci]

public:
	static void Initialize();																								//inicjalizacja GUI

	static void Show();																										//narysowanie GUI

};