#include "DIRECTINPUT.h"

//inicjalizacja zmiennych statycznych
DIRECTINPUT_KEYBOARD* DIRECTINPUT::m_pKeyboard = NULL;
DIRECTINPUT_MOUSE* DIRECTINPUT::m_pMouse = NULL;
LPDIRECTINPUT8 DIRECTINPUT::m_pDI = NULL;

//--------------------------------------------------------------------------------------------------------------
//-----------------------------------------------MAKRO FAILED(..)-----------------------------------------------
//----------------zwraca wartoœæ 'TRUE' jeœli operacja wywo³ana wewn¹trz makra nie powiod³a siê-----------------
//------------------------------jesli operacj powiod³a siê zwraca wartoœæ 'FALSE'-------------------------------
//--------------------------------------------------------------------------------------------------------------
//**************************************************************************************************************
//--------------------------------------------------------------------------------------------------------------
//--------------------------------------------------KLAWIATURA--------------------------------------------------
//--------------------------------------------------------------------------------------------------------------

//konstruktor dla systemu wejœcia klawiatury [pobiera wskaŸnik do obiekty systemu wejœcia oraz uchwyt okna WinAPI]
DIRECTINPUT_KEYBOARD::DIRECTINPUT_KEYBOARD(LPDIRECTINPUT8 pDI, HWND hwnd)
{
	//stworzenie obiektu klawiatury
	pDI->CreateDevice(GUID_SysKeyboard, &m_pDIDev, NULL);

	//ustawienie formatu danych [domyœlny]
	m_pDIDev->SetDataFormat(&c_dfDIKeyboard);

	//ustawienie trybu pracy [apliakcja korzysta z systemu wejscia jeœli jest na pierwszym planie i nie koliduje z innymi]
	m_pDIDev->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);

	//pobranie dostêpu do systemu wejœcia
	m_pDIDev->Acquire();

	//wyczyszczenie tablicy sygna³ów wejœciwoych
	Clear();
}

//destruktor 
DIRECTINPUT_KEYBOARD::~DIRECTINPUT_KEYBOARD()
{
	//jeœli obiekt klawaitury zosta³ stworzony
	if (m_pDIDev)
	{
		//zwolnienie systemu wejœcia klawiatury
		m_pDIDev->Unacquire();
		m_pDIDev->Release();
	}
}

//odœwie¿enia sygna³ów wejœciowych z klawiatury
bool DIRECTINPUT_KEYBOARD::Update()
{
	//pobranie stanu klawiatury i zapisanie go do do tablicy 'm_keys'
	if (FAILED(m_pDIDev->GetDeviceState(sizeof(m_keys), (LPVOID)m_keys)))
	{
		//jeœli operacja nie powiod³a siê to pobieramy dostêp do sysemtu wejœcia z klawiatury
		if (FAILED(m_pDIDev->Acquire()))
		{
			return false;
		}
		//ponowana próba wczytania stanu kalwiatury i zapisania go do tablicy 'm_keys'
		if (FAILED(m_pDIDev->GetDeviceState(sizeof(m_keys), (LPVOID)m_keys)))
		{
			return false;
		}
	}
	return true;
}

//pobranie dostêpu do systemu wejœcia z klawiatury
bool DIRECTINPUT_KEYBOARD::Acquire()
{
	//wyczyszczenie tablicy sygna³ów wejœciowych
	Clear();			
	return (!FAILED(m_pDIDev->Acquire()));
}

//zwolnienie dostepu do systemu wejscia z klawiatury
bool DIRECTINPUT_KEYBOARD::Unacquire()
{
	//wyczyszczenie tablicy sygna³ów wejœciowych
	Clear();
	return (!FAILED(m_pDIDev->Unacquire()));
} 

//--------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------MYSZ-----------------------------------------------------
//--------------------------------------------------------------------------------------------------------------

//konstruktor dla systemu wejœcia myszy [pobiera wskaŸnik dla interefejsu systemu g³ównego wejscia Direct Input, uchwyt okna apliakcji WinAPI oraz 
//znacznik czy mysz ma byæ na wy³¹cznoœæ aplikacji czy nie]
DIRECTINPUT_MOUSE::DIRECTINPUT_MOUSE(LPDIRECTINPUT8 pDI, HWND hwnd, bool isExclusive)
{
	//stworznie obiektu dla systemu wejœcia myszy
	pDI->CreateDevice(GUID_SysMouse, &m_pDIDev, NULL);

	//usatwinie formatu danych [domyœlny]
	m_pDIDev->SetDataFormat(&c_dfDIMouse);

	//ustawienie flag w zale¿noœci od zancznika wy³¹cznoœci
	DWORD flags;
	//apliakcja u¿ywa myszy gdy jest na pierwszym planie, na wy³acznoœæ oraz dezaktywuje dzia³anie przycisku Windows
	if (isExclusive) flags = DISCL_FOREGROUND | DISCL_EXCLUSIVE | DISCL_NOWINKEY;
	//aplikacja u¿ywa myszy gdy jest na piewszym planie oraz gdy nie koliduje z innymi aplikacjami
	else flags = DISCL_FOREGROUND | DISCL_NONEXCLUSIVE;

	//usatwienie trybu pracy w zale¿noœci od wartoœci flag
	m_pDIDev->SetCooperativeLevel(hwnd, flags);

	//pobranie dostêpu do systemu wejœcia myszy
	m_pDIDev->Acquire();

	//pobranie stanu myszy oraz zapisanie go do struktry m_state
	m_pDIDev->GetDeviceState(sizeof(DIMOUSESTATE), &m_state);
}

//destruktor
DIRECTINPUT_MOUSE::~DIRECTINPUT_MOUSE()
{
	//jeœli obiekt myszy istnieje
	if (m_pDIDev)
	{
		//zwolnij dostêp do systemu wejœcia myszy
		m_pDIDev->Unacquire();
		m_pDIDev->Release();
	}
}

//uaktualnienie danych dla sygna³ów systemu wejœcia myszy
bool DIRECTINPUT_MOUSE::Update()
{
	//pobranie stanu urz¹dzenia i zapisanie go do struktury 'm_state'
	if (FAILED(m_pDIDev->GetDeviceState(sizeof(DIMOUSESTATE), &m_state)))
	{
		//jeœli operacja nie powiod³a siê nasepuje próba pobrania dostepu do systemu wejsæia myszy
		if (FAILED(m_pDIDev->Acquire()))
		{
			return false;
		}
		//ponowna próba pobrania stanu urz¹dzenia i zapisania go do struktury 'm_state'
		if (FAILED(m_pDIDev->GetDeviceState(sizeof(DIMOUSESTATE), &m_state)))
		{
			return false;
		}
	}

	return true;
}


//pobranie dostêpu do systemu wejœcia myszy
bool DIRECTINPUT_MOUSE::Acquire()
{
	return (!FAILED(m_pDIDev->Acquire()));
}

//zwolnienie dostêpu do systemu wejscia myszy
bool DIRECTINPUT_MOUSE::Unacquire()
{
	return (!FAILED(m_pDIDev->Unacquire()));
}

//--------------------------------------------------------------------------------------------------------------
//-------------------------------------------------DIRECTINPUT--------------------------------------------------
//--------------------------------------------------------------------------------------------------------------

//inicjalizacja systemu wejœcia DirectInput oraz jego sk³adowych [klawiatury, myszy]
//jako argumenty przyjmuje znacznik czy system wejœcia ma byæ 'ekskluzywny' dla aplikacji oraz flagi jakie sytemy wejœcia wczytaæ [mysz/klawiaturê]
bool DIRECTINPUT::Initialize(bool isExclusive, DWORD flags)
{
	//stworzenie g³ównego systemu wejœcia Direct Input okreœlonego wskaŸnikiem 'm_pDI' dla okna instancji 'WinAPIwindow::hInstance'
	if (FAILED(DirectInput8Create(WinAPIwindow::hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_pDI, NULL))) return false;
	//jeœli flaga obs³ugi klawiatury jest ustawiona
	if (flags & IS_USEKEYBOARD)
	{
		//stwórz nowy obiekt systemu wejœcia dla klawiatury
		m_pKeyboard = new DIRECTINPUT_KEYBOARD(m_pDI, WinAPIwindow::hwnd);
		//jeœli operacja siê nie powiod³¹ zwróæ komunikat b³êdu
		if (m_pKeyboard == NULL) return false;
	}
	//jeœli flaga obs³ugi myszy jest ustawiona
	if (flags & IS_USEMOUSE)
	{
		//stwórz nowy obiekt systemu wejœcia dla myszy
		m_pMouse = new DIRECTINPUT_MOUSE(m_pDI, WinAPIwindow::hwnd, isExclusive);
		//jeœli operacja nie powiod³a siê zwróæ b³¹d
		if (m_pMouse == NULL) return false;
	}
	//pobierz dostêp do wszystkich obiektów systemu wejœcia
	AcquireAll();
	//odœwie¿ stan sygna³ów wejœciowych systemów wejœcia
	Update();

	return true;
}

//koniec pracy systemu wejœcia ['zamytkanie' obecnego systemu wejœcia]
bool DIRECTINPUT::Shutdown()
{
	//zwolnij dostêp do wszystkich systemów wejœcia w systemie
	UnacquireAll();
	//jesli obiekt klawiatury istnieje
	if (m_pKeyboard)
	{
		//usuniêcie obiektu klawiatury
		delete m_pKeyboard;												
		m_pKeyboard = NULL;										
	}
	//jeœli obiekt myszy istnieje
	if (m_pMouse)
	{
		//usuniêcie obiektu myszy
		delete m_pMouse;
		m_pMouse = NULL;
	}
	//zwolnienie dostêpu do g³ównego systemu wejœcia
	if (FAILED(m_pDI->Release())) return false;

	return true;
}

//uaktulanienie stanów sygna³ów systemów wejœcia
bool DIRECTINPUT::Update()
{
	if (m_pKeyboard) m_pKeyboard->Update();

	if (m_pMouse) m_pMouse->Update();

	return true;
}

//pobranie dostêpu do wszystkich obecnych systemów wejœcia
void DIRECTINPUT::AcquireAll()
{
	if (m_pKeyboard) m_pKeyboard->Acquire();

	if (m_pMouse) m_pMouse->Acquire();
}

//zwolnienie dostêpu do wszystkich obecnych systemów wejœcia
void DIRECTINPUT::UnacquireAll()
{
	if (m_pKeyboard) m_pKeyboard->Unacquire();

	if (m_pMouse) m_pMouse->Unacquire();
}
