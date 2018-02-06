#pragma once
#include <dinput.h>
#include "WinAPIwindow.h"

#define IS_USEKEYBOARD 0x01
#define IS_USEMOUSE 0x02

//----------------------------------------------------------------------------------------------------
//--------------------------------------OBS£UGA SYSTEMU WEJŒCIA---------------------------------------
//--------------------obs³uga systemu wejœcia przy u¿yciu biblioteki DirectX Input--------------------
//--------------------------------obs³uga wejœcia z myszy i klawiatury--------------------------------
//----------------------------------------------------------------------------------------------------

//wejœcie z klawiatury
class DIRECTINPUT_KEYBOARD
{
	LPDIRECTINPUTDEVICE8 m_pDIDev;										//wskaŸnik dla interfejsu wejœcia poprzez Direct Input
	unsigned char m_keys[256];											//tablica przetrzymuj¹ca aktualne wartoœci wejœcia z klawiatury
public:
	DIRECTINPUT_KEYBOARD(LPDIRECTINPUT8 pDI, HWND hwnd);				//konstruktor
	~DIRECTINPUT_KEYBOARD();											//destruktor

	bool KeyDown(int key) {												//metoda zwracaj¹ca aktualny stan wciœniêcia przycisku okreœlonego kodem 'key'
		if (m_keys[key]) return true;
		else return false;
	}
	bool KeyUp(int key) {												//metoda zwracja¹ca aktualny stan puszczenia przycisk okreœlonego kodem 'key'
		if (m_keys[key]) return false;
		else return true;
	}

	bool Update();														//uaktualnienie tablicy wejœæ

	void Clear() { ZeroMemory(m_keys, 256 * sizeof(char)); }			//wyczyszczenie z pamiêci tablicy 'm_keys'

	bool Acquire();														//pobranie dostêpu do systemu wejœcia (klawiatury)
	bool Unacquire();													//zwolnienie dostêpu do systemu wejœcia (klawiatury)
};

//wejœcie z myszy
class DIRECTINPUT_MOUSE
{
	LPDIRECTINPUTDEVICE8 m_pDIDev;										//wskaŸnik dla interfejsu wejœcia Direct Input
	DIMOUSESTATE m_state;												//struktura opisuj¹ca stan myszy							
public:
	//konstruktor
	DIRECTINPUT_MOUSE(LPDIRECTINPUT8 pDI, HWND hwnd, bool isExclusive = true);
	~DIRECTINPUT_MOUSE();												//desktruktor

	//funkcje zwracaj¹ce stan przycisków myszy
	bool ButtonDown(int button) { return (m_state.rgbButtons[button] & 0x80) ? true : false; }
	bool ButtonUp(int button) { return (m_state.rgbButtons[button] & 0x80) ? false : true; }
	//zwraca przesuniêcie kó³ka myszki [pozycja wzglêdna]
	int GetWheelMovement() { return  m_state.lZ; }			
	//zwraca przemieszczenie myszy [pozycja wzglêdna]
	void GetMovement(int &dx, int &dy) { dx = m_state.lX; dy = m_state.lY; }

	bool Update();														//odœwie¿enie stanu myszy

	bool Acquire();														//pobranie dostêpu do systemu wejœcia [myszy]
	bool Unacquire();													//zwolnienie dostêpu do systemu wejœcia [myszy]
};

class DIRECTINPUT
{
	DIRECTINPUT();														//konstruktor domyœlny

	static DIRECTINPUT_KEYBOARD* m_pKeyboard;							//wskaŸnik na obiekt klawiatury
	static DIRECTINPUT_MOUSE* m_pMouse;									//wskaŸnik na obiekt myszy
	static LPDIRECTINPUT8 m_pDI;										//wskaŸnik dla interfejsu Direct Input

public:
	//inicjalizacja systemu wejœcia Direct Input
	static bool Initialize(bool isExclusive=true, DWORD flags = IS_USEKEYBOARD | IS_USEMOUSE);
	static bool Shutdown();												//zamkniêcie/zwolnienie systemu wejœcia 

	static void AcquireAll();											//pobranie wszystkich elementów systemu wejœcia
	static void UnacquireAll();											//zwolnienie wszystkich elementów systemu wejœcia

	static DIRECTINPUT_KEYBOARD* GetKeyboard() { return m_pKeyboard; }	//zwrócenie wskaŸnika na obiekt klawiatury
	static DIRECTINPUT_MOUSE* GetMouse() { return m_pMouse; }			//zwrócenie wskaŸnika na obiekt myszy

	static bool Update();												//aktualizowanie systemów wejœcia

	//funkcje sprawdzaj¹ce stan wciœniêcia danego klawisza na klawiaturze
	static bool KeyDown(int key) { return (m_pKeyboard && m_pKeyboard->KeyDown(key)); }
	static bool KeyUp(int key) { return (m_pKeyboard && m_pKeyboard->KeyUp(key)); }

	//funkcje sprawdzaj¹ce stan wciœniêcia danego przycisku na myszy
	static bool ButtonDown(int button) { return (m_pMouse && m_pMouse->ButtonDown(button)); }
	static bool ButtonUp(int button) { return (m_pMouse && m_pMouse->ButtonUp(button)); }

	//pobranie przemiszczenia myszy [pozycji wzglêdnej]
	static void GetMouseMovement(int &dx, int &dy) { if (m_pMouse) m_pMouse->GetMovement(dx, dy); }
	//pobranie przemieszczenia rolki myszy
	static int GetMouseWheelMovement() { return (m_pMouse) ? m_pMouse->GetWheelMovement() : 0; }
};