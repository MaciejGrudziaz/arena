#pragma once
//#include "WinAPIwindow.h"
#include<Windows.h>
#include<gl\GL.h>

//-------------------------------------------------------------------------------------------------------------------
//----------------------------------------CZCIONKI WYŒWIETLANE NA EKRANIE GRY----------------------------------------
//--------------------klasa odpowiadaj¹ca za definicjê czcionki rastrowej wyœwietlanej na ekranie--------------------
//-------------------------------------------------------------------------------------------------------------------

class FONT {
	HDC hDC;																		//uchwyt aktualnego okna dla czcionki

	unsigned int fontSize;															//rozmiar czcionki

	unsigned int listBase;															//identyfikator listy bazowej dla czcionki

	unsigned int CreateBitmapFont(char* fontName, int fontSize);					//stworzenie i zarejestrowanie w systemie czcionki o danej nazwie i rozmiarze

public:
	FONT() { listBase = 0; fontSize = 0; }														//kosntruktor domyœlny [domyœlna wartoœæ listBase=0 - lista nie zosta³a stworzona]
	FONT(char* fontName, unsigned int FontSize):fontSize(FontSize) {									//konstruktor tworz¹cy czcionkê o danej nazwie i rozmiarze
		listBase = CreateBitmapFont(fontName, FontSize);
	}
	~FONT() {																		//destruktor [usuwa wszystkie listy przechowuj¹ce czcionki]
		if (listBase != 0)
			glDeleteLists(listBase, 96);											//zwolnienie wszystkich list wyœwietlania
	}

	void Initilize(char* fontName, unsigned int fontSize, HDC _hDC);				//inicjalizacja wybranej czcionki

	void PrintString(char* str);													//wypisanie stringa na ekranie

	unsigned int GetFontSize() { return fontSize; }
};