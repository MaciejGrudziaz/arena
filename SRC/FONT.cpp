#include "FONT.h"

//inicjalizacja wybranej czcionki
void FONT::Initilize(char* fontName, unsigned int FontSize, HDC _hDC) {
	fontSize = FontSize;
	hDC = _hDC;

	if (listBase == 0)																	//jeœli nie zosta³a zainicjalizowana ¿adna lista czcionek
		listBase = CreateBitmapFont(fontName, FontSize);								//stwórz czcionkê o danej nazwie i rozmiarze
	else {
		glDeleteLists(listBase, 96);													//w przeciwnym wypadku - usuñ istniej¹ce listy

		listBase = CreateBitmapFont(fontName, FontSize);								//i stwórz nowe
	}
}

//wypisanie stringa na ekranie
void FONT::PrintString(char* str) {
	if ((listBase == 0) || (str == NULL))												//sprawdzenie czy czionka istnieje i czy podany string istnieje
		return;

	glPushAttrib(GL_LIST_BIT);															//zachowanie atrybutu GL_LIST_BIT na stosie											
	glListBase(listBase - 32);															//wyznacza offset dla funkcji glCallLists do wygenerowania listy
	glCallLists(strlen(str), GL_UNSIGNED_BYTE, str);									//wywo³anie list [ 'strlen(str)' - iloœæ list do wygenerowania, 'GL_UNSIGNED_BYTE' - rodzaj danych reprezontowanych 
																						//przez listê, 'str' - adres tablicy offsetów dla list do wygenerowania]
	glPopAttrib();																		//przywrócenie atrybutów zachowanych na stosie
}

//stworzenie i zarejestrowanie w systemie czcionki o danej nazwie i rozmiarze
unsigned int FONT::CreateBitmapFont(char* fontName, int fontSize) {
	HFONT hFont;																		//uchwyt dla czionki
	unsigned int base;																	//identyfiaktor listy bazowej dla czionki

	base = glGenLists(96);																//wygenerowanie 96 list wyœwietlania

	if (_stricmp(fontName, "symbol") == 0) {											//jeœli jako nazwê czcionki podano nazwê 'symbol'
		hFont = CreateFont(fontSize, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,				//wczytaj zestaw znaków SYMBOL_CHARSET
			SYMBOL_CHARSET, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS,
			ANTIALIASED_QUALITY, FF_DONTCARE | DEFAULT_PITCH,
			fontName);
	}
	else {
		hFont = CreateFont(fontSize, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,				//w przeciwnym wypadku wczytaj zestaw znaków ANSI_CHARSET
			ANSI_CHARSET, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS,
			ANTIALIASED_QUALITY, FF_DONTCARE | DEFAULT_PITCH,
			fontName);
	}

	if (!hFont)																			//jeœli nie udao siê zarejestrowaæ czcionki zwróc wartoœæ identyfiakotra listy bazowej = 0
		return 0;
																						//jeœli czcionka zosta³a zarejestrowana
	SelectObject(hDC, hFont);												//wybranie kontekstu urz¹dzenia	
	wglUseFontBitmaps(hDC, 32, 96, base);									//przygotowanie czcionki do u¿ycia w OpenGL

	return base;																		//zwrócenie wartoœci identyfikatora listy bazowej dla stworzonej czcionki
}